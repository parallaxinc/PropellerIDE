#include "mainwindow.h"

#include <QMessageBox> 
#include <QStatusBar> 
#include <QToolBar> 
#include <QFileDialog> 
#include <QMenu> 

#include <QPrinter>
#include <QPrintDialog>

#include <MemoryMap>
#include <PropTerm>

#include "newfromtemplate.h"

#include "ui_about.h"
#include "setup.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
{
    setWindowTitle(QCoreApplication::applicationName());
    ui.setupUi(this);

    // setup preferences dialog
    connect(&preferences,   SIGNAL(accepted()),                     this,   SLOT(getApplicationSettings()));

    connect(&builder,   SIGNAL(highlightLine(const QString &,
                                             int,
                                             int,
                                             const QString &)),

            this,       SLOT(highlightFileLine(const QString &,
                                               int,
                                               int)));

    connect(&builder,   SIGNAL(finished()),     this,           SLOT(setBuildAvailable()));
    connect(&builder,   SIGNAL(buildError()),   &preferences,   SLOT(showPreferences()));

    parser = language.parser();

    connect(&preferences, SIGNAL(updateColors()),               this,   SLOT(recolorProjectView()));
    connect(&preferences, SIGNAL(updateFonts(const QFont &)),   this,   SLOT(recolorProjectView()));

    connect(&preferences, SIGNAL(updateFonts(const QFont &)),   this,   SLOT(recolorBuildManager()));

    connect(&preferences, SIGNAL(accepted()),                   ui.editorTabs,   SIGNAL(accepted()));
    connect(&preferences, SIGNAL(updateColors()),               ui.editorTabs,   SIGNAL(updateColors()));
    connect(&preferences, SIGNAL(updateFonts(const QFont &)),   ui.editorTabs,   SIGNAL(updateFonts(const QFont &)));

    recolorProjectView();
    recolorBuildManager();

    // project editor tabs
    ui.finder->connectFileManager(ui.editorTabs);
    QSplitterHandle *hndl = ui.splitter->handle(1);
    hndl->setEnabled(false);

    connect(ui.editorTabs, SIGNAL(fileUpdated(int)),    this,   SLOT(setProject()));

    // File Menu
    connect(ui.action_New,              SIGNAL(triggered()),    ui.editorTabs,  SLOT(newFile()));
    connect(ui.actionNew_From_File,     SIGNAL(triggered()),    ui.editorTabs,  SLOT(newFromFile()));
    connect(ui.actionNew_From_Template, SIGNAL(triggered()),    this,           SLOT(newFromTemplate()));
    connect(ui.action_Open,             SIGNAL(triggered()),    ui.editorTabs,  SLOT(open()));

    connect(ui.action_Save,SIGNAL(triggered()),ui.editorTabs,SLOT(save()));
    connect(ui.actionSave_As,SIGNAL(triggered()),ui.editorTabs,SLOT(saveAs()));
    connect(ui.actionSave_All,SIGNAL(triggered()),ui.editorTabs,SLOT(saveAll()));

    ui.actionPrint->setEnabled(true);
    connect(ui.actionPrint,SIGNAL(triggered()),this,SLOT(printFile()));

    ui.action_Zip->setEnabled(true);
    connect(ui.action_Zip,SIGNAL(triggered()),this,SLOT(zipFiles()));

    recentFiles = findChildren<QAction *>(QRegExp("action_[0-9]+_File"));
    for (int i = 0; i < recentFiles.size(); i++)
        connect(recentFiles.at(i), SIGNAL(triggered()),this, SLOT(openRecentFile()));
    
    connect(ui.action_Close,       SIGNAL(triggered()),    ui.editorTabs, SLOT(closeFile()));
    connect(ui.actionClose_All,    SIGNAL(triggered()),    ui.editorTabs, SLOT(closeAll()));
    connect(ui.action_Quit,        SIGNAL(triggered()),    this,          SLOT(quitProgram()));

    connect(ui.editorTabs, SIGNAL(saveAvailable(bool)),    this,          SLOT(setSaveAvailable(bool)));
    connect(ui.editorTabs, SIGNAL(saveAvailable(bool)),    this,          SLOT(setSaveAvailable(bool)));
    connect(ui.editorTabs, SIGNAL(closeAvailable(bool)),   this,          SLOT(setCloseAvailable(bool)));
    connect(ui.editorTabs, SIGNAL(closeAvailable(bool)),   this,          SLOT(setCloseAvailable(bool)));
    connect(ui.editorTabs, SIGNAL(closeAvailable(bool)),   this,          SLOT(setBuildAvailable(bool)));

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

    connect(ui.actionPreferences,  SIGNAL(triggered()), &preferences, SLOT(showPreferences()));

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
    connect(ui.actionMemory_Map,SIGNAL(triggered()), this, SLOT(spawnMemoryMap()));
    connect(ui.actionBuild,     SIGNAL(triggered()), this, SLOT(programBuild()));
    connect(ui.actionRun,       SIGNAL(triggered()), this, SLOT(programRun()));
    connect(ui.actionWrite,     SIGNAL(triggered()), this, SLOT(programWrite()));
    connect(ui.actionTerminal,  SIGNAL(triggered()), this, SLOT(spawnTerminal()));

    // Help Menu
    connect(ui.actionPropeller_Quick_Reference, SIGNAL(triggered()), this, SLOT(propellerQuickReference()));
    connect(ui.actionPropeller_Datasheet,       SIGNAL(triggered()), this, SLOT(propellerDatasheet()));
    connect(ui.actionPropeller_Manual,          SIGNAL(triggered()), this, SLOT(propellerManual()));
    connect(ui.actionPropBASIC_Manual,          SIGNAL(triggered()), this, SLOT(propBasicManual()));
    connect(ui.action_About,                    SIGNAL(triggered()), this, SLOT(about()));

    // Toolbar Extras
    cbPort = new QComboBox(this);
    cbPort->setLayoutDirection(Qt::LeftToRight);
    cbPort->setToolTip(tr("Select Serial Port"));
    cbPort->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    ui.toolBar->addWidget(cbPort);

    connect(ui.projectview, SIGNAL(showFileLine(QString, int)),
            this,           SLOT(highlightFileLine(QString, int)));

    updateRecentFileActions();

    connect(ui.editorTabs, SIGNAL(sendMessage(const QString &)),   this,SLOT(showMessage(const QString &)));
    connect(ui.finder,     SIGNAL(sendMessage(const QString &)),   this,SLOT(showMessage(const QString &)));

    restoreGeometry(QSettings().value("windowSize").toByteArray());

    getApplicationSettings();

    connect(&manager, SIGNAL(portListChanged()), this, SLOT(updatePorts()));
    manager.enablePortMonitor(true);
    updatePorts();

    
    QSettings settings;
    QString lastport = settings.value("lastPort", QString()).toString();
    if (!lastport.isEmpty())
    {
        int index = cbPort->findText(lastport);
        if (index != -1)
            cbPort->setCurrentIndex(index);
    }

    ui.editorTabs->newFile();
    loadSession();

    installEventFilter(this);
    statusBar();
}

