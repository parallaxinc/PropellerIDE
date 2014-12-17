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
#include "colorchooser.h"


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
    QSettings settings(publisherKey, PropellerIdeGuiKey);
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
    QSettings settings(publisherKey, PropellerIdeGuiKey);
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

    QSettings settings(publisherKey, PropellerIdeGuiKey,this);
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
    connect(&fontButton,SIGNAL(clicked()),this,SLOT(showFontDialog()));
    edlayout->addRow(new QLabel(tr("Set Editor Font")), &fontButton);
}

void Preferences::showFontDialog()
{
    emit openFontDialog();
}

int Preferences::getTabSpaces()
{
    bool ok;
    int count = tabspaceLedit.text().toInt(&ok);
    if(ok) return count;
    return 2;
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

QHBoxLayout * Preferences::createPathSelector(
        QString const & labelname,
        QString const & errormessage,
        QLineEdit * lineEdit,
        const char * slot) 
{
    QHBoxLayout * layout = new QHBoxLayout();

    QLabel * label = new QLabel(labelname);
    label->setMinimumWidth(70);

    lineEdit->setMinimumWidth(300);
    lineEdit->setToolTip(errormessage);

    layout->addWidget(label);
    layout->addWidget(lineEdit);
    QPushButton * browseButton = new QPushButton(tr("Browse"), this);
    layout->addWidget(browseButton);

    connect(browseButton, SIGNAL(clicked()), this, slot);
    return layout;
}


void Preferences::setupFolders()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QFrame *box = new QFrame();
    box->setLayout(layout);
    tabWidget.addTab(box,"System");

    layout->addLayout(createPathSelector(
                tr("Compiler"),
                tr("Must add a compiler."),
                &lineEditCompiler,
                SLOT(browseCompiler())
                ));

    layout->addLayout(createPathSelector(
                tr("Loader"),
                tr("Must add a loader program."),
                &lineEditLoader,
                SLOT(browseLoader())
                ));

    layout->addLayout(createPathSelector(
                tr("Library Path"),
                tr("Must add a library path."),
                &lineEditLibrary,
                SLOT(browseLibrary())
                ));

    QSettings settings(publisherKey, PropellerIdeGuiKey,this);

    QString compiler(DEFAULT_COMPILER);
    QString library(QString(APP_RESOURCES_PATH)+"/library");
    compiler = QApplication::applicationDirPath()+compiler;
    library  = QApplication::applicationDirPath()+library;

    QVariant compv = settings.value(spinCompilerKey, compiler);
    QVariant incv = settings.value(spinIncludesKey, library);

    if(compv.canConvert(QVariant::String)) {
        QString s = compv.toString();
        s = QDir::fromNativeSeparators(s);
        if(s.length() > 0) {
            lineEditCompiler.setText(s);
        }
        else {
            lineEditCompiler.setText(compiler);
        }
    }
    else {
        lineEditCompiler.setText(compiler);
    }

    if(incv.canConvert(QVariant::String)) {
        QString s = incv.toString();
        s = QDir::fromNativeSeparators(s);
        if(s.length() > 0) {
            lineEditLibrary.setText(s);
        }
        else {
            lineEditLibrary.setText(library);
        }
    }
    else {
        lineEditLibrary.setText(library);
    }

    settings.setValue(spinCompilerKey,lineEditCompiler.text());
    settings.setValue(spinIncludesKey,lineEditLibrary.text());

    QString loader(DEFAULT_LOADER);
    loader = QApplication::applicationDirPath()+loader;

    QVariant loadv = settings.value(spinLoaderKey,loader);

    if(loadv.canConvert(QVariant::String)) {
        QString s = loadv.toString();
        s = QDir::fromNativeSeparators(s);
        if(s.length() > 0) {
            lineEditLoader.setText(s);
        }
        else {
            lineEditLoader.setText(loader);
        }
    }
    else {
        lineEditLoader.setText(loader);
    }

    settings.setValue(spinLoaderKey,lineEditLoader.text());

}

void Preferences::updateColor(const QColor & color)
{
    qDebug() << "Sending" << color.name() << "to nowhere...";
}

