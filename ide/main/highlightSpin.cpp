/****************************************************************************
  This class extends Highlighter
*****************************************************************************/

#include "qtversion.h"

#include "highlightSpin.h"

SpinHighlighter::SpinHighlighter(QTextDocument *parent, Preferences *prop)
    : Highlighter(parent, prop)
{
    highlight();
}

/*
 * this is mostly experimental. not sure if it can be used yet.
 */
void SpinHighlighter::highlight()
{
    this->parent();

    getPreferences();

    HighlightingRule rule;

    // quoted strings
    quotationFormat.setFontItalic(hlQuoteStyle);
    quotationFormat.setForeground(hlQuoteColor);
    quotationFormat.setFontWeight(hlQuoteWeight);
    rule.pattern = QRegExp("[\"].*[\"]");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // numbers
    numberFormat.setForeground(hlNumColor);
    numberFormat.setFontWeight(hlNumWeight);
    numberFormat.setFontItalic(hlNumStyle);
    rule.format = numberFormat;
    rule.pattern = QRegExp("\\b[0-9_]+\\b",Qt::CaseSensitive,QRegExp::RegExp2);
    highlightingRules.append(rule);
    rule.pattern = QRegExp("\\$[0-9,a-f,_]*",Qt::CaseInsensitive,QRegExp::RegExp);
    highlightingRules.append(rule);

    // functions before keywords if names are keywords
    functionFormat.setFontItalic(hlFuncStyle);
    functionFormat.setForeground(hlFuncColor);
    functionFormat.setFontWeight(hlFuncWeight);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_.]+(?=\\()\\b");
    //rule.pattern = QRegExp("\\bpub[ \t].+[^\r^\n]\\b|\\bpri[ \t].+[^\r^\n]\\b|\\b[A-Za-z0-9_.]+(?=\\()\\b");
    rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // handle Spin keywords
    keywordFormat.setForeground(hlKeyWordColor);
    keywordFormat.setFontWeight(hlKeyWordWeight);
    keywordFormat.setFontItalic(hlKeyWordStyle);
    QStringList keywordPatterns;
    /*
     * add spin patterns later
     */
    keywordPatterns
            << "\\b_CLKFREQ\\b"
            << "\\b_CLKMODE\\b"
            << "\\b_FREE\\b"
            << "\\b_STACK\\b"
            << "\\b_XINFREQ\\b"
            << "\\bABORT\\b"
            << "\\bABS\\b"
            << "\\bABSNEG\\b"
            << "\\bADD\\b"
            << "\\bADDABS\\b"
            << "\\bADDS\\b"
            << "\\bADDSX\\b"
            << "\\bADDX\\b"
            << "\\bAND\\b"
            << "\\bANDN\\b"
            << "\\bBYTE\\b"
            << "\\bBYTEFILL\\b"
            << "\\bBYTEMOVE\\b"
            << "\\bCALL\\b"
            << "\\bCASE\\b"
            << "\\bCHIPVER\\b"
            << "\\bCLKFREQ\\b"
            << "\\bCLKMODE\\b"
            << "\\bCLKSET\\b"
            << "\\bCMP\\b"
            << "\\bCMPS\\b"
            << "\\bCMPSUB\\b"
            << "\\bCMPSX\\b"
            << "\\bCMPX\\b"
            << "\\bCNT\\b"
            << "\\bCOGID\\b"
            << "\\bCOGINIT\\b"
            << "\\bCOGNEW\\b"
            << "\\bCOGSTOP\\b"
            << "\\bCON\\b"
            << "\\bCONSTANT\\b"
            << "\\bCTRA\\b"
            << "\\bCTRB\\b"
            << "\\bDAT\\b"
            << "\\bDIRA\\b"
            << "\\bDIRB#\\b"
            << "\\bDJNZ\\b"
            << "\\bELSE\\b"
            << "\\bELSEIF\\b"
            << "\\bELSEIFNOT\\b"
            << "\\bENC#\\b"
            << "\\bFALSE\\b"
            << "\\bFILE\\b"
            << "\\bFIT\\b"
            << "\\bFLOAT\\b"
            << "\\bFROM\\b"
            << "\\bFRQA\\b"
            << "\\bFRQB\\b"
            << "\\bHUBOP\\b"
            << "\\bIF\\b"
            << "\\bIFNOT\\b"
            << "\\bIF_A\\b"
            << "\\bIF_AE\\b"
            << "\\bIF_ALWAYS\\b"
            << "\\bIF_B\\b"
            << "\\bIF_BE\\b"
            << "\\bIF_C\\b"
            << "\\bIF_C_AND_NZ\\b"
            << "\\bIF_C_AND_Z\\b"
            << "\\bIF_C_EQ_Z\\b"
            << "\\bIF_C_NE_Z\\b"
            << "\\bIF_C_OR_NZ\\b"
            << "\\bIF_C_OR_Z\\b"
            << "\\bIF_E\\b"
            << "\\bIF_NC\\b"
            << "\\bIF_NC_AND_NZ\\b"
            << "\\bIF_NC_AND_Z\\b"
            << "\\bIF_NC_OR_NZ\\b"
            << "\\bIF_NC_OR_Z\\b"
            << "\\bIF_NE\\b"
            << "\\bIF_NEVER\\b"
            << "\\bIF_NZ\\b"
            << "\\bIF_NZ_AND_C\\b"
            << "\\bIF_NZ_AND_NC\\b"
            << "\\bIF_NZ_OR_C\\b"
            << "\\bIF_NZ_OR_NC\\b"
            << "\\bIF_Z\\b"
            << "\\bIF_Z_AND_C\\b"
            << "\\bIF_Z_AND_NC\\b"
            << "\\bIF_Z_EQ_C\\b"
            << "\\bIF_Z_NE_C\\b"
            << "\\bIF_Z_OR_C\\b"
            << "\\bIF_Z_OR_NC\\b"
            << "\\bINA\\b"
            << "\\bINB#\\b"
            << "\\bJMP\\b"
            << "\\bJMPRET\\b"
            << "\\bLOCKCLR\\b"
            << "\\bLOCKNEW\\b"
            << "\\bLOCKRET\\b"
            << "\\bLOCKSET\\b"
            << "\\bLONG\\b"
            << "\\bLONGFILL\\b"
            << "\\bLONGMOVE\\b"
            << "\\bLOOKDOWN\\b"
            << "\\bLOOKDOWNZ\\b"
            << "\\bLOOKUP\\b"
            << "\\bLOOKUPZ\\b"
            << "\\bMAX\\b"
            << "\\bMAXS\\b"
            << "\\bMIN\\b"
            << "\\bMINS\\b"
            << "\\bMOV\\b"
            << "\\bMOVD\\b"
            << "\\bMOVI\\b"
            << "\\bMOVS\\b"
            << "\\bMUL#\\b"
            << "\\bMULS#\\b"
            << "\\bMUXC\\b"
            << "\\bMUXNC\\b"
            << "\\bMUXNZ\\b"
            << "\\bMUXZ\\b"
            << "\\bNEG\\b"
            << "\\bNEGC\\b"
            << "\\bNEGNC\\b"
            << "\\bNEGNZ\\b"
            << "\\bNEGX\\b"
            << "\\bNEGZ\\b"
            << "\\bNEXT\\b"
            << "\\bNOP\\b"
            << "\\bNOT\\b"
            << "\\bNR\\b"
            << "\\bOBJ\\b"
            << "\\bONES#\\b"
            << "\\bOR\\b"
            << "\\bORG\\b"
            << "\\bOTHER\\b"
            << "\\bOUTA\\b"
            << "\\bOUTB#\\b"
            << "\\bPAR\\b"
            << "\\bPHSA\\b"
            << "\\bPHSB\\b"
            << "\\bPI\\b"
            << "\\bPLL1X\\b"
            << "\\bPLL2X\\b"
            << "\\bPLL4X\\b"
            << "\\bPLL8X\\b"
            << "\\bPLL16X\\b"
            << "\\bPOSX\\b"
            << "\\bPRI\\b"
            << "\\bPUB\\b"
            << "\\bQUIT\\b"
            << "\\bRCFAST\\b"
            << "\\bRCL\\b"
            << "\\bRCR\\b"
            << "\\bRCSLOW\\b"
            << "\\bRDBYTE\\b"
            << "\\bRDLONG\\b"
            << "\\bRDWORD\\b"
            << "\\bREBOOT\\b"
            << "\\bREPEAT\\b"
            << "\\bRES\\b"
            << "\\bRESULT\\b"
            << "\\bRET\\b"
            << "\\bRETURN\\b"
            << "\\bREV\\b"
            << "\\bROL\\b"
            << "\\bROR\\b"
            << "\\bROUND\\b"
            << "\\bSAR\\b"
            << "\\bSHL\\b"
            << "\\bSHR\\b"
            << "\\bSPR\\b"
            << "\\bSTEP\\b"
            << "\\bSTRCOMP\\b"
            << "\\bSTRING\\b"
            << "\\bSTRSIZE\\b"
            << "\\bSUB\\b"
            << "\\bSUBABS\\b"
            << "\\bSUBS\\b"
            << "\\bSUBSX\\b"
            << "\\bSUBX\\b"
            << "\\bSUMC\\b"
            << "\\bSUMNC\\b"
            << "\\bSUMNZ\\b"
            << "\\bSUMZ\\b"
            << "\\bTEST\\b"
            << "\\bTESTN\\b"
            << "\\bTJNZ\\b"
            << "\\bTJZ\\b"
            << "\\bTO\\b"
            << "\\bTRUE\\b"
            << "\\bTRUNC\\b"
            << "\\bUNTIL\\b"
            << "\\bVAR\\b"
            << "\\bVCFG\\b"
            << "\\bVSCL\\b"
            << "\\bWAITCNT\\b"
            << "\\bWAITPEQ\\b"
            << "\\bWAITPNE\\b"
            << "\\bWAITVID\\b"
            << "\\bWC\\b"
            << "\\bWHILE\\b"
            << "\\bWORD\\b"
            << "\\bWORDFILL\\b"
            << "\\bWORDMOVE\\b"
            << "\\bWR\\b"
            << "\\bWRBYTE\\b"
            << "\\bWRLONG\\b"
            << "\\bWRWORD\\b"
            << "\\bWZ\\b"
            << "\\bXINPUT\\b"
            << "\\bXOR\\b"
            << "\\bXTAL1\\b"
            << "\\bXTAL2\\b"
            << "\\bXTAL3\\b"

            << "--"
            << "++"
            << "?"
            << "@"
            << "@@"
            << "+"
            << "-"
            << "^^"
            << "||"
            << "|<"
            << ">|"
            << "!"
            << "<-"
            << "<-="
            << "->"
            << "->="
            << "<<"
            << "<<="
            << ">>"
            << ">>="
            << "->"
            << "->="
            << "><"
            << "><="
            << "&"
            << "&="
            << "|"
            << "|="
            << "^"
            << "^="
            << "*"
            << "*="
            << "**"
            << "**="
            << "/"
            << "/="
            << "//"
            << "//="
            << "+="
            << "-="
            << "#>"
            << "#>="
            << "<#"
            << "<#="
            << "<"
            << "<="
            << ">"
            << ">="
            << "<>"
            << "<>="
            << "=="
            << "==="
            << "=<"
            << "=<="
            << "=>"
            << "=>="
            << "\\bNOT\\b"
            << "\\bAND\\b"
            << "\\bAND=\\b"
            << "\\bOR\\b"
            << "\\bOR=\\b"
            << "="
            << ":="
            << ":"
            << "~~"
            << "~"
            ;
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern,Qt::CaseInsensitive);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    preprocessorFormat.setFontItalic(hlPreProcStyle);
    preprocessorFormat.setForeground(hlPreProcColor);
    preprocessorFormat.setFontWeight(hlPreProcWeight);
    QStringList preprocessorPatterns;
    preprocessorPatterns
            << "\\bdefine\\b"
            << "\\bdefined\\b"
            << "\\berror\\b"
            << "\\belif\\b"
            << "\\bendif\\b"
            << "\\bifdef\\b"
            << "\\binclude\\b"
            << "\\bundef\\b"
            << "\\bwarning\\b"
            ;
    foreach (const QString &pattern, preprocessorPatterns) {
        rule.pattern = QRegExp(pattern,Qt::CaseInsensitive);
        rule.format = preprocessorFormat;
        highlightingRules.append(rule);
    }

    // single line comments
    singleLineCommentFormat.setFontItalic(hlLineComStyle);
    singleLineCommentFormat.setForeground(hlLineComColor);
    singleLineCommentFormat.setFontWeight(hlLineComWeight);
    rule.pattern = QRegExp("'[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // multilineline comments
    multiLineCommentFormat.setFontItalic(hlBlockComStyle);
    multiLineCommentFormat.setForeground(hlBlockComColor);
    multiLineCommentFormat.setFontWeight(hlBlockComWeight);
    commentStartExpression = QRegExp("{",Qt::CaseInsensitive,QRegExp::Wildcard);
    commentEndExpression = QRegExp("*}",Qt::CaseInsensitive,QRegExp::Wildcard);

}

