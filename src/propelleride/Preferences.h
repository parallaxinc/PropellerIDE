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

#include "colors.h"
#include "ColorScheme.h"

#define publisherKey        "Parallax"
#define publisherComKey     "parallax.com"
#define PropellerIdeGuiKey  "PropellerIDE"

#define useKeys             "UseKeys"
#define spinCompilerKey     "SpinCompiler"
#define spinIncludesKey     "SpinIncludes"
#define spinLoaderKey       "SpinLoader"
#define editorFontKey       "EditorFont"
#define fontSizeKey         "FontSize"
#define lastFileNameKey     "LastFileName"
#define lastTermXposKey     "LastTermXposition"
#define lastTermYposKey     "LastTermYposition"
#define termGeometryKey     "TermGeometry"
#define lastDirectoryKey    "LastDirectory"
#define mainWindowGeometry  "WindowGeometry"
#define tabSpacesKey        "TabSpacesCount"
#define recentFilesKey      "recentFileList"

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
  #define APP_RESOURCES_PATH       "/.."

#else
  #error "We don't support that OS yet..."
#endif

#define DEFAULT_COMPILER            "/openspin"  APP_EXTENSION 
#define DEFAULT_LOADER              "/p1load"    APP_EXTENSION

class Preferences : public QDialog
{
    Q_OBJECT
public:
    explicit Preferences(QWidget *parent = 0);
    void showPreferences(QString lastDir);
    QString getSpinLibraryString();

    int  getTabSpaces();
    bool getAutoCompleteEnable();
    bool getSpinSuggestEnable();
    QLineEdit *getTabSpaceLedit();


signals:
    void openFontDialog();
    void updateColors();

public slots:

    void configSettings();
    void cleanSettings();
    void showFontDialog();
    void browseCompiler();
    void browseLibrary();
    void browseLoader();
    void accept();
    void reject();
    void updateColor(int key, const QColor & color);

private:

    void setupFolders();
    void setupOptions();
    void setupHighlight();

    QHBoxLayout * createPathSelector(
            QString const & labelname,
            QString const & errormessage,
            QLineEdit * lineEdit,
            const char * slot
        );

    void fileStringProperty(QVariant *var, QLineEdit *ledit, const char *key, QString *value);

    void browsePath(
            QString const & pathlabel, 
            QString const & pathregex,  
            QLineEdit * currentvalue,
            QString * oldvalue,
            bool isfolder
        );

    ColorScheme * currentTheme;

    QTabWidget  tabWidget;

    QString     lastFolder;

    QLineEdit   lineEditCompiler;
    QLineEdit   lineEditLibrary;
    QLineEdit   lineEditLoader;
    QString     spinCompilerStr;
    QString     spinIncludesStr;

    QString     spinLoaderStr;

    QString     tabSpacesStr;
    QCheckBox   autoCompleteEnable;
    QCheckBox   spinSuggestEnable;
    QLineEdit   tabspaceLedit;
    QPushButton clearSettingsButton;
    QPushButton fontButton;
    QFontDialog fontDialog;

    bool        autoCompleteEnableSaved;
    bool        spinSuggestEnableSaved;
};
