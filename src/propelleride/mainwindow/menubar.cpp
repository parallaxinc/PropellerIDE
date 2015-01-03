#include "mainwindow.h"

#include <QString>
#include <QUrl>
#include <QDesktopServices>
#include <QMenuBar> 
#include <QMenu> 
#include <QKeySequence>
#include <QAction>
#include <QList>
#include <QMessageBox>

void MainWindow::setupFileMenu()
{
    QMenu *fileMenu = new QMenu(trUtf8("&File"), this);
    menuBar()->addMenu(fileMenu);

    fileMenu->addAction(QIcon(":/icons/file-new.png"),      trUtf8("&New"),         this, SLOT(newFileAction()),    QKeySequence::New);
    fileMenu->addAction(QIcon(":/icons/file-open.png"),     trUtf8("&Open"),        this, SLOT(openFile()),         QKeySequence::Open);

    fileMenu->addSeparator();

    fileMenu->addAction(QIcon(":/icons/file-save.png"),     trUtf8("&Save"),        this, SLOT(saveFile()),         QKeySequence::Save);
    fileMenu->addAction(QIcon(":/icons/file-save-as.png"),  trUtf8("Save &As"),     this, SLOT(saveAsFile()),       QKeySequence::SaveAs);

    fileMenu->addSeparator();

    // fileMenu->addAction(QIcon(":/icons/file-print.png"), trUtf8("Print"),        this, SLOT(printFile()),        QKeySequence::Print);
    fileMenu->addAction(QIcon(":/icons/file-zip.png"),      trUtf8("Zip Project"),  this, SLOT(zipFiles()),         0);

    separatorFileAct = fileMenu->addSeparator();

    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),this, SLOT(openRecentFile()));
        fileMenu->addAction(recentFileActs[i]);
    }

    updateRecentFileActions();

    fileMenu->addSeparator();

    fileMenu->addAction(QIcon(":/icons/file-exit.png"),     trUtf8("E&xit"),        this, SLOT(quitProgram()),      QKeySequence::Quit);

}

void MainWindow::setupEditMenu()
{

    QMenu * editMenu = new QMenu(trUtf8("&Edit"), this);
    menuBar()->addMenu(editMenu);

    editMenu->addAction(QIcon(":/icons/edit-undo.png"),     trUtf8("&Undo"),        this, SLOT(undo()),             QKeySequence::Undo);
    editMenu->addAction(QIcon(":/icons/edit-redo.png"),     trUtf8("&Redo"),        this, SLOT(redo()),             QKeySequence::Redo);

    editMenu->addSeparator();

    editMenu->addAction(QIcon(":/icons/edit-cut.png"),      trUtf8("&Cut"),         this, SLOT(cut()),              QKeySequence::Cut);
    editMenu->addAction(QIcon(":/icons/edit-copy.png"),     trUtf8("&Copy"),        this, SLOT(copy()),             QKeySequence::Copy);
    editMenu->addAction(QIcon(":/icons/edit-paste.png"),    trUtf8("&Paste"),       this, SLOT(paste()),            QKeySequence::Paste);
    editMenu->addAction(QIcon(":/icons/edit-selectall.png"),trUtf8("&Select All"),  this, SLOT(selectAll()),        QKeySequence::SelectAll);

    editMenu->addSeparator();

    editMenu->addAction(QIcon(":/icons/edit-find.png"),     trUtf8("Find"),         this, SLOT(showFindFrame()),    QKeySequence::Find);
    editMenu->addAction(QIcon(":/icons/go-next.png"),       trUtf8("Find Next"),    this, SLOT(findNextClicked()),  QKeySequence::FindNext);
    editMenu->addAction(QIcon(":/icons/go-previous.png"),   trUtf8("Find Previous"),this, SLOT(findPrevClicked()),  QKeySequence::FindPrevious);

    editMenu->addSeparator();

    editMenu->addAction(QIcon(":/icons/preferences.png"), trUtf8("Preferences"),    this, SLOT(preferences()),      Qt::Key_F5);
}

