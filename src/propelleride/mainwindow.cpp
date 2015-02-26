#include "mainwindow.h"

#include <QMessageBox> 
#include <QStatusBar> 
#include <QToolBar> 
#include <QFileDialog> 
#include <QMenu> 
#include <QSerialPortInfo>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), statusMutex(QMutex::Recursive), statusDone(true)
{
    ui.setupUi(this);
    /* setup preferences dialog */
    propDialog = new Preferences(this);
    connect(propDialog,SIGNAL(accepted()),this,SLOT(preferencesAccepted()));

    projectModel = NULL;
    referenceModel = NULL;

    connect(&builder,SIGNAL(compilerErrorInfo(QString,int)), this, SLOT(highlightFileLine(QString,int)));

    /* main container */
    setWindowTitle(QCoreApplication::applicationName());
    QSplitter *vsplit = new QSplitter(this);
    setCentralWidget(vsplit);
    /* project tools */
    setupProjectTools(vsplit);

    /* minimum window height */
    this->setMinimumHeight(500);

    /* setup gui components */

    // File Menu
    connect(ui.action_New,SIGNAL(triggered()),editorTabs,SLOT(newFile()));
    connect(ui.action_Open,SIGNAL(triggered()),editorTabs,SLOT(open()));

    connect(ui.action_Save,SIGNAL(triggered()),editorTabs,SLOT(save()));
    connect(ui.actionSave_As,SIGNAL(triggered()),editorTabs,SLOT(saveAs()));
    connect(ui.actionSave_All,SIGNAL(triggered()),editorTabs,SLOT(saveAll()));

    connect(ui.action_Zip_Project,SIGNAL(triggered()),this,SLOT(zipFiles()));

    recentFiles = findChildren<QAction *>(QRegExp("action_[0-9]+_File"));
    for (int i = 0; i < recentFiles.size(); i++)
        connect(recentFiles.at(i), SIGNAL(triggered()),this, SLOT(openRecentFile()));
    
    connect(ui.action_Close,SIGNAL(triggered()),editorTabs,SLOT(closeFile()));

    connect(editorTabs, SIGNAL(saveAvailable(bool)),ui.action_Save,SLOT(setEnabled(bool)));
    connect(editorTabs, SIGNAL(saveAvailable(bool)),ui.actionSave_All,SLOT(setEnabled(bool)));
    connect(editorTabs, SIGNAL(closeAvailable(bool)),ui.action_Close,SLOT(setEnabled(bool)));
    connect(editorTabs, SIGNAL(closeAvailable(bool)),ui.actionClose_All,SLOT(setEnabled(bool)));


    // Edit Menu
    connect(ui.action_Undo,        SIGNAL(triggered()), editorTabs, SLOT(undo()));
    connect(ui.action_Redo,        SIGNAL(triggered()), editorTabs, SLOT(redo()));

    connect(ui.action_Cut,         SIGNAL(triggered()), editorTabs, SLOT(cut()));
    connect(ui.action_Copy,        SIGNAL(triggered()), editorTabs, SLOT(copy()));
    connect(ui.action_Paste,       SIGNAL(triggered()), editorTabs, SLOT(paste()));
    connect(ui.actionSelect_All,   SIGNAL(triggered()), editorTabs, SLOT(selectAll()));

    connect(ui.action_Find,        SIGNAL(triggered()), this, SLOT(showFindFrame()));
    connect(ui.actionFind_Next,    SIGNAL(triggered()), this, SLOT(findNextClicked()));
    connect(ui.actionFind_Previous,SIGNAL(triggered()), this, SLOT(findPrevClicked()));

    connect(ui.actionPreferences,  SIGNAL(triggered()), this, SLOT(preferences()));

    connect(editorTabs, SIGNAL(undoAvailable(bool)), ui.action_Undo,SLOT(setEnabled(bool)));
    connect(editorTabs, SIGNAL(redoAvailable(bool)), ui.action_Redo,SLOT(setEnabled(bool)));
    connect(editorTabs, SIGNAL(copyAvailable(bool)), ui.action_Cut,SLOT(setEnabled(bool)));
    connect(editorTabs, SIGNAL(copyAvailable(bool)), ui.action_Copy,SLOT(setEnabled(bool)));


    // View Menu
    connect(ui.actionShow_Browser, SIGNAL(triggered()), this, SLOT(showBrowser()));
    connect(ui.actionBigger_Font,  SIGNAL(triggered()), this, SLOT(fontBigger()));
    connect(ui.actionSmaller_Font, SIGNAL(triggered()), this, SLOT(fontSmaller()));

    ui.actionBigger_Font->setShortcuts(QList<QKeySequence>() << QKeySequence::ZoomIn
                                                             << Qt::CTRL+Qt::Key_Equal);


    // Project Menu
    connect(ui.actionView_Info, SIGNAL(triggered()), this, SLOT(viewInfo()));
    connect(ui.actionBuild,     SIGNAL(triggered()), this, SLOT(programBuild()));
    connect(ui.actionRun,       SIGNAL(triggered()), this, SLOT(programRun()));
    connect(ui.actionBurn,      SIGNAL(triggered()), this, SLOT(programBurnEE()));
    connect(ui.actionTerminal,  SIGNAL(triggered()), this, SLOT(spawnTerminal()));

    // Help Menu
    connect(ui.actionPropeller_Datasheet,   SIGNAL(triggered()), this, SLOT(propellerDatasheet()));
    connect(ui.actionPropeller_Manual,      SIGNAL(triggered()), this, SLOT(propellerManual()));
    connect(ui.action_About,                SIGNAL(triggered()), this, SLOT(about()));

    // Toolbar Extras
    cbPort = new QComboBox(this);
    cbPort->setLayoutDirection(Qt::LeftToRight);
    cbPort->setToolTip(tr("Select Serial Port"));
    cbPort->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(cbPort,SIGNAL(currentIndexChanged(int)),this,SLOT(setCurrentPort(int)));
    ui.toolBar->addWidget(cbPort);

//    ctrlToolBar = addToolBar(tr("Control"));
//    ctrlToolBar->setLayoutDirection(Qt::RightToLeft);
//    ctrlToolBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
//    ctrlToolBar->addWidget(cbPort);

    updateRecentFileActions();

    /* start with an empty file if fresh install */
    connect(editorTabs,SIGNAL(fileUpdated(int)), this, SLOT(setProject()));
    connect(editorTabs,SIGNAL(sendMessage(const QString &)),this,SLOT(showMessage(const QString &)));
    editorTabs->newFile();

    resize(800,600);
    restoreGeometry(QSettings().value("windowSize").toByteArray());

    QApplication::processEvents();

    getApplicationSettings();

    /* get available ports at startup */
    enumeratePorts();

    portConnectionMonitor = new PortConnectionMonitor();
    connect(portConnectionMonitor, SIGNAL(portChanged()), this, SLOT(enumeratePorts()));

    connect(this,SIGNAL(signalStatusDone(bool)),this,SLOT(setStatusDone(bool)));

    loadSession();

    installEventFilter(this);

}

