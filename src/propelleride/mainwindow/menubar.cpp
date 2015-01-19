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


    QAction * actSave = new QAction(QIcon(":/icons/file-save.png"), tr("&Save"), this);
    actSave->setShortcut(QKeySequence::Save);
    actSave->setEnabled(false);

    fileMenu->addAction(QIcon(":/icons/file-new.png"),      tr("&New"),         editorTabs, SLOT(newFile()),    QKeySequence::New);
    fileMenu->addAction(QIcon(":/icons/file-open.png"),     tr("&Open"),        editorTabs, SLOT(open()),         QKeySequence::Open);

    fileMenu->addSeparator();

    fileMenu->addAction(actSave);
    fileMenu->addAction(QIcon(":/icons/file-save-as.png"),  tr("Save &As..."),     editorTabs, SLOT(saveAs()),     QKeySequence::SaveAs);
    fileMenu->addAction(QIcon(),                            tr("Save All"),     editorTabs, SLOT(saveAll()),    0);

    fileMenu->addSeparator();

    // fileMenu->addAction(QIcon(":/icons/file-print.png"), tr("Print"),        this, SLOT(printFile()),        QKeySequence::Print);
    fileMenu->addAction(QIcon(":/icons/file-zip.png"),      tr("Zip Project"),  this, SLOT(zipFiles()),         0);

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

    QAction * actClose = new QAction(QIcon(":/icons/file-close.png"), tr("&Close"), this);
    actClose->setShortcut(QKeySequence::Close);
    actClose->setEnabled(false);

    fileMenu->addAction(actClose);
    fileMenu->addAction(QIcon(),                            tr("Close All"),    editorTabs, SLOT(closeAll()),   0);
    fileMenu->addAction(QIcon(":/icons/file-exit.png"),     tr("E&xit"),        this, SLOT(quitProgram()),      QKeySequence::Quit);

    connect(actSave, SIGNAL(triggered()), editorTabs, SLOT(save()));
    connect(actClose, SIGNAL(triggered()), editorTabs, SLOT(closeFile()));

    connect(editorTabs,SIGNAL(saveAvailable(bool)),actSave,SLOT(setEnabled(bool)));
    connect(editorTabs,SIGNAL(closeAvailable(bool)),actClose,SLOT(setEnabled(bool)));

}


// edit menu

void MainWindow::setupEditMenu()
{

    QMenu * editMenu = new QMenu(tr("&Edit"), this);
    menuBar()->addMenu(editMenu);

    QAction * actUndo = new QAction(QIcon(":/icons/edit-undo.png"), tr("&Undo"), this);
    actUndo->setShortcut(QKeySequence::Undo);
    actUndo->setEnabled(false);

    QAction * actRedo = new QAction(QIcon(":/icons/edit-redo.png"), tr("&Redo"), this);
    actRedo->setShortcut(QKeySequence::Redo);
    actRedo->setEnabled(false);

    QAction * actCut = new QAction(QIcon(":/icons/edit-cut.png"), tr("&Cut"), this);
    actCut->setShortcut(QKeySequence::Cut);
    actCut->setEnabled(false);

    QAction * actCopy = new QAction(QIcon(":/icons/edit-copy.png"), tr("&Copy"), this);
    actCopy->setShortcut(QKeySequence::Copy);
    actCopy->setEnabled(false);

    editMenu->addAction(actUndo);
    editMenu->addAction(actRedo);

    editMenu->addSeparator();

    editMenu->addAction(actCut);
    editMenu->addAction(actCopy);

    editMenu->addAction(QIcon(":/icons/edit-paste.png"),    tr("&Paste"),       editorTabs, SLOT(paste()),            QKeySequence::Paste);
    editMenu->addAction(QIcon(":/icons/edit-selectall.png"),tr("&Select All"),  editorTabs, SLOT(selectAll()),        QKeySequence::SelectAll);

    editMenu->addSeparator();

    editMenu->addAction(QIcon(":/icons/edit-find.png"),     tr("Find"),         this, SLOT(showFindFrame()),    QKeySequence::Find);
    editMenu->addAction(QIcon(":/icons/go-next.png"),       tr("Find Next"),    this, SLOT(findNextClicked()),  QKeySequence::FindNext);
    editMenu->addAction(QIcon(":/icons/go-previous.png"),   tr("Find Previous"),this, SLOT(findPrevClicked()),  QKeySequence::FindPrevious);

    editMenu->addSeparator();

    editMenu->addAction(QIcon(":/icons/preferences.png"), tr("Preferences"),    this, SLOT(preferences()),      Qt::Key_F5);

    connect(actUndo, SIGNAL(triggered()), editorTabs, SLOT(undo()));
    connect(actRedo, SIGNAL(triggered()), editorTabs, SLOT(redo()));
    connect(actCut, SIGNAL(triggered()), editorTabs, SLOT(cut()));
    connect(actCopy, SIGNAL(triggered()), editorTabs, SLOT(copy()));

    connect(editorTabs,SIGNAL(undoAvailable(bool)),actUndo,SLOT(setEnabled(bool)));
    connect(editorTabs,SIGNAL(redoAvailable(bool)),actRedo,SLOT(setEnabled(bool)));
    connect(editorTabs,SIGNAL(copyAvailable(bool)),actCut,SLOT(setEnabled(bool)));
    connect(editorTabs,SIGNAL(copyAvailable(bool)),actCopy,SLOT(setEnabled(bool)));

}

