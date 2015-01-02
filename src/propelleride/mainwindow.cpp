#include "mainwindow.h"

#include <QMessageBox> 
#include <QStatusBar> 
#include <QToolBar> 
#include <QFileDialog> 
#include <QMenu> 


#include "StatusDialog.h"
#include "qext/qextserialenumerator.h"

#define AUTOPORT "AUTO"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), statusMutex(QMutex::Recursive), statusDone(true)
{
}

void MainWindow::init()
{
    /* the program name used in all windows/diaglogs */
    programName = QString(PropellerIdeGuiKey);

    /* setup application registry info */
    QCoreApplication::setOrganizationName(publisherKey);
    QCoreApplication::setOrganizationDomain(publisherComKey);
    QCoreApplication::setApplicationName(PropellerIdeGuiKey);

    /* setup preferences dialog */
    propDialog = new Preferences(this);
    connect(propDialog,SIGNAL(accepted()),this,SLOT(preferencesAccepted()));

    projectModel = NULL;
    referenceModel = NULL;

    spinBuilder = new SpinBuilder();

    connect(spinBuilder,SIGNAL(compilerErrorInfo(QString,int)), this, SLOT(highlightFileLine(QString,int)));

    /* setup gui components */
    setupFileMenu();
    setupEditMenu();
    setupViewMenu();
    setupProjectMenu();
    setupHelpMenu();

    setupToolBars();

    /* main container */
    setWindowTitle(programName);
    QSplitter *vsplit = new QSplitter(this);
    setCentralWidget(vsplit);

    /* minimum window height */
    this->setMinimumHeight(500);

    /* project tools */
    setupProjectTools(vsplit);

    /* start with an empty file if fresh install */
    newFile();

    /* status bar for progressbar */
    QStatusBar *statusBar = new QStatusBar();

    sizeLabel = new QLabel;
    sizeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    msgLabel  = new QLabel;

    progress = new QProgressBar();
    //progress->setMaximumSize(100,25);
    progress->setValue(0);
    progress->setMinimum(0);
    progress->setMaximum(100);
    progress->setTextVisible(false);
    progress->setVisible(false);

    statusBar->addPermanentWidget(msgLabel,70);
    statusBar->addPermanentWidget(sizeLabel,15);
    statusBar->addPermanentWidget(progress,15);

    this->setStatusBar(statusBar);

    /* get last geometry. using x,y,w,h is unreliable.
    */
    QVariant geov = QSettings().value(mainWindowGeometry);
    if(geov.canConvert(QVariant::ByteArray)) {
        // byte array convert is always possible
        QByteArray geo = geov.toByteArray();
        // restoreGeometry makes sure the array is valid
        this->restoreGeometry(geo);
    }
    else {
        this->resize(800,600);
    }

    QApplication::processEvents();

    /* Get app settings at startup and before any compiler call.
     * Some people want to be prepared, others just want to hack
     * their way through life ... i guess it's fun.
     * The latter is most common, so let's do it like that.
     * In this case, we need the preferences if their set, but we can't complain.
     */
    getApplicationSettings(false);

    /* setup the terminal dialog box */
    term = new Terminal(this);

    QVariant gv = QSettings().value(termGeometryKey);
    if(gv.canConvert(QVariant::ByteArray)) {
        QByteArray geo = gv.toByteArray();
        term->restoreGeometry(geo);
    }

    /* setup the port listener */
    portListener = new PortListener(this, term->getEditor());
    portListener->setTerminalWindow(term->getEditor());
    connect(spinBuilder,SIGNAL(terminalReceived(QString)), portListener, SLOT(appendConsole(QString)));

    term->setPortListener(portListener);

    //term->setWindowTitle(QString(ASideGuiKey)+" "+tr("Simple Terminal"));
    // education request that the window title be SimpleIDE Terminal
    term->setWindowTitle(QString(PropellerIdeGuiKey)+" "+tr("Terminal"));

    connect(term,SIGNAL(accepted()),this,SLOT(terminalClosed()));
    connect(term,SIGNAL(rejected()),this,SLOT(terminalClosed()));

    /* get available ports at startup */
    enumeratePorts();

    portConnectionMonitor = new PortConnectionMonitor();
    connect(portConnectionMonitor, SIGNAL(portChanged()), this, SLOT(enumeratePortsEvent()));

    /* Do this before using the dialog. */
    statusDialog = new StatusDialog(this);
    portListener->setStatusDialog(statusDialog);

    connect(this,SIGNAL(signalStatusDone(bool)),this,SLOT(setStatusDone(bool)));

    // Show GUI now
    QApplication::processEvents();
    this->show();

    openLastFile();
}

int  MainWindow::isPackageSource(QString fileName)
{
    QString dir = QFileInfo(fileName).absolutePath();
    QString file= dir+"/tmp_"+QString("%1").arg(QDateTime::currentDateTime().toTime_t());

    bool fw = QFileInfo(fileName).isWritable();
    bool pw = false;

    QFile wf(file);
    if(wf.open(QFile::WriteOnly)) {
        wf.close();
        pw = true;
        wf.remove();
    }
    return fw && pw;
}

