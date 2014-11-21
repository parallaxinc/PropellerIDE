#include "mainwindow.h"

#include <QToolBar> 

void MainWindow::setupToolBars()
{
    fileToolBar = addToolBar(tr("File"));
//    QToolButton *btnBrowser = new QToolButton(this);
    QToolButton *btnFileNew = new QToolButton(this);
    QToolButton *btnFileOpen = new QToolButton(this);
    QToolButton *btnFileSave = new QToolButton(this);
    QToolButton *btnFileSaveAs = new QToolButton(this);

//    btnBrowser->setCheckable(true);
//    addToolButton(fileToolBar, btnBrowser, QString(":/icons/blks.png"));
    addToolButton(fileToolBar, btnFileNew, QString(":/icons/newfile.png"));
    addToolButton(fileToolBar, btnFileOpen, QString(":/icons/openfile.png"));
    addToolButton(fileToolBar, btnFileSave, QString(":/icons/savefile.png"));
    addToolButton(fileToolBar, btnFileSaveAs, QString(":/icons/saveasfile.png"));

//    connect(btnBrowser,SIGNAL(clicked()),this,SLOT(showBrowser()));
    connect(btnFileNew,SIGNAL(clicked()),this,SLOT(newFileAction()));
    connect(btnFileOpen,SIGNAL(clicked()),this,SLOT(openFile()));
    connect(btnFileSave,SIGNAL(clicked()),this,SLOT(saveFile()));
    connect(btnFileSaveAs,SIGNAL(clicked()),this,SLOT(saveAsFile()));

//    btnBrowser->setToolTip(tr("Show Browser"));
    btnFileNew->setToolTip(tr("New"));
    btnFileOpen->setToolTip(tr("Open"));
    btnFileSave->setToolTip(tr("Save"));
    btnFileSaveAs->setToolTip(tr("Save As"));

    propToolBar = addToolBar(tr("Tools"));

    //btnFilePrint->setToolTip(tr("Print"));
    //connect(btnFilePrint,SIGNAL(clicked()),this,SLOT(printFile()));
    //addToolButton(fileToolBar, btnFilePrint, QString(":/icons/print.png"));
    //QToolButton *btnFilePrint = new QToolButton(this);
#ifdef ENABLE_ZIP
    QToolButton *btnFileZip = new QToolButton(this);
    addToolButton(propToolBar, btnFileZip, QString(":/icons/zip.png"));
    connect(btnFileZip,SIGNAL(clicked()),this,SLOT(zipFiles()));
    btnFileZip->setToolTip(tr("Zip Project"));
#endif

    //propToolBar = addToolBar(tr("Preferences"));

    QToolButton *btnProjectPreferences = new QToolButton(this);
    addToolButton(propToolBar, btnProjectPreferences, QString(":/icons/preferences.png"));
    connect(btnProjectPreferences,SIGNAL(clicked()),this,SLOT(preferences()));
    btnProjectPreferences->setToolTip(tr("Preferences"));

    debugToolBar = addToolBar(tr("Debug"));
    QToolButton *btnDebugDebugTerm = new QToolButton(this);
    QToolButton *btnDebugRun = new QToolButton(this);
    QToolButton *btnDebugBuild = new QToolButton(this);
    QToolButton *btnDebugBurnEEP = new QToolButton(this);
    QToolButton *btnDebugIdHw = new QToolButton(this);

    addToolButton(debugToolBar, btnDebugIdHw, QString(":/icons/PropHatAlpha.png"));
    addToolButton(debugToolBar, btnDebugBuild, QString(":/icons/build2.png"));
    addToolButton(debugToolBar, btnDebugBurnEEP, QString(":/icons/burnee.png"));
    addToolButton(debugToolBar, btnDebugRun, QString(":/icons/run.png"));
    addToolButton(debugToolBar, btnDebugDebugTerm, QString(":/icons/debug2.png"));

    connect(btnDebugIdHw,SIGNAL(clicked()),this,SLOT(findHardware()));
    connect(btnDebugBuild,SIGNAL(clicked()),this,SLOT(programBuild()));
    connect(btnDebugBurnEEP,SIGNAL(clicked()),this,SLOT(programBurnEE()));
    connect(btnDebugDebugTerm,SIGNAL(clicked()),this,SLOT(programDebug()));
    connect(btnDebugRun,SIGNAL(clicked()),this,SLOT(programRun()));

    btnDebugBuild->setToolTip(tr("Build"));
    btnDebugBurnEEP->setToolTip(tr("Burn EEPROM"));
    btnDebugDebugTerm->setToolTip(tr("Debug"));
    btnDebugRun->setToolTip(tr("Run"));
    btnDebugIdHw->setToolTip(tr("Detect Attached Propellers"));

    ctrlToolBar = addToolBar(tr("Control"));
    ctrlToolBar->setLayoutDirection(Qt::RightToLeft);
    cbPort = new QComboBox(this);
    cbPort->setLayoutDirection(Qt::LeftToRight);
    cbPort->setToolTip(tr("Select Serial Port"));
    cbPort->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(cbPort,SIGNAL(currentIndexChanged(int)),this,SLOT(setCurrentPort(int)));

    btnConnected = new QToolButton(this);
    btnConnected->setToolTip(tr("View Serial Terminal"));
    btnConnected->setCheckable(true);
    connect(btnConnected,SIGNAL(clicked()),this,SLOT(connectButton()));

    addToolButton(ctrlToolBar, btnConnected, QString(":/icons/connected2.png"));
    ctrlToolBar->addWidget(cbPort);
    ctrlToolBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
}