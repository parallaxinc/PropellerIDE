#include "preferences.h"

#include <QApplication>
#include <QDialog>
#include <QFileInfo>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QFontDialog>
#include <QSettings>
#include <QVariant>
#include <QStringList>
#include <QLabel>
#include <QDebug>
#include <QDirIterator>

#include "colorchooser.h"

Preferences::Preferences(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    currentTheme = &Singleton<ColorScheme>::Instance();

    setupThemes();
    setupFonts();
    setupColors();
    setupLanguages();

    QSettings settings;

    QVariant tabsv = settings.value("tabStop","4");
    if(tabsv.canConvert(QVariant::String)) {
        ui.tabStop->setText(tabsv.toString());
    }

    connect(ui.buttonBox,   SIGNAL(clicked(QAbstractButton *)), this,   SLOT(buttonBoxClicked(QAbstractButton *)));
    connect(ui.tabStop,     SIGNAL(textChanged(QString)),       this,   SIGNAL(tabStopChanged()));
    connect(ui.themeEdit,   SIGNAL(currentIndexChanged(int)),   this,   SLOT(loadTheme(int)));

}

void Preferences::setupThemes()
{
    defaultTheme = ":/themes/Ice.theme";

    QDirIterator it(":/themes", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString filename = it.next();
        QString prettyname = QFileInfo(filename).baseName().replace("_"," ");
        ui.themeEdit->addItem(prettyname, filename);
    }

    QSettings settings;
    loadTheme(settings.value("theme", defaultTheme).toString());
    currentTheme->load();
    settings.setValue("theme", ui.themeEdit->itemData(ui.themeEdit->currentIndex()));
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

int Preferences::getTabSpaces()
{
    bool ok;
    int count = ui.tabStop->text().toInt(&ok);
    if(ok) return count;
    return 4;
}

bool Preferences::getAutoCompleteEnable()
{
    return ui.enableCodeCompletion->isChecked();
}

void Preferences::setupLanguages()
{
    PathSelector * spin;
    QString app = QApplication::applicationDirPath();
    spin = new PathSelector("spin",
            app + QString(DEFAULT_COMPILER),
            QStringList() << app + 
                    QString(APP_RESOURCES_PATH) +
                    QString("/library/library"));
    ui.languageLayout->addWidget(spin);

    connect(this, SIGNAL(accepted()), spin, SLOT(accept()));
    connect(this, SIGNAL(rejected()), spin, SLOT(reject()));
    connect(this, SIGNAL(restored()), spin, SLOT(restore()));
}

void Preferences::updateColor(int key, const QColor & color)
{
    qDebug() << "Preferences::updateColor(" << key << "," << color.name() << ")";
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

    updateAll();
}

void Preferences::showPreferences()
{
    tabStopStr = ui.tabStop->text();

    autoCompleteEnableSaved = ui.enableCodeCompletion->isChecked();

    this->show();
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
    loadTheme(QSettings().value(defaultTheme).toString());
}

void Preferences::accept()
{
    QSettings settings;

    settings.setValue("tabStop",ui.tabStop->text());
    settings.setValue("codeCompletion", ui.enableCodeCompletion->isChecked());
    settings.setValue("theme", ui.themeEdit->itemData(ui.themeEdit->currentIndex()));

    currentTheme->save();
    updateAll();

    done(QDialog::Accepted);
}

void Preferences::reject()
{
    ui.tabStop->setText(tabStopStr);

    ui.enableCodeCompletion->setChecked(autoCompleteEnableSaved);

    ui.themeEdit->setCurrentIndex(
            ui.themeEdit->findData(QSettings().value("theme").toString())
            );

    currentTheme->load();
    updateAll();

    done(QDialog::Rejected);
}