int  MainWindow::extractSource(QString &fileName)
{
    fileName = this->saveAsFile();

    if(fileName.length() > 0) {
        return isPackageSource(fileName);
    }
    return 0;
}

// call from main ?
void MainWindow::openLastFile()
{
    /* load the last file into the editor to make user happy */
    QString welcome = propDialog->getSpinLibraryString()+"Welcome.spin";
    QString fileName;
    QVariant lastfilev = QSettings().value(lastFileNameKey, welcome);
    if(!lastfilev.isNull()) {
        if(lastfilev.canConvert(QVariant::String)) {
            if(lastfilev.toString().length() > 0) {
                fileName = lastfilev.toString();
            }
            openFileName(fileName);
            QApplication::processEvents();

            if(fileName.compare(welcome) == 0) {
                QMessageBox::information(this,tr("Welcome to PropellerIDE"),
                        tr("The Welcome.spin file must be saved to a user folder where you can change it.")+" "+
                        tr("The installed package location is not writable by most users.")+"\n\n"+
                        tr("Please note that opening a library file from the installed package will also require saving to a user folder for compiling or modifications.")+" "+
                        tr("Don't worry, you will be reminded if necessary.")+"\n\n"+
                        tr("The Save As dialog will now open to let you choose a working folder."));
                lastDirectory = QDir::homePath()+"/Documents";
                if(!QFile::exists(lastDirectory)) {
                    lastDirectory = QDir::homePath();
                }
                fileName = saveAsFile();
            }
        }
    }

    /* load the last directory to make user happy */
    lastDirectory = filePathName(fileName);
    QVariant lastdirv = QSettings().value(lastDirectoryKey, lastDirectory);
    if(!lastdirv.isNull()) {
        if(lastdirv.canConvert(QVariant::String)) {
            if(lastdirv.toString().length() > 0) {
                lastDirectory = lastdirv.toString();
            }
        }
    }
}

void MainWindow::getApplicationSettings(bool complain)
{
    QDir dir(this->lastDirectory);
    QFile file;
    QVariant compv;

    compv = QSettings().value("Compiler");
    if(compv.canConvert(QVariant::String)) {
        spinCompiler = compv.toString();
        spinCompiler = dir.fromNativeSeparators(spinCompiler);
    }
    if(complain && !file.exists(spinCompiler)) {
        propDialog->showPreferences();
    }
    compv = QSettings().value("Loader");
    if(compv.canConvert(QVariant::String)) {
        spinLoader = compv.toString();
        spinLoader = dir.fromNativeSeparators(spinLoader);
    }

    /* get the compiler path */
    spinCompilerPath = filePathName(spinCompiler);
    spinCompilerPath = dir.fromNativeSeparators(spinCompilerPath);

    /* get the include path and config file set by user */
    QVariant incv;

    incv = QSettings().value("Library Path");
    if(incv.canConvert(QVariant::String)) {
        spinIncludes = incv.toString();
        spinIncludes = dir.fromNativeSeparators(spinIncludes);
    }
    qDebug() << spinIncludes;
}

bool MainWindow::exitSave()
{
    bool saveAll = false;
    QMessageBox mbox(QMessageBox::Question, "Save File?", "",
            QMessageBox::Discard | QMessageBox::Save | QMessageBox::SaveAll | QMessageBox::Cancel, this);

    for(int tab = editorTabs->count()-1; tab > -1; tab--)
    {
        QString tabName = editorTabs->tabText(tab);
        if(tabName.at(tabName.length()-1) == '*')
        {
            mbox.setInformativeText(tr("Save File: ") + tabName.mid(0,tabName.indexOf(" *")) + tr(" ?"));
            if(saveAll)
            {
                saveFileByTabIndex(tab);
            }
            else
            {
                int ret = mbox.exec();
                switch (ret) {
                    case QMessageBox::Cancel:
                        return false;
                        break;
                    case QMessageBox::Discard:
                        // Don't Save was clicked
                        return true;
                        break;
                    case QMessageBox::Save:
                        // Save was clicked
                        saveFileByTabIndex(tab);
                        break;
                    case QMessageBox::SaveAll:
                        // save all was clicked
                        saveAll = true;
                        break;
                    default:
                        // should never be reached
                        break;
                }
            }
        }
    }
    return true;
}

void MainWindow::quitProgram()
{
    QCloseEvent e;
    closeEvent(&e);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if(!exitSave()) {
        if(e) e->ignore();
        return;
    }

    portListener->close();
    portConnectionMonitor->stop();

    QString filestr = editorTabs->tabToolTip(editorTabs->currentIndex());

    if(QSettings().value(useKeys).toInt())
    {
        QSettings().setValue(lastFileNameKey,filestr);
        QSettings().setValue(lastDirectoryKey, lastDirectory);

        // save user's width/height
        QByteArray geo = this->saveGeometry();
        QSettings().setValue(mainWindowGeometry,geo);
    }
    if(e) e->accept();
    qApp->exit(0);
}

