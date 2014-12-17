#pragma once

#include "Highlighter.h"

#include <QHash>
#include <QTextCharFormat>

#include "Preferences.h"
#include "ColorScheme.h"

class SpinHighlighter : public Highlighter
{
    Q_OBJECT
private:
    ColorScheme      *currentTheme;

public:
    SpinHighlighter(QTextDocument *parent);
    void highlight();

};
