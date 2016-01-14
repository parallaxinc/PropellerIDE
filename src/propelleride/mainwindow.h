#pragma once

#include <QIcon>
#include <QMainWindow>
#include <QSplitter>
#include <QPlainTextEdit> 
#include <QTreeView> 
#include <QDesktopServices> 
#include <QDirIterator> 

#include <PropellerManager>
#include <ProjectParser>
#include <ProjectView>

#include "ui_mainwindow.h"

#include "preferences.h"
#include "editor.h"
#include "zipper.h"
#include "filemanager.h"
#include "buildmanager.h"
#include "finder.h"
#include "logging.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow ui;

public:
    MainWindow(QWidget *parent = 0);

    PropellerManager manager;
    Zipper          zipper;

    BuildManager    builder;
    Language        language;

    ProjectParser   *parser;
    Preferences     *propDialog;

    QString         spinCompiler;
    QString         spinIncludes;
    QString         projectFile;

    QList<QAction *>    recentFiles;
    QComboBox   * cbPort;


public slots:
    void showMessage(const QString & message);

    // file menu
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

    void setProject();

    void programBuild();
    void programRun();
    void programWrite();
    void spawnMemoryMap();
    void spawnTerminal();

    void recolorMemoryMap(QWidget * widget);
    void recolorProjectView();
    void recolorBuildManager();
    void closeEvent(QCloseEvent *event);
    void quitProgram();

    void updatePorts();
    void showBrowser();

    void addRecentFile(const QString &fileName);
    void updateRecentFileActions();
    void openRecentFile();

    void highlightFileLine(QString file, int line);
    void getApplicationSettings();
    void setEnableBuild(bool enabled);

private:
    void loadSession();
    void saveSession();
    void clearSession();

    void checkAndSaveFiles();

    bool eventFilter(QObject *target, QEvent *event);

    int  runCompiler(bool load = false, bool write = false);

};
