#include "Highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    currentTheme = &Singleton<ColorScheme>::Instance();
    highlight();
}

void Highlighter::addRules(QStringList rules,
        QTextCharFormat format)
{
    HighlightingRule rule;
    rule.format = format;

    foreach(QString r, rules)
    {
        rule.pattern = QRegExp(r,Qt::CaseInsensitive);
        highlightingRules.append(rule);
    }
}

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
    if(rules == 0)
        return;

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
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

void Highlighter::highlight()
{
    HighlightingRule rule;

    // numbers
    numberFormat.setForeground(currentTheme->getColor(ColorScheme::SyntaxNumbers));
    numberFormat.setFontWeight(QFont::Normal);
    addRules(lang.listNumbers(),numberFormat);

    // functions before keywords if names are keywords
    functionFormat.setForeground(currentTheme->getColor(ColorScheme::SyntaxFunctions));
    functionFormat.setFontWeight(QFont::Normal);
    addRules(lang.listFunctions(),functionFormat);

    // handle Spin keywords
    keywordFormat.setForeground(currentTheme->getColor(ColorScheme::SyntaxKeywords));
    keywordFormat.setFontWeight(QFont::Bold);
    addRules(lang.listKeywords(),keywordFormat);
    addRules(lang.listOperators(),keywordFormat);

    // quoted strings
    quotationFormat.setForeground(currentTheme->getColor(ColorScheme::SyntaxQuotes));
    quotationFormat.setFontWeight(QFont::Normal);
    addRules(lang.listStrings(),quotationFormat);

    // single line comments
    singleLineCommentFormat.setForeground(currentTheme->getColor(ColorScheme::SyntaxComments));
    singleLineCommentFormat.setFontWeight(QFont::Normal);
    addRules(lang.listComments(),singleLineCommentFormat);

    // multilineline comments
    multiLineCommentFormat.setForeground(currentTheme->getColor(ColorScheme::SyntaxComments));
    multiLineCommentFormat.setFontWeight(QFont::Normal);
    commentStartExpression = QRegExp("{",Qt::CaseInsensitive,QRegExp::Wildcard);
    commentEndExpression = QRegExp("*}",Qt::CaseInsensitive,QRegExp::Wildcard);

}

