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

#include "qtversion.h"

#include "highlighter.h"

//! [0]
Highlighter::Highlighter(QTextDocument *parent, Properties *prop)
    : QSyntaxHighlighter(parent)
{
    properties = prop;
    highlight();
}

bool Highlighter::getStyle(QString key, bool *italic)
{
    QSettings settings(publisherKey, PropellerIdeGuiKey, this);
    QVariant var = settings.value(key, false);

    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        *italic = var.toBool();
        return true;
    }
    return false;
}

bool Highlighter::getWeight(QString key, QFont::Weight *weight)
{
    QSettings settings(publisherKey, PropellerIdeGuiKey, this);
    QVariant var = settings.value(key, false);

    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        *weight = var.toBool() ? QFont::Bold : QFont::Normal;
        return true;
    }
    return false;
}

bool Highlighter::getColor(QString key, Qt::GlobalColor *color)
{
    QSettings settings(publisherKey, PropellerIdeGuiKey, this);
    QVariant var = settings.value(key, false);

    if(var.canConvert(QVariant::Int)) {
        QString s = var.toString();
        int n = var.toInt();
        *color = (Qt::GlobalColor) properties->getQtColor(n);
        return true;
    }
    return false;
}

void Highlighter::getProperties()
{
    bool   style;
    QFont::Weight   weight;
    Qt::GlobalColor color;

    if(getStyle(hlNumStyleKey,&style))
        hlNumStyle = style;
    if(getWeight(hlNumWeightKey, &weight))
        hlNumWeight = weight;
    if(getColor(hlNumColorKey, &color))
        hlNumColor = color;

    if(getStyle(hlFuncStyleKey,&style))
        hlFuncStyle = style;
    if(getWeight(hlFuncWeightKey, &weight))
        hlFuncWeight = weight;
    if(getColor(hlFuncColorKey, &color))
        hlFuncColor = color;

    if(getStyle(hlKeyWordStyleKey,&style))
        hlKeyWordStyle = style;
    if(getWeight(hlKeyWordWeightKey, &weight))
        hlKeyWordWeight = weight;
    if(getColor(hlKeyWordColorKey, &color))
        hlKeyWordColor = color;

    if(getStyle(hlPreProcStyleKey,&style))
        hlPreProcStyle = style;
    if(getWeight(hlPreProcWeightKey, &weight))
        hlPreProcWeight = weight;
    if(getColor(hlPreProcColorKey, &color))
        hlPreProcColor = color;

    if(getStyle(hlQuoteStyleKey,&style))
        hlQuoteStyle = style;
    if(getWeight(hlQuoteWeightKey, &weight))
        hlQuoteWeight = weight;
    if(getColor(hlQuoteColorKey, &color))
        hlQuoteColor = color;

    if(getStyle(hlLineComStyleKey,&style))
        hlLineComStyle = style;
    if(getWeight(hlLineComWeightKey, &weight))
        hlLineComWeight = weight;
    if(getColor(hlLineComColorKey, &color))
        hlLineComColor = color;

    if(getStyle(hlBlockComStyleKey,&style))
        hlBlockComStyle = style;
    if(getWeight(hlBlockComWeightKey, &weight))
        hlBlockComWeight = weight;
    if(getColor(hlBlockComColorKey, &color))
        hlBlockComColor = color;
}

void Highlighter::highlight()
{

}

//! [7]
void Highlighter::highlightBlock(const QString &text)
{
    int rules = 0;
    foreach (const HighlightingRule &rule, highlightingRules) {
        rules++;
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            if(length == 0)
                break;
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    // don't parse empty rules
    if(rules == 0)
        return;

//! [7] //! [8]
    setCurrentBlockState(0);
//! [8]

//! [9]
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

//! [9] //! [10]
    while (startIndex >= 0) {
//! [10] //! [11]
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
//! [11]
