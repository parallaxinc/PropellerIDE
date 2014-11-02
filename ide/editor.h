#ifndef EDITOR_H
#define EDITOR_H

#include "qtversion.h"

#include "highlighter.h"
#include "spinparser.h"

class LineNumberArea;


class Editor : public QPlainTextEdit
{
    Q_OBJECT
public:
    Editor(QWidget *parent);
    virtual ~Editor();

    void initSpin(SpinParser *parser);
    SpinParser *getSpinParser();

    void setHighlights(QString filename = "");
    void setLineNumber(int num);

    void clearCtrlPressed();

    typedef enum CodeTypeEnum {
        CodeTypeUTF8, CodeTypeUTF16, CodeTypeOther
    } CodeType;

    inline CodeType getCodeType()           { return codeType; }
    inline void setCodeType(CodeType type)  { codeType = type; }

private:
    int  autoEnterColumn();
    int  autoEnterColumnC();
    int  autoEnterColumnSpin();
    int  braceMatchColumn();
    bool isCommentOpen(int line);
    bool isSpinCommentOpen(int line);
    QString spinPrune(QString s);
    int addAutoItem(QString type, QString s);
    void spinAutoShow(int width);
    int  spinAutoComplete();
    int  spinAutoCompleteCON();
    int  contextHelp();
    int  tabBlockShift();
    void highlightBlock(QTextCursor *cur);
    bool isNotAutoComplete();
    QString selectAutoComplete();
    QString deletePrefix(QString s);
    void spinSuggest(int key);
    void selectSpinSuggestion(int key);
    void useSpinSuggestion(int key);
    QPoint keyPopPoint(QTextCursor cursor);

protected:
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseDoubleClickEvent (QMouseEvent *e);

private:
    QWidget *mainwindow;
    QTextCursor lastCursor;
    QPoint  mousepos;
    bool    ctrlPressed;
    SpinParser  *spinParser;
    bool    isSpin;
    Highlighter *highlighter;

    QComboBox *cbAuto;

    CodeType codeType;

    bool expectAutoComplete;

    Properties *propDialog;

private slots:
    void cbAutoSelected(int index);
    void cbAutoSelected0insert(int index);

/* lineNumberArea support below this line: see Nokia Copyright below */
public:
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine(QColor lineColor = QColor::Invalid);
    void updateBackgroundColors();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
    QString fileName;

signals:
    void saveEditorFile();
};



/* Code below here is taken from the Nokia CodeEditor example */

/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(Editor *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    Editor *codeEditor;
};

#endif // EDITOR_H
