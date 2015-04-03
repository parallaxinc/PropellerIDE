#pragma once

#include <QIcon>
#include <QMainWindow>
#include <QSplitter>
#include <QPlainTextEdit> 
#include <QTreeView> 
#include <QDesktopServices> 
#include <QDirIterator> 

#include "ui_mainwindow.h"

#include "treemodel.h"
#include "Preferences.h"
#include "editor.h"
#include "PortConnectionMonitor.h"
#include "spinzip/zipper.h"
#include "ReferenceTree.h"
#include "FileManager.h"
#include "BuildManager.h"
#include "Finder.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow ui;

public:
    MainWindow(QWidget *parent = 0);

    Preferences  *propDialog;
    QSplitter   *leftSplit;
    QSplitter   *findSplit;

public slots:
    void showMessage(const QString & message);

    // file menu
    void newProjectTrees();
    void printFile();
    void zipFiles();
    void openFiles(const QStringList & files);

    // view
    void fontBigger();
    void fontSmaller();

    // help
    void openFileResource(QString const & resource);
    void propellerManual();
    void propellerDatasheet();
    void propellerQuickReference();
    void about();

    void findMultilineComment(QPoint point);
    void findMultilineComment(QTextCursor cur);
    void projectTreeClicked(QModelIndex index);
    void referenceTreeClicked(QModelIndex index);
    void setCurrentPort(int index);
    void spawnTerminal();
    void setProject();
    void preferences();
    void preferencesAccepted();
    void programBuild();
    void programBurnEE();
    void programRun();
    void programDebug();
    void viewInfo();
    void recolorInfo(QWidget * widget);
    void closeEvent(QCloseEvent *event);
    void quitProgram();

    void enumeratePorts();
    void showBrowser();

    void addRecentFile(const QString &fileName);
    void updateRecentFileActions();
    void openRecentFile();

    void highlightFileLine(QString file, int line);

private:
    void loadSession();
    void saveSession();
    void clearSession();

    void getApplicationSettings();
    void checkAndSaveFiles();

    bool eventFilter(QObject *target, QEvent *event);

    void setupFileMenu();
    void setupEditMenu();
    void setupViewMenu();
    void setupProjectMenu();
    void setupHelpMenu();

    void setupProjectTools(QSplitter *vsplit);
    void openTreeFile(QString fileName);
    void updateProjectTree(QString fileName);
    void updateSpinProjectTree(QString fileName);
    void updateReferenceTree(QString fileName, QString text);
    void updateSpinReferenceTree(QString fileName, QString includes, QString objname, int level);

    typedef enum COMPILE_TYPE { COMPILE_ONLY, COMPILE_RUN, COMPILE_BURN } COMPILE_TYPE_T;
    int  runCompiler(COMPILE_TYPE type);
    int  loadProgram(int type);

    QString     spinCompiler;
    QString     spinIncludes;
    QString     spinTerminal;
    QString     spinLoader;

    QToolBar    *ctrlToolBar;

    QList<QAction *> recentFiles;

    QVBoxLayout *verticalLayout;
    QFrame      *findFrame;

    Finder * finder;
    FileManager     *editorTabs;
    BuildManager    builder;

    QString         projectFile;
    ReferenceTree   *projectTree;
    ReferenceTree   *referenceTree;
    TreeModel       *projectModel;
    TreeModel       *referenceModel;

    QComboBox   *cbPort;

    PortConnectionMonitor *portConnectionMonitor;

    Zipper      zipper;

    enum { LoadRunHubRam = 1 };
    enum { LoadRunEeprom = 2 };

    QMutex      statusMutex;
    bool        statusDone;

signals:
    void signalStatusDone(bool done);

public slots:
    void setStatusDone(bool done);

};