void MainWindow::loadSession()
{
    QSettings settings;
    int size = settings.beginReadArray("session");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        editorTabs->openFile(settings.value("file").toString());
    }
    settings.endArray();
}

void MainWindow::clearSession()
{
    QSettings settings;
    settings.beginGroup("session");
    settings.remove("");
    settings.endGroup();
}

void MainWindow::saveSession()
{
    clearSession();

    QSettings settings;
    settings.beginWriteArray("session");
    for (int i = 0; i < editorTabs->count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("file",editorTabs->tabToolTip(i));
    }
    settings.endArray();
}

void MainWindow::openFiles(const QStringList & files)
{
    for (int i = 0; i < files.size(); i++)
    {
        qDebug() << files.at(i);
        editorTabs->openFile(files.at(i));
    }
}

void MainWindow::getApplicationSettings()
{
    QSettings settings;
    settings.beginGroup("Paths");

    spinCompiler = settings.value("Compiler").toString();
    spinLoader = settings.value("Loader").toString();
    spinIncludes = settings.value("Library").toString();
    spinTerminal = settings.value("Terminal").toString();

    settings.endGroup();
}

void MainWindow::preferences()
{
    propDialog->showPreferences();
}


void MainWindow::preferencesAccepted()
{
    getApplicationSettings();
}


