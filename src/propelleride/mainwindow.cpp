#include "mainwindow.h"

#include <QMessageBox> 
#include <QStatusBar> 
#include <QToolBar> 
#include <QFileDialog> 
#include <QMenu> 
#include <QSerialPortInfo>
#include <QProcess>

#include "ui_about.h"

#include "memorymap.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
{
    setWindowTitle(QCoreApplication::applicationName());
    ui.setupUi(this);

    // setup preferences dialog
    propDialog = new Preferences(this);
    connect(propDialog,SIGNAL(accepted()),this,SLOT(getApplicationSettings()));

    connect(&builder,SIGNAL(compilerErrorInfo(QString,int)), this, SLOT(highlightFileLine(QString,int)));

    parser = language.getParser();
    connect(propDialog,SIGNAL(updateColors()),this,SLOT(recolorProjectView()));
    connect(propDialog,SIGNAL(updateFonts()),this,SLOT(recolorProjectView()));

    connect(propDialog,SIGNAL(updateFonts()),this,SLOT(recolorBuildManager()));

    recolorProjectView();
    recolorBuildManager();


    // project editor tabs
    ui.finder->connectFileManager(ui.editorTabs);
    QSplitterHandle *hndl = ui.splitter->handle(1);
    hndl->setEnabled(false);

    connect(ui.editorTabs, SIGNAL(tabCloseRequested(int)), ui.editorTabs,         SLOT(closeFile(int)));
    connect(ui.editorTabs, SIGNAL(currentChanged(int)),    ui.editorTabs,         SLOT(changeTab(int)));

    // File Menu
    connect(ui.action_New,SIGNAL(triggered()),ui.editorTabs,SLOT(newFile()));
    connect(ui.action_Open,SIGNAL(triggered()),ui.editorTabs,SLOT(open()));

    connect(ui.action_Save,SIGNAL(triggered()),ui.editorTabs,SLOT(save()));
    connect(ui.actionSave_As,SIGNAL(triggered()),ui.editorTabs,SLOT(saveAs()));
    connect(ui.actionSave_All,SIGNAL(triggered()),ui.editorTabs,SLOT(saveAll()));

    ui.action_Zip->setEnabled(true);
    connect(ui.action_Zip,SIGNAL(triggered()),this,SLOT(zipFiles()));

    recentFiles = findChildren<QAction *>(QRegExp("action_[0-9]+_File"));
    for (int i = 0; i < recentFiles.size(); i++)
        connect(recentFiles.at(i), SIGNAL(triggered()),this, SLOT(openRecentFile()));
    
    connect(ui.action_Close,       SIGNAL(triggered()), ui.editorTabs, SLOT(closeFile()));
    connect(ui.actionClose_All,    SIGNAL(triggered()), ui.editorTabs, SLOT(closeAll()));
    connect(ui.action_Quit,        SIGNAL(triggered()), this,          SLOT(quitProgram()));

    connect(ui.editorTabs, SIGNAL(saveAvailable(bool)),    ui.action_Save,     SLOT(setEnabled(bool)));
    connect(ui.editorTabs, SIGNAL(saveAvailable(bool)),    ui.actionSave_All,  SLOT(setEnabled(bool)));
    connect(ui.editorTabs, SIGNAL(closeAvailable(bool)),   ui.action_Close,    SLOT(setEnabled(bool)));
    connect(ui.editorTabs, SIGNAL(closeAvailable(bool)),   ui.actionClose_All, SLOT(setEnabled(bool)));

    // Edit Menu
    connect(ui.action_Undo,        SIGNAL(triggered()), ui.editorTabs, SLOT(undo()));
    connect(ui.action_Redo,        SIGNAL(triggered()), ui.editorTabs, SLOT(redo()));

    connect(ui.action_Cut,         SIGNAL(triggered()), ui.editorTabs, SLOT(cut()));
    connect(ui.action_Copy,        SIGNAL(triggered()), ui.editorTabs, SLOT(copy()));
    connect(ui.action_Paste,       SIGNAL(triggered()), ui.editorTabs, SLOT(paste()));
    connect(ui.actionSelect_All,   SIGNAL(triggered()), ui.editorTabs, SLOT(selectAll()));

    connect(ui.action_Find,        SIGNAL(triggered()), ui.finder, SLOT(showFinder()));
    connect(ui.actionFind_Next,    SIGNAL(triggered()), ui.finder, SLOT(findNext()));
    connect(ui.actionFind_Previous,SIGNAL(triggered()), ui.finder, SLOT(findPrevious()));

    connect(ui.actionPreferences,  SIGNAL(triggered()), propDialog, SLOT(showPreferences()));

    connect(ui.editorTabs, SIGNAL(undoAvailable(bool)), ui.action_Undo,SLOT(setEnabled(bool)));
    connect(ui.editorTabs, SIGNAL(redoAvailable(bool)), ui.action_Redo,SLOT(setEnabled(bool)));
    connect(ui.editorTabs, SIGNAL(copyAvailable(bool)), ui.action_Cut,SLOT(setEnabled(bool)));
    connect(ui.editorTabs, SIGNAL(copyAvailable(bool)), ui.action_Copy,SLOT(setEnabled(bool)));

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
    connect(ui.actionPropeller_Quick_Reference, SIGNAL(triggered()), this, SLOT(propellerQuickReference()));
    connect(ui.actionPropeller_Datasheet,       SIGNAL(triggered()), this, SLOT(propellerDatasheet()));
    connect(ui.actionPropeller_Manual,          SIGNAL(triggered()), this, SLOT(propellerManual()));
    connect(ui.action_About,                    SIGNAL(triggered()), this, SLOT(about()));

    // Toolbar Extras
    cbPort = new QComboBox(this);
    cbPort->setLayoutDirection(Qt::LeftToRight);
    cbPort->setToolTip(tr("Select Serial Port"));
    cbPort->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    ui.toolBar->addWidget(cbPort);

    connect(ui.projectview,SIGNAL(showFileLine(QString, int)),this,SLOT(highlightFileLine(QString, int)));

    updateRecentFileActions();

    connect(ui.editorTabs, SIGNAL(fileUpdated(int)),               this,SLOT(setProject()));
    connect(ui.editorTabs, SIGNAL(closeAvailable(bool)),           this,SLOT(setProject()));

    connect(ui.editorTabs, SIGNAL(sendMessage(const QString &)),   this,SLOT(showMessage(const QString &)));
    connect(ui.finder,     SIGNAL(sendMessage(const QString &)),   this,SLOT(showMessage(const QString &)));

    restoreGeometry(QSettings().value("windowSize").toByteArray());

    getApplicationSettings();

    // get available ports at startup
    enumeratePorts();
    connect(&portMonitor, SIGNAL(portChanged()), this, SLOT(enumeratePorts()));

    ui.editorTabs->newFile();
    loadSession();
    installEventFilter(this);
}

