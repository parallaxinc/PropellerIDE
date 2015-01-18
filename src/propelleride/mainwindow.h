#pragma once

#include <iostream>
#include <exception>

#include <stdio.h>

#include <QIcon>
#include <QMainWindow>
#include <QSplitter>
#include <QPlainTextEdit> 
#include <QTreeView> 

#include "SpinBuilder.h"
#include "SpinParser.h"

#include "treemodel.h"

#include "PortListener.h"
#include "qext/qextserialport.h"
#include "Terminal.h"
#include "Preferences.h"
#include "editor.h"
#include "PortConnectionMonitor.h"
#include "StatusDialog.h"
#include "spinzip/zipper.h"
#include "ReferenceTree.h"
#include "FileManager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    void init();

    Preferences  *propDialog;
    QSplitter   *leftSplit;
    QSplitter   *findSplit;

public:
    QTextDocument::FindFlag getFlags(int prev = 0);

    bool showBeginMessage(QString type);
    bool showEndMessage(QString type);

    void setEditor(QPlainTextEdit *ed);

public slots:
    void findChanged(QString text);
    void findClicked();
    void findNextClicked();
    void findPrevClicked();
    void replaceClicked();
    void replaceNextClicked();
    void replacePrevClicked();
    void replaceAllClicked();

signals:
    void doPortEnumerate();
    void updateBackgroundColors();

public slots:

    // file menu
    void newProjectTrees();
    void printFile();
    void zipFiles();
    void openFiles(const QStringList & files);

    // edit
    void cut();
    void copy();
    void paste();
    void undo();
    void redo();
    void selectAll();

    // view
    void fontBigger();
    void fontSmaller();

    // help
    void propellerManual();
    void propellerDatasheet();
    void about();

    void findMultilineComment(QPoint point);
    void findMultilineComment(QTextCursor cur);
    void projectTreeClicked(QModelIndex index);
    void referenceTreeClicked(QModelIndex index);
    void changeTab(int index = 0);
    void setCurrentPort(int index);
    void connectButton(bool show = true);
    void terminalClosed();
    void setProject();
    void preferences();
    void preferencesAccepted();
    void programBuild();
    void programBurnEE();
    void programRun();
    void programDebug();
    void findHardware(bool showFoundBox = true);
    void closeEvent(QCloseEvent *event);
    void quitProgram();

    void enumeratePorts();
    void enumeratePortsEvent();
    void showFindFrame();
    void hideFindFrame();
    void showBrowser();

    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    void openRecentFile();

    void highlightFileLine(QString file, int line);


private:
    void openLastFile();
    bool exitSave();
    void getApplicationSettings(bool complain);
    int  checkCompilerInfo();
    QStringList getCompilerParameters(QString compilerOptions);
    void checkAndSaveFiles();
    Editor *createEditor();

    bool eventFilter(QObject *target, QEvent *event);

    void setupFileMenu();
    void setupEditMenu();
    void setupViewMenu();
    void setupProjectMenu();
    void setupHelpMenu();

    void setupToolBars();
    void setupProjectTools(QSplitter *vsplit);
    void addToolButton(QToolBar *bar, QToolButton *btn, QString imgfile);
    int  isFileOpen(QString fileName);
    void openTreeFile(QString fileName);
    void updateProjectTree(QString fileName);
    void updateSpinProjectTree(QString fileName);
    void updateReferenceTree(QString fileName, QString text);
    void updateSpinReferenceTree(QString fileName, QString includes, QString objname, int level);

    void checkConfigSerialPort();
    QFrame *newFindFrame(QSplitter *split);

    typedef enum COMPILE_TYPE { COMPILE_ONLY, COMPILE_RUN, COMPILE_BURN } COMPILE_TYPE_T;
    int  runCompiler(COMPILE_TYPE type);
    int  loadProgram(int type, QString file = QString());

    QString     spinCompiler;
    QString     spinCompilerPath;
    QString     spinIncludes;
    QString     spinLoader;

    QToolBar    *fileToolBar;
    QToolBar    *propToolBar;
    QToolBar    *debugToolBar;
    QToolBar    *ctrlToolBar;

    enum { MaxRecentFiles = 10 };
    QAction *recentFileActs[MaxRecentFiles];
    QAction *separatorFileAct;

    QString     findText;
    QString     replaceText;
    bool        showFindMessage(QString type);
    
    QHBoxLayout *findLayout;
    QLineEdit   *findEdit;
    QLabel      *findLabel;
    QToolButton *findPreviousBtn;
    QToolButton *findNextBtn;
    QToolButton *wholeWordFilterBtn;
    QToolButton *caseSensitiveFilterBtn;
    QToolButton *doneFindBtn;
    
    QHBoxLayout *replaceLayout;
    QLineEdit   *replaceEdit;
    QLabel      *replaceLabel;
    QToolButton *replacePreviousBtn;
    QToolButton *replaceNextBtn;
    QToolButton *replaceAllBtn;

    QVBoxLayout *verticalLayout;
    QFrame      *findFrame;

    /* last find position */
    int         findPosition;
    int         wasClicked;

    FileManager *editorTabs;

    QString     projectFile;
    ReferenceTree   *projectTree;
    ReferenceTree   *referenceTree;
    TreeModel       *projectModel;
    TreeModel       *referenceModel;

    SpinBuilder     *spinBuilder;

    QString     basicPath;
    QString     includePath;
    QString     lastDirectory;

    QComboBox   *cbBoard;
    QComboBox   *cbPort;
    QToolButton *btnConnected;
    PortListener *portListener;
    Terminal    *term;
    int         termXpos;
    int         termYpos;

    QString     boardName;

    QProcess    *proc;

    int progMax;
    int progCount;
    QLabel sizeLabel;
    QLabel msgLabel;
    QProgressBar *progress;
    QString compileResult;

    PortConnectionMonitor *portConnectionMonitor;

    Zipper      zipper;

    enum { LoadRunHubRam = 1 };
    enum { LoadRunEeprom = 2 };

    StatusDialog *statusDialog;
    QMutex      statusMutex;
    bool        statusDone;

signals:
    void signalStatusDone(bool done);

public slots:
    void setStatusDone(bool done);

};
