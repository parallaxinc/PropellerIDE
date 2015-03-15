#include <Qt>
#include <QRegExp>
#include <QString>
#include <QStringList>

#include "SpinHighlighter.h"
#include "Language.h"

SpinHighlighter::SpinHighlighter(QTextDocument *parent)
    : Highlighter(parent)
{
    currentTheme = &Singleton<ColorScheme>::Instance();
    highlight();
}

void SpinHighlighter::addRules(QStringList rules,
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

void SpinHighlighter::highlight()
{
    this->parent();
    HighlightingRule rule;

    Language lang = Language();

    // quoted strings
    quotationFormat.setForeground(currentTheme->getColor(ColorScheme::SyntaxQuotes));
    quotationFormat.setFontWeight(QFont::Normal);
    addRules(lang.listStrings(),quotationFormat);

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

