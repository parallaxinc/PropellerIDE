#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QList>
#include <QFont>
#include <Qt>

#include "language.h"
#include "colorscheme.h"

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

    ColorScheme * currentTheme;
    Language lang;

    QRegularExpression re_tokens;

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

    QHash<QString, OnePartRule> onepartrules;
    QList<TwoPartRule> twopartrules;

public:
    Highlighter(QTextDocument *parent);

    void addOnePartRules(QString name, QStringList rules);
    void addTwoPartRules(QStringList rules,
                         QTextCharFormat format);

public slots:
    void rehighlight();

protected:
    void highlightBlock(const QString &text);

};