void MainWindow::newFileAction()
{
    newFile();
    int tab = editorTabs->count()-1;
    setEditorCodeType(getEditor(tab), QString("UTF-16"));
}

void MainWindow::newFile()
{
    changeTabDisable = true;
    fileChangeDisable = true;
    editorTabs->addTab(createEditor(),(const QString&)untitledstr);
    int tab = editorTabs->count()-1;
    editorTabs->setCurrentIndex(tab);
    editorTabs->setTabToolTip(tab, QString(""));
    delete projectModel;
    projectModel = new TreeModel("", this);
    if(leftSplit->isVisible()) {
        delete referenceModel;
        referenceModel = new TreeModel("", this);
    }
    spinParser.clearDB();
    fileChangeDisable = false;
    changeTabDisable = false;
}

void MainWindow::openFile(const QString &path)
{
    QString fileName = path;

    if (fileName.isNull())
        fileName = QFileDialog::getOpenFileName(this,
                tr("Open File"), lastDirectory, "Spin Files (*.spin);;All Files (*)");
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    openFileName(fileName);
    lastDirectory = filePathName(fileName);
    QApplication::processEvents();
    QApplication::restoreOverrideCursor();
}

bool MainWindow::isFileUTF16(QFile *file)
{
    char str[2];
    file->read(str,2);
    file->seek(0);
    if(str[0] == -1 && str[1] == -2) {
        return true;
    }
    return false;
}

void MainWindow::openFileName(QString fileName)
{
    QString data;
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly))
        {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            QTextStream in(&file);
            if(this->isFileUTF16(&file)) {
                in.setCodec("UTF-16");
            }
            else {
                in.setCodec("UTF-8");
            }
            data = in.readAll();
            file.close();

            QApplication::processEvents();
            QApplication::restoreOverrideCursor();

            QString sname = this->shortFileName(fileName);
            if(editorTabs->count()>0) {
                for(int n = editorTabs->count()-1; n > -1; n--) {
                    if(editorTabs->tabText(n) == sname) {
                        setEditor(n, sname, fileName, data);
                        setEditorCodeType(getEditor(n), QString(in.codec()->name()));
                        setProject();
                        return;
                    }
                }
            }
            if(editorTabs->tabText(0) == untitledstr) {
                setEditor(0, sname, fileName, data);
                setEditorCodeType(getEditor(0), QString(in.codec()->name()));
                setProject();
                return;
            }
            newFile();
            int tab = editorTabs->count()-1;
            setEditor(tab, sname, fileName, data);
            setEditorCodeType(getEditor(tab), QString(in.codec()->name()));
            setProject();
        }
    }
}

bool MainWindow::saveAsCodec(QString fileName, Editor *ed)
{
    QFile file(fileName);
    if (file.open(QFile::WriteOnly)) {
        QTextStream os(&file);
        QString data = ed->toPlainText();

        setCurrentFile(fileName);

        if(ed->getCodeType() == Editor::CodeTypeUTF16) {
            os.setGenerateByteOrderMark(true);
            os.setCodec("UTF-16");
            os << data;
        }
        else {
            os.setCodec("UTF-8");
            os << data;
        }
        file.close();
        QString tab = editorTabs->tabText(editorTabs->currentIndex());
        if(tab.endsWith('*')) {
            tab = tab.mid(0, tab.length()-1);
            tab = tab.trimmed();
            editorTabs->setTabText(editorTabs->currentIndex(),tab);
        }
        return true;
    }
    else {
        QMessageBox::information(this, tr("Can't Save File"), tr("Can't save file ")+fileName, QMessageBox::Ok);
    }
    return false;
}

void MainWindow::saveFile()
{
    try {
        int n = this->editorTabs->currentIndex();
        QString fileName = editorTabs->tabToolTip(n);
        if (fileName.isEmpty()) {
            saveAsFile(fileName);
            if (fileName.isEmpty())
                return;
        }
        Editor *ed = getEditor(n);
        this->editorTabs->setTabText(n,shortFileName(fileName));

        if (!fileName.isEmpty()) {
            if(saveAsCodec(fileName, ed))
                setProject();
        }
    } catch(...) {
    }
}

void MainWindow::saveFileByTabIndex(int tab)
{
    try {
        QString fileName = editorTabs->tabToolTip(tab);

        this->editorTabs->setTabText(tab,shortFileName(fileName));
        if (!fileName.isEmpty()) {
            saveAsCodec(fileName, getEditor(tab));
        }
    } catch(...) {
    }
}

