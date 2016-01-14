#include "preferences.h"

#include <QApplication>
#include <QDialog>
#include <QFileInfo>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>
#include <QVariant>
#include <QDateTime>
#include <QStringList>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDebug>
#include <QDirIterator>

#include "colorchooser.h"

Preferences::Preferences(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Preferences"));
    ui.setupUi(this);

    currentTheme = &Singleton<ColorScheme>::Instance();

    QSettings settings;

    QVariant tabsv = settings.value("tabSpaces","4");
    if(tabsv.canConvert(QVariant::String)) {
        ui.tabSpaces->setText(tabsv.toString());
    }

    connect(ui.restoreDefaults, SIGNAL(clicked()),              this,   SLOT(cleanSettings()));
    connect(ui.setEditorFont,   SIGNAL(clicked()),              this,   SLOT(fontDialog()));
    connect(ui.tabSpaces,       SIGNAL(textChanged(QString)),   this,   SLOT(tabSpacesChanged()));


    QDirIterator it(":/themes", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString filename = it.next();
        QString prettyname = QFileInfo(filename).baseName().replace("_"," ");
        themeEdit.addItem(prettyname, filename);
    }

    // this routine is repeated often and needs to be abstracted
    int themeindex = themeEdit.findData(settings.value("Theme", ":/themes/Ice.theme").toString());
    themeEdit.setCurrentIndex(themeindex);
    loadTheme(themeindex);
    settings.setValue("Theme",themeEdit.itemData(themeEdit.currentIndex()));




//    setupFolders();
//    setupOptions();
//    setupHighlight();

//    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
//    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
//    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(&themeEdit, SIGNAL(currentIndexChanged(int)), this, SLOT(loadTheme(int)));

    setWindowFlags(Qt::Tool);
}

void Preferences::cleanSettings()
{
    QSettings().clear();
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
    currentTheme->load(themeEdit.itemData(index).toString());

    emit updateColors();
    emit updateFonts(currentTheme->getFont());
}


//void Preferences::setupHighlight()
//{
//    QList<QLabel *> colors = findChildren<QLabel *>(QRegularExpression("color_"));
//
//
//    QMap<ColorScheme::Color, ColorScheme::colorcontainer> colors = 
//        currentTheme->getColorList();
//
//    QMap<ColorScheme::Color, ColorScheme::colorcontainer>::const_iterator i;
//    for (i = colors.constBegin(); i != colors.constEnd(); ++i)
//    {
//        QString prettyname = QString(i.value().key);
//        ColorChooser * colorPicker = new ColorChooser(i.key(), i.value().color.name(), this);
//
//        if (i.value().key.startsWith("Syntax_"))
//        {
//            prettyname.remove("Syntax_").replace("_"," ");
//            synlayout->addRow(new QLabel(prettyname), colorPicker);
//        }
//        else if (i.value().key.startsWith("Block_"))
//        {
//            prettyname.remove("Block_").replace("_"," ");
//            blocklayout->addRow(new QLabel(prettyname), colorPicker);
//        }
//    
//        colorPicker->setStatusTip(prettyname);
//        colorPicker->setToolTip(prettyname);
//    
//        connect(colorPicker,SIGNAL(sendColor(int, const QColor &)), 
//                this,       SLOT(updateColor(int, const QColor &)) );
//        connect(this,       SIGNAL(updateColors()), 
//                colorPicker,SLOT(updateColor()) );
//    }
//}

void Preferences::browseCompiler()
{
    compilerpath->browsePath(
            tr("Select Compiler"),
            "OpenSpin (openspin*);;BST Compiler (bstc*)",
            false
        );
}

void Preferences::browseLibrary()
{
    librarypath->browsePath(
            tr("Select Spin Library Path"),
            NULL,
            true
        );
}

void Preferences::accept()
{
    compilerpath->save();
    librarypath->save();

    QSettings settings;

    settings.setValue("tabSpaces",ui.tabSpaces->text());

    settings.setValue(enableAutoComplete,ui.enableCodeCompletion->isChecked());
    settings.setValue("Theme",themeEdit.itemData(themeEdit.currentIndex()));

    currentTheme->save();
    emit updateColors();
    emit updateFonts(currentTheme->getFont());

    done(QDialog::Accepted);
}

void Preferences::reject()
{
    compilerpath->restore();
    librarypath->restore();

    ui.tabSpaces->setText(tabSpacesStr);

    ui.enableCodeCompletion->setChecked(autoCompleteEnableSaved);

    themeEdit.setCurrentIndex(
            themeEdit.findData(QSettings().value("Theme").toString())
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
