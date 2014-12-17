#include "ColorScheme.h"

ColorScheme::ColorScheme(QString name)
{
    mName = name;
    mFileName = QDir(QStandardPaths::writableLocation(
                QStandardPaths::DataLocation)).filePath(mName + ".theme.conf");

    // Default Colors
    colors[EditorFG]            = (color) { QColor(0  ,0  ,0  )   , "Editor Background"    };
    colors[EditorBG]            = (color) { QColor(255,248,192)   , "Editor Foreground"    };
    colors[EditorHL]            = (color) { QColor(255,255,0  )   , "Editor Highlight"     };

    colors[ConBG]               = (color) { QColor(255,248,192)   , "CON Background"       };
    colors[VarBG]               = (color) { QColor(255,223,191)   , "VAR Background"       };
    colors[ObjBG]               = (color) { QColor(255,191,191)   , "OBJ Background"       };
    colors[PubBG]               = (color) { QColor(191,223,255)   , "PUB Background"       };
    colors[PriBG]               = (color) { QColor(191,248,255)   , "PRI Background"       };
    colors[DatBG]               = (color) { QColor(191,255,200)   , "DAT Background"       };

    colors[ConAltBG]            = (color) { QColor(253,243,168)   , "CON Alt Background"   };
    colors[VarAltBG]            = (color) { QColor(253,210,167)   , "VAR Alt Background"   };
    colors[ObjAltBG]            = (color) { QColor(253,167,167)   , "OBJ Alt Background"   };
    colors[PubAltBG]            = (color) { QColor(167,210,253)   , "PUB Alt Background"   };
    colors[PriAltBG]            = (color) { QColor(167,243,253)   , "PRI Alt Background"   };
    colors[DatAltBG]            = (color) { QColor(167,253,179)   , "DAT Alt Background"   };

    // Syntax Highlighting};
    colors[SyntaxNumbers]       = (color) { Qt::magenta           , "Syntax Numbers"       };
    colors[SyntaxFunctions]     = (color) { Qt::blue              , "Syntax Functions"     };
    colors[SyntaxKeywords]      = (color) { Qt::darkBlue          , "Syntax Keywords"      };
    colors[SyntaxPreprocessor]  = (color) { Qt::darkYellow        , "Syntax Preprocessor"  };
    colors[SyntaxQuotes]        = (color) { Qt::red               , "Syntax Quotes"        };
    colors[SyntaxLineComments]  = (color) { Qt::darkGreen         , "Syntax Line Comments" };
    colors[SyntaxBlockComments] = (color) { Qt::darkGreen         , "Syntax Block Comments"};

    if( QFile::exists(mFileName) ){
        load();
    }
    else
    {
        save();
    }

}

void ColorScheme::save()
{
    QSettings theme_settings(mFileName, QSettings::NativeFormat);
    theme_settings.beginGroup("Colors");

    QMap<int, color>::iterator i;
    for (i = colors.begin(); i != colors.end(); ++i)
    {
        theme_settings.setValue(i.value().key,i.value().color.name());
    }

    theme_settings.endGroup();
}


void ColorScheme::load()
{
    QSettings theme_settings(mFileName, QSettings::NativeFormat);
    theme_settings.beginGroup("Colors");

    QMap<int, color>::iterator i;
    for (i = colors.begin(); i != colors.end(); ++i)
    {
        QString strNamedColor = theme_settings.value(i.value().key).toString();

        QColor qcColor = QColor(strNamedColor);
        if( qcColor.isValid() ){
            i.value().color = qcColor;
        }
    }

    theme_settings.endGroup();
}


QColor ColorScheme::getColor(ColorScheme::Color key){
    return colors[key].color;
};