void MainWindow::fontBigger()
{
    propDialog->adjustFontSize(1.25);
}

void MainWindow::fontSmaller()
{
    propDialog->adjustFontSize(0.8);
}


void MainWindow::quitProgram()
{
    QCloseEvent e;
    closeEvent(&e);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    saveSession();
    editorTabs->closeAll();

    if (editorTabs->count())
    {
        if(e) e->ignore();
        return;
    }

    portConnectionMonitor->stop();

    QSettings().setValue("windowSize",saveGeometry());

    if(e) e->accept();
    qApp->exit(0);
}


void MainWindow::newProjectTrees()
{
    delete projectModel;
    projectModel = new TreeModel("", this);
    if(leftSplit->isVisible())
    {
        delete referenceModel;
        referenceModel = new TreeModel("", this);
    }
}


void MainWindow::addRecentFile(const QString &fileName)
{
    QStringList files = QSettings().value("recentFiles").toStringList();

    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > recentFiles.size())
        files.removeLast();

    QSettings().setValue("recentFiles", files);
    updateRecentFileActions();
}


void MainWindow::updateRecentFileActions()
{
    QStringList files = QSettings().value("recentFiles").toStringList();
    files.removeAll("");
    int numRecentFiles = qMin(files.size(), recentFiles.size());

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QString estr = files.at(i);
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(estr).fileName());
        recentFiles.at(i)->setText(text);
        recentFiles.at(i)->setData(estr);
        recentFiles.at(i)->setVisible(true);
    }
    for (int j = numRecentFiles; j < recentFiles.size(); ++j)
        recentFiles.at(j)->setVisible(false);

    if (!numRecentFiles)
    {
        recentFiles.at(0)->setText("No recent files...");
        recentFiles.at(0)->setVisible(true);
        recentFiles.at(0)->setEnabled(false);
    }
    else
    {
        recentFiles.at(0)->setEnabled(true);
    }
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        editorTabs->openFile(action->data().toString());
}

void MainWindow::printFile()
{
}

void MainWindow::setProject()
{
    int index = editorTabs->currentIndex();

    if(index < 0)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString fileName = editorTabs->tabToolTip(index);
    addRecentFile(fileName);

    if (editorTabs->count() > 0)
        setWindowTitle(editorTabs->tabText(index) + " - " +
                       QCoreApplication::applicationName());
    else
        setWindowTitle(QCoreApplication::applicationName());

    QString text = editorTabs->getEditor(index)->toPlainText();
    updateProjectTree(fileName);
    updateReferenceTree(fileName,text);

    QApplication::restoreOverrideCursor();
}

void MainWindow::showBrowser()
{
    if(leftSplit->isVisible()) {
        if(referenceModel) {
            delete referenceModel;
            referenceModel = NULL;
        }
        leftSplit->hide();
    }
    else {
        QString text;
        QString fileName;
        if(!projectModel || !referenceModel) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            int index = editorTabs->currentIndex();
            text = editorTabs->getEditor(index)->toPlainText();
            fileName = editorTabs->tabToolTip(index);
        }
        if(!referenceModel)
            updateReferenceTree(fileName,text);
        leftSplit->show();
        QApplication::processEvents();
        QApplication::restoreOverrideCursor();
    }
}

void MainWindow::setCurrentPort(int index)
{
    QString portName = cbPort->itemText(index);
    qDebug() << "Item text: " << portName;
    cbPort->setCurrentIndex(index);
}

