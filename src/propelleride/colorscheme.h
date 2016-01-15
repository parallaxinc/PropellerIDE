#pragma once

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QFile>
#include <QMap>
#include <QColor>
#include <QFont>

#include "templates/singleton.h"

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
        SyntaxOperators,
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
    void save(const QString & filename);
    void save(QSettings * settings);

    void load();
    void load(const QString & filename);
    void load(QSettings * settings);

    void defaults();

public:
    QColor getColor(ColorScheme::Color key);
    void setColor(ColorScheme::Color key, const QColor & newcolor);

    QFont getFont();
    void setFont(const QFont & newfont);

    const QMap<ColorScheme::Color, colorcontainer>& getColorList() const;

};
