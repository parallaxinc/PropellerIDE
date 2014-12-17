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

#define useKeys             PropellerIdeGuiKey "_UseKeys"
#define spinCompilerKey     PropellerIdeGuiKey "_SpinCompiler"
#define spinIncludesKey     PropellerIdeGuiKey "_SpinIncludes"
#define spinLoaderKey       PropellerIdeGuiKey "_SpinLoader"
#define editorFontKey       PropellerIdeGuiKey "_EditorFont"
#define fontSizeKey         PropellerIdeGuiKey "_FontSize"
#define lastFileNameKey     PropellerIdeGuiKey "_LastFileName"
#define lastTermXposKey     PropellerIdeGuiKey "_LastTermXposition"
#define lastTermYposKey     PropellerIdeGuiKey "_LastTermYposition"
#define termGeometryKey     PropellerIdeGuiKey "_TermGeometry"
#define lastDirectoryKey    PropellerIdeGuiKey "_LastDirectory"
#define mainWindowGeometry  PropellerIdeGuiKey "_WindowGeometry"
#define tabSpacesKey        PropellerIdeGuiKey "_TabSpacesCount"
#define recentFilesKey      PropellerIdeGuiKey "_recentFileList"
#define hlEnableKey         PropellerIdeGuiKey "_HighlightEnable"

#define enableKeyClearScreen        PropellerIdeGuiKey "_enableClearScreen"
#define enableKeyClearScreen16      PropellerIdeGuiKey "_enableClearScreen16"
#define enableKeyHomeCursor         PropellerIdeGuiKey "_enableHomeCursor"
#define enableKeyPosXYCursor        PropellerIdeGuiKey "_enablePosXYCursor"
#define enableKeyMoveCursorLeft     PropellerIdeGuiKey "_enableMoveCursorLeft"
#define enableKeyMoveCursorRight    PropellerIdeGuiKey "_enableMoveCursorRight"
#define enableKeyMoveCursorUp       PropellerIdeGuiKey "_enableMoveCursorUp"
#define enableKeyMoveCursorDown     PropellerIdeGuiKey "_enableMoveCursorDown"
#define enableKeyBeepSpeaker        PropellerIdeGuiKey "_enableBeepSpeaker"
#define enableKeyBackspace          PropellerIdeGuiKey "_enableBackspace"
#define enableKeyTab                PropellerIdeGuiKey "_enableTab"
#define enableKeyCReturn            PropellerIdeGuiKey "_enableCReturn"
#define enableKeyClearToEOL         PropellerIdeGuiKey "_enableClearToEOL"
#define enableKeyClearLinesBelow    PropellerIdeGuiKey "_enableClearLinesBelow"
#define enableKeyNewLine            PropellerIdeGuiKey "_enableNewLine"
#define enableKeyPosCursorX         PropellerIdeGuiKey "_enablePosCursorX"
#define enableKeyPosCursorY         PropellerIdeGuiKey "_enablePosCursorY"
#define enableKeySwapNLCR           PropellerIdeGuiKey "_enableSwapNLCR"
#define enableKeyAddNLtoCR          PropellerIdeGuiKey "_enableAddNLtoCR"
#define enableKeyEnterIsNL          PropellerIdeGuiKey "_enableEnterIsNL"

#define termKeyForeground           PropellerIdeGuiKey "_termForeground"
#define termKeyBackground           PropellerIdeGuiKey "_termBackground"
#define termKeyFontFamily           PropellerIdeGuiKey "_termFontFamily"
#define termKeyFontSize             PropellerIdeGuiKey "_termFontSize"
#define termKeyFontWeight           PropellerIdeGuiKey "_termFontWeight"
#define termKeyFontItalic           PropellerIdeGuiKey "_termFontItalic"
#define termKeyWrapMode             PropellerIdeGuiKey "_termWrapMode"
#define termKeyPageLineSize         PropellerIdeGuiKey "_termPageLineSize"
#define termKeyBufferLines          PropellerIdeGuiKey "_termBufferLines"
#define termKeyTabSize              PropellerIdeGuiKey "_termTabSize"
#define termKeyHexMode              PropellerIdeGuiKey "_termHexMode"
#define termKeyHexDump              PropellerIdeGuiKey "_termHexDumpMode"

#define terminalEchoOn              PropellerIdeGuiKey "_termKeyEchoOn"
#define termKeyBaudRate             PropellerIdeGuiKey "_termBaudRate"

#define enableAutoComplete          PropellerIdeGuiKey "_enableAutoComplete"
#define enableSpinSuggest           PropellerIdeGuiKey "_enableSpinSuggest"


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

public slots:

    void configSettings();
    void cleanSettings();
    void showFontDialog();
    void browseCompiler();
    void browseLibrary();
    void browseLoader();
    void accept();
    void reject();
    void updateColor(const QColor & color);

private:

    void setupFolders();
    void setupOptions();
    void addHighlights(QComboBox *box, QVector<PColor*> p);
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

    bool         hlNumStyleBool;
    bool         hlNumWeightBool;
    int          hlNumColorIndex;

    QToolButton hlNumButton;

    QCheckBox   hlEnable;

    QCheckBox   hlNumStyle;
    QCheckBox   hlNumWeight;
    QComboBox   hlNumColor;
};
