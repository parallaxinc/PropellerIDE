#ifndef PROPERTIES_H
#define PROPERTIES_H

#include "qtversion.h"

#include "propertycolor.h"

#define publisherKey        "MicroCSource"
#define publisherComKey     "MicroCSource.com"
#define PropellerIdeGuiKey  "PropellerIDE"

#define useKeys             PropellerIdeGuiKey "_UseKeys"
#define spinCompilerKey     PropellerIdeGuiKey "_SpinCompiler"
#define spinIncludesKey     PropellerIdeGuiKey "_SpinIncludes"
#define spinLoaderKey       PropellerIdeGuiKey "_SpinLoader"
#define xBasicCompilerKey   PropellerIdeGuiKey "_xBasicCompiler"
#define xBasicIncludesKey   PropellerIdeGuiKey "_xBasicIncludes"
//#define separatorKey      PropellerIdeGuiKey "_PathSeparator"
//#define configFileKey     PropellerIdeGuiKey "_ConfigFile"
#define editorFontKey       PropellerIdeGuiKey "_EditorFont"
#define fontSizeKey         PropellerIdeGuiKey "_FontSize"
#define lastFileNameKey     PropellerIdeGuiKey "_LastFileName"
//#define lastBoardNameKey  PropellerIdeGuiKey "_LastBoardName"
//#define lastPortNameKey   PropellerIdeGuiKey "_LastPortName"
#define lastTermXposKey     PropellerIdeGuiKey "_LastTermXposition"
#define lastTermYposKey     PropellerIdeGuiKey "_LastTermYposition"
#define termGeometryKey     PropellerIdeGuiKey "_TermGeometry"
#define lastDirectoryKey    PropellerIdeGuiKey "_LastDirectory"
#define mainWindowGeometry  PropellerIdeGuiKey "_WindowGeometry"
#define tabSpacesKey        PropellerIdeGuiKey "_TabSpacesCount"
#define recentFilesKey      PropellerIdeGuiKey "_recentFileList"
#define hlEnableKey         PropellerIdeGuiKey "_HighlightEnable"
#define hlNumStyleKey       PropellerIdeGuiKey "_HighlightNumberStyle"
#define hlNumWeightKey      PropellerIdeGuiKey "_HighlightNumberWeight"
#define hlNumColorKey       PropellerIdeGuiKey "_HighlightNumberColor"
#define hlFuncStyleKey      PropellerIdeGuiKey "_HighlightFunctionStyle"
#define hlFuncWeightKey     PropellerIdeGuiKey "_HighlightFunctionWeight"
#define hlFuncColorKey      PropellerIdeGuiKey "_HighlightFunctionColor"
#define hlKeyWordStyleKey   PropellerIdeGuiKey "_HighlightKeyWordTypeStyle"
#define hlKeyWordWeightKey  PropellerIdeGuiKey "_HighlightKeyWordTypeWeight"
#define hlKeyWordColorKey   PropellerIdeGuiKey "_HighlightKeyWordTypeColor"
#define hlPreProcStyleKey   PropellerIdeGuiKey "_HighlightPreProcTypeStyle"
#define hlPreProcWeightKey  PropellerIdeGuiKey "_HighlightPreProcTypeWeight"
#define hlPreProcColorKey   PropellerIdeGuiKey "_HighlightPreProcTypeColor"
#define hlQuoteStyleKey     PropellerIdeGuiKey "_HighlightQuoteTypeStyle"
#define hlQuoteWeightKey    PropellerIdeGuiKey "_HighlightQuoteTypeWeight"
#define hlQuoteColorKey     PropellerIdeGuiKey "_HighlightQuoteTypeColor"
#define hlLineComStyleKey   PropellerIdeGuiKey "_HighlightLineCommmentTypeStyle"
#define hlLineComWeightKey  PropellerIdeGuiKey "_HighlightLineCommmentTypeWeight"
#define hlLineComColorKey   PropellerIdeGuiKey "_HighlightLineCommmentTypeColor"
#define hlBlockComStyleKey  PropellerIdeGuiKey "_HighlightBlockCommentTypeStyle"
#define hlBlockComWeightKey PropellerIdeGuiKey "_HighlightBlockCommentTypeWeight"
#define hlBlockComColorKey  PropellerIdeGuiKey "_HighlightBlockCommentTypeColor"

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

