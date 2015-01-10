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
    struct color {
        QColor color;
        QString key;
    };


private:

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
//        SyntaxPreprocessor,
        SyntaxQuotes,
        SyntaxComments
    };

    ColorScheme(QObject * parent = 0);


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

    const QMap<int, color>& getColorList() const;

};