void Preferences::setupHighlight()
{
    QGridLayout *hlayout = new QGridLayout();
    QFrame *hlbox = new QFrame();
    hlbox->setLayout(hlayout);
    tabWidget.addTab(hlbox,tr("Highlight"));

    QSettings settings(publisherKey, PropellerIdeGuiKey);
    QVariant var;


    int hlrow = 0;
    QMap<int, ColorScheme::color> colors = 
        currentTheme->getColorList();

    QMap<int, ColorScheme::color>::const_iterator i;
    for (i = colors.constBegin(); i != colors.constEnd(); ++i)
    {

        QLabel * name = new QLabel(i.value().key);
        hlayout->addWidget(name,hlrow,0);

        QCheckBox * setWeight = new QCheckBox(this);
        setWeight->setText(tr("Bold"));
        setWeight->setChecked(false);
        hlayout->addWidget(setWeight,hlrow,1);
        
        ColorChooser * colorPicker = new ColorChooser(i.value().color.name(), this);
        colorPicker->setStatusTip(i.value().key);
        colorPicker->setToolTip(i.value().key);
        hlayout->addWidget(colorPicker, hlrow, 2);

        connect(colorPicker, SIGNAL(sendColor(const QColor &)), 
                this,        SLOT(updateColor(const QColor &)) );

        hlrow++;
    }
}

void Preferences::browsePath(
        QString const & pathlabel, 
        QString const & pathregex,  
        QLineEdit * currentvalue,
        QString * oldvalue,
        bool isfolder
        )
{
    QString folder = *oldvalue = currentvalue->text();
    QString pathname;

    if (!folder.length()) 
        folder = lastFolder;

    if (isfolder) 
        pathname = QFileDialog::getExistingDirectory(this,
                pathlabel, folder, QFileDialog::ShowDirsOnly);
    else
        pathname = QFileDialog::getOpenFileName(this,
                pathlabel, folder, pathregex);

    QString s = QDir::fromNativeSeparators(pathname);

    if(s.length() == 0)
    {
        qDebug() << "browsePath(" << pathlabel << "): " << "No selection";
        return;
    }

    if (isfolder)
        if(!s.endsWith("/"))
            s += "/";

    qDebug() << "browsePath(" << pathlabel << "): " << s;
    currentvalue->setText(s);

    lastFolder = s.mid(0,s.lastIndexOf("/")+1);
}


void Preferences::browseCompiler()
{
    browsePath(
            tr("Select Compiler"),
            "OpenSpin (openspin*);;BST Compiler (bstc*)",
            &lineEditCompiler,
            &spinCompilerStr,
            false
        );
}

void Preferences::browseLoader()
{
    browsePath(
            tr("Select Loader"),
            "Loader (p1load* p2load*);;BST Loader (bstl*)",
            &lineEditLoader,
            &spinLoaderStr,
            false
        );
}

void Preferences::browseLibrary()
{
    browsePath(
            tr("Select Spin Library Path"),
            NULL,
            &lineEditLibrary,
            &spinIncludesStr,
            true
        );
}

void Preferences::accept()
{
    QSettings settings(publisherKey, PropellerIdeGuiKey,this);
    settings.setValue(spinCompilerKey,lineEditCompiler.text());
    settings.setValue(spinIncludesKey,lineEditLibrary.text());
    settings.setValue(spinLoaderKey,lineEditLoader.text());

    settings.setValue(tabSpacesKey,tabspaceLedit.text());

    settings.setValue(enableAutoComplete,autoCompleteEnable.isChecked());
    settings.setValue(enableSpinSuggest,spinSuggestEnable.isChecked());

    done(QDialog::Accepted);
}

void Preferences::reject()
{
    lineEditCompiler.setText(spinCompilerStr);
    lineEditLibrary.setText(spinIncludesStr);
    lineEditLoader.setText(spinLoaderStr);

    tabspaceLedit.setText(tabSpacesStr);

    autoCompleteEnable.setChecked(autoCompleteEnableSaved);
    spinSuggestEnable.setChecked(spinSuggestEnableSaved);

    done(QDialog::Rejected);
}

void Preferences::showPreferences(QString lastDir)
{
    this->lastFolder = lastDir;
    spinCompilerStr = lineEditCompiler.text();
    spinIncludesStr = lineEditLibrary.text();
    spinLoaderStr = lineEditLoader.text();

    tabSpacesStr = tabspaceLedit.text();

    autoCompleteEnableSaved = autoCompleteEnable.isChecked();
    spinSuggestEnableSaved = spinSuggestEnable.isChecked();

    this->show();
}

QString Preferences::getSpinLibraryString()
{
    return this->lineEditLibrary.text();
}