void MainWindow::setSaveAvailable(bool enabled)
{
    _save_available = enabled;
    updateActionStates();
}

void MainWindow::setCloseAvailable(bool enabled)
{
    _close_available = enabled;
    updateActionStates();
}

void MainWindow::setBuildAvailable(bool enabled)
{
    _build_available = enabled;
    updateActionStates();
}

void MainWindow::setBuildAvailable()
{
    _build_available = true;
    updateActionStates();
}


void MainWindow::setSaveEnabled(bool enabled)
{
    ui.action_Save->setEnabled(enabled);
    ui.actionSave_All->setEnabled(enabled);
}

void MainWindow::setCloseEnabled(bool enabled)
{
    ui.action_Close->setEnabled(enabled);
    ui.actionClose_All->setEnabled(enabled);
}

void MainWindow::setBuildEnabled(bool enabled)
{
    ui.actionBuild->setEnabled(enabled);
//    ui.actionMemory_Map->setEnabled(enabled);
}

void MainWindow::setLoadEnabled(bool enabled)
{
    ui.actionRun->setEnabled(enabled);
    ui.actionWrite->setEnabled(enabled);
}

void MainWindow::updateActionStates()
{
    if (!_close_available)
    {
        setSaveEnabled(false);
        setCloseEnabled(false);
        setBuildEnabled(false);
        setLoadEnabled(false);
        return;
    }

    setCloseEnabled(true);
    setSaveEnabled(_save_available);

    int index = ui.editorTabs->currentIndex();
    if (index < 0)
    {
        setBuildEnabled(false);
        setLoadEnabled(false);
    }
    else
    {
        setBuildEnabled(_build_available);
        if (_build_available && cbPort->count())
            setLoadEnabled(true);
        else
            setLoadEnabled(false);
    }
}

