#include "highlightC.h"

HighlightC::HighlightC(QTextDocument *parent, Properties *prop)
    : Highlighter(parent, prop)
{
    highlight();
}

void HighlightC::highlight()
{
    getProperties();

    HighlightingRule rule;

    // numbers
    numberFormat.setForeground(hlNumColor);
    numberFormat.setFontWeight(hlNumWeight);
    numberFormat.setFontItalic(hlNumStyle);
    rule.format = numberFormat;
    rule.pattern = QRegExp("\\b\\d+");
    highlightingRules.append(rule);
    rule.pattern = QRegExp("\\b0x[0-9,a-f,A-F]*");
    highlightingRules.append(rule);

    // do "functions" first so we can override if names are keywords
    functionFormat.setFontItalic(hlFuncStyle);
    functionFormat.setForeground(hlFuncColor);
    functionFormat.setFontWeight(hlFuncWeight);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // handle C keywords
    keywordFormat.setForeground(hlKeyWordColor);
    keywordFormat.setFontWeight(hlKeyWordWeight);
    keywordFormat.setFontItalic(hlKeyWordStyle);
    QStringList keywordPatterns;
    keywordPatterns
            << "\\bauto\\b"
            << "\\bbreak\\b"
            << "\\bcase\\b"
            << "\\bchar\\b"
            << "\\bconst\\b"
            << "\\bcontinue\\b"
            << "\\bdefault\\b"
            << "\\bdo\\b"
            << "\\bdouble\\b"
            << "\\belse\\b"
            << "\\benum\\b"
            << "\\bextern\\b"
            << "\\bfloat\\b"
            << "\\bfor\\b"
            << "\\bgoto\\b"
            << "\\bif\\b"
            << "\\bint\\b"
            << "\\blong\\b"
            << "\\bstruct\\b"
            << "\\bswitch\\b"
            << "\\bregister\\b"
            << "\\breturn\\b"
            << "\\bshort\\b"
            << "\\bsigned\\b"
            << "\\bsizeof\\b"
            << "\\bstatic\\b"
            << "\\btypedef\\b"
            << "\\bunion\\b"
            << "\\bunsigned\\b"
            << "\\bvoid\\b"
            << "\\bvolatile\\b"
            << "\\bwhile\\b"
            << "={2,}" << "+{2,}" << "-{2,}" << "_{2,}" << "\\{2,}"
            ;
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    preprocessorFormat.setFontItalic(hlPreProcStyle);
    preprocessorFormat.setForeground(hlPreProcColor);
    preprocessorFormat.setFontWeight(hlPreProcWeight);
    QStringList preprocessorPatterns;
    preprocessorPatterns
            << "\\bassert\\b"
            << "\\bclass\\b"
            << "\\bdefine\\b"
            << "\\bdefined\\b"
            << "\\berror\\b"
            << "\\bident\\b"
            << "\\bimport\\b"
            << "\\binclude\\b"
            << "\\binclude_next\\b"
            << "\\bline\\b"
            << "\\bpragma\\b"
            << "\\bpublic\\b"
            << "\\bprivate\\b"
            << "\\bunassert\\b"
            << "\\bundef\\b"
            << "\\bwarning\\b"
            << "\\bint\\d+_t"
            << "\\buint\\d+_t"
            << "\\belif\\b"
            << "\\bifdef\\b"
            << "\\bifndef\\b"
            << "\\bendif\\b"
            ;
    foreach (const QString &pattern, preprocessorPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = preprocessorFormat;
        highlightingRules.append(rule);
    }

    // quoted strings
    quotationFormat.setFontItalic(hlQuoteStyle);
    quotationFormat.setForeground(hlQuoteColor);
    quotationFormat.setFontWeight(hlQuoteWeight);

    rule.pattern = QRegExp("[\"].*[\"]");
    rule.format = quotationFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegExp("[<][a-z,A-Z].*[^-][>]");
    highlightingRules.append(rule);

    // single line comments
    singleLineCommentFormat.setFontItalic(hlLineComStyle);
    singleLineCommentFormat.setForeground(hlLineComColor);
    singleLineCommentFormat.setFontWeight(hlLineComWeight);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // multilineline comments
    multiLineCommentFormat.setFontItalic(hlBlockComStyle);
    multiLineCommentFormat.setForeground(hlBlockComColor);
    multiLineCommentFormat.setFontWeight(hlBlockComWeight);
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}
