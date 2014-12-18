#pragma once

#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QFile>
#include <QMap>
#include <QColor>

#include "templates/Singleton.h"

class ColorScheme
{
public:
    struct color {
        QColor color;
        QString key;
    };


private:

    QString mName;
    QString mFileName;

    QMap<int, color> colors;

public:

    enum Color {
        /* Default Editor Colors */
        EditorBG,
        EditorFG,
        EditorHL,
        /* Section Background Colors */
        ConBG,
        VarBG,
        ObjBG,
        PubBG,
        PriBG,
        DatBG,
        /* Code Syntax Colors */
        SyntaxNumbers,
        SyntaxFunctions,
        SyntaxKeywords,
        SyntaxPreprocessor,
        SyntaxQuotes,
        SyntaxLineComments,
        SyntaxBlockComments
    };

    ColorScheme();

    void save();
    void load();
    QColor getColor(ColorScheme::Color key);
    void setColor(ColorScheme::Color key, QColor newcolor);

    const QMap<int, color>& getColorList() const;


private:

};