void MainWindow::loadSession()
{
    QSettings settings;
    int size = settings.beginReadArray("Session");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        ui.editorTabs->openFile(settings.value("file").toString());
    }
    settings.endArray();
}

void MainWindow::clearSession()
{
    QSettings settings;
    settings.beginGroup("Session");
    settings.remove("");
    settings.endGroup();
}

void MainWindow::saveSession()
{
    clearSession();

    QSettings settings;
    settings.beginWriteArray("Session");
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
        ui.editorTabs->openFile(files.at(i));
    }
}

void MainWindow::getApplicationSettings()
{
    QSettings settings;
    settings.beginGroup("Paths");
    settings.beginGroup("spin");

    spinCompiler = settings.value("compiler").toString();
    spinIncludes = settings.value("includes").toStringList();

    settings.endGroup();
    settings.endGroup();
}

void MainWindow::fontBigger()
{
    preferences.adjustFontSize(1.25);
}

void MainWindow::fontSmaller()
{
    preferences.adjustFontSize(0.8);
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

    QSettings settings;
    settings.setValue("lastPort",   cbPort->currentText());
    settings.setValue("windowSize", saveGeometry());

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

void MainWindow::setProject()
{
    QString shortname, filename;

    int index = ui.editorTabs->currentIndex();
    if (index > -1)
    {
        shortname = ui.editorTabs->tabText(index);
        filename =  ui.editorTabs->tabToolTip(index);
    }

    if (ui.editorTabs->count() > 0)
        setWindowTitle(shortname + " - " +
                       QCoreApplication::applicationName());
    else
        setWindowTitle(QCoreApplication::applicationName());

    addRecentFile(filename);

    language.loadExtension(QFileInfo(filename).suffix());
    parser = language.parser();
    parser->setFile(filename);
    parser->setLibraryPaths(spinIncludes);

    recolorProjectView();
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

void MainWindow::checkAndSaveFiles(QStringList files)
{
    for (int i = 0; i < ui.editorTabs->count(); i++)
    {
        if (files == QStringList() || files.contains(ui.editorTabs->tabToolTip(i)))
        {
            if (ui.editorTabs->getView(i)->contentChanged())
            {
                ui.editorTabs->save(i);
            }
        }
    }
}

QString MainWindow::resolveFileName(QString filename)
{
    foreach (QString s, parser->getFileList())
    {
        if (s == filename || QFileInfo(s).fileName() == filename)
            return s;
    }
    return QString();
}

void MainWindow::highlightFileLine(QString filename, int line)
{
    highlightFileLine(filename, line, 0);
}

void MainWindow::highlightFileLine(QString filename, int line, int col)
{
    filename = resolveFileName(filename);

    if (filename.isEmpty())
        return;

    ui.editorTabs->openFile(filename);

    EditorView * editor = ui.editorTabs->getView(ui.editorTabs->currentIndex());
    if(editor)
    {
        QTextCursor cur = editor->textCursor();
        cur.movePosition(QTextCursor::Start);
        cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line);
        cur.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, col);
        cur.clearSelection();
        editor->setTextCursor(cur);
    }
}

