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
#include "editorview.h"
#include "zipper.h"
#include "filemanager.h"
#include "buildmanager.h"
#include "finder.h"
#include "logging.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow ui;

    PropellerManager manager;
    Zipper zipper;

    Preferences preferences;

    BuildManager builder;
    Language language;

    ProjectParser * parser;

    QString spinCompiler;
    QStringList spinIncludes;
    QString projectFile;

    QList<QAction *> recentFiles;
    QComboBox * cbPort;

    bool _save_available;
    bool _close_available;
    bool _build_available;

    void setSaveEnabled(bool enabled);
    void setCloseEnabled(bool enabled);
    void setBuildEnabled(bool enabled);
    void setLoadEnabled(bool enabled);

public:
    MainWindow(QWidget * parent = 0);

public slots:
    void showMessage(const QString & message);

    // file menu
    void printFile();
    void zipFiles();
    void openFiles(const QStringList & files);

    // action states
    void updateActionStates();
    void setSaveAvailable(bool enabled);
    void setCloseAvailable(bool enabled);
    void setBuildAvailable(bool enabled);
    void setBuildAvailable();

    // view
    void fontBigger();
    void fontSmaller();

    // help
    void openFileResource(QString const & resource);
    void propellerManual();
    void propellerDatasheet();
    void propellerQuickReference();
    void propBasicManual();
    void about();
    void newFromTemplate();

    void setProject();

    bool runCompiler(bool load = false, bool write = false, const QString & name = QString());
    void programBuild();
    void programRun();
    void programWrite();
    void spawnMemoryMap(const QString & name = QString());
    void spawnTerminal(const QString & portname = QString());

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

    QString resolveFileName(QString filename);
    void highlightFileLine(QString file,
            int line);
    void highlightFileLine(QString file,
            int line,
            int col);
    void getApplicationSettings();
    void setEnableBuild(bool enabled);

private:
    void loadSession();
    void saveSession();
    void clearSession();

    void checkAndSaveFiles(QStringList files = QStringList());

    bool eventFilter(QObject *target, QEvent *event);


};
