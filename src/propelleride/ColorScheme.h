#pragma once

#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QFile>
#include <QMap>
#include <QColor>

class ColorScheme
{
private:

    QString mName;
    QString mFileName;

    struct color {
        QColor color;
        QString key;
    };

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
        ConAltBG,
        VarAltBG,
        ObjAltBG,
        PubAltBG,
        PriAltBG,
        DatAltBG,
        /* Code Syntax Colors */
        SyntaxNumbers,
        SyntaxFunctions,
        SyntaxKeywords,
        SyntaxPreprocessor,
        SyntaxQuotes,
        SyntaxLineComments,
        SyntaxBlockComments
    };

    ColorScheme(QString name);

    void save();
    void load();
    QColor getColor(ColorScheme::Color key);

private:

};