void MainWindow::preferences()
{
    propDialog->showPreferences();
}


void MainWindow::preferencesAccepted()
{
    getApplicationSettings(false);
}


// view menu

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

    viewMenu->addAction(QIcon(":/icons/preferences-font-smaller.png"),  tr("Smaller Font"), this, SLOT(fontSmaller()), QKeySequence::ZoomOut);
    QAction *bigger = new QAction(QIcon(":/icons/preferences-font-bigger.png"), tr("Bigger Font"), this);

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

void MainWindow::fontBigger()
{
    propDialog->adjustFontSize(1.25);
}

void MainWindow::fontSmaller()
{
    propDialog->adjustFontSize(0.8);
}


// project menu

void MainWindow::setupProjectMenu()
{
    QMenu * projectMenu = new QMenu(tr("&Project"), this);
    menuBar()->addMenu(projectMenu);

    projectMenu->addAction(QIcon(":/icons/project-identify.png"), tr("Identify Hardware"), this, SLOT(findHardware()), Qt::Key_F7);
    projectMenu->addAction(QIcon(":/icons/project-terminal.png"), tr("Debug"), this, SLOT(programDebug()), Qt::Key_F8);
    projectMenu->addAction(QIcon(":/icons/project-build.png"), tr("Build"), this, SLOT(programBuild()), Qt::Key_F9);
    projectMenu->addAction(QIcon(":/icons/project-run.png"), tr("Run"), this, SLOT(programRun()), Qt::Key_F10);
    projectMenu->addAction(QIcon(":/icons/project-burn.png"), tr("Burn"), this, SLOT(programBurnEE()), Qt::Key_F11);
}


// help menu

void MainWindow::setupHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(helpMenu);

    helpMenu->addAction(QIcon(":/icons/help-datasheet.png"), tr("Propeller &Datasheet"), this, SLOT(propellerDatasheet()));
    helpMenu->addAction(QIcon(":/icons/help-manual.png"), tr("Propeller &Manual"), this, SLOT(propellerManual()));

    helpMenu->addSeparator();

    helpMenu->addAction(QIcon(":/icons/help-about.png"), tr("&About"), this, SLOT(about()));
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
    QString version = QString(QCoreApplication::applicationName() 
                     + " v" + QCoreApplication::applicationVersion()
                     );
    QMessageBox::about(this, tr("About") + " " + QCoreApplication::applicationName(),
           "<h2>" + version + "</h2>"
           "<p>PropellerIDE is an easy-to-use, cross-platform development tool for the Parallax Propeller microcontroller.</p>"
           "<p>Use it for writing Spin code, downloading programs to your Propeller board, and debugging your applications with the built-in serial terminal.<p>"
           "<p>PropellerIDE is built in Qt and is fully cross-platform.</p>"

           "<h3>Credits</h3>"
           "<p>Copyright &copy; 2014 by Parallax, Inc.</p>"
           "<p>Originally developed by Steve Denson, Dennis Gately, and Roy Eltham. "
           "Now developed by LameStation LLC in collaboration with Parallax.</p>");
}