QString MainWindow::saveAsFile(const QString &path)
{
    QString fileName = path;
    try {
        int n = this->editorTabs->currentIndex();

        if (fileName.isEmpty())
            fileName = QFileDialog::getSaveFileName(this,
                    tr("Save As File"), lastDirectory+"/"+this->editorTabs->tabText(n), "Spin Files (*.spin)");

        if (fileName.isEmpty()) {
            return QString();
        }
        lastDirectory = filePathName(fileName);

        this->editorTabs->setTabText(n,shortFileName(fileName));
        editorTabs->setTabToolTip(n,fileName);

        if (!fileName.isEmpty()) {
            if(saveAsCodec(fileName, getEditor(n)))
                setProject();
        }
    } catch(...) {
        return QString();
    }
    return fileName;
}

void MainWindow::fileChanged()
{
    if(fileChangeDisable)
        return;
    int index = editorTabs->currentIndex();
    QString name = editorTabs->tabText(index);
    QString fileName = editorTabs->tabToolTip(index);
    if(!QFile::exists(fileName))
        return;
    QFile file(fileName);
    if(file.open(QFile::ReadOnly))
    {
        QString curt = getEditor(index)->toPlainText();
        QString text;
        QTextStream in(&file);
        if(this->isFileUTF16(&file)) {
            in.setCodec("UTF-16");
        }
        else {
            in.setCodec("UTF-8");
        }
        text = in.readAll();
        file.close();

        if(text == curt) {
            if(name.at(name.length()-1) == '*')
                editorTabs->setTabText(index, this->shortFileName(fileName));
            return;
        }
    }
    if(!name.endsWith('*')) {
        name += tr(" *");
        editorTabs->setTabText(index, name);
    }
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    QStringList files = QSettings().value(recentFilesKey).toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    QSettings().setValue(recentFilesKey, files);

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
            mainWin->updateRecentFileActions();
    }
}

void MainWindow::updateRecentFileActions()
{
    QStringList files = QSettings().value(recentFilesKey).toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString estr = files.at(i);
        if(estr.length() == 0)
            continue;
        //QString filename = QFileInfo(projects[i]).fileName();
        QString text = tr("&%1 %2").arg(i + 1).arg(estr);
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(estr);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    separatorFileAct->setVisible(numRecentFiles > 0);
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        openFile(action->data().toString());
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
    setCurrentFile(fileName);

    setWindowTitle(programName +" "+fileName);
    sizeLabel->setText("");
    msgLabel->setText("");

    QString text = getEditor(index)->toPlainText();
    updateProjectTree(fileName);
    if(leftSplit->isVisible()) {
        updateReferenceTree(fileName,text);
    }

    QApplication::processEvents();
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
            text = getEditor(index)->toPlainText();
            fileName = editorTabs->tabToolTip(index);
        }
        if(!referenceModel)
            updateReferenceTree(fileName,text);
        leftSplit->show();
        QApplication::processEvents();
        QApplication::restoreOverrideCursor();
    }
}


void MainWindow::fontBigger()
{
    propDialog->adjustFontSize(1.25);
}

void MainWindow::fontSmaller()
{
    propDialog->adjustFontSize(0.8);
}

void MainWindow::preferences()
{
    propDialog->showPreferences();
}

void MainWindow::preferencesAccepted()
{
    /* set preferences */
    getApplicationSettings(false);
}

void MainWindow::setCurrentPort(int index)
{
    QString portName = cbPort->itemText(index);
    term->setPortName(portName);
    cbPort->setCurrentIndex(index);
    if(portName.length()) {
        portListener->init(portName, term->getBaud());  // signals get hooked up internally
#ifdef Q_OS_WIN
        // do this so that the handle will change for windows
        if(portListener->isOpen()) {
            portListener->close();
            portListener->open();
        }
#endif
    }
}

void MainWindow::checkAndSaveFiles()
{
    if(projectModel == NULL)
        return;
    QString title = projectModel->getTreeName();
    QString modTitle = title + " *";
    for(int tab = editorTabs->count()-1; tab > -1; tab--)
    {
        QString tabName = editorTabs->tabText(tab);
        if(tabName == modTitle)
        {
            saveFileByTabIndex(tab);
            editorTabs->setTabText(tab,title);
        }
    }

    int len = projectModel->rowCount();
    for(int n = 0; n < len; n++)
    {
        QModelIndex root = projectModel->index(n,0);
        QVariant vs = projectModel->data(root, Qt::DisplayRole);
        if(!vs.canConvert(QVariant::String))
            continue;
        QString name = vs.toString();
        QString modName = name + " *";
        for(int tab = editorTabs->count()-1; tab > -1; tab--)
        {
            QString tabName = editorTabs->tabText(tab);
            if(tabName == modName)
            {
                saveFileByTabIndex(tab);
                editorTabs->setTabText(tab,name);
            }
        }
    }
}

