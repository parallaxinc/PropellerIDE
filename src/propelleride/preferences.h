#pragma once

#include <Qt>
#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QTabWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QFontDialog>
#include <QVector>
#include <QToolButton>
#include <QHBoxLayout>

#include "colorscheme.h"
#include "pathselector.h"

#include "ui_preferences.h"

#define enableAutoComplete          "enableAutoComplete"

#if defined(Q_OS_WIN) || defined(CYGWIN)
  #define APP_EXTENSION            ".exe"
  #define APP_RESOURCES_PATH        "/"

#elif defined(Q_OS_MAC)
  #define APP_EXTENSION            ""
  #define APP_RESOURCES_PATH        "/../Resources"

#elif defined(Q_OS_UNIX)
  #define APP_EXTENSION            ""
  #define APP_RESOURCES_PATH       "/../share/propelleride"

#else
  #error "We don't support that OS yet..."
#endif

#define DEFAULT_COMPILER            "/openspin"  APP_EXTENSION 

class Preferences : public QDialog
{
    Q_OBJECT

    Ui::Preferences ui;

public:
    explicit Preferences(QWidget *parent = 0);

    int  getTabSpaces();
    bool getAutoCompleteEnable();

    void adjustFontSize(float ratio);

signals:
    void updateColors();
    void updateFonts(const QFont &);
    void tabSpacesChanged();

public slots:

    void cleanSettings();
    void fontDialog();
    void browseCompiler();
    void browseLibrary();
    void accept();
    void reject();
    void updateColor(int key, const QColor & color);
    void showPreferences();

private slots:
    void loadTheme(int index);

private:

    void setupFolders();
    void setupOptions();
    void setupHighlight();

    ColorScheme * currentTheme;

    QTabWidget  tabWidget;

    PathSelector * compilerpath;
    PathSelector * librarypath;

    QString     tabSpacesStr;
    QLineEdit   tabspaceLedit;

    QComboBox   themeEdit;

    bool        autoCompleteEnableSaved;
};
