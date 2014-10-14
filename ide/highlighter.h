/****************************************************************************
  The basis of this code is the Qt Highlighter example having the copyright
  below. This code is substanially different for choosing the rules, and it
  adds registry selection for highlight attributes, but the basic highlighter
  mechanism is the same as the original.
*****************************************************************************/

/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

#include "properties.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent, Properties *prop);

    bool getStyle(QString key,  bool *italic);
    bool getWeight(QString key, QFont::Weight *weight);
    bool getColor(QString key,  Qt::GlobalColor *color);

    void getProperties();

    virtual void highlight();

protected:
    void highlightBlock(const QString &text);

    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat preprocessorFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat numberFormat;

    Properties      *properties;

    bool            hlNumStyle;
    QFont::Weight   hlNumWeight;
    Qt::GlobalColor hlNumColor;
    bool            hlFuncStyle;
    QFont::Weight   hlFuncWeight;
    Qt::GlobalColor hlFuncColor;
    bool            hlKeyWordStyle;
    QFont::Weight   hlKeyWordWeight;
    Qt::GlobalColor hlKeyWordColor;
    bool            hlPreProcStyle;
    QFont::Weight   hlPreProcWeight;
    Qt::GlobalColor hlPreProcColor;
    bool            hlQuoteStyle;
    QFont::Weight   hlQuoteWeight;
    Qt::GlobalColor hlQuoteColor;
    bool            hlLineComStyle;
    QFont::Weight   hlLineComWeight;
    Qt::GlobalColor hlLineComColor;
    bool            hlBlockComStyle;
    QFont::Weight   hlBlockComWeight;
    Qt::GlobalColor hlBlockComColor;
};

#endif
