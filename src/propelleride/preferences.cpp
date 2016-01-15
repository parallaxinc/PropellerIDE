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

    defaultTheme = ":/themes/Ice.theme";
    currentTheme = &Singleton<ColorScheme>::Instance();

    QSettings settings;

    QVariant tabsv = settings.value("tabStop","4");
    if(tabsv.canConvert(QVariant::String)) {
        ui.tabStop->setText(tabsv.toString());
    }

    connect(ui.buttonBox,   SIGNAL(clicked(QAbstractButton *)), this,   SLOT(buttonBoxClicked(QAbstractButton *)));
    connect(ui.tabStop,     SIGNAL(textChanged(QString)),       this,   SIGNAL(tabStopChanged()));


    QDirIterator it(":/themes", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString filename = it.next();
        QString prettyname = QFileInfo(filename).baseName().replace("_"," ");
        ui.themeEdit->addItem(prettyname, filename);
    }

    // this routine is repeated often and needs to be abstracted
    loadTheme(settings.value("theme", defaultTheme).toString());
    currentTheme->load();
    settings.setValue("theme", ui.themeEdit->itemData(ui.themeEdit->currentIndex()));

    //    setupFolders();
    
    setupFonts();
    setupColors();

    connect(ui.themeEdit,   SIGNAL(currentIndexChanged(int)),   this,   SLOT(loadTheme(int)));
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

//void Preferences::setupFolders()
//{
//    compilerpath = new PathSelector(
//            tr("Compiler"),
//            QApplication::applicationDirPath() +
//                    QString(DEFAULT_COMPILER),
//            tr("Must add a compiler."),
//            SLOT(browseCompiler()),
//            this
//            );
//
//    librarypath = new PathSelector(
//            tr("Library"),
//            QApplication::applicationDirPath() +
//                    QString(APP_RESOURCES_PATH) +
//                    QString("/library"),
//            tr("Must add a library path."),
//            SLOT(browseLibrary()),
//            this
//            );
//
//    pathlayout->addWidget(compilerpath);
//    pathlayout->addWidget(librarypath);
//    vlayout->addWidget(paths);
//
//}

void Preferences::updateColor(int key, const QColor & color)
{
    qDebug() << "Preferences::updateColor(" << key << "," << color.name() << ")";
    currentTheme->setColor(
            static_cast<ColorScheme::Color>(key), 
            color);
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
    qDebug() << ui.themeEdit->itemData(index).toString();
    currentTheme->load(ui.themeEdit->itemData(index).toString());

    updateAll();
}

//void Preferences::browseCompiler()
//{
//    compilerpath->browsePath(
//            tr("Select Compiler"),
//            "OpenSpin (openspin*);;BST Compiler (bstc*)",
//            false
//            );
//}
//
//void Preferences::browseLibrary()
//{
//    librarypath->browsePath(
//            tr("Select Spin Library Path"),
//            NULL,
//            true
//            );
//}

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
            break;
        case QDialogButtonBox::Cancel: 
            reject();
            break;
        case QDialogButtonBox::RestoreDefaults: 
            restore();
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
//    compilerpath->save();
//    librarypath->save();

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
//    compilerpath->restore();
//    librarypath->restore();

    ui.tabStop->setText(tabStopStr);

    ui.enableCodeCompletion->setChecked(autoCompleteEnableSaved);

    ui.themeEdit->setCurrentIndex(
            ui.themeEdit->findData(QSettings().value("theme").toString())
            );

    currentTheme->load();
    updateAll();

    done(QDialog::Rejected);
}