bool MainWindow::runCompiler(bool load, bool write, const QString & name)
{
    if(!ui.editorTabs->count())
        return false;

    QString filename = name;
    if (name.isEmpty())
    { 
        int index = ui.editorTabs->currentIndex();
        filename = ui.editorTabs->tabToolTip(index);
    }

    if (filename.isEmpty())
    {
        if (!ui.editorTabs->save())
            return false;
    }

    getApplicationSettings();

    checkAndSaveFiles(parser->getFileList());
    setProject();

    if (parser->status() == ProjectParser::CircularDependencyError)
    {
        QMessageBox::critical(0, 
                QObject::tr("Circular dependency!"), 
                QObject::tr("<p>Your project has a circular dependency. "
                    "That means one of your objects is including <i>itself</i>.</p>"
                    "<p>Revise your code to build your project.</p>"));
        return false;
    }

    setBuildAvailable(false);

    BuildManager::Configuration config;

    config.compiler = spinCompiler;
    config.includes = spinIncludes;
    config.file     = filename;
    config.port     = cbPort->currentText();

    config.load     = load;
    config.write    = write;

    config.manager  = &manager;

    builder.setConfiguration(config);
    builder.build();

    return true;
}

void MainWindow::programBuild()
{
    runCompiler();
}

void MainWindow::programRun()
{
    runCompiler(true);
}

void MainWindow::programWrite()
{
    runCompiler(true, true);
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
    parser->styleRule("public",QIcon(),theme->getColor(ColorScheme::SyntaxKeywords));
    parser->styleRule("private",QIcon(),theme->getColor(ColorScheme::SyntaxKeywords));
    parser->styleRule("functions",QIcon(),theme->getColor(ColorScheme::SyntaxKeywords));
    parser->styleRule("subroutines",QIcon(),theme->getColor(ColorScheme::SyntaxKeywords));
    parser->styleRule("constants",QIcon(),theme->getColor(ColorScheme::SyntaxKeywords));
    parser->styleRule("variables",QIcon(),theme->getColor(ColorScheme::SyntaxKeywords));
    parser->styleRule("pins",QIcon(),theme->getColor(ColorScheme::SyntaxKeywords));
    parser->styleRule("_includes_",QIcon(),theme->getColor(ColorScheme::SyntaxText));
    parser->setFont(theme->getFont());

    qDebug() << "updating project view";

    parser->buildModel();
    ui.projectview->setModel(parser->treeModel());
}

void MainWindow::spawnMemoryMap(const QString & name)
{
    qDebug() << "spawnMemoryMap()";

    QString filename = name;
    if (name.isEmpty())
    {
        int index = ui.editorTabs->currentIndex();
        filename = ui.editorTabs->tabToolTip(index);
    }

    if (!runCompiler(false, false, filename))
        return;

    MemoryMap * map = new MemoryMap(&manager);
    map->setAttribute(Qt::WA_DeleteOnClose, true);
    map->setWindowIcon(QIcon(":/icons/project-info3.png"));

    connect(&preferences,   SIGNAL(updateColors()),             map,        SLOT(updateColors()));
    connect(&preferences,   SIGNAL(updateFonts(const QFont &)), map,        SLOT(updateColors()));
    connect(map,            SIGNAL(getRecolor(QWidget *)),      this,       SLOT(recolorMemoryMap(QWidget *)));

    connect(map,            SIGNAL(run(const QByteArray &)),    &builder,   SLOT(load(const QByteArray &)));
    connect(map,            SIGNAL(write(QByteArray)),          this,       SLOT(programWrite()));


    recolorMemoryMap(map);

    builder.hide();
    QFileInfo fi(filename);

    QString binaryname = fi.completeBaseName()+".binary";
    map->setWindowTitle(binaryname + " - " + tr("Memory Map"));

    binaryname = fi.dir().filePath(binaryname);

    map->openFile(binaryname);
//    map->setModel(parser->treeModel());
    map->show();
}

void MainWindow::recolorMemoryMap(QWidget * widget)
{
    ColorScheme * theme = &Singleton<ColorScheme>::Instance();
    MemoryMap * map = (MemoryMap *) widget;
    map->recolor(
            theme->getColor(ColorScheme::PubBG),
            theme->getColor(ColorScheme::DatBG),
            theme->getColor(ColorScheme::SyntaxText),
            theme->getColor(ColorScheme::SyntaxKeywords),
            theme->getColor(ColorScheme::ConBG),
            theme->getColor(ColorScheme::SyntaxComments),
            theme->getColor(ColorScheme::SyntaxText)
           );
    map->setFont(theme->getFont());
}

