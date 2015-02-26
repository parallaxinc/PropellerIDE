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

#include "ColorScheme.h"
#include "PathSelector.h"

#define tabSpacesKey        "TabSpacesCount"

#define enableKeyClearScreen        "enableClearScreen"
#define enableKeyClearScreen16      "enableClearScreen16"
#define enableKeyHomeCursor         "enableHomeCursor"
#define enableKeyPosXYCursor        "enablePosXYCursor"
#define enableKeyMoveCursorLeft     "enableMoveCursorLeft"
#define enableKeyMoveCursorRight    "enableMoveCursorRight"
#define enableKeyMoveCursorUp       "enableMoveCursorUp"
#define enableKeyMoveCursorDown     "enableMoveCursorDown"
#define enableKeyBeepSpeaker        "enableBeepSpeaker"
#define enableKeyBackspace          "enableBackspace"
#define enableKeyTab                "enableTab"
#define enableKeyCReturn            "enableCReturn"
#define enableKeyClearToEOL         "enableClearToEOL"
#define enableKeyClearLinesBelow    "enableClearLinesBelow"
#define enableKeyNewLine            "enableNewLine"
#define enableKeyPosCursorX         "enablePosCursorX"
#define enableKeyPosCursorY         "enablePosCursorY"
#define enableKeySwapNLCR           "enableSwapNLCR"
#define enableKeyAddNLtoCR          "enableAddNLtoCR"
#define enableKeyEnterIsNL          "enableEnterIsNL"

#define termKeyForeground           "termForeground"
#define termKeyBackground           "termBackground"
#define termKeyFontFamily           "termFontFamily"
#define termKeyFontSize             "termFontSize"
#define termKeyFontWeight           "termFontWeight"
#define termKeyFontItalic           "termFontItalic"
#define termKeyWrapMode             "termWrapMode"
#define termKeyPageLineSize         "termPageLineSize"
#define termKeyBufferLines          "termBufferLines"
#define termKeyTabSize              "termTabSize"
#define termKeyHexMode              "termHexMode"
#define termKeyHexDump              "termHexDumpMode"

#define terminalEchoOn              "termKeyEchoOn"
#define termKeyBaudRate             "termBaudRate"

#define enableAutoComplete          "enableAutoComplete"
#define enableSpinSuggest           "enableSpinSuggest"


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
#define DEFAULT_LOADER              "/p1load"    APP_EXTENSION
#define DEFAULT_TERMINAL            "/propterm"  APP_EXTENSION

class Preferences : public QDialog
{
    Q_OBJECT
public:
    explicit Preferences(QWidget *parent = 0);
    void showPreferences();

    int  getTabSpaces();
    bool getAutoCompleteEnable();
    bool getSpinSuggestEnable();
    QLineEdit *getTabSpaceLedit();

    void adjustFontSize(float ratio);


signals:
    void updateColors();
    void updateFonts();

public slots:

    void cleanSettings();
    void fontDialog();
    void browseCompiler();
    void browseLibrary();
    void browseLoader();
    void browseTerminal();
    void accept();
    void reject();
    void updateColor(int key, const QColor & color);

private slots:
    void loadTheme(int index);

private:

    void setupFolders();
    void setupOptions();
    void setupHighlight();

    ColorScheme * currentTheme;

    QTabWidget  tabWidget;

    PathSelector * compilerpath;
    PathSelector * loaderpath;
    PathSelector * librarypath;
    PathSelector * terminalpath;

    QString     tabSpacesStr;
    QCheckBox   autoCompleteEnable;
    QCheckBox   spinSuggestEnable;
    QLineEdit   tabspaceLedit;
    QPushButton clearSettingsButton;
    QPushButton fontButton;

    QComboBox   themeEdit;

    bool        autoCompleteEnableSaved;
    bool        spinSuggestEnableSaved;
};