void MainWindow::highlightFileLine(QString file, int line)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString name;
    if(QFile::exists(file)) {
        name = file;
    }
    else if(QFile::exists(filePathName(projectFile)+"/"+file)) {
        name = filePathName(projectFile)+"/"+file;
    }
    else if(QFile::exists(spinIncludes+"/"+file)) {
        name = spinIncludes+"/"+file;
    }
    else {
        return;
    }

    Editor *editor = NULL;
    int len = editorTabs->count();
    /* open file in tab if not there already */
    for(int n = 0; n < len; n++) {
        QString s = editorTabs->tabToolTip(n);
        if(s.length() && s.compare(name) == 0) {
            editor = getEditor(n);
            editorTabs->setCurrentIndex(n);
            break;
        }
    }
    if(editor == NULL) {
        openFileName(name);
        editor = getEditor(editorTabs->currentIndex());
    }
    if(editor != NULL)
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

void MainWindow::buildSourceWriteError(QString fileName)
{
    QMessageBox::critical(this, tr("Build Problem"),
            "\n"+tr("Problem building:")+" "+this->shortFileName(fileName)+"\n\n"+
            tr("Can't write the file or folder.")+" "+
            tr("This can happen when compiling a packaged file.")+"\n\n"+
            tr("The code must be saved to a folder with write permissions.")+" "+
            tr("A good location would be the user's Documents folder or a folder with other sources.")+"\n\n"+
            tr("Please save to a different file and/or folder when prompted."));
}


int  MainWindow::runCompiler(COMPILE_TYPE type)
{
    QString copts;
    int rc = -1;

    int index;
    QString fileName;
    QString text;

    // if find in progress, ignore request
    if(!statusDone) {
        return -1;
    }

    emit signalStatusDone(false);

    if(projectModel == NULL) {
        QMessageBox mbox(QMessageBox::Critical, "No File",
                "Please create or load a file.", QMessageBox::Ok);
        mbox.exec();
        goto endRunCompiler;
    }

    if(this->editorTabs->currentIndex() < 0) {
        QMessageBox mbox(QMessageBox::Critical, "Error No Source",
                "Please open a source file.", QMessageBox::Ok);
        mbox.exec();
        goto endRunCompiler;
    }

    // don't allow if no port available
    if(type != COMPILE_ONLY && cbPort->count() < 1) {
        QMessageBox mbox(QMessageBox::Critical, tr("No Serial Port"),
                tr("Serial port not available.")+" "+tr("Connect a USB Propeller board, turn it on, and try again."),
                QMessageBox::Ok);
        mbox.exec();
        goto endRunCompiler;
    }

    index = editorTabs->currentIndex();
    fileName = editorTabs->tabToolTip(index);
    text = getEditor(index)->toPlainText();

    if(!isPackageSource(fileName)) {
        buildSourceWriteError(fileName);
        if(!extractSource(fileName)) {
            buildSourceWriteError(fileName);
            goto endRunCompiler;
        }
    }

    updateProjectTree(fileName);
    // updateReferenceTree(fileName,text);

    sizeLabel->setText("");
    msgLabel->setText("");

    progress->setValue(0);
    progress->setVisible(true);

    getApplicationSettings(true);

    checkAndSaveFiles();

    if(fileName.contains(".spin")) {
        statusDialog->init("Compiling Program", this->shortFileName(this->projectFile));
        spinBuilder->setParameters(spinCompiler, spinIncludes, spinCompilerPath, projectFile, compileResult);
        spinBuilder->setObjects(msgLabel, sizeLabel, progress, cbPort);
        spinBuilder->setLoader(spinLoader);

        statusDialog->stop();

        copts = "-b";
        rc = spinBuilder->runCompiler(copts);
        goto endRunCompiler;
    }
    else {
        QMessageBox::critical(this,tr("Can't compile unknown file type"), tr("Files must be of type '.spin'"));
    }
endRunCompiler:
    emit signalStatusDone(true);
    return rc;
}

void MainWindow::programBuild()
{
    runCompiler(COMPILE_ONLY);
}

int  MainWindow::loadProgram(int type, QString file)
{
    int rc = -1;
    QString copts;

    // if find in progress, ignore request
    if(!statusDone) {
        return -1;
    }
    emit signalStatusDone(false);

    if(!file.length()) {
        file = projectFile.replace(".spin",".binary",Qt::CaseInsensitive);
    }

    bool stat = portListener->isOpen();
    if(cbPort->currentText().length() == 0) {
        QMessageBox::critical(this,tr("Propeller Load"), tr("Port not available. Please connect Propeller board."), QMessageBox::Ok);
        goto endLoadProgram;
    }

    // changing selected port now changes terminal port
    if(cbPort->currentText().compare(portListener->getPortName()) != 0) {
        setCurrentPort(cbPort->currentIndex());
        term->setPortName(cbPort->currentText());
    }
    if(stat) {
        portListener->close();
    }

    portListener->setLoadEnable(true);
    switch (type) {
        case MainWindow::LoadRunHubRam:
            copts += "-r -p"+portListener->getPortName();
            rc = spinBuilder->loadProgram(copts);
            break;
        case MainWindow::LoadRunEeprom:
            copts += "-e -p"+portListener->getPortName();
            rc = spinBuilder->loadProgram(copts);
            break;
        default:
            break;
    }
    portListener->setLoadEnable(false);

    if(stat) {
        portListener->init(cbPort->currentText(),term->getBaud());
        portListener->open();
    }

    if(rc) {
        QMessageBox::critical(this,tr("Propeller Load Failed"), tr("Failed to load Propeller on port")+" "+cbPort->currentText(), QMessageBox::Ok);
    }
endLoadProgram:
    emit signalStatusDone(true);
    return rc;
}

