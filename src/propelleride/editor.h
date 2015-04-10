#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QString>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QTextCursor>

#include "Highlighter.h"
#include "Preferences.h"

class LineNumberArea;


class Editor : public QPlainTextEdit
{
    Q_OBJECT
public:
    Editor(QWidget *parent);
    virtual ~Editor();
    
    Language lang;

    void setHighlights();

    void clearCtrlPressed();

    void saveContent();
    int contentChanged();

public slots:
    bool getUndo();
    bool getRedo();
    bool getCopy();
    void setUndo(bool available);
    void setRedo(bool available);
    void setCopy(bool available);

protected:
    void paintEvent(QPaintEvent *event);


private:
    bool canUndo;
    bool canRedo;
    bool canCopy;

    int  autoIndent();
    int addAutoItem(QString type, QString s);
    void spinAutoShow(int width);
    int  spinAutoComplete();
    int  tabBlockShift();
    QString selectAutoComplete();
    QPoint keyPopPoint(QTextCursor cursor);

    ColorScheme * currentTheme;
    QMap<ColorScheme::Color, ColorScheme::color> colors;
    QMap<ColorScheme::Color, ColorScheme::color> colorsAlt;

    QString oldcontents;

protected:
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);

private:
    QTextCursor lastCursor;
    QPoint  mousepos;
    bool    ctrlPressed;
    bool    isSpin;
    Highlighter *highlighter;

    QComboBox *cbAuto;

    bool expectAutoComplete;

    Preferences *propDialog;

private slots:
    void cbAutoSelected(int index);
    void cbAutoSelected0insert(int index);
    void updateColors();
    void updateFonts();
    void tabSpacesChanged();

/* lineNumberArea support below this line: see Nokia Copyright below */
public:
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void updateLineNumberAreaWidth();
    void updateBackgroundColors();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
    QString fileName;
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
