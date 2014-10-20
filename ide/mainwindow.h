/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qtversion.h"

#include <QIcon>
#include <QMainWindow>
#include <iostream>
#include <exception>
#include "stdio.h"
#include "highlighter.h"
#include "SpinBuilder.h"
#include "XBasicBuilder.h"
#include "treemodel.h"
#include "SpinModel.h"
#include "XBasicModel.h"
#include "PortListener.h"
#include "qextserialport.h"
#include "terminal.h"
#include "terminal2.h"
#include "properties.h"
#include "console.h"
#include "editor.h"
#include "spinparser.h"
#include "PortConnectionMonitor.h"
#include "StatusDialog.h"
#include "zipper.h"

#define untitledstr "Untitled"

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    void init();

    QString     programName;
    Properties  *propDialog;
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
    void about();
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
    void fontDialog();
    void fontBigger();
    void fontSmaller();
    void hardware();
    void properties();
    void propertiesAccepted();
    void programBuild();
    void programBurnEE();
    void programRun();
    void programDebug();
    void findHardware(bool showFoundBox = true);
    void closeEvent(QCloseEvent *event);
    void quitProgram();

    void fileChanged();
    void keyHandler(QKeyEvent* event);
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
    void setupEditor();
    void clearTabHighlight();
    void setupFileMenu();
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
    QString     xBasicCompiler;
    QString     xBasicCompilerPath;
    QString     xBasicIncludes;

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
    QVector<Editor*> *editors;
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
    XBasicModel *xBasicProjectModel;
    XBasicModel *xBasicReferenceModel;

    SpinBuilder     *spinBuilder;
    XBasicBuilder   *xBasicBuilder;

    QString     basicPath;
    QString     includePath;
    QString     lastDirectory;

    QComboBox   *cbBoard;
    QComboBox   *cbPort;
    QToolButton *btnConnected;
    Console     *termEditor;
    PortListener *portListener;
    Terminal2   *term;
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
//! [0]

// Macro which connects a signal to a slot, and which causes application to
// abort if the connection fails.  This is intended to catch programming errors
// such as mis-typing a signal or slot name.  It is necessary to write our own
// macro to do this - the following idiom
//     Q_ASSERT(connect(source, signal, receiver, slot));
// will not work because Q_ASSERT compiles to a no-op in release builds.

#define CHECKED_CONNECT(source, signal, receiver, slot) \
    if(!connect(source, signal, receiver, slot)) \
        qt_assert_x(Q_FUNC_INFO, "CHECKED_CONNECT failed", __FILE__, __LINE__);

#endif
