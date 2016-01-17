#pragma once

#include <QString>
#include <QComboBox>

#include "colorscheme.h"
#include "pathselector.h"

#include "ui_preferences.h"

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

    QString defaultTheme;

    ColorScheme * currentTheme;

    void setupThemes();
    void setupFonts();
    void setupColors();
    void setupLanguages();

    void updateAll();

    QString     tabStopStr;

    bool        autoCompleteEnableSaved;

public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();

    int  getTabSpaces();
    bool getAutoCompleteEnable();
    void adjustFontSize(float ratio);

signals:
    void updateColors();
    void updateFonts(const QFont &);
    void tabStopChanged();

    void accepted();
    void rejected();
    void restored();

public slots:
    void updateColor(int key, const QColor & color);
    void updateFont(const QFont & font);
    void showPreferences();
    void setFontSize(int size);

private slots:
    void loadTheme(int index);
    void loadTheme(QString filename);

    void buttonBoxClicked(QAbstractButton * button);
    void restore();
    void accept();
    void reject();
};
