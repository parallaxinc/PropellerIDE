#include "preferences.h"

#include <QApplication>
#include <QDialog>
#include <QFileInfo>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QSettings>
#include <QStringList>
#include <QDebug>
#include <QDirIterator>
#include <QIntValidator>

#include "colorchooser.h"
#include "language.h"

Preferences::Preferences(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    ui.tabStop->setValidator(new QIntValidator(0, 24, this));

    currentTheme = &Singleton<ColorScheme>::Instance();

    setupThemes();
    setupFonts();
    setupColors();
    setupLanguages();

    connect(ui.buttonBox,   SIGNAL(clicked(QAbstractButton *)), this,   SLOT(buttonBoxClicked(QAbstractButton *)));
    connect(ui.themeEdit,   SIGNAL(currentIndexChanged(int)),   this,   SLOT(loadTheme(int)));
}

void Preferences::setupThemes()
{
    defaultTheme = ":/themes/Default.theme";

    QDirIterator it(":/themes", QDirIterator::Subdirectories);
    QStringList themes;

    while (it.hasNext())
        themes.append(it.next());

    themes.sort();

    foreach (QString t, themes)
    {
        QString prettyname = QFileInfo(t).baseName().replace("_"," ");
        ui.themeEdit->addItem(prettyname, t);
    }

    QSettings settings;
    loadTheme(settings.value("theme", defaultTheme).toString());
    currentTheme->load();
    currentTheme->save();
}

void Preferences::setupFonts()
{
    ui.fontFamily->setCurrentFont(currentTheme->getFont());

    connect(ui.fontFamily,  SIGNAL(currentFontChanged(const QFont &)), 
            this,           SLOT(updateFont(const QFont &)) );

    connect(this,           SIGNAL(updateFonts(const QFont &)), 
            ui.fontFamily,  SLOT(setCurrentFont(const QFont &)) );

    connect(ui.fontSize,    SIGNAL(valueChanged(int)),
            this,           SLOT(setFontSize(int)));
}

void Preferences::setupColors()
{
    QMap<ColorScheme::Color, ColorScheme::colorcontainer> colors = 
        currentTheme->getColorList();

    QMap<ColorScheme::Color, ColorScheme::colorcontainer>::const_iterator i;

    QList<ColorChooser *> colorchoosers = findChildren<ColorChooser *>(QRegularExpression("color_.*"));

    foreach (ColorChooser * chooser, colorchoosers)
    {
        QString name = chooser->objectName().remove("color_");

        for (i = colors.constBegin(); i != colors.constEnd(); ++i)
        {
            if (name == QString(i.value().key))
            {
                chooser->setKey(i.key());
                chooser->setColor(i.value().color);
            } 
        }

        connect(chooser,    SIGNAL(sendColor(int, const QColor &)), 
                this,       SLOT(updateColor(int, const QColor &)) );
        connect(this,       SIGNAL(updateColors()), 
                chooser,    SLOT(updateColor()) );
    }
}

Preferences::~Preferences()
{
}

void Preferences::setupLanguages()
{
    QString app = QApplication::applicationDirPath();

    QStringList languages = Language::languages();
    languages.sort();

    foreach (QString key, languages)
    {
        PathSelector * selector = new PathSelector(key);
        ui.languageLayout->addWidget(selector);
    
        connect(this, SIGNAL(accepted()), selector, SLOT(accept()));
        connect(this, SIGNAL(rejected()), selector, SLOT(reject()));
        connect(this, SIGNAL(restored()), selector, SLOT(restore()));
    }
}

void Preferences::updateColor(int key, const QColor & color)
{
//    qDebug() << "Preferences::updateColor(" << key << "," << color.name() << ")";
    currentTheme->setColor((ColorScheme::Color) key, color);
    emit updateColors();
}

void Preferences::updateFont(const QFont & font)
{
    currentTheme->setFont(font);
    emit updateFonts(font);
}

void Preferences::updateAll()
{
    updateColors();
    updateFonts(currentTheme->getFont());
}

void Preferences::loadTheme(QString filename)
{
    QSettings settings;
    int themeindex = ui.themeEdit->findData(filename);
    ui.themeEdit->setCurrentIndex(themeindex);
    loadTheme(themeindex);
}

void Preferences::loadTheme(int index)
{
    currentTheme->load(ui.themeEdit->itemData(index).toString());
    QSettings().setValue("theme", ui.themeEdit->itemData(index));
    updateAll();
}

void Preferences::showPreferences()
{
    load();
    show();
}

void Preferences::setFontSize(int size)
{
    if (size < 4 || size > 96) return;

    QFont font = currentTheme->getFont();
    font.setPointSize(size);
    ui.fontSize->setValue(size);

    currentTheme->setFont(font);
    currentTheme->save();

    emit updateFonts(font);
}

void Preferences::adjustFontSize(float ratio)
{
    QFont font = currentTheme->getFont();
    int size = font.pointSize();

    size = (int) ((float) size*ratio);

    if ((ratio < 1.0 && size > 3) ||
            (ratio > 1.0 && size < 90))
        setFontSize(size);

}

void Preferences::buttonBoxClicked(QAbstractButton * button)
{
    QDialogButtonBox::StandardButton standardbutton = ui.buttonBox->standardButton(button);

    switch(standardbutton)
    {
        case QDialogButtonBox::Ok: 
            accept();
            emit accepted();
            break;
        case QDialogButtonBox::Cancel: 
            reject();
            emit rejected();
            break;
        case QDialogButtonBox::RestoreDefaults: 
            restore();
            emit restored();
            break;
        default:
            break;
    }
} 

void Preferences::restore()
{
    QSettings().clear();
    loadTheme(defaultTheme);
}

void Preferences::accept()
{
    save();
    updateAll();

    done(QDialog::Accepted);
}

void Preferences::reject()
{
    load();
    updateAll();

    done(QDialog::Rejected);
}

void Preferences::save()
{
    QSettings settings;

    settings.beginGroup("Features");
    settings.setValue("tabStop",        ui.tabStop->text().toInt());
    settings.setValue("autoComplete",   ui.autoComplete->isChecked());
    settings.setValue("smartIndent",    ui.smartIndent->isChecked());
    settings.setValue("indentGuides",   ui.indentGuides->isChecked());
    settings.setValue("highlightLine",  ui.highlightLine->isChecked());
    settings.endGroup();

    settings.setValue("theme",          ui.themeEdit->itemData(ui.themeEdit->currentIndex()));

    currentTheme->save();
}

void Preferences::load()
{
    QSettings settings;

    settings.beginGroup("Features");
    ui.tabStop->setText(            settings.value("tabStop", 4).toString());
    ui.autoComplete->setChecked(    settings.value("autoComplete", true).toBool());
    ui.smartIndent->setChecked(     settings.value("smartIndent", true).toBool());
    ui.indentGuides->setChecked(    settings.value("indentGuides", true).toBool());
    ui.highlightLine->setChecked(   settings.value("highlightLine", true).toBool());
    settings.endGroup();

    ui.themeEdit->setCurrentIndex(
            ui.themeEdit->findData(QSettings().value("theme").toString())
            );

    currentTheme->load();
}
