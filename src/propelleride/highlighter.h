#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QVector>
#include <QFont>
#include <Qt>

#include "language.h"
#include "preferences.h"
#include "colorscheme.h"

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

private:
    ColorScheme * currentTheme;
    Language lang;

public:
    Highlighter(QTextDocument *parent);

    void addOnePartRules(QStringList rules,
                         QTextCharFormat format);

    void addTwoPartRules(QStringList rules,
                         QTextCharFormat format);

    void highlight();

protected:
    void highlightBlock(const QString &text);

    struct OnePartRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    struct TwoPartRule
    {
        QRegularExpression start;
        QRegularExpression end;
        QTextCharFormat format;
        bool newline;
    };

    QVector<OnePartRule> onepartrules;
    QVector<TwoPartRule> twopartrules;

    QRegularExpression re_tokens;

    QTextCharFormat keywordFormat;
    QTextCharFormat preprocessorFormat;
    QTextCharFormat classFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat numberFormat;
};