void MainWindow::programBurnEE()
{
    if(runCompiler(COMPILE_BURN))
        return;

    if(cbPort->itemText(cbPort->currentIndex()).compare(AUTOPORT) == 0) {
        findHardware(false);
    }
    setCurrentPort(cbPort->currentIndex());

    loadProgram(MainWindow::LoadRunEeprom);
}

void MainWindow::programRun()
{
    if(runCompiler(COMPILE_RUN))
        return;

    if(cbPort->itemText(cbPort->currentIndex()).compare(AUTOPORT) == 0) {
        findHardware(false);
    }
    setCurrentPort(cbPort->currentIndex());

    loadProgram(MainWindow::LoadRunHubRam);
}

void MainWindow::programDebug()
{
    if(runCompiler(COMPILE_RUN))
        return;

    if(cbPort->itemText(cbPort->currentIndex()).compare(AUTOPORT) == 0) {
        findHardware(false);
    }
    setCurrentPort(cbPort->currentIndex());

    term->getEditor()->clear();
    portListener->init(cbPort->currentText(), term->getBaud());
    setCurrentPort(cbPort->currentIndex());
    portListener->open();
    if(!loadProgram(MainWindow::LoadRunHubRam)) {
        btnConnected->setChecked(true);
        connectButton(true);
    }
}


void MainWindow::setStatusDone(bool done)
{
    statusMutex.lock();
    statusDone = done;
    statusMutex.unlock();
}

void MainWindow::findHardware(bool showFoundBox)
{
    int count = 0;
    QString savePort;
    QString currentPort = cbPort->itemText(cbPort->currentIndex());

    // if find in progress, ignore request
    if(!statusDone) return;

    emit signalStatusDone(false);

    bool stat = portListener->isOpen();
    if(stat) {
        savePort = portListener->getPortName();
        portListener->close();
    }

    statusDialog->init("Searching for Propellers", "");
    portConnectionMonitor->stop();

    for(int n = 0; n < this->cbPort->count(); n++) {
        QString portstr = cbPort->itemText(n);
        if(portstr.compare(AUTOPORT) == 0) {
            continue;
        }
        portListener->init(portstr, term->getBaud());
        if(portListener->isDevice(portstr)) {
            count++;
            if(currentPort.compare(AUTOPORT) == 0) {
                cbPort->setCurrentIndex(n);
            }
        }
    }
    portConnectionMonitor->start();

    if(!count) {
        if(this->cbPort->count())
            statusDialog->addMessage("\nPropeller not found on any port.");
        else
            statusDialog->addMessage("Please connect a serial port.");
        statusDialog->stop();
    }
    else {
        statusDialog->stop();
        if(showFoundBox) {
            QMessageBox::information(this,tr("Propeller found"), statusDialog->getMessage());
        }
    }

    if(stat) {
        portListener->init(savePort, term->getBaud());
        portListener->open();
    }

    emit signalStatusDone(true);
}

void MainWindow::terminalClosed()
{
    btnConnected->setChecked(false);
    connectButton(false);
}


void MainWindow::findMultilineComment(QPoint point)
{
    Editor *editor = getEditor(editorTabs->currentIndex());
    QTextCursor cur = editor->cursorForPosition(point);
    findMultilineComment(cur);
}