void MainWindow::checkAndSaveFiles()
{
    if(projectModel == NULL)
        return;

    QString title = projectModel->getTreeName();

    for (int i = 0; i < editorTabs->count(); i++)
    {
        qDebug() << title;
        if (editorTabs->getEditor(i)->contentChanged())
        {
            editorTabs->save(i);
        }
    }

    int len = projectModel->rowCount();
    for (int n = 0; n < len; n++)
    {
        QModelIndex root = projectModel->index(n,0);
        QString name = projectModel->data(root, Qt::DisplayRole).toString();
        qDebug() <<  name;
        for(int i = 0; i < editorTabs->count(); i++)
        {
            if (editorTabs->getEditor(i)->contentChanged())
            {
                editorTabs->save(i);
            }
        }
    }
}

void MainWindow::highlightFileLine(QString file, int line)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    qDebug() << "Open file: " << file;
    QString name;
    if(QFile::exists(file)) {
        name = file;
    }
    else if (QFile::exists(QDir(QFileInfo(projectFile).path()).filePath(file)))
    {
        name = QDir(QFileInfo(projectFile).path()).filePath(file);
    }
    else if (QFile::exists(QDir(spinIncludes).filePath(file)))
    {
        name = QDir(spinIncludes).filePath(file);
    }
    else
    {
        return;
    }

    qDebug() << "Open name: " << name;
    qDebug() << "Open highlight!: " << name;
    editorTabs->openFile(name);
    Editor * editor = editorTabs->getEditor(editorTabs->currentIndex());
    
    if(editor)
    {
        QTextCursor cur = editor->textCursor();
        cur.movePosition(QTextCursor::Start);
        cur.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,line);
        cur.clearSelection();
        editor->setTextCursor(cur);
    }

    QApplication::processEvents();
    QApplication::restoreOverrideCursor();
}


int  MainWindow::runCompiler(COMPILE_TYPE type)
{
    builder.show();

    QString copts;
    int rc = -1;

    int index;
    QString fileName;
    QString text;

    if(!statusDone) {
        return -1;
    }

    emit signalStatusDone(false);

    if(!projectModel)
        return 1;

    if(!editorTabs->count())
        return 1;

    // don't allow if no port available
    if(type != COMPILE_ONLY && cbPort->count() < 1) {
        QMessageBox mbox(QMessageBox::Critical, tr("No Serial Port"),
                tr("Serial port not available.")+" "+tr("Connect a USB Propeller board, turn it on, and try again."),
                QMessageBox::Ok);
        mbox.exec();
        return 1;
    }

    index = editorTabs->currentIndex();
    fileName = editorTabs->tabToolTip(index);
    text = editorTabs->getEditor(index)->toPlainText();

    updateProjectTree(fileName);
    updateReferenceTree(fileName,text);

    getApplicationSettings();

    checkAndSaveFiles();

    if(fileName.contains(".spin")) {
        builder.setParameters(spinCompiler, spinLoader, spinIncludes, projectFile);

        copts = "-b";
        rc = builder.runCompiler(copts);
    }
    else 
    {
        QMessageBox::critical(this,tr("Can't compile unknown file type"), tr("Files must be of type '.spin'"));
    }

    emit signalStatusDone(true);
    return rc;
}

void MainWindow::programBuild()
{
    runCompiler(COMPILE_ONLY);
}

int  MainWindow::loadProgram(int type)
{
    int rc = -1;
    QString copts;

    // if find in progress, ignore request
    if(!statusDone) {
        return -1;
    }
    emit signalStatusDone(false);

    if(cbPort->currentText().length() == 0)
    {
        QMessageBox::critical(this,tr("Propeller Load"), tr("Port not available. Please connect Propeller board."), QMessageBox::Ok);
        goto endLoadProgram;
    }

    switch (type) {
        case MainWindow::LoadRunHubRam:
            copts += "-r -p"+cbPort->currentText();
            rc = builder.loadProgram(copts);
            break;
        case MainWindow::LoadRunEeprom:
            copts += "-e -p"+cbPort->currentText();
            rc = builder.loadProgram(copts);
            break;
        default:
            break;
    }

endLoadProgram:
    emit signalStatusDone(true);
    return rc;
}

