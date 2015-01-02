#pragma once

#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QFile>
#include <QMap>
#include <QColor>
#include <QFont>

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
    QFont font;

public:

    enum Color {
        /* Block Colors */
        ConBG,
        VarBG,
        ObjBG,
        PubBG,
        PriBG,
        DatBG,
        /* Syntax Colors */
        SyntaxText,
        SyntaxNumbers,
        SyntaxFunctions,
        SyntaxKeywords,
        SyntaxPreprocessor,
        SyntaxQuotes,
        SyntaxComments
    };

    ColorScheme();

    void save();
    void load();

    QColor getColor(ColorScheme::Color key);
    void setColor(ColorScheme::Color key, const QColor & newcolor);

    QFont getFont();
    void setFont(const QFont & newfont);

    const QMap<int, color>& getColorList() const;


private:

};
