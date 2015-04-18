#include "Preferences.h"

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

#include "ColorChooser.h"

Preferences::Preferences(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Preferences"));

    currentTheme = &Singleton<ColorScheme>::Instance();

    setupFolders();
    setupOptions();
    setupHighlight();

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(&themeEdit, SIGNAL(currentIndexChanged(int)), this, SLOT(loadTheme(int)));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(&tabWidget);
    layout->addWidget(buttonBox);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);

    setWindowFlags(Qt::Tool);
    resize(500,260);
}

void Preferences::cleanSettings()
{
    QSettings().clear();
}

void Preferences::setupOptions()
{
    QFrame *frame = new QFrame(this);
    QHBoxLayout *hlayout = new QHBoxLayout();

    QFormLayout *edlayout = new QFormLayout(this);
    QGroupBox *editor = new QGroupBox(tr("Editor Settings"));
    editor->setLayout(edlayout);

    QFormLayout *otlayout = new QFormLayout(this);
    QGroupBox *other  = new QGroupBox(tr("Other Settings"));
    other->setLayout(otlayout);

    hlayout->addWidget(editor);
    hlayout->addWidget(other);

    frame->setLayout(hlayout);
    tabWidget.addTab(frame,tr("General"));

    QSettings settings;
    QVariant enac = settings.value(enableAutoComplete,true);
    QVariant enss = settings.value(enableSpinSuggest,true);

    autoCompleteEnable.setChecked(enac.toBool());
    edlayout->addRow(new QLabel(tr("Enable AutoComplete")), &autoCompleteEnable);

    spinSuggestEnable.setChecked(enss.toBool());
    edlayout->addRow(new QLabel(tr("Enable Code Suggestion")), &spinSuggestEnable);

    QVariant tabsv = settings.value("tabSpaces","4");
    if(tabsv.canConvert(QVariant::String)) {
        tabspaceLedit.setText(tabsv.toString());
    }
    else 
    {
        tabspaceLedit.setText("4");
    }
    edlayout->addRow(new QLabel(tr("Editor Tab Space Count")), &tabspaceLedit);

    clearSettingsButton.setText(tr("Clear Settings"));
    clearSettingsButton.setToolTip(tr("Clear Settings on Exit"));
    connect(&clearSettingsButton,SIGNAL(clicked()), this, SLOT(cleanSettings()));
    otlayout->addRow(new QLabel(tr("Clear Settings on Exit")), &clearSettingsButton);

    fontButton.setText(tr("Set Editor Font"));
    connect(&fontButton,SIGNAL(clicked()),this,SLOT(fontDialog()));
    edlayout->addRow(new QLabel(tr("Set Editor Font")), &fontButton);
}

void Preferences::fontDialog()
{
    bool ok;

    QFontDialog fd(this);
    QFont font = currentTheme->getFont();
    font = fd.getFont(&ok, font, this);

    if(ok) {
        currentTheme->setFont(font);
        emit updateFonts();
    }
}

int Preferences::getTabSpaces()
{
    bool ok;
    int count = tabspaceLedit.text().toInt(&ok);
    if(ok) return count;
    return 4;
}

bool Preferences::getAutoCompleteEnable()
{
    return autoCompleteEnable.isChecked();;
}

bool Preferences::getSpinSuggestEnable()
{
    return spinSuggestEnable.isChecked();
}

QLineEdit *Preferences::getTabSpaceLedit()
{
    return &tabspaceLedit;
}

void Preferences::setupFolders()
{
    QVBoxLayout * vlayout = new QVBoxLayout(this);
    QFrame *box = new QFrame();
    box->setLayout(vlayout);
    tabWidget.addTab(box,tr("System"));

    QFormLayout * pathlayout = new QFormLayout(this);
    QGroupBox *paths= new QGroupBox(tr("System Paths"));
    paths->setLayout(pathlayout);

    compilerpath = new PathSelector(
            tr("Compiler"),
            QApplication::applicationDirPath() +
                    QString(DEFAULT_COMPILER),
            tr("Must add a compiler."),
            SLOT(browseCompiler()),
            this
            );

    loaderpath = new PathSelector(
            tr("Loader"),
            QApplication::applicationDirPath() +
                    QString(DEFAULT_LOADER),
            tr("Must add a loader program."),
            SLOT(browseLoader()),
            this
            );

    librarypath = new PathSelector(
            tr("Library"),
            QApplication::applicationDirPath() +
                    QString(APP_RESOURCES_PATH) +
                    QString("/library"),
            tr("Must add a library path."),
            SLOT(browseLibrary()),
            this
            );

    terminalpath = new PathSelector(
            tr("Terminal"),
            QApplication::applicationDirPath() +
                    QString(DEFAULT_TERMINAL),
            tr("Must add a terminal path."),
            SLOT(browseTerminal()),
            this
            );

    pathlayout->addWidget(compilerpath);
    pathlayout->addWidget(loaderpath);
    pathlayout->addWidget(librarypath);
    pathlayout->addWidget(terminalpath);
    vlayout->addWidget(paths);

}

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
    emit updateFonts();
}