void MainWindow::programBurnEE()
{
    if(runCompiler(COMPILE_BURN))
        return;

    setCurrentPort(cbPort->currentIndex());

    loadProgram(MainWindow::LoadRunEeprom);
}

void MainWindow::programRun()
{
    if(runCompiler(COMPILE_RUN))
        return;

    setCurrentPort(cbPort->currentIndex());

    loadProgram(MainWindow::LoadRunHubRam);
}

void MainWindow::programDebug()
{
    if(runCompiler(COMPILE_RUN))
        return;

    setCurrentPort(cbPort->currentIndex());

    if(!loadProgram(MainWindow::LoadRunHubRam))
    {
        spawnTerminal();
    }
}


void MainWindow::setStatusDone(bool done)
{
    statusMutex.lock();
    statusDone = done;
    statusMutex.unlock();
}

void MainWindow::viewInfo()
{


}

void MainWindow::findMultilineComment(QPoint point)
{
    Editor *editor = editorTabs->getEditor(editorTabs->currentIndex());
    QTextCursor cur = editor->cursorForPosition(point);
    findMultilineComment(cur);
}

void MainWindow::findMultilineComment(QTextCursor cur)
{
    QRegExp commentStartExpression = QRegExp("{*",Qt::CaseInsensitive,QRegExp::Wildcard);
    QRegExp commentEndExpression = QRegExp("}*",Qt::CaseInsensitive,QRegExp::Wildcard);

    Editor *editor = editorTabs->getEditor(editorTabs->currentIndex());
    if(editor)
    {
        QString text = cur.selectedText();

        if(text.length() == 0) {
            cur.select(QTextCursor::WordUnderCursor);
            text = cur.selectedText();
        }

        int selectedWordLength = text.length();

        if(text.length() > 0 && (commentStartExpression.indexIn(text, 0)!= -1 ))
        {
            int startPos = cur.position();
            cur.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
            editor->setTextCursor(cur);
            text = cur.selectedText();

            int endIndex;
            if ((endIndex = commentEndExpression.indexIn(text, 0)) != -1 )
            {
                cur.setPosition(startPos - selectedWordLength);
                cur.setPosition(endIndex + startPos, QTextCursor::KeepAnchor);
                editor->setTextCursor(cur);
                editor->setFocus();
            }
        }
        else if(text.length() > 0 && (commentEndExpression.indexIn(text, 0)!= -1 ))
        {
            int endPos = cur.position();
            cur.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
            editor->setTextCursor(cur);
            text = cur.selectedText();

            int startIndex;
            if ((startIndex = commentStartExpression.lastIndexIn(text, -1)) != -1)
            {
                cur.setPosition(endPos);
                cur.setPosition(startIndex - selectedWordLength + 1, QTextCursor::KeepAnchor);
                editor->setTextCursor(cur);
                editor->setFocus();
            }
        }
    }
    return;
}

void MainWindow::openTreeFile(QString fileName)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    fileName = fileName.trimmed();
    qDebug() << "openTreeFile opening " << fileName;
    QString userFile = editorTabs->tabToolTip(editorTabs->currentIndex());
    QString userPath = QFileInfo(userFile).path();
    QFile file;
    QString fileToOpen;

    if (file.exists(QDir(userPath).filePath(fileName)))
    {
        fileToOpen = QDir(userPath).filePath(fileName);
        qDebug() << "File in " << userPath;
    }
    else if (file.exists(QDir(spinIncludes).filePath(fileName)))
    {
        fileToOpen = QDir(spinIncludes).filePath(fileName);
        qDebug() << "File in " << spinIncludes;
    }
    else
    {
        qDebug() << "File not found!";
    }

    editorTabs->openFile(fileToOpen);

    QApplication::restoreOverrideCursor();
}