void MainWindow::findMultilineComment(QTextCursor cur)
{
    QRegExp commentStartExpression = QRegExp("{*",Qt::CaseInsensitive,QRegExp::Wildcard);
    QRegExp commentEndExpression = QRegExp("}*",Qt::CaseInsensitive,QRegExp::Wildcard);

    Editor *editor = getEditor(editorTabs->currentIndex());
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

int MainWindow::isFileOpen(QString fileName)
{
    QString tabName;
    fileName = fileName.trimmed();
    for(int n = editorTabs->count()-1; n > -1; n--) {
        tabName = editorTabs->tabToolTip(n);
        tabName = tabName.trimmed();
        if(fileName.compare(tabName) == 0) {
            return n;
        }
    }
    return -1;
}

void MainWindow::openTreeFile(QString fileName)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    fileName = fileName.trimmed();
    qDebug() << "openTreeFile opening " << fileName;
    QString userFile = editorTabs->tabToolTip(editorTabs->currentIndex());
    QString userPath = filePathName(userFile);
    QFile file;
    QString fileToOpen;
    if(file.exists(userPath+fileName)) {
        fileToOpen = userPath+fileName;
    }
    else if(file.exists(spinIncludes+fileName)) {
        fileToOpen = spinIncludes+fileName;
    }
    int index = isFileOpen(fileToOpen);
    if(index < 0)
        openFileName(fileToOpen);
    else
        editorTabs->setCurrentIndex(index);
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

void MainWindow::closeTab(int index)
{
    fileChangeDisable = true;
    getEditor(index)->setPlainText("");
    if(editorTabs->count() == 1)
        newFile();
    editorTabs->removeTab(index);
    fileChangeDisable = false;
}

void MainWindow::changeTab(int index)
{
    /* index is a QAction menu state. we don't really care about it
    */
    if(index < 0) return; // compiler happy now

    if(!changeTabDisable)
        setProject();
}

void MainWindow::addToolButton(QToolBar *bar, QToolButton *btn, QString imgfile)
{
    const QSize buttonSize(24, 24);
    btn->setIcon(QIcon(QPixmap(imgfile.toLatin1())));
    btn->setMinimumSize(buttonSize);
    btn->setMaximumSize(buttonSize);
    btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    bar->addWidget(btn);
}

void MainWindow::zipFiles()
{
    int n = this->editorTabs->currentIndex();
    QString fileName = editorTabs->tabToolTip(n);
    if (fileName.isEmpty()) {
        return;
    }
    QString spinLibPath     = propDialog->getSpinLibraryString();
    QStringList fileTree    = spinParser.spinFileTree(fileName, spinLibPath);
    if(fileTree.count() > 0) {
        zipper.makeZip(fileName, fileTree, spinLibPath);
    }
}

void MainWindow::updateProjectTree(QString fileName)
{
    projectFile = fileName;
    QString s = this->shortFileName(fileName);
    basicPath = filePathName(fileName);

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
    QStringList flist = spinParser.spinFileTree(fileName, propDialog->getSpinLibraryString());
    for(int n = 0; n < flist.count(); n ++) {
        QString s = flist[n];
        //qDebug() << s;
        projectModel->addRootItem(s);
    }
}

void MainWindow::updateReferenceTree(QString fileName, QString text)
{
    QString s = this->shortFileName(fileName);
    basicPath = filePathName(fileName);

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
    QString path = filePathName(fileName);

    QStringList mlist = spinParser.spinMethods(fileName, objname);

    // move objects to end of the list
    for(int n = mlist.count()-1; n > -1; n--) {
        if(mlist[n].at(0) == 'o') {
            mlist.insert(mlist.count(),mlist[n]);
            mlist.removeAt(n);
        }
    }
    // display all
    for(int n = 0; n < mlist.count(); n ++) {
        QString s = mlist[n];

        if(s.at(0) == 'o') {

            /* get the file name */
            QString file = s.mid(s.indexOf(":")+1);
            file = file.mid(0, file.indexOf("\t"));
            file = file.trimmed();
            if(file.startsWith("\""))
                file = file.mid(1);
            if(file.endsWith("\""))
                file = file.mid(0,file.length()-1);
            file = file.trimmed();
            if(file.endsWith(".spin",Qt::CaseInsensitive) == false)
                file += ".spin";

            /* prepend path info to new file if found*/
            if(QFile::exists(file)) {
                referenceModel->addRootItem(file, file);
            }
            else if(QFile::exists(path+file)) {
                referenceModel->addRootItem(file, path+file);
                file = path+file;
            }
            else if(QFile::exists(includes+file)) {
                referenceModel->addRootItem(file, includes+file);
                file = includes+file;
            }
            else {
                qDebug() << "updateSpinReferenceTree can't find file" << file;
            }

            /* get the object name */
            QString obj = s.mid(s.indexOf("\t")+1);
            obj = obj.mid(0,obj.indexOf(":"));
            obj = obj.trimmed();

            updateSpinReferenceTree(file, includes, obj, level+1);
        }
        else {
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

void MainWindow::setEditor(int num, QString shortName, QString fileName, QString text)
{
    fileChangeDisable = true;
    Editor *editor = getEditor(num);
    editor->setPlainText(text);
    editorTabs->setTabText(num,shortName);
    editorTabs->setTabToolTip(num,fileName);
    editorTabs->setCurrentIndex(num);
    QApplication::processEvents();
    fileChangeDisable = false;
}

Editor *MainWindow::getEditor(int num)
{
    return (Editor *)editorTabs->widget(num);
}

void MainWindow::setEditorCodeType(Editor *ed, QString name)
{
    if(name.contains("UTF-16")) {
        ed->setCodeType(Editor::CodeTypeUTF16);
    }
    else {
        ed->setCodeType(Editor::CodeTypeUTF8);
    }
}

void MainWindow::checkConfigSerialPort()
{
    if(!cbPort->count()) {
        enumeratePorts();
    } else {
        QString name = cbPort->currentText();
        QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
        int index = -1;
        for (int i = 0; i < ports.size(); i++) {
            if(ports.at(i).portName.contains(name, Qt::CaseInsensitive)) {
                index = i;
                break;
            }
        }
        if(index < 0) {
            enumeratePorts();
        }
    }
}

void MainWindow::enumeratePortsEvent()
{
    enumeratePorts();
    int len = this->cbPort->count();

    // need to check if the port we are using disappeared.
    bool notFound = true;
    QString plPortName = this->term->getPortName();
    for(int n = this->cbPort->count()-1; n > -1; n--) {
        QString name = cbPort->itemText(n);
        if(!name.compare(plPortName)) {
            notFound = false;
        }
    }
    if(notFound) {
        btnConnected->setChecked(false);
    }
    else {
        btnConnected->setChecked(true);
    }

    if(len > 1) {
        if(this->isActiveWindow())
            this->cbPort->showPopup();
    }

}

void MainWindow::enumeratePorts()
{
    if(cbPort == NULL)
        return;

    cbPort->clear();
    cbPort->addItem(AUTOPORT);

    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    QStringList stringlist;
    QString name;
    stringlist << "List of ports:";

    for (int i = 0; i < ports.size(); i++) {
        progress->setValue(i*100/ports.size());
        stringlist << "port name:" << ports.at(i).portName;
        stringlist << "friendly name:" << ports.at(i).friendName;
        stringlist << "physical name:" << ports.at(i).physName;
        stringlist << "enumerator name:" << ports.at(i).enumName;
        stringlist << "vendor ID:" << QString::number(ports.at(i).vendorID, 16);
        stringlist << "product ID:" << QString::number(ports.at(i).productID, 16);
        stringlist << "===================================";
#if defined(Q_OS_WIN32)
        name = ports.at(i).portName;
        if(name.contains(QString("COM"),Qt::CaseInsensitive)) {
            //cbPort->addItem(name, QVariant(ports.at(i).physName));
            cbPort->addItem(name);
        }
#elif defined(Q_OS_MAC)
        name = ports.at(i).portName;
        if(name.indexOf("cu.usbserial",0,Qt::CaseInsensitive) > -1)
            cbPort->addItem(name);
#else
        name = ports.at(i).physName;
        if(name.indexOf("usb",0,Qt::CaseInsensitive) > -1) {
            cbPort->addItem(name);
        }
#endif
    }
    if(!cbPort->count()) {
        btnConnected->setCheckable(false);
    }
    else {
        btnConnected->setCheckable(true);
    }
    QApplication::processEvents();
}

void MainWindow::connectButton(bool show)
{
    if(btnConnected->isChecked()) {
        btnConnected->setDisabled(true);
        if(!portListener->isOpen()) {
            qDebug() << "connect enable port";
            portListener->init(cbPort->currentText(), term->getBaud());
            setCurrentPort(cbPort->currentIndex());
            portListener->open();
        }
        btnConnected->setDisabled(false);
        term->setPortEnabled(true);
        term->setPortName(portListener->getPort()->portName());
        if(show) {
            term->show();
            term->activateWindow();
            term->getEditor()->setFocus();
        }
    }
    else {
        term->setPortEnabled(false);
        portListener->close();
        term->hide();
    }
}

QString MainWindow::shortFileName(QString fileName)
{
    QString rets;
    if(fileName.indexOf("/") > -1)
        rets = fileName.mid(fileName.lastIndexOf("/")+1);
    else
        rets = fileName;
    return rets;
}

QString MainWindow::filePathName(QString fileName)
{
    QString rets;
    if(fileName.lastIndexOf("/") > -1)
        rets = fileName.mid(0,fileName.lastIndexOf("/")+1);
    else
        rets = fileName;
    return rets;
}

Editor *MainWindow::createEditor()
{
    Editor *editor = new Editor(this);
    editor->initSpin(&spinParser);

    connect(editor,SIGNAL(textChanged()),this,SLOT(fileChanged()));

    return editor;
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

    projectTree->updateColors();
    referenceTree->updateColors();

    projectTree->updateFonts();
    referenceTree->updateFonts();

    leftSplit->addWidget(referenceTree);

    leftSplit->setStretchFactor(0,1);
    leftSplit->setStretchFactor(1,2);

    findSplit = new QSplitter(this);
    findSplit->setOrientation(Qt::Vertical);
    vsplit->addWidget(findSplit);

    // project editor tabs
    editorTabs = new QTabWidget(this);
    editorTabs->setTabsClosable(true);
    editorTabs->setMovable(true);
    connect(editorTabs,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
    connect(editorTabs,SIGNAL(currentChanged(int)),this,SLOT(changeTab(int)));
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
