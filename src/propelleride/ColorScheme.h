#pragma once

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QFile>
#include <QMap>
#include <QColor>
#include <QFont>

#include "templates/Singleton.h"

class ColorScheme : public QObject
{
    Q_OBJECT

public:
    struct colorcontainer {
        QColor color;
        QString key;
    };


public:

    enum Color {
        Invalid,
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
//        SyntaxPreprocessor,
        SyntaxQuotes,
        SyntaxComments
    };

    ColorScheme(QObject * parent = 0);

private:

    QMap<ColorScheme::Color, colorcontainer> colors;
    QFont font;

public slots:

    void save();
    void load();
    void load(const QString & filename);
    void defaults();

public:
    void load(QSettings * settings);

    QColor getColor(ColorScheme::Color key);
    void setColor(ColorScheme::Color key, const QColor & newcolor);

    QFont getFont();
    void setFont(const QFont & newfont);

    const QMap<ColorScheme::Color, colorcontainer>& getColorList() const;

};
