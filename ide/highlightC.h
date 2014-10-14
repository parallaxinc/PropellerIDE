#ifndef HIGHLIGHTC_H
#define HIGHLIGHTC_H

#include <QObject>
#include "highlighter.h"

class HighlightC : public Highlighter
{
public:
    HighlightC(QTextDocument *parent, Properties *prop);
    void highlight();
};

#endif // HIGHLIGHTC_H