void MainWindow::projectTreeClicked(QModelIndex index)
{
    if(projectModel == NULL)
        return;
    QString fileName;
    QVariant vs = projectModel->data(index, Qt::DisplayRole);
    if(vs.canConvert(QVariant::String)){
        fileName = vs.toString();
    }
    if(fileName.length() > 0) {
        openTreeFile(fileName);
    }
}

void MainWindow::referenceTreeClicked(QModelIndex index)
{
    QString method = NULL;
    QString myFile;

    QVariant vs = referenceModel->data(index, Qt::DisplayRole);
    if(vs.canConvert(QVariant::String)) {
        method = QString(vs.toString());
        method = method.trimmed();
    }

    myFile = referenceModel->file(index);
    if(myFile.length() < 1) {
        qDebug() << "referenceModel did not have a file.";
        return;
    }
    if(referenceModel->hashCount() == 0) {
        qDebug() << "referenceModel did not find any symbols.";
        return;
    }
    if(method.length() > 0 && method.indexOf(QRegExp("pub|pri",Qt::CaseInsensitive)) < 0) {
        openTreeFile(method);
    }
    else {
        int num;
        QString file = referenceModel->getSymbolInfo(myFile+"::"+method.trimmed(), &num);
        if(file.length() > 0) {
            qDebug() << "referenceTreeClicked looks for " << method << "on line #" << num;
            qDebug() << "referenceTreeClicked opening " << file;

            highlightFileLine(file, num);
        }
    }
}

void MainWindow::zipFiles()
{
    int n = this->editorTabs->currentIndex();
    QString fileName = editorTabs->tabToolTip(n);

    if (fileName.isEmpty())
        return;

    QString spinLibPath     = QSettings().value("Library").toString();
    QStringList fileTree    = editorTabs->getEditor(
            editorTabs->currentIndex()
            )->spinParser.spinFileTree(fileName, spinLibPath);
    if(fileTree.count() > 0)
    {
        zipper.makeZip(fileName, fileTree, spinLibPath);
    }
}

void MainWindow::updateProjectTree(QString fileName)
{
    projectFile = fileName;
    QString s = QFileInfo(fileName).fileName();

    if(projectModel != NULL) {
        delete projectModel;
    }
    if(fileName.endsWith(".spin",Qt::CaseInsensitive)) {
        projectModel = new TreeModel(s, this);
        updateSpinProjectTree(fileName);
    }
    else {
        projectModel = new TreeModel(s, this);
    }
    projectTree->setWindowTitle(s);
    projectTree->setModel(projectModel);
    projectTree->show();

}

void MainWindow::updateSpinProjectTree(QString fileName)
{
    /* for spin we always parse the program and stuff the file list */
    QStringList flist = editorTabs->getEditor(editorTabs->currentIndex())->spinParser.spinFileTree(fileName, QSettings().value("Library").toString());

    foreach (QString s, flist)
    {
        projectModel->addRootItem(s);
    }
}

void MainWindow::updateReferenceTree(QString fileName, QString text)
{
    QString s = QFileInfo(fileName).fileName();

    // our startup strategy should change so that we have a tree and it starts collapsed.

    if(referenceModel != NULL) {
        delete referenceModel;
    }
    if(fileName.endsWith(".spin",Qt::CaseInsensitive)) {
        referenceModel = new TreeModel(s, this);
        if(text.length() > 0) {
            updateSpinReferenceTree(fileName, spinIncludes, "", 0); // start at top object
        }
    }
    else {
        referenceModel = new TreeModel(s, this);
    }

    referenceTree->setWindowTitle(s);
    referenceTree->setModel(referenceModel);
    referenceTree->show();
}

