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
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);

    fileMenu->addAction(QIcon(":/icons/newfile.png"), tr("&New"), this, SLOT(newFileAction()), QKeySequence::New);
    fileMenu->addAction(QIcon(":/icons/openfile.png"), tr("&Open"), this, SLOT(openFile()), QKeySequence::Open);

    fileMenu->addSeparator();

    fileMenu->addAction(QIcon(":/icons/savefile.png"), tr("&Save"), this, SLOT(saveFile()), QKeySequence::Save);
    fileMenu->addAction(QIcon(":/icons/saveasfile.png"), tr("Save &As"), this, SLOT(saveAsFile()), QKeySequence::SaveAs);

    fileMenu->addSeparator();

    // fileMenu->addAction(QIcon(":/icons/print.png"), tr("Print"), this, SLOT(printFile()), QKeySequence::Print);

    // Enable zipFiles after zipper works.
    fileMenu->addAction(QIcon(":/icons/zip.png"), tr("Zip Project"), this, SLOT(zipFiles()), 0);


    // recent file actions
    separatorFileAct = fileMenu->addSeparator();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),this, SLOT(openRecentFile()));
    }

    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActs[i]);

    updateRecentFileActions();

    fileMenu->addSeparator();

    fileMenu->addAction(QIcon(":/icons/exit.png"), tr("E&xit"), this, SLOT(quitProgram()), QKeySequence::Quit);

}

void MainWindow::setupEditMenu()
{

    QMenu * editMenu = new QMenu(tr("&Edit"), this);
    menuBar()->addMenu(editMenu);

    editMenu->addAction(QIcon::fromTheme("edit-undo"), tr("&Undo"), this, SLOT(undo()), QKeySequence::Undo);
    editMenu->addAction(QIcon::fromTheme("edit-redo"), tr("&Redo"), this, SLOT(redo()), QKeySequence::Redo);

    editMenu->addSeparator();

    editMenu->addAction(QIcon::fromTheme("edit-find"), tr("Find"), this, SLOT(showFindFrame()), QKeySequence::Find);
    editMenu->addAction(QIcon::fromTheme("go-next"), tr("Find Next"), this, SLOT(findNextClicked()), QKeySequence::FindNext);
    editMenu->addAction(QIcon::fromTheme("go-previous"), tr("Find Previous"), this, SLOT(findPrevClicked()), QKeySequence::FindPrevious);

    editMenu->addSeparator();

    editMenu->addAction(QIcon(":/icons/preferences.png"), tr("Preferences"), this, SLOT(preferences()), Qt::Key_F5);
}


void MainWindow::undo()
{
    int index = editorTabs->currentIndex();
    Editor * editor = getEditor(index);
    editor->undo();
}

void MainWindow::redo()
{
    int index = editorTabs->currentIndex();
    Editor * editor = getEditor(index);
    editor->redo();
}


void MainWindow::setupViewMenu()
{
    QMenu * viewMenu = new QMenu(tr("&View"), this);
    menuBar()->addMenu(viewMenu);

    QAction * browser = new QAction(tr("&Show Browser"), this);
    browser->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_B));
    browser->setCheckable(true);
    browser->setChecked(true);
    connect(browser, SIGNAL(triggered()), this, SLOT(showBrowser()));
    viewMenu->addAction(browser);

    viewMenu->addSeparator();

#ifdef Q_OS_MAC
    viewMenu->addAction(QIcon(":/icons/FontTT.png"), tr("Font"), this, SLOT(fontDialog()), QKeySequence(tr("Ctrl+T")));
#else
    viewMenu->addAction(QIcon(":/icons/FontTT.png"), tr("Font"), this, SLOT(fontDialog()));
#endif

    viewMenu->addAction(QIcon(":/icons/fontsmaller.png"), tr("Smaller Font"), this, SLOT(fontSmaller()), QKeySequence::ZoomOut);

    /* special provision for bigger fonts to use default ZoomIn or Ctrl+= */
    QAction *bigger = new QAction(QIcon(":/icons/fontbigger.png"), tr("Bigger Font"), this);
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
    QMenu * projectMenu = new QMenu(tr("&Project"), this);
    menuBar()->addMenu(projectMenu);

    projectMenu->addAction(QIcon(":/icons/PropHatAlpha.png"), tr("Identify Hardware"), this, SLOT(findHardware()), Qt::Key_F7);
    projectMenu->addAction(QIcon(":/icons/debug2.png"), tr("Debug"), this, SLOT(programDebug()), Qt::Key_F8);
    projectMenu->addAction(QIcon(":/icons/build2.png"), tr("Build"), this, SLOT(programBuild()), Qt::Key_F9);
    projectMenu->addAction(QIcon(":/icons/run.png"), tr("Run"), this, SLOT(programRun()), Qt::Key_F10);
    projectMenu->addAction(QIcon(":/icons/burnee.png"), tr("Burn"), this, SLOT(programBurnEE()), Qt::Key_F11);
#if defined(IDEDEBUG)
    projectMenu->addAction(QIcon(":/icons/Eye.png"), tr("IDE Debug Console"), this, SLOT(ideDebugConsole()));
#endif
}


void MainWindow::setupHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(helpMenu);

    helpMenu->addAction(QIcon(":/icons/helpsymbol.png"), tr("Propeller &Datasheet"), this, SLOT(propellerDatasheet()));
    helpMenu->addAction(QIcon(":/icons/helpsymbol.png"), tr("Propeller &Manual"), this, SLOT(propellerManual()));

    helpMenu->addSeparator();

    helpMenu->addAction(QIcon::fromTheme("help-about"), tr("&About"), this, SLOT(about()));
    helpMenu->addAction(tr("About &Qt"), this, SLOT(aboutQt()));
}

void MainWindow::propellerManual()
{
    QString path = "/usr/share/propelleride/doc/pdf/P8X32A-Web-PropellerManual-v1.2_0.pdf";
    QUrl pathUrl = QUrl::fromLocalFile(path);
    QDesktopServices::openUrl(pathUrl);
}
    
    
void MainWindow::propellerDatasheet()
{
    QString path = "/usr/share/propelleride/doc/pdf/P8X32A-Propeller-Datasheet-v1.4.0_0.pdf";
    QUrl pathUrl = QUrl::fromLocalFile(path);
    QDesktopServices::openUrl(pathUrl);
}

void MainWindow::about()
{
    QString version = QString(programName + " Version %1.%2")
            .arg(IDEVERSION).arg(MINVERSION);
    QMessageBox::about(this, tr("About") + " " + programName,
           "<h2>" + version + "</h2>"
           "<p>PropellerIDE is an easy-to-use, cross-platform development tool for the Parallax Propeller microcontroller.</p>"
           "<p>Use it for writing Spin code, downloading programs to your Propeller board, and debugging your applications with the built-in serial terminal.<p>"
           "<p>PropellerIDE is built in Qt and is fully cross-platform.</p>"

           "<h3>Credits</h3>"
           "<p>Copyright &copy; 2014 by Parallax, Inc.</p>"
           "<p>Originally developed by Steve Denson, Dennis Gately, and Roy Eltham."
           "Now developed by LameStation LLC in collaboration with Parallax.</p>"
           "<h4>Get the source:</p>"
           "<ul><li><a href=\"https://github.com/parallaxinc/PropellerIDE\">PropellerIDE Repository</a></li>"
           "<li><a href=\"https://github.com/parallaxinc/OpenSpin\">OpenSpin Repository</a></li></ul>");
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}