#define externalLoader              "p1load"
#define externalWinLoader           externalLoader ".exe"

#define enableAutoComplete          PropellerIdeGuiKey "_enableAutoComplete"
#define enableSpinSuggest           PropellerIdeGuiKey "_enableSpinSuggest"

class Properties : public QDialog
{
    Q_OBJECT
public:
    explicit Properties(QWidget *parent = 0);
    void showProperties(QString lastDir);
    QString getSpinLibraryString();
    Qt::GlobalColor getQtColor(int index);

    int  getTabSpaces();
    bool getAutoCompleteEnable();
    bool getSpinSuggestEnable();
    bool getPortBoardNameEnable();
    QLineEdit *getTabSpaceLedit();

signals:
    void openFontDialog();

public slots:

    void configSettings();
    void cleanSettings();
    void showFontDialog();
    void spinBrowseCompiler();
    void spinBrowseIncludes();
    void xBasicBrowseCompiler();
    void xBasicBrowseIncludes();
    void spinBrowseLoader();
    void accept();
    void reject();

private:

    void setupFolders();
    void setupOptions();
    void addHighlights(QComboBox *box, QVector<PColor*> p);
    void setupHighlight();

    void fileStringProperty(QVariant *var, QLineEdit *ledit, const char *key, QString *value);

    QTabWidget  tabWidget;

    QString     lastFolder;

    enum Language { SPIN = 1, XBASIC = 2 };

    QLineEdit   *spinLeditCompiler;
    QLineEdit   *spinLeditIncludes;
    QString     spinCompilerStr;
    QString     spinIncludesStr;

    QLineEdit   *xBasicLeditCompiler;
    QLineEdit   *xBasicLeditIncludes;
    QString     xBasicCompilerStr;
    QString     xBasicIncludesStr;

    QLineEdit   *spinLoadLedit;
    QString     spinLoaderStr;

    QString     tabSpacesStr;
    QCheckBox   autoCompleteEnable;
    QCheckBox   spinSuggestEnable;
    QCheckBox   portBoardNameEnable;
    QLineEdit   tabspaceLedit;
    QPushButton clearSettingsButton;
    QPushButton fontButton;
    QFontDialog fontDialog;

    bool        autoCompleteEnableSaved;
    bool        spinSuggestEnableSaved;

    bool         hlNumStyleBool;
    bool         hlNumWeightBool;
    int          hlNumColorIndex;
    bool         hlFuncStyleBool;
    bool         hlFuncWeightBool;
    int          hlFuncColorIndex;
    bool         hlKeyWordStyleBool;
    bool         hlKeyWordWeightBool;
    int          hlKeyWordColorIndex;
    bool         hlPreProcStyleBool;
    bool         hlPreProcWeightBool;
    int          hlPreProcColorIndex;
    bool         hlQuoteStyleBool;
    bool         hlQuoteWeightBool;
    int          hlQuoteColorIndex;
    bool         hlLineComStyleBool;
    bool         hlLineComWeightBool;
    int          hlLineComColorIndex;
    bool         hlBlockComStyleBool;
    bool         hlBlockComWeightBool;
    int          hlBlockComColorIndex;

    QToolButton hlNumButton;

    QCheckBox   hlEnable;

    QCheckBox   hlNumStyle;
    QCheckBox   hlNumWeight;
    QComboBox   hlNumColor;
    QCheckBox   hlFuncStyle;
    QCheckBox   hlFuncWeight;
    QComboBox   hlFuncColor;
    QCheckBox   hlKeyWordStyle;
    QCheckBox   hlKeyWordWeight;
    QComboBox   hlKeyWordColor;
    QCheckBox   hlPreProcStyle;
    QCheckBox   hlPreProcWeight;
    QComboBox   hlPreProcColor;
    QCheckBox   hlQuoteStyle;
    QCheckBox   hlQuoteWeight;
    QComboBox   hlQuoteColor;
    QCheckBox   hlLineComStyle;
    QCheckBox   hlLineComWeight;
    QComboBox   hlLineComColor;
    QCheckBox   hlBlockComStyle;
    QCheckBox   hlBlockComWeight;
    QComboBox   hlBlockComColor;

    QVector<PColor*> propertyColors;
};

#endif // PROPERTIES_H