void MainWindow::updateSpinReferenceTree(QString fileName, QString includes, QString objname, int level)
{
    QString path = QFileInfo(fileName).path();

    QStringList mlist = editorTabs->getEditor(
            editorTabs->currentIndex())->spinParser.spinMethods(fileName,  objname);

    // move objects to end of the list
    for (int n = mlist.count()-1; n > -1; n--)
    {
        if(mlist[n].at(0) == 'o')
        {
            mlist.insert(mlist.count(),mlist[n]);
            mlist.removeAt(n);
        }
    }

    // display all
    for (int n = 0; n < mlist.count(); n ++)
    {
        QString s = mlist[n];

        if (s.at(0) == 'o')
        {

            /* get the file name */
            QString file = s.mid(s.indexOf(":")+1);
            file = file.mid(0, file.indexOf("\t"));
            file = file.trimmed();

            if (file.startsWith("\""))
                file = file.mid(1);

            if (file.endsWith("\""))
                file = file.mid(0,file.length()-1);

            file = file.trimmed();

            if (!file.endsWith(".spin",Qt::CaseInsensitive))
                file += ".spin";

            /* prepend path info to new file if found*/
            if (QFile::exists(file))
            {
                referenceModel->addRootItem(file, file);
            }
            else if(QFile::exists(QDir(path).filePath(file)))
            {
                referenceModel->addRootItem(file, QDir(path).filePath(file));
                file = QDir(path).filePath(file);
            }
            else if(QFile::exists(QDir(includes).filePath(file)))
            {
                referenceModel->addRootItem(file, QDir(includes).filePath(file));
                file = QDir(includes).filePath(file);
            }
            else
            {
                qDebug() << "updateSpinReferenceTree can't find file" << file;
            }

            /* get the object name */
            QString obj = s.mid(s.indexOf("\t")+1);
            obj = obj.mid(0,obj.indexOf(":"));
            obj = obj.trimmed();

            updateSpinReferenceTree(file, includes, obj, level+1);
        }
        else
        {
            int line = 0;
            QString sl = s.mid(s.lastIndexOf("\t")+1);
            line = sl.toInt();
            s = s.mid(s.indexOf("\t")+1);
            s = s.mid(0,s.indexOf("\t"));
            if(s.indexOf(":") != -1)
                s = s.mid(0, s.indexOf(":"));
            if(s.indexOf("|") != -1)
                s = s.mid(0, s.indexOf("|"));
            s = s.trimmed();

            referenceModel->addSymbolInfo(s, fileName, line);

            for(int n = 0; n < level; n++)
                s = "    "+s;
            //methods.append(s);
            referenceModel->addRootItem(s, fileName);
        }
    }
}

void MainWindow::enumeratePorts()
{
    if(cbPort == NULL)
        return;

    cbPort->clear();

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    foreach(QSerialPortInfo port, ports)
    {
//        qDebug() << port.systemLocation();
        if (!port.systemLocation().contains("ttyS"))
            cbPort->addItem(port.systemLocation());
    }
    if(cbPort->count())
    {
        cbPort->setEnabled(true);
        ui.actionTerminal->setEnabled(true);
    }
    else
    {
        cbPort->setEnabled(false);
        ui.actionTerminal->setEnabled(false);
    }
}

void MainWindow::spawnTerminal()
{
    QProcess * termtime = new QProcess();
    termtime->start(spinTerminal);
}