void Preferences::setupHighlight()
{
    QFrame * box = new QFrame();
    QVBoxLayout *vlayout = new QVBoxLayout();
    box->setLayout(vlayout);

    tabWidget.addTab(box,tr("Appearance"));

    // row 1
    QHBoxLayout * themelayout = new QHBoxLayout(this);
    QGroupBox * themebox = new QGroupBox(tr("Themes"));
    themebox->setLayout(themelayout);

//    QPushButton * themesave = new QPushButton(tr("Save"),this);
//    QPushButton * themeload = new QPushButton(tr("Load"),this);

    themeEdit.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
//    themesave->setSizePolicy(QSizePolicy::Minimum,   QSizePolicy::Minimum);
//    themeload->setSizePolicy(QSizePolicy::Minimum,   QSizePolicy::Minimum);

    QDirIterator it(":/themes", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString filename = it.next();
        QString prettyname = QFileInfo(filename).baseName().replace("_"," ");
        themeEdit.addItem(prettyname, filename);
    }

    QSettings settings;

    // this routine is repeated often and needs to be abstracted
    int themeindex = themeEdit.findData(settings.value("Theme", ":/themes/Midnight_Grace.theme").toString());
    themeEdit.setCurrentIndex(themeindex);
    loadTheme(themeindex);
    settings.setValue("Theme",themeEdit.itemData(themeEdit.currentIndex()));
    qDebug() << "themeEdit" << themeEdit.currentText();


    themelayout->addWidget(&themeEdit);
//    themelayout->addWidget(themesave);
//    themelayout->addWidget(themeload);

    vlayout->addWidget(themebox);

    // row 2
    QFormLayout *synlayout = new QFormLayout(this);
    QGroupBox *synbox = new QGroupBox(tr("Syntax Colors"));
    synbox->setLayout(synlayout);

    QFormLayout *blocklayout = new QFormLayout(this);
    QGroupBox *blockbox = new QGroupBox(tr("Block Colors"));
    blockbox->setLayout(blocklayout);


    QHBoxLayout *h2layout = new QHBoxLayout();
    h2layout->addWidget(synbox);
    h2layout->addWidget(blockbox);

    vlayout->addLayout(h2layout);

    box->setLayout(vlayout);

    QMap<ColorScheme::Color, ColorScheme::color> colors = 
        currentTheme->getColorList();

    QMap<ColorScheme::Color, ColorScheme::color>::const_iterator i;
    for (i = colors.constBegin(); i != colors.constEnd(); ++i)
    {
        QString prettyname = QString(i.value().key);
        ColorChooser * colorPicker = new ColorChooser(i.key(), i.value().color.name(), this);

        if (i.value().key.startsWith("Syntax_"))
        {
            prettyname.remove("Syntax_").replace("_"," ");
            synlayout->addRow(new QLabel(prettyname), colorPicker);
        }
        else if (i.value().key.startsWith("Block_"))
        {
            prettyname.remove("Block_").replace("_"," ");
            blocklayout->addRow(new QLabel(prettyname), colorPicker);
        }
    
        colorPicker->setStatusTip(prettyname);
        colorPicker->setToolTip(prettyname);
    
        connect(colorPicker,SIGNAL(sendColor(int, const QColor &)), 
                this,       SLOT(updateColor(int, const QColor &)) );
        connect(this,       SIGNAL(updateColors()), 
                colorPicker,SLOT(updateColor()) );
    }
}

void Preferences::browseCompiler()
{
    compilerpath->browsePath(
            tr("Select Compiler"),
            "OpenSpin (openspin*);;BST Compiler (bstc*)",
            false
        );
}

void Preferences::browseLoader()
{
    loaderpath->browsePath(
            tr("Select Loader"),
            "propman (propman*);;p1load (p1load* p2load*);;BST Loader (bstl*)",
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

void Preferences::browseTerminal()
{
    terminalpath->browsePath(
            tr("Select terminal path"),
            NULL,
            false 
        );
}


void Preferences::accept()
{
    compilerpath->save();
    loaderpath->save();
    librarypath->save();
    terminalpath->save();

    QSettings settings;

    settings.setValue("tabSpaces",tabspaceLedit.text());

    settings.setValue(enableAutoComplete,autoCompleteEnable.isChecked());
    settings.setValue(enableSpinSuggest,spinSuggestEnable.isChecked());
    settings.setValue("Theme",themeEdit.itemData(themeEdit.currentIndex()));

    currentTheme->save();
    emit updateColors();
    emit updateFonts();

    done(QDialog::Accepted);
}

void Preferences::reject()
{
    compilerpath->restore();
    loaderpath->restore();
    librarypath->restore();
    terminalpath->restore();


    tabspaceLedit.setText(tabSpacesStr);

    autoCompleteEnable.setChecked(autoCompleteEnableSaved);
    spinSuggestEnable.setChecked(spinSuggestEnableSaved);

    themeEdit.setCurrentIndex(
            themeEdit.findData(QSettings().value("Theme").toString())
            );


    currentTheme->load();
    emit updateColors();
    emit updateFonts();

    done(QDialog::Rejected);
}

void Preferences::showPreferences()
{
    tabSpacesStr = tabspaceLedit.text();

    autoCompleteEnableSaved = autoCompleteEnable.isChecked();
    spinSuggestEnableSaved = spinSuggestEnable.isChecked();

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

    emit updateFonts();
}