void MainWindow::loadSession()
{
    QSettings settings;
    int size = settings.beginReadArray("session");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        ui.editorTabs->openFile(settings.value("file").toString());
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
    for (int i = 0; i < ui.editorTabs->count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("file",ui.editorTabs->tabToolTip(i));
    }
    settings.endArray();
}

void MainWindow::openFiles(const QStringList & files)
{
    for (int i = 0; i < files.size(); i++)
    {
        qDebug() << files.at(i);
        ui.editorTabs->openFile(files.at(i));
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
    ui.editorTabs->closeAll();

    if (ui.editorTabs->count())
    {
        if(e) e->ignore();
        return;
    }

    QSettings().setValue("windowSize",saveGeometry());

    if(e) e->accept();
    qApp->exit(0);
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
        ui.editorTabs->openFile(action->data().toString());
}

void MainWindow::printFile()
{
}

void MainWindow::setProject()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    int index = ui.editorTabs->currentIndex();
    QString shortname, filename;
    if (index > -1)
    {
        shortname = ui.editorTabs->tabText(index);
        filename =  ui.editorTabs->tabToolTip(index);
    }
    else
    {
        shortname = tr("Untitled");
    }

    if (ui.editorTabs->count() > 0)
        setWindowTitle(shortname + " - " +
                       QCoreApplication::applicationName());
    else
        setWindowTitle(QCoreApplication::applicationName());

    addRecentFile(filename);
    parser->setFile(filename);
    parser->setLibraryPaths(QStringList() << spinIncludes);

    recolorProjectView();
    QApplication::restoreOverrideCursor();
}

void MainWindow::showBrowser()
{
    if (ui.projectview->isVisible())
    {
        ui.projectview->hide();
    }
    else
    {
        ui.projectview->show();
    }
}

