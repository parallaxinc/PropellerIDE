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

    foreach(QString r, rules)
    {
        rule.pattern = QRegularExpression(r,
                QRegularExpression::CaseInsensitiveOption);
        onepartrules.append(rule);
    }
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
    int rules = 0;

    foreach (OnePartRule rule, onepartrules)
    {
        rules++;

        QRegularExpression re = rule.pattern;
        QRegularExpressionMatchIterator i = re.globalMatch(text);

        while (i.hasNext())
        {
            QRegularExpressionMatch match = i.next();
            int length = match.capturedLength();
            if (length == 0)
                break;
            int start = match.capturedStart();
            setFormat(start, length, rule.format);
        }
    }
    if(rules == 0)
        return;


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
    // numbers
    numberFormat.setForeground(currentTheme->getColor(ColorScheme::SyntaxNumbers));
    numberFormat.setFontWeight(QFont::Normal);
    addOnePartRules(lang.listNumbers(), numberFormat);

    // functions 
    functionFormat.setForeground(currentTheme->getColor(ColorScheme::SyntaxFunctions));
    functionFormat.setFontWeight(QFont::Normal);
    addOnePartRules(lang.listOperators(), functionFormat);

    // keywords
    keywordFormat.setForeground(currentTheme->getColor(ColorScheme::SyntaxKeywords));
    keywordFormat.setFontWeight(QFont::Bold);
    addOnePartRules(lang.listKeywords(), keywordFormat);

    // comments
    commentFormat.setForeground(currentTheme->getColor(ColorScheme::SyntaxComments));
    commentFormat.setFontWeight(QFont::Normal);
    addTwoPartRules(lang.listComments(), commentFormat);

    // quoted strings
    quotationFormat.setForeground(currentTheme->getColor(ColorScheme::SyntaxQuotes));
    quotationFormat.setFontWeight(QFont::Normal);
    addTwoPartRules(lang.listStrings(), quotationFormat);

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

