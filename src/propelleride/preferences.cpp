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

    QSettings settings;

    QVariant tabsv = settings.value("tabSpaces","4");
    if(tabsv.canConvert(QVariant::String)) {
        ui.tabSpaces->setText(tabsv.toString());
    }

    connect(ui.buttonBox,       SIGNAL(clicked(QAbstractButton *)), this,   SLOT(buttonBoxClicked(QAbstractButton *)));
    connect(ui.setEditorFont,   SIGNAL(clicked()),                  this,   SLOT(fontDialog()));
    connect(ui.tabSpaces,       SIGNAL(textChanged(QString)),       this,   SIGNAL(tabSpacesChanged()));


    QDirIterator it(":/themes", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString filename = it.next();
        QString prettyname = QFileInfo(filename).baseName().replace("_"," ");
        ui.themeEdit->addItem(prettyname, filename);
    }

    // this routine is repeated often and needs to be abstracted
    int themeindex = ui.themeEdit->findData(settings.value("Theme", ":/themes/Ice.theme").toString());
    ui.themeEdit->setCurrentIndex(themeindex);
    loadTheme(themeindex);
    settings.setValue("Theme", ui.themeEdit->itemData(ui.themeEdit->currentIndex()));




    //    setupFolders();
    //    setupOptions();
    setupHighlight();

    connect(ui.themeEdit, SIGNAL(currentIndexChanged(int)), this, SLOT(loadTheme(int)));
}

Preferences::~Preferences()
{
}

void Preferences::fontDialog()
{
    bool ok;

    QFontDialog fd(this);
    QFont font = currentTheme->getFont();
    font = fd.getFont(&ok, font, this);

    if(ok) {
        currentTheme->setFont(font);
        emit updateFonts(font);
    }
}

int Preferences::getTabSpaces()
{
    bool ok;
    int count = ui.tabSpaces->text().toInt(&ok);
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

void Preferences::loadTheme(int index)
{
    currentTheme->load(ui.themeEdit->itemData(index).toString());

    emit updateColors();
    emit updateFonts(currentTheme->getFont());
}


void Preferences::setupHighlight()
{
    QMap<ColorScheme::Color, ColorScheme::colorcontainer> colors = 
        currentTheme->getColorList();

    QList<ColorChooser *> colorchoosers = findChildren<ColorChooser *>(QRegularExpression("color_.*"));

    foreach (ColorChooser * chooser, colorchoosers)
    {
        QString name = chooser->objectName().remove("color_");
        QString prettyname = name;
        prettyname.replace("_"," ");

        QMap<ColorScheme::Color, ColorScheme::colorcontainer>::const_iterator i;
        for (i = colors.constBegin(); i != colors.constEnd(); ++i)
        {
            qDebug() << "testing" << i.value().key << name;
            if (name == QString(i.value().key))
            {
                qDebug() << "ITS A MATCH" << name;
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

void Preferences::restore()
{
    QSettings().clear();
}

void Preferences::accept()
{
//    compilerpath->save();
//    librarypath->save();

    QSettings settings;

    settings.setValue("tabSpaces",ui.tabSpaces->text());

    settings.setValue(enableAutoComplete, ui.enableCodeCompletion->isChecked());
    settings.setValue("Theme", ui.themeEdit->itemData(ui.themeEdit->currentIndex()));

    currentTheme->save();
    emit updateColors();
    emit updateFonts(currentTheme->getFont());

    done(QDialog::Accepted);
}

void Preferences::reject()
{
//    compilerpath->restore();
//    librarypath->restore();

    ui.tabSpaces->setText(tabSpacesStr);

    ui.enableCodeCompletion->setChecked(autoCompleteEnableSaved);

    ui.themeEdit->setCurrentIndex(
            ui.themeEdit->findData(QSettings().value("Theme").toString())
            );

    currentTheme->load();
    emit updateColors();
    emit updateFonts(currentTheme->getFont());

    done(QDialog::Rejected);
}

void Preferences::showPreferences()
{
    tabSpacesStr = ui.tabSpaces->text();

    autoCompleteEnableSaved = ui.enableCodeCompletion->isChecked();

    this->show();
}

void Preferences::adjustFontSize(float ratio)
{
    QFont font = currentTheme->getFont();
    int size = font.pointSize();

    QString fname = font.family();
    size = (int) ((float) size*ratio);

    if ((ratio < 1.0 && size > 3) ||
            (ratio > 1.0 && size < 90))
        font.setPointSize(size);

    currentTheme->setFont(font);
    currentTheme->save();

    emit updateFonts(font);
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

