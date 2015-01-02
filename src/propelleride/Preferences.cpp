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

#include "Preferences.h"
#include "ColorChooser.h"


Preferences::Preferences(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Preferences"));

    currentTheme = &Singleton<ColorScheme>::Instance();

    configSettings();

    setupFolders();
    setupOptions();
    setupHighlight();

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(&tabWidget);
    layout->addWidget(buttonBox);
    setLayout(layout);

    setWindowFlags(Qt::Tool);
    resize(500,260);
}

void Preferences::configSettings()
{
    bool ok;
    QSettings settings;
    QString app = QApplication::applicationFilePath();
    QVariant keyv = settings.value(useKeys);
    int keyday = keyv.toInt(&ok);
    QDateTime appinfo = QFileInfo(app).lastModified();
    int appday = appinfo.date().toJulianDay();
    if(appday > keyday) {
        cleanSettings();
        settings.setValue(useKeys,appday);
    }
    return;
}

void Preferences::cleanSettings()
{
    QSettings settings;
    QStringList list = settings.allKeys();

    foreach(QString key, list) {
        if(key.indexOf(PropellerIdeGuiKey) == 0) {
            settings.remove(key);
        }
    }
    settings.setValue(useKeys,0);
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

    QVariant tabsv = settings.value(tabSpacesKey,"4");
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
    bool ok = true;

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
    tabWidget.addTab(box,"System");

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
            tr("Library Path"),
            QApplication::applicationDirPath() +
                    QString(APP_RESOURCES_PATH) +
                    QString("/library"),
            tr("Must add a library path."),
            SLOT(browseLibrary()),
            this
            );

    pathlayout->addWidget(compilerpath);
    pathlayout->addWidget(loaderpath);
    pathlayout->addWidget(librarypath);
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

void Preferences::setupHighlight()
{
    QFrame *hlbox = new QFrame();
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlbox->setLayout(hlayout);

    tabWidget.addTab(hlbox,tr("Highlighting"));

    QFormLayout *synlayout = new QFormLayout(this);
    QGroupBox *synbox = new QGroupBox(tr("Syntax Colors"));
    synbox->setLayout(synlayout);

    QFormLayout *blocklayout = new QFormLayout(this);
    QGroupBox *blockbox = new QGroupBox(tr("Block Colors"));
    blockbox->setLayout(blocklayout);

    hlayout->addWidget(synbox);
    hlayout->addWidget(blockbox);
    hlbox->setLayout(hlayout);

    QMap<int, ColorScheme::color> colors = 
        currentTheme->getColorList();

    QMap<int, ColorScheme::color>::const_iterator i;
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
    
        connect(colorPicker, SIGNAL(sendColor(int, const QColor &)), 
                this,        SLOT(updateColor(int, const QColor &)) );
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
            "Loader (p1load* p2load*);;BST Loader (bstl*)",
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
    loaderpath->save();
    librarypath->save();

    QSettings settings;

    settings.setValue(tabSpacesKey,tabspaceLedit.text());

    settings.setValue(enableAutoComplete,autoCompleteEnable.isChecked());
    settings.setValue(enableSpinSuggest,spinSuggestEnable.isChecked());

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


    tabspaceLedit.setText(tabSpacesStr);

    autoCompleteEnable.setChecked(autoCompleteEnableSaved);
    spinSuggestEnable.setChecked(spinSuggestEnableSaved);

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

QString Preferences::getSpinLibraryString()
{
    return QString();//this->lineEditLibrary.text();
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
