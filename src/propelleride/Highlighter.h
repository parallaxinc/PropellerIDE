#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QVector>
#include <QFont>
#include <Qt>

#include "Language.h"
#include "Preferences.h"
#include "ColorScheme.h"

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

private:
    ColorScheme * currentTheme;
    Language lang;

public:
    Highlighter(QTextDocument *parent);

    void addRules(QStringList rules,
            QTextCharFormat format);

    void highlight();

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
};
