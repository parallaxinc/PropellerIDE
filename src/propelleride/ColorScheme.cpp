#include "ColorScheme.h"

#include <QDebug>

ColorScheme::ColorScheme(QObject * parent) :
    QObject(parent)
{
    defaults();
    load();
}

void ColorScheme::defaults()
{
    // Background Colors
    colors[ConBG]               = (color) { QColor("#484848") , "Block_CON"  };
    colors[VarBG]               = (color) { QColor("#4f002a") , "Block_VAR"  };
    colors[ObjBG]               = (color) { QColor("#3d3d3d") , "Block_OBJ"  };
    colors[PubBG]               = (color) { QColor("#24003b") , "Block_PUB"  };
    colors[PriBG]               = (color) { QColor("#15004d") , "Block_PRI"  };
    colors[DatBG]               = (color) { QColor("#003951") , "Block_DAT"  };

    // Syntax Highlighting
    colors[SyntaxText]          = (color) { QColor("#eeeeee") , "Syntax_Text"          };
    colors[SyntaxNumbers]       = (color) { QColor("#ff7fff") , "Syntax_Numbers"       };
    colors[SyntaxFunctions]     = (color) { QColor("#6565ff") , "Syntax_Functions"     };
    colors[SyntaxKeywords]      = (color) { QColor("#ffffff") , "Syntax_Keywords"      };
    colors[SyntaxQuotes]        = (color) { QColor("#a2b2ff") , "Syntax_Quotes"        };
    colors[SyntaxComments]      = (color) { QColor("#cccccc") , "Syntax_Comments"      };

    setFont(QFont("Monospace"));
    font.setPointSize(12);
}


void ColorScheme::save()
{
    QSettings settings;

    settings.beginGroup("Colors");

    QMap<ColorScheme::Color, color>::iterator i;
    for (i = colors.begin(); i != colors.end(); ++i)
    {
        settings.setValue(i.value().key,i.value().color.name());
    }

    settings.endGroup();

    settings.beginGroup("Font");

    settings.setValue("Family", font.family());
    settings.setValue("Size", font.pointSize());

    settings.endGroup();

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
    settings->beginGroup("Colors");

    QMap<ColorScheme::Color, color>::iterator i;
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

    if (! settings->contains("Family"))
        settings->setValue("Family", font.family());
    if (! settings->contains("Size"))
        settings->setValue("Size", font.pointSize());

    font.setFamily(
            settings->value("Family", font.family()).toString()
            );
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

const QMap<ColorScheme::Color, ColorScheme::color>& ColorScheme::getColorList() const
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