void MainWindow::setupProjectTools(QSplitter *vsplit)
{
    int handlewidth = 8;

    // container for project, etc...
    leftSplit = new QSplitter(this);
    leftSplit->setOrientation(Qt::Vertical);
    leftSplit->setChildrenCollapsible(false);
    leftSplit->setHandleWidth(handlewidth);
    vsplit->addWidget(leftSplit);

    // project tree
    projectTree = new ReferenceTree(tr("Current Project"),ColorScheme::ConBG);
    connect(projectTree,SIGNAL(clicked(QModelIndex)),this,SLOT(projectTreeClicked(QModelIndex)));

    leftSplit->addWidget(projectTree);

    // project reference tree
    referenceTree = new ReferenceTree(tr("Project References"), ColorScheme::PubBG);
    connect(referenceTree,SIGNAL(clicked(QModelIndex)),this,SLOT(referenceTreeClicked(QModelIndex)));

    connect(propDialog,SIGNAL(updateColors()),referenceTree,SLOT(updateColors()));
    connect(propDialog,SIGNAL(updateColors()),projectTree,SLOT(updateColors()));

    connect(propDialog,SIGNAL(updateFonts()),referenceTree,SLOT(updateFonts()));
    connect(propDialog,SIGNAL(updateFonts()),projectTree,SLOT(updateFonts()));

    leftSplit->addWidget(referenceTree);


    projectTree->updateColors();
    referenceTree->updateColors();

    projectTree->updateFonts();
    referenceTree->updateFonts();


    leftSplit->setStretchFactor(0,1);
    leftSplit->setStretchFactor(1,2);

    findSplit = new QSplitter(this);
    findSplit->setOrientation(Qt::Vertical);
    vsplit->addWidget(findSplit);

    // project editor tabs
    editorTabs = new FileManager(this);
    connect(editorTabs,SIGNAL(tabCloseRequested(int)),editorTabs,SLOT(closeFile(int)));
    connect(editorTabs,SIGNAL(currentChanged(int)),editorTabs,SLOT(changeTab(int)));
    findSplit->addWidget(editorTabs);

    findSplit->setStretchFactor(0,1);
    findSplit->setStretchFactor(1,0);
    findSplit->setContentsMargins(0,0,handlewidth,0);
    findSplit->addWidget(newFindFrame(findSplit));
    QSplitterHandle *hndl = findSplit->handle(1);
    hndl->setEnabled(false);


    QList<int> vsizes;
    vsizes.append(130);
    vsizes.append(550);
    vsplit->setSizes(vsizes);
    vsplit->setLineWidth(handlewidth*1/2);
    vsplit->setHandleWidth(handlewidth);
    vsplit->setChildrenCollapsible(true);

}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *e = static_cast<QKeyEvent *>(event);

        if (e->modifiers() & Qt::ControlModifier)
        {
            switch (e->key())
            {
            case (Qt::Key_T):
                editorTabs->newFile();
                return true;
            case (Qt::Key_W):
                editorTabs->closeFile();
                return true;
            case (Qt::Key_PageUp):
                editorTabs->previousTab();
                return true;
            case (Qt::Key_PageDown):
                editorTabs->nextTab();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(target, event);
}

void MainWindow::openFileResource(QString const & resource)
{
    QString path = QApplication::applicationDirPath()
            + QString(APP_RESOURCES_PATH)
            + resource;
    if (QFileInfo(path).exists() && QFileInfo(path).isFile())
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    else
        showMessage(tr("File %1 not found...").arg(path));
}

void MainWindow::propellerManual()
{
    openFileResource("/doc/pdf/P8X32A-Web-PropellerManual-v1.2_0.pdf");
}

void MainWindow::propellerDatasheet()
{
    openFileResource("/doc/pdf/P8X32A-Propeller-Datasheet-v1.4.0_0.pdf");
}

void MainWindow::about()
{
    QString version = QString(QCoreApplication::applicationName() 
                     + " v" + QCoreApplication::applicationVersion()
                     );
    QMessageBox::about(this, tr("About") + " " + QCoreApplication::applicationName(),
           "<h2>" + version + "</h2>"
           "<p>PropellerIDE is an easy-to-use, cross-platform development tool for the Parallax Propeller microcontroller.</p>"
           "<p>Use it for writing Spin code, downloading programs to your Propeller board, and debugging your applications with the built-in serial terminal.<p>"
           "<p>PropellerIDE is built in Qt and is fully cross-platform.</p>"

           "<h3>Credits</h3>"
           "<p>Copyright &copy; 2014-2015 by Parallax, Inc. "
           "Developed by LameStation LLC in collaboration with Parallax. Originally created by Steve Denson.</p>"
           "<p>PropellerIDE is free software, released under the GPLv3 license.</p>"
           );
}

void MainWindow::showMessage(const QString & message)
{
    statusBar()->showMessage(message, 2000);
}