void MainWindow::checkAndSaveFiles()
{
    for (int i = 0; i < ui.editorTabs->count(); i++)
    {
        if (ui.editorTabs->getEditor(i)->contentChanged())
        {
            ui.editorTabs->save(i);
        }
    }
}

void MainWindow::highlightFileLine(QString filename, int line)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFileInfo fi(filename);
    if (!fi.exists(filename) || !fi.isFile())
        return;

    ui.editorTabs->openFile(filename);

    Editor * editor = ui.editorTabs->getEditor(ui.editorTabs->currentIndex());
    if(editor)
    {
        QTextCursor cur = editor->textCursor();
        cur.movePosition(QTextCursor::Start);
        cur.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,line);
        cur.clearSelection();
        editor->setTextCursor(cur);
    }
    QApplication::restoreOverrideCursor();
}


int MainWindow::runCompiler()
{
    builder.show();

    int index;
    QString fileName;

    if(!ui.editorTabs->count())
        return 1;

    index = ui.editorTabs->currentIndex();
    fileName = ui.editorTabs->tabToolTip(index);

    getApplicationSettings();

    checkAndSaveFiles();

    builder.setParameters(spinCompiler, spinLoader, spinIncludes, fileName);
    int rc = builder.runCompiler();

    return rc;
}

int MainWindow::loadProgram(int type)
{
    int rc = -1;
    QString options;
    options += "-d"+cbPort->currentText();

    switch (type) {
        case MainWindow::LoadRunHubRam:
            rc = builder.loadProgram(options);
            break;
        case MainWindow::LoadRunEeprom:
            options += " -w";
            rc = builder.loadProgram(options);
            break;
        default:
            break;
    }

    return rc;
}

void MainWindow::programBuild()
{
    if(runCompiler())
        return;

    builder.waitClose();
}


void MainWindow::programRun()
{
    if(runCompiler())
        return;

    loadProgram(LoadRunHubRam);
}

void MainWindow::programBurnEE()
{
    if(runCompiler())
        return;

    loadProgram(MainWindow::LoadRunEeprom);
}

void MainWindow::programDebug()
{
    if(runCompiler())
        return;

    if(!loadProgram(LoadRunHubRam))
    {
        spawnTerminal();
    }
}

void MainWindow::recolorBuildManager()
{
    ColorScheme * theme = &Singleton<ColorScheme>::Instance();
    builder.setFont(theme->getFont());
}

void MainWindow::recolorProjectView()
{
    ColorScheme * theme = &Singleton<ColorScheme>::Instance();
    ui.projectview->updateColors(theme->getColor(ColorScheme::PubBG));
    parser->styleRule("public",QIcon(),theme->getColor(ColorScheme::SyntaxFunctions));
    parser->styleRule("private",QIcon(),theme->getColor(ColorScheme::SyntaxFunctions));
    parser->styleRule("constants",QIcon(),theme->getColor(ColorScheme::SyntaxKeywords));
    parser->styleRule("_includes_",QIcon(),theme->getColor(ColorScheme::SyntaxText));
    parser->setFont(theme->getFont());
    parser->buildModel();
    ui.projectview->setModel(parser->treeModel());
}

void MainWindow::viewInfo()
{
    MemoryMap * map = new MemoryMap();
    map->setAttribute(Qt::WA_DeleteOnClose, true);

    connect(propDialog,SIGNAL(updateColors()),map,SLOT(updateColors()));
    connect(propDialog,SIGNAL(updateFonts()),map,SLOT(updateColors()));
    connect(map,SIGNAL(getRecolor(QWidget *)),this,SLOT(recolorInfo(QWidget *)));

    connect(map,SIGNAL(run(QByteArray)), this, SLOT(programRun()));
    connect(map,SIGNAL(write(QByteArray)), this, SLOT(programBurnEE()));

    recolorInfo(map);

    int index = ui.editorTabs->currentIndex();
    QString filename = ui.editorTabs->tabToolTip(index);

    if(runCompiler())
        return;

    builder.hide();
    QFileInfo fi(filename);

    QString binaryname = fi.completeBaseName()+".binary";
    map->setWindowTitle(binaryname + " - " + tr("Memory Map"));

    binaryname = fi.dir().filePath(binaryname);
    qDebug() << binaryname;

    map->loadFile(binaryname);

    map->show();
}