void MainWindow::printFile()
{
    int n = this->ui.editorTabs->currentIndex();
    QString fileName = ui.editorTabs->tabToolTip(n);

    if (fileName.isEmpty())
        return;

    qDebug() << "printing file:" << fileName;

    EditorView * editor = ui.editorTabs->getView(n);

    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print %1").arg(fileName));

    if (editor->textCursor().hasSelection())
        dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    if (dialog->exec() != QDialog::Accepted)
        return;

    editor->print(&printer);
}

void MainWindow::zipFiles()
{
    int n = this->ui.editorTabs->currentIndex();
    QString fileName = ui.editorTabs->tabToolTip(n);

    if (fileName.isEmpty())
        return;

    QString spinLibPath  = QSettings().value("Library").toString();
    QStringList files = parser->getFileList();

    qDebug() << "zipping files:" << files;

    if(files.count() > 0)
    {
        zipper.makeZip(fileName, files);
    }
}

void MainWindow::setEnableBuild(bool enabled)
{
    cbPort->setEnabled(enabled);
    ui.actionRun->setEnabled(enabled);
    ui.actionWrite->setEnabled(enabled);
    ui.actionTerminal->setEnabled(enabled);
}

void MainWindow::updatePorts()
{
    if(cbPort == NULL)
        return;

    cbPort->clear();

    foreach(QString port, manager.listPorts())
    {
        cbPort->addItem(port);
    }
    if(cbPort->count())
    {
        setEnableBuild(true);
        QStringList latest = manager.latestPorts();
        if (!latest.isEmpty())
        {
            cbPort->setCurrentIndex(cbPort->findText(latest.last()));
        }
    }
    else
    {
        setEnableBuild(false);
    }
}

void MainWindow::spawnTerminal(const QString & portname)
{
    qDebug() << "creating terminal";

    QString port = portname;
    if (port.isEmpty())
        port = cbPort->currentText();

    ColorScheme * theme = &Singleton<ColorScheme>::Instance();
    PropTerm * term = new PropTerm(&manager, port);

    term->setAttribute(Qt::WA_DeleteOnClose);
    term->setWindowIcon(QIcon(":/icons/project-terminal.png"));
    term->setFont(theme->getFont());

    connect(&preferences,   SIGNAL(updateFonts(const QFont &)),     term,   SLOT(setFont(const QFont &)));

    term->show();
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
        }
        else if (e->modifiers() & Qt::ShiftModifier)
        {
            if (QApplication::focusWidget()->parent() == ui.finder)
            {
                switch (e->key())
                {
                    case (Qt::Key_Enter):
                    case (Qt::Key_Return):
                        ui.finder->findPrevious();
                        return true;
                }
            }
        }
        else
        {
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

    QFileInfo fi(path);
    if (!fi.exists() || !fi.isFile())
    {
        qCritical() << tr("File '%1' not found in '%2'").arg(fi.fileName()).arg(fi.path());
        QMessageBox::critical(this,
                tr("File Not Found"),
                tr("<p>Unable to find '<code>%1</code>' at:</p>"
                    "<pre>%2</pre>"
                    "<p>Verify that the file exists and try again.</p>")
                .arg(fi.fileName())
                .arg(fi.path()));
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
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

void MainWindow::propBasicManual()
{
    openFileResource("/doc/pdf/PropBASIC.pdf");
}

void MainWindow::about()
{
    QDialog * about = new QDialog();
    Ui::About ui;
    ui.setupUi(about);
    ui.label_4->setText(tr("Version %1").arg(qApp->applicationVersion()));
    about->show();
}

void MainWindow::newFromTemplate()
{
    getApplicationSettings();

    NewFromTemplate * newfromtemplate = new NewFromTemplate(spinIncludes);
    newfromtemplate->exec();
    
    QString selected = newfromtemplate->selectedTemplate();
    if (!selected.isEmpty())
        ui.editorTabs->newFromFile(selected);

    delete newfromtemplate;
}

void MainWindow::showMessage(const QString & message)
{
    statusBar()->showMessage(message, 2000);
}
