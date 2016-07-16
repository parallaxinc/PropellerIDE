#include "colorscheme.h"

#include <QDebug>
#include <QFontDatabase>

#include "logging.h"

ColorScheme::ColorScheme(QObject * parent) :
    QObject(parent)
{
    defaults();
//    load();
}

void ColorScheme::defaults()
{
    // Background Colors
    colors[ConBG]               = (struct colorcontainer) { QColor("#484848") , "Block_CON"  };
    colors[VarBG]               = (struct colorcontainer) { QColor("#4f002a") , "Block_VAR"  };
    colors[ObjBG]               = (struct colorcontainer) { QColor("#3d3d3d") , "Block_OBJ"  };
    colors[PubBG]               = (struct colorcontainer) { QColor("#24003b") , "Block_PUB"  };
    colors[PriBG]               = (struct colorcontainer) { QColor("#15004d") , "Block_PRI"  };
    colors[DatBG]               = (struct colorcontainer) { QColor("#003951") , "Block_DAT"  };

    // Syntax Highlighting
    colors[SyntaxText]          = (struct colorcontainer) { QColor("#eeeeee") , "Syntax_Text"          };
    colors[SyntaxNumbers]       = (struct colorcontainer) { QColor("#ff7fff") , "Syntax_Numbers"       };
    colors[SyntaxOperators]     = (struct colorcontainer) { QColor("#6565ff") , "Syntax_Operators"     };
    colors[SyntaxKeywords]      = (struct colorcontainer) { QColor("#ffffff") , "Syntax_Keywords"      };
    colors[SyntaxQuotes]        = (struct colorcontainer) { QColor("#a2b2ff") , "Syntax_Strings"       };
    colors[SyntaxComments]      = (struct colorcontainer) { QColor("#cccccc") , "Syntax_Comments"      };

}

void ColorScheme::save()
{
    QSettings settings;
    save(&settings);
}

void ColorScheme::save(const QString & filename)
{
    QSettings settings(filename, QSettings::IniFormat);
    save(&settings);
}

void ColorScheme::save(QSettings * settings)
{
    qDebug() << "saving" << settings->fileName();

    settings->beginGroup("Colors");

    QMap<ColorScheme::Color, colorcontainer>::iterator i;
    for (i = colors.begin(); i != colors.end(); ++i)
    {
        settings->setValue(i.value().key,i.value().color.name());
    }

    settings->endGroup();

    settings->beginGroup("Font");

    settings->setValue("Family", font.family());
    settings->setValue("Size", font.pointSize());

    settings->endGroup();

}

void ColorScheme::load()
{
    QSettings settings;
    load(&settings);
}

void ColorScheme::load(const QString & filename)
{
    QSettings settings(filename, QSettings::IniFormat);
    load(&settings);
}

void ColorScheme::load(QSettings * settings)
{
    qDebug() << "loading" << settings->fileName();

    settings->beginGroup("Colors");

    QMap<ColorScheme::Color, colorcontainer>::iterator i;
    for (i = colors.begin(); i != colors.end(); ++i)
    {
        if (! settings->contains(i.value().key))
            settings->setValue(i.value().key,i.value().color.name());

        QString strNamedColor = settings->value(i.value().key).toString();

        QColor qcColor = QColor(strNamedColor);
        if( qcColor.isValid() ){
            i.value().color = qcColor;
        }
    }

    settings->endGroup();

    settings->beginGroup("Font");


    if (!settings->contains("Family") || settings->value("Family").toString().isEmpty())
    {
        font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        font.setPointSize(12);
        font.setStyleHint(QFont::TypeWriter);
        settings->setValue("Family", font.family());
    }
    else
    {
        font.setFamily(
            settings->value("Family", font.family()).toString()
            );
    }

    if (!settings->contains("Size"))
        settings->setValue("Size", font.pointSize());

    font.setPointSize(
            settings->value("Size", font.pointSize()).toInt()
            );

    settings->endGroup();
}

void ColorScheme::setColor(ColorScheme::Color key, const QColor & color)
{
    colors[key].color = color;
}

QColor ColorScheme::getColor(ColorScheme::Color key)
{
    return colors[key].color;
};

const QMap<ColorScheme::Color, ColorScheme::colorcontainer>& ColorScheme::getColorList() const
{
    return colors;
}

void ColorScheme::setFont(const QFont & newfont)
{
    font = newfont;
    font.setStyleHint(QFont::TypeWriter);
    font.setFixedPitch(true);
}

QFont ColorScheme::getFont()
{
    return font;
}