void MainWindow::recolorInfo(QWidget * widget)
{
    ColorScheme * theme = &Singleton<ColorScheme>::Instance();
    MemoryMap * map = (MemoryMap *) widget;
    map->recolor(
            theme->getColor(ColorScheme::PubBG),
            theme->getColor(ColorScheme::DatBG),
            theme->getColor(ColorScheme::SyntaxFunctions),
            theme->getColor(ColorScheme::SyntaxFunctions),
            theme->getColor(ColorScheme::ConBG),
            theme->getColor(ColorScheme::SyntaxComments),
            theme->getColor(ColorScheme::SyntaxText)
           );
    map->setFont(theme->getFont());
}

void MainWindow::findMultilineComment(QPoint point)
{
    Editor *editor = ui.editorTabs->getEditor(ui.editorTabs->currentIndex());
    QTextCursor cur = editor->cursorForPosition(point);
    findMultilineComment(cur);
}

void MainWindow::findMultilineComment(QTextCursor cur)
{
    QRegExp commentStartExpression = QRegExp("{*",Qt::CaseInsensitive,QRegExp::Wildcard);
    QRegExp commentEndExpression = QRegExp("}*",Qt::CaseInsensitive,QRegExp::Wildcard);

    Editor *editor = ui.editorTabs->getEditor(ui.editorTabs->currentIndex());
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

void MainWindow::zipFiles()
{
    int n = this->ui.editorTabs->currentIndex();
    QString fileName = ui.editorTabs->tabToolTip(n);

    if (fileName.isEmpty())
        return;

    QString spinLibPath  = QSettings().value("Library").toString();
    QStringList files = parser->getFileList();
    qDebug() << files;

    if(files.count() > 0)
    {
        zipper.makeZip(fileName, files);
    }
}

void MainWindow::setEnableBuild(bool enabled)
{
    cbPort->setEnabled(enabled);
    ui.actionTerminal->setEnabled(enabled);
    ui.actionRun->setEnabled(enabled);
    ui.actionBurn->setEnabled(enabled);
}

void MainWindow::enumeratePorts()
{
    if(cbPort == NULL)
        return;

    cbPort->clear();

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    foreach(QSerialPortInfo port, ports)
    {
        if (!port.systemLocation().contains("ttyS") &&
            !port.systemLocation().contains("Bluetooth"))
            cbPort->addItem(port.systemLocation());
    }

    if(cbPort->count())
    {
        setEnableBuild(true);
    }
    else
    {
        setEnableBuild(false);
    }
}

void MainWindow::spawnTerminal()
{
    QString term = QDir::toNativeSeparators(spinTerminal);
    qDebug() << "Running" << term;
    if (!QProcess::startDetached(term, QStringList()))
        qDebug() << "Failed to detach" << term;
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *e = static_cast<QKeyEvent *>(event);

        if (e->modifiers() & Qt::ControlModifier)
        {
            switch (e->key())
            {
            // project controls
            case (Qt::Key_K):
            case (Qt::Key_L):
                ui.projectview->setFocus();
                return true;

            // tab controls
            case (Qt::Key_T):
                ui.editorTabs->newFile();
                return true;
            case (Qt::Key_W):
                ui.editorTabs->closeFile();
                return true;
            case (Qt::Key_PageUp):
                ui.editorTabs->previousTab();
                return true;
            case (Qt::Key_PageDown):
                ui.editorTabs->nextTab();
                return true;
            }
        } else {
            if (QApplication::focusWidget()->parent() == ui.finder)
            {
                switch (e->key())
                {
                case (Qt::Key_Enter):
                case (Qt::Key_Return):
                    ui.finder->findNext();
                    return true;
                case (Qt::Key_Escape):
                    ui.finder->hide();
                    ui.editorTabs->currentWidget()->setFocus();
                    return true;
                }
            }
            else if (QApplication::focusWidget()->parent() == ui.projectview)
            {
                switch (e->key())
                {
                case (Qt::Key_Escape):
                    ui.projectview->clearSearch();
                    ui.editorTabs->currentWidget()->setFocus();
                    return true;
                }
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

void MainWindow::propellerQuickReference()
{
    openFileResource("/doc/pdf/QuickReference-v15.pdf");
}

void MainWindow::about()
{
    QDialog * about = new QDialog();
    Ui::About ui;
    ui.setupUi(about);
    ui.label_4->setText(tr("Version %1").arg(qApp->applicationVersion()));
    about->show();
}

void MainWindow::showMessage(const QString & message)
{
    statusBar()->showMessage(message, 2000);
}
