#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    currentTheme = &Singleton<ColorScheme>::Instance();
    highlight();
}

void Highlighter::addOnePartRules(QStringList rules,
        QTextCharFormat format)
{
    OnePartRule rule;
    rule.format = format;

    QStringList tokens;
    QString tokenstring;
    foreach (QString r, rules)
    {
        tokens.append(r);
    }
    tokenstring = tokens.join("|");
    tokenstring = "("+tokenstring+")";

    rule.pattern = QRegularExpression(tokenstring, 
            QRegularExpression::CaseInsensitiveOption); // this needs to pull from language.cpp

    onepartrules.append(rule);
}

void Highlighter::addTwoPartRules(QStringList rules,
                                  QTextCharFormat format)
{
    TwoPartRule rule;
    rule.format = format;

    if (rules.size() % 2 != 0) return;

    for (int i = 0; i < rules.size(); i += 2)
    {
        rule.start = QRegularExpression(rules[i],
                     QRegularExpression::CaseInsensitiveOption);

        rule.end   = QRegularExpression(rules[i+1],
                     QRegularExpression::CaseInsensitiveOption);

        twopartrules.append(rule);
    }
}

void Highlighter::highlightBlock(const QString &text)
{
    if (onepartrules.size() == 0)
        return;

    foreach (OnePartRule rule, onepartrules)
    {
        QRegularExpressionMatchIterator i = rule.pattern.globalMatch(text);

        while (i.hasNext())
        {
            QRegularExpressionMatch match = i.next();
            int length = match.capturedLength();
            if (length == 0)
                continue;
            int start = match.capturedStart();
            setFormat(start, length, rule.format);
        }
    }

    int state = 0;
    setCurrentBlockState(0);

    int offset = 0, end = 0;
    TwoPartRule rule;
    QRegularExpressionMatch matchstart;

    if (previousBlockState() < 1)
    {
        matchstart = re_tokens.match(text);
        offset = matchstart.capturedStart();

        for (state = 0; state < twopartrules.size(); state++)
        {
            rule = twopartrules[state];
            if (rule.start.pattern() == matchstart.captured())
            {
                break;
            }
        }
        if (state >= twopartrules.size()) state = -1;
    }
    else
    {
        state = previousBlockState()-1;
        rule = twopartrules[state];
    }

    while (offset >= 0)
    {
        QRegularExpressionMatch matchend = rule.end.match(text, offset + matchstart.capturedLength());
        end = matchend.capturedStart();

        int length;
        if (end == -1)
        {
            setCurrentBlockState(state+1);
            length = text.length() - offset;
        }
        else
        {
            length = end - offset + matchend.capturedLength();
        }

        setFormat(offset, length, rule.format);

        matchstart = re_tokens.match(text, offset + length);
        offset = matchstart.capturedStart();

        for (state = 0; state < twopartrules.size(); state++)
        {
            rule = twopartrules[state];
            if (rule.start.pattern() == matchstart.captured())
                break;
        }
        if (state >= twopartrules.size()) state = -1;
    }
}

void Highlighter::highlight()
{
    QTextCharFormat format;

    // numbers
    format.setForeground(currentTheme->getColor(ColorScheme::SyntaxNumbers));
    format.setFontWeight(QFont::Normal);
    addOnePartRules(lang.listNumbers(), format);

    // operators 
    format.setForeground(currentTheme->getColor(ColorScheme::SyntaxOperators));
    format.setFontWeight(QFont::Normal);
    addOnePartRules(lang.listOperators(), format);

    // keywords
    format.setForeground(currentTheme->getColor(ColorScheme::SyntaxKeywords));
    format.setFontWeight(QFont::Bold);
    addOnePartRules(lang.listKeywords(), format);

    // comments
    format.setForeground(currentTheme->getColor(ColorScheme::SyntaxComments));
    format.setFontWeight(QFont::Normal);
    addTwoPartRules(lang.listComments(), format);

    // quoted strings
    format.setForeground(currentTheme->getColor(ColorScheme::SyntaxQuotes));
    format.setFontWeight(QFont::Normal);
    addTwoPartRules(lang.listStrings(), format);

    QStringList tokens;
    QString tokenstring;
    foreach (TwoPartRule r, twopartrules)
    {
        tokens.append(r.start.pattern());
    }
    tokenstring = tokens.join("|");
    tokenstring = "("+tokenstring+")";
    re_tokens = QRegularExpression(tokenstring, QRegularExpression::MultilineOption);
}

