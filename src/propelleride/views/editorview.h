#pragma once

#include <QComboBox>
#include <QCompleter>
#include <QPlainTextEdit>
#include <QString>
#include <QTabWidget>
#include <QTextCursor>
#include <QWidget>

#include "highlighter.h"
#include "language.h"

class EditorView : public QPlainTextEdit
{
    Q_OBJECT

    ColorScheme *   currentTheme;
    Language        language;
    ProjectParser * parser;

    Highlighter *   highlighter;
    QComboBox *     cbAuto;

    QStringList blocks;
    QRegularExpression re_blocks;

    bool    tabOn;
    int     tabStop;
    bool    smartIndent;
    bool    indentGuides;
    bool    autoComplete;
    bool    highlightLine;

    bool    canUndo;
    bool    canRedo;
    bool    canCopy;

    int     addAutoCompleteItem(QString type, QString s);
    QString selectAutoComplete();
    int     handleAutoComplete(QChar c);

    void    tabBlockShift();
    int     autoIndent();
    void    dedent();
    void    indent();

    QPoint  keyPopPoint(QTextCursor cursor);

    QMap<ColorScheme::Color, ColorScheme::colorcontainer> colors;
    QMap<ColorScheme::Color, ColorScheme::colorcontainer> colorsAlt;

    QString oldcontents;
    QColor  contrastColor(QColor color, int amount = 20);

public:
    EditorView(QWidget * parent);
    ~EditorView();
    void    setExtension(QString ext);

    void    saveContent();
    int     contentChanged();

protected:
    void    paintEvent(QPaintEvent * e);
    void    keyPressEvent(QKeyEvent* e);

private slots:
    void    finishAutoComplete(int index);
    void    updateColors();
    void    updateFonts();

public slots:
    bool    getUndo();
    bool    getRedo();
    bool    getCopy();
    void    setUndo(bool available);
    void    setRedo(bool available);
    void    setCopy(bool available);

    void    loadPreferences();

    // lineNumberArea stuff
public:
    void lineNumberAreaPaintEvent(QPaintEvent * event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
};