void MainWindow::setupViewMenu()
{
    QMenu * viewMenu = new QMenu(trUtf8("&View"), this);
    menuBar()->addMenu(viewMenu);

    QAction * browser = new QAction(trUtf8("&Show Browser"), this);
    browser->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_B));
    browser->setCheckable(true);
    browser->setChecked(true);
    connect(browser, SIGNAL(triggered()), this, SLOT(showBrowser()));
    viewMenu->addAction(browser);

    viewMenu->addSeparator();

    viewMenu->addAction(QIcon(":/icons/preferences-font-smaller.png"),  trUtf8("Smaller Font"), this, SLOT(fontSmaller()), QKeySequence::ZoomOut);
    QAction *bigger = new QAction(QIcon(":/icons/preferences-font-bigger.png"), trUtf8("Bigger Font"), this);

    /* special provision for bigger fonts to use default ZoomIn or Ctrl+= */
    QList<QKeySequence> biggerKeys;
    biggerKeys.append(QKeySequence::ZoomIn);
    biggerKeys.append(QKeySequence(Qt::CTRL+Qt::Key_Equal));
    bigger->setShortcuts(biggerKeys);

    connect(bigger,SIGNAL(triggered()),this,SLOT(fontBigger()));

    /* insert action before smaller font action */
    QList<QAction*> alist = viewMenu->actions();
    QAction *last = alist.last();
    viewMenu->insertAction(last,bigger);

}


void MainWindow::setupProjectMenu()
{
    QMenu * projectMenu = new QMenu(trUtf8("&Project"), this);
    menuBar()->addMenu(projectMenu);

    projectMenu->addAction(QIcon(":/icons/project-identify.png"), trUtf8("Identify Hardware"), this, SLOT(findHardware()), Qt::Key_F7);
    projectMenu->addAction(QIcon(":/icons/project-terminal.png"), trUtf8("Debug"), this, SLOT(programDebug()), Qt::Key_F8);
    projectMenu->addAction(QIcon(":/icons/project-build.png"), trUtf8("Build"), this, SLOT(programBuild()), Qt::Key_F9);
    projectMenu->addAction(QIcon(":/icons/project-run.png"), trUtf8("Run"), this, SLOT(programRun()), Qt::Key_F10);
    projectMenu->addAction(QIcon(":/icons/project-burn.png"), trUtf8("Burn"), this, SLOT(programBurnEE()), Qt::Key_F11);
}


void MainWindow::setupHelpMenu()
{
    QMenu *helpMenu = new QMenu(trUtf8("&Help"), this);
    menuBar()->addMenu(helpMenu);

    helpMenu->addAction(QIcon(":/icons/help-datasheet.png"), trUtf8("Propeller &Datasheet"), this, SLOT(propellerDatasheet()));
    helpMenu->addAction(QIcon(":/icons/help-manual.png"), trUtf8("Propeller &Manual"), this, SLOT(propellerManual()));

    helpMenu->addSeparator();

    helpMenu->addAction(QIcon(":/icons/help-about.png"), trUtf8("&About"), this, SLOT(about()));
}

void openFileResource(QString const & resource)
{
    QString path = QApplication::applicationDirPath()
            + QString(APP_RESOURCES_PATH)
            + resource;
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

void MainWindow::about()
{
    QString version = QString(QCoreApplication::applicationName() + " v" +VERSION);
    QMessageBox::about(this, trUtf8("About") + " " + QCoreApplication::applicationName(),
           "<h2>" + version + "</h2>"
           "<p>PropellerIDE is an easy-to-use, cross-platform development tool for the Parallax Propeller microcontroller.</p>"
           "<p>Use it for writing Spin code, downloading programs to your Propeller board, and debugging your applications with the built-in serial terminal.<p>"
           "<p>PropellerIDE is built in Qt and is fully cross-platform.</p>"

           "<h3>Credits</h3>"
           "<p>Copyright &copy; 2014 by Parallax, Inc.</p>"
           "<p>Originally developed by Steve Denson, Dennis Gately, and Roy Eltham. "
           "Now developed by LameStation LLC in collaboration with Parallax.</p>");
}


void MainWindow::cut()
{
    getEditor(editorTabs->currentIndex())->cut();
}

void MainWindow::copy()
{
    getEditor(editorTabs->currentIndex())->copy();
}

void MainWindow::paste()
{
    getEditor(editorTabs->currentIndex())->paste();
}

void MainWindow::undo()
{
    getEditor(editorTabs->currentIndex())->undo();
}

void MainWindow::redo()
{
    getEditor(editorTabs->currentIndex())->redo();
}

void MainWindow::selectAll()
{
    getEditor(editorTabs->currentIndex())->selectAll();
}


