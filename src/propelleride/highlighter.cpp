#include "highlighter.h"

Highlighter::Highlighter(QString ext, QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    language.loadExtension(ext);
    currentTheme = &Singleton<ColorScheme>::Instance();

    addOnePartRules("numbers", language.listNumbers());
    addOnePartRules("operators", language.listOperators());
    addOnePartRules("keywords", language.listKeywords());

    rehighlight();
}

void Highlighter::addOnePartRules(QString name, QStringList rules)
{
    OnePartRule rule;
    rule.pattern = language.buildTokenizer(rules);
    onepartrules[name] = rule;
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

void Highlighter::highlightOnePartRules(const QString &text)
{
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
}

void Highlighter::highlightTwoPartRules(const QString &text)
{
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

void Highlighter::highlightBlock(const QString &text)
{
    if (onepartrules.size() > 0)
    {
        highlightOnePartRules(text);
    }

    if (twopartrules.size() > 0)
    {
        highlightTwoPartRules(text);
    }
}

void Highlighter::rehighlight()
{
    QTextCharFormat format;

    format.setForeground(currentTheme->getColor(ColorScheme::SyntaxNumbers));
    format.setFontWeight(QFont::Normal);
    onepartrules["numbers"].format = format;

    format.setForeground(currentTheme->getColor(ColorScheme::SyntaxOperators));
    format.setFontWeight(QFont::Normal);
    onepartrules["operators"].format = format;

    format.setForeground(currentTheme->getColor(ColorScheme::SyntaxKeywords));
    format.setFontWeight(QFont::Bold);
    onepartrules["keywords"].format = format;


    twopartrules.clear();

    format.setForeground(currentTheme->getColor(ColorScheme::SyntaxComments));
    format.setFontWeight(QFont::Normal);
    addTwoPartRules(language.listComments(), format);

    format.setForeground(currentTheme->getColor(ColorScheme::SyntaxQuotes));
    format.setFontWeight(QFont::Normal);
    addTwoPartRules(language.listStrings(), format);

    QStringList tokens;
    foreach (TwoPartRule r, twopartrules)
    {
        tokens.append(r.start.pattern());
    }
    re_tokens = language.buildTokenizer(tokens);

    QSyntaxHighlighter::rehighlight();
}

