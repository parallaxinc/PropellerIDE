#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <exception>

#include <stdio.h>

#include <QIcon>
#include <QMainWindow>
#include <QSplitter>
#include <QPlainTextEdit> 
#include <QTreeView> 

#include "highlighter.h"
#include "SpinBuilder.h"
#include "SpinParser.h"
#include "SpinModel.h"

#include "PortListener.h"
#include "qext/qextserialport.h"
#include "terminal.h"
#include "Preferences.h"
#include "console.h"
#include "editor.h"
#include "PortConnectionMonitor.h"
#include "StatusDialog.h"
#include "spinzip/zipper.h"

#define untitledstr "Untitled"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    void init();

    QString     programName;
    Preferences  *propDialog;
    QSplitter   *leftSplit;
    QSplitter   *rightSplit;

public:
    QTextDocument::FindFlag getFlags(int prev = 0);

    QString getFindText();
    void    clearFindText();
    QString getReplaceText();
    void    clearReplaceText();
    void    setFindText(QString text);

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
    void highlightCurrentLine(QColor lineColor);
    void updateBackgroundColors();

public slots:
    void terminalEditorTextChanged();
    void newFile();
    void newFileAction();
    void openFile(const QString &path = QString());
    bool saveAsCodec(QString fileName, Editor *ed);
    void saveFile();
    void saveFileByTabIndex(int tab);
    QString saveAsFile(const QString &path = QString());
    void printFile();    // not implemented
    void zipFiles();


    // edit
    void undo();
    void redo();


    // view
    void fontDialog();
    void fontBigger();
    void fontSmaller();

    // help
    void propellerManual();
    void propellerDatasheet();
    void about();
    void aboutQt();

    void findMultilineComment(QPoint point);
    void findMultilineComment(QTextCursor cur);
    void projectTreeClicked(QModelIndex index);
    void referenceTreeClicked(QModelIndex index);
    void closeTab(int index = 0);
    void changeTab(int index);
    void setCurrentPort(int index);
    void connectButton(bool show = true);
    void terminalClosed();
    void setProject();
    void hardware();
    void preferences();
    void preferencesAccepted();
    void programBuild();
    void programBurnEE();
    void programRun();
    void programDebug();
    void findHardware(bool showFoundBox = true);
    void closeEvent(QCloseEvent *event);
    void quitProgram();

    void fileChanged();
    void enumeratePorts();
    void enumeratePortsEvent();
    void initBoardTypes();
    void showFindFrame();
    void hideFindFrame();
    void showBrowser();

    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    void openRecentFile();

    void highlightFileLine(QString file, int line);

    void ideDebugConsole();
    void tabSpacesChanged();

private:
    void openLastFile();
    bool exitSave();
    void getApplicationSettings(bool complain);
    int  checkCompilerInfo();
    QStringList getCompilerParameters(QString compilerOptions);
    bool isFileUTF16(QFile *file);
    void openFileName(QString fileName);
    void checkAndSaveFiles();
    Editor *createEditor();
    void clearTabHighlight();

    void setupFileMenu();
    void setupEditMenu();
    void setupViewMenu();
    void setupProjectMenu();
    void setupHelpMenu();

    void setupToolBars();
    void setupProjectTools(QSplitter *vsplit);
    void addToolButton(QToolBar *bar, QToolButton *btn, QString imgfile);
    int isFileOpen(QString fileName);
    void openTreeFile(QString fileName);
    void updateProjectTree(QString fileName, QString text);
    void updateSpinProjectTree(QString fileName);
    void updateReferenceTree(QString fileName, QString text);
    void updateSpinReferenceTree(QString fileName, QString includes, QString objname, int level);
    void setEditor(int num, QString shortName, QString fileName, QString text);
    Editor *getEditor(int num);
    void setEditorCodeType(Editor *ed, QString name);

    QString shortFileName(QString fileName);
    QString filePathName(QString fileName);
    void checkConfigSerialPort();
    QFrame *newFindFrame(QSplitter *split);

    typedef enum COMPILE_TYPE { COMPILE_ONLY, COMPILE_RUN, COMPILE_BURN } COMPILE_TYPE_T;
    int  runCompiler(COMPILE_TYPE type);
    int  loadProgram(int type, bool closePort, QString file = QString());

    int  isPackageSource(QString fileName);
    int  extractSource(QString &fileName);
    void buildSourceWriteError(QString fileName);

    QSettings   *settings;
    QString     spinCompiler;
    QString     spinCompilerPath;
    QString     spinIncludes;
    QString     spinLoader;
    Highlighter *highlighter;

    QToolBar    *fileToolBar;
    QToolBar    *propToolBar;
    QToolBar    *debugToolBar;
    QToolBar    *ctrlToolBar;

    enum { MaxRecentFiles = 10 };
    QAction *recentFileActs[MaxRecentFiles];
    QAction *separatorFileAct;

    QPushButton *regexButton;
    QString     findText;
    QString     replaceText;
    
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

    QTabWidget  *editorTabs;
    QFont       editorFont;
    bool        fileChangeDisable;
    bool        changeTabDisable;

    SpinParser  spinParser;

    QString     projectFile;
    QTreeView   *projectTree;
    QTreeView   *referenceTree;
    TreeModel   *projectModel;
    TreeModel   *referenceModel;
    SpinModel   *spinProjectModel;
    SpinModel   *spinReferenceModel;

    SpinBuilder     *spinBuilder;

    QString     basicPath;
    QString     includePath;
    QString     lastDirectory;

    QComboBox   *cbBoard;
    QComboBox   *cbPort;
    QToolButton *btnConnected;
    Console     *termEditor;
    PortListener *portListener;
    Terminal    *term;
    int         termXpos;
    int         termYpos;

    QString     boardName;

    QProcess    *proc;

    int progMax;
    int progCount;
    QLabel *sizeLabel;
    QLabel *msgLabel ;
    QProgressBar *progress;
    QString compileResult;

    PortConnectionMonitor *portConnectionMonitor;

    Zipper      zipper;
    QDialog     *ideDebugFrame;

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


#endif// MAINWINDOW_H
