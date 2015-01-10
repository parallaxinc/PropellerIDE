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
    colors[ConBG]               = (color) { QColor(255,248,192)   , "Block_CON"  };
    colors[VarBG]               = (color) { QColor(255,223,191)   , "Block_VAR"  };
    colors[ObjBG]               = (color) { QColor(255,191,191)   , "Block_OBJ"  };
    colors[PubBG]               = (color) { QColor(191,223,255)   , "Block_PUB"  };
    colors[PriBG]               = (color) { QColor(191,248,255)   , "Block_PRI"  };
    colors[DatBG]               = (color) { QColor(191,255,200)   , "Block_DAT"  };

    // Syntax Highlighting
    colors[SyntaxText]          = (color) { Qt::black             , "Syntax_Text"          };
    colors[SyntaxNumbers]       = (color) { Qt::magenta           , "Syntax_Numbers"       };
    colors[SyntaxFunctions]     = (color) { Qt::blue              , "Syntax_Functions"     };
    colors[SyntaxKeywords]      = (color) { Qt::darkBlue          , "Syntax_Keywords"      };
//    colors[SyntaxPreprocessor]  = (color) { Qt::darkYellow        , "Syntax_Preprocessor"  };
    colors[SyntaxQuotes]        = (color) { Qt::red               , "Syntax_Quotes"        };
    colors[SyntaxComments]      = (color) { Qt::darkGreen         , "Syntax_Comments"      };

    // Editor font
    font = QFont("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    font.setFixedPitch(true);
    font.setPointSize(14);
}


void ColorScheme::save()
{
    QSettings settings;

    settings.beginGroup("Colors");

    QMap<int, color>::iterator i;
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

    QMap<int, color>::iterator i;
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

const QMap<int, ColorScheme::color>& ColorScheme::getColorList() const
{
    return colors;
}

void ColorScheme::setFont(const QFont & newfont)
{
    font = newfont;
}

QFont ColorScheme::getFont()
{
    return font;
}


