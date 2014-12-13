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

Preferences::Preferences(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(QString(PropellerIdeGuiKey)+tr(" Preferences"));

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
    } else {
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

void Preferences::addHighlights(QComboBox *box, QVector<PColor*> pcolor)
{
    for(int n = 0; n < pcolor.count(); n++) {
        QPixmap pixmap(20,20);
        pixmap.fill(pcolor.at(n)->getValue());
        QIcon icon(pixmap);
        box->addItem(icon, static_cast<PColor*>(propertyColors[n])->getName());
    }
}

void Preferences::setupHighlight()
{
    QGridLayout *hlayout = new QGridLayout();
    QFrame *hlbox = new QFrame();
    hlbox->setLayout(hlayout);
    tabWidget.addTab(hlbox,tr("Highlight"));

    propertyColors.insert(PColor::Black, new PColor(tr("Black"), "Black", Qt::black));
    propertyColors.insert(PColor::DarkGray, new PColor(tr("Dark Gray"), "Dark Gray", Qt::darkGray));
    propertyColors.insert(PColor::Gray, new PColor(tr("Gray"), "Gray", Qt::gray));
    propertyColors.insert(PColor::LightGray, new PColor(tr("Light Gray"), "Light Gray", Qt::lightGray));
    propertyColors.insert(PColor::Blue, new PColor(tr("Blue"), "Blue", Qt::blue));
    propertyColors.insert(PColor::DarkBlue, new PColor(tr("Dark Blue"), "Dark Blue", Qt::darkBlue));
    propertyColors.insert(PColor::Cyan, new PColor(tr("Cyan"), "Cyan", Qt::cyan));
    propertyColors.insert(PColor::DarkCyan, new PColor(tr("Dark Cyan"), "Dark Cyan", Qt::darkCyan));
    propertyColors.insert(PColor::Green, new PColor(tr("Green"), "Green", Qt::green));
    propertyColors.insert(PColor::DarkGreen, new PColor(tr("Dark Green"), "Dark Green", Qt::darkGreen));
    propertyColors.insert(PColor::Magenta, new PColor(tr("Magenta"), "Magenta", Qt::magenta));
    propertyColors.insert(PColor::DarkMagenta, new PColor(tr("Dark Magenta"), "Dark Magenta", Qt::darkMagenta));
    propertyColors.insert(PColor::Red, new PColor(tr("Red"), "Red", Qt::red));
    propertyColors.insert(PColor::DarkRed, new PColor(tr("Dark Red"), "Dark Red", Qt::darkRed));
    propertyColors.insert(PColor::Yellow, new PColor(tr("Yellow"), "Yellow", Qt::yellow));
    propertyColors.insert(PColor::DarkYellow, new PColor(tr("Dark Yellow"), "Dark Yellow", Qt::darkYellow));
    propertyColors.insert(PColor::White, new PColor(tr("White"), "White", Qt::white));

    QStringList colorlist;
    for(int n = 0; n < propertyColors.count(); n++) {
        colorlist.append(static_cast<PColor*>(propertyColors[n])->getName());
    }

    QSettings settings(publisherKey, PropellerIdeGuiKey);
    QVariant var;

    int hlrow = 0;
    bool checkBold = true;

    QLabel *lNumStyle = new QLabel(tr("Numbers"));
    hlayout->addWidget(lNumStyle,hlrow,0);
    hlNumWeight.setText(tr("Bold"));
    hlNumWeight.setChecked(false);
    hlayout->addWidget(&hlNumWeight,hlrow,1);
    hlNumStyle.setText(tr("Italic"));
    hlNumStyle.setChecked(false);
    hlayout->addWidget(&hlNumStyle,hlrow,2);
    addHighlights(&hlNumColor, propertyColors);
    hlayout->addWidget(&hlNumColor,hlrow,3);
    hlNumColor.setCurrentIndex(PColor::Magenta);
    hlrow++;

    var = settings.value(hlNumWeightKey,checkBold);
    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        hlNumWeight.setChecked(var.toBool());
        settings.setValue(hlNumWeightKey,var.toBool());
    }

    var = settings.value(hlNumStyleKey,false);
    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        hlNumStyle.setChecked(var.toBool());
        settings.setValue(hlNumStyleKey,var.toBool());
    }

    var = settings.value(hlNumColorKey,PColor::Magenta);
    if(var.canConvert(QVariant::Int)) {
        QString s = var.toString();
        int n = var.toInt();
        hlNumColor.setCurrentIndex(n);
        settings.setValue(hlNumColorKey,n);
    }

    QLabel *lFuncStyle = new QLabel(tr("Functions"));
    hlayout->addWidget(lFuncStyle,hlrow,0);
    hlFuncWeight.setText(tr("Bold"));
    hlFuncWeight.setChecked(false);
    hlayout->addWidget(&hlFuncWeight,hlrow,1);
    hlFuncStyle.setText(tr("Italic"));
    hlFuncStyle.setChecked(false);
    hlayout->addWidget(&hlFuncStyle,hlrow,2);
    addHighlights(&hlFuncColor, propertyColors);
    hlayout->addWidget(&hlFuncColor,hlrow,3);
    hlFuncColor.setCurrentIndex(PColor::Blue);
    hlrow++;

    var = settings.value(hlFuncWeightKey,false);
    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        hlFuncWeight.setChecked(var.toBool());
        settings.setValue(hlFuncWeightKey,var.toBool());
    }

    var = settings.value(hlFuncStyleKey,checkBold);
    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        hlFuncStyle.setChecked(var.toBool());
        settings.setValue(hlFuncStyleKey,var.toBool());
    }

    var = settings.value(hlFuncColorKey,PColor::Blue);
    if(var.canConvert(QVariant::Int)) {
        QString s = var.toString();
        int n = var.toInt();
        hlFuncColor.setCurrentIndex(n);
        settings.setValue(hlFuncColorKey,n);
    }

    QLabel *lKeyWordStyle = new QLabel(tr("Key Words"));
    hlayout->addWidget(lKeyWordStyle,hlrow,0);
    hlKeyWordWeight.setText(tr("Bold"));
    hlKeyWordWeight.setChecked(checkBold);
    hlayout->addWidget(&hlKeyWordWeight,hlrow,1);
    hlKeyWordStyle.setText(tr("Italic"));
    hlKeyWordStyle.setChecked(false);
    hlayout->addWidget(&hlKeyWordStyle,hlrow,2);
    addHighlights(&hlKeyWordColor, propertyColors);
    hlayout->addWidget(&hlKeyWordColor,hlrow,3);
    hlKeyWordColor.setCurrentIndex(PColor::DarkBlue);
    hlrow++;

    var = settings.value(hlKeyWordWeightKey,checkBold);
    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        hlKeyWordWeight.setChecked(var.toBool());
        settings.setValue(hlKeyWordWeightKey,var.toBool());
    }

    var = settings.value(hlKeyWordStyleKey,false);
    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        hlKeyWordStyle.setChecked(var.toBool());
        settings.setValue(hlKeyWordStyleKey,var.toBool());
    }

    var = settings.value(hlKeyWordColorKey,PColor::DarkBlue);
    if(var.canConvert(QVariant::Int)) {
        QString s = var.toString();
        int n = var.toInt();
        hlKeyWordColor.setCurrentIndex(n);
        settings.setValue(hlKeyWordColorKey,n);
    }

    QLabel *lPreProcStyle = new QLabel(tr("Pre-Processor"));
    hlayout->addWidget(lPreProcStyle,hlrow,0);
    hlPreProcWeight.setText(tr("Bold"));
    hlPreProcWeight.setChecked(false);
    hlayout->addWidget(&hlPreProcWeight,hlrow,1);
    hlPreProcStyle.setText(tr("Italic"));
    hlPreProcStyle.setChecked(false);
    hlayout->addWidget(&hlPreProcStyle,hlrow,2);
    addHighlights(&hlPreProcColor, propertyColors);
    hlayout->addWidget(&hlPreProcColor,hlrow,3);
    hlPreProcColor.setCurrentIndex(PColor::DarkYellow);
    hlrow++;

    var = settings.value(hlPreProcWeightKey,false);
    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        hlPreProcWeight.setChecked(var.toBool());
        settings.setValue(hlPreProcWeightKey,var.toBool());
    }

    var = settings.value(hlPreProcStyleKey,false);
    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        hlPreProcStyle.setChecked(var.toBool());
        settings.setValue(hlPreProcStyleKey,var.toBool());
    }

    var = settings.value(hlPreProcColorKey,PColor::DarkYellow);
    if(var.canConvert(QVariant::Int)) {
        QString s = var.toString();
        int n = var.toInt();
        hlPreProcColor.setCurrentIndex(n);
        settings.setValue(hlPreProcColorKey,n);
    }

    QLabel *lQuoteStyle = new QLabel(tr("Quotes"));
    hlayout->addWidget(lQuoteStyle,hlrow,0);
    hlQuoteWeight.setText(tr("Bold"));
    hlQuoteWeight.setChecked(false);
    hlayout->addWidget(&hlQuoteWeight,hlrow,1);
    hlQuoteStyle.setText(tr("Italic"));
    hlQuoteStyle.setChecked(false);
    hlayout->addWidget(&hlQuoteStyle,hlrow,2);
    addHighlights(&hlQuoteColor, propertyColors);
    hlayout->addWidget(&hlQuoteColor,hlrow,3);
    hlQuoteColor.setCurrentIndex(PColor::Red);
    hlrow++;

    var = settings.value(hlQuoteWeightKey,false);
    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        hlQuoteWeight.setChecked(var.toBool());
        settings.setValue(hlQuoteWeightKey,var.toBool());
    }

    var = settings.value(hlQuoteStyleKey,false);
    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        hlQuoteStyle.setChecked(var.toBool());
        settings.setValue(hlQuoteStyleKey,var.toBool());
    }

    var = settings.value(hlQuoteColorKey,PColor::Red);
    if(var.canConvert(QVariant::Int)) {
        QString s = var.toString();
        int n = var.toInt();
        hlQuoteColor.setCurrentIndex(n);
        settings.setValue(hlQuoteColorKey,n);
    }

    QLabel *lLineComStyle = new QLabel(tr("Line Comments"));
    hlayout->addWidget(lLineComStyle,hlrow,0);
    hlLineComWeight.setText(tr("Bold"));
    hlLineComWeight.setChecked(false);
    hlayout->addWidget(&hlLineComWeight,hlrow,1);
    hlLineComStyle.setText(tr("Italic"));
    hlLineComStyle.setChecked(false);
    hlayout->addWidget(&hlLineComStyle,hlrow,2);
    addHighlights(&hlLineComColor, propertyColors);
    hlayout->addWidget(&hlLineComColor,hlrow,3);
    hlLineComColor.setCurrentIndex(PColor::Green);
    hlrow++;

    var = settings.value(hlLineComWeightKey,false);
    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        hlLineComWeight.setChecked(var.toBool());
        settings.setValue(hlLineComWeightKey,var.toBool());
    }

    var = settings.value(hlLineComStyleKey,false);
    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        hlLineComStyle.setChecked(var.toBool());
        settings.setValue(hlLineComStyleKey,var.toBool());
    }

    var = settings.value(hlLineComColorKey,PColor::DarkGreen);
    if(var.canConvert(QVariant::Int)) {
        QString s = var.toString();
        int n = var.toInt();
        hlLineComColor.setCurrentIndex(n);
        settings.setValue(hlLineComColorKey,n);
    }

    QLabel *lBlockComStyle = new QLabel(tr("Block Comments"));
    hlayout->addWidget(lBlockComStyle,hlrow,0);
    hlBlockComWeight.setText(tr("Bold"));
    hlBlockComWeight.setChecked(false);
    hlayout->addWidget(&hlBlockComWeight,hlrow,1);
    hlBlockComStyle.setText(tr("Italic"));
    hlBlockComStyle.setChecked(false);
    hlayout->addWidget(&hlBlockComStyle,hlrow,2);
    addHighlights(&hlBlockComColor, propertyColors);
    hlayout->addWidget(&hlBlockComColor,hlrow,3);
    hlBlockComColor.setCurrentIndex(PColor::Green);
    hlrow++;

    var = settings.value(hlBlockComWeightKey,false);
    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        hlBlockComWeight.setChecked(var.toBool());
        settings.setValue(hlBlockComWeightKey,var.toBool());
    }

    var = settings.value(hlBlockComStyleKey,false);
    if(var.canConvert(QVariant::Bool)) {
        QString s = var.toString();
        hlBlockComStyle.setChecked(var.toBool());
        settings.setValue(hlBlockComStyleKey,var.toBool());
    }

    var = settings.value(hlBlockComColorKey,PColor::DarkGreen);
    if(var.canConvert(QVariant::Int)) {
        QString s = var.toString();
        int n = var.toInt();
        hlBlockComColor.setCurrentIndex(n);
        settings.setValue(hlBlockComColorKey,n);
    }

}

Qt::GlobalColor Preferences::getQtColor(int index)
{
    if(index > -1 && index < propertyColors.count()) {
        return static_cast<PColor*>(propertyColors.at(index))->getValue();
    }
    return Qt::black; // just return black on failure
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

    settings.setValue(hlNumStyleKey,hlNumStyle.isChecked());
    settings.setValue(hlNumWeightKey,hlNumWeight.isChecked());
    settings.setValue(hlNumColorKey,hlNumColor.currentIndex());
    settings.setValue(hlFuncStyleKey,hlFuncStyle.isChecked());
    settings.setValue(hlFuncWeightKey,hlFuncWeight.isChecked());
    settings.setValue(hlFuncColorKey,hlFuncColor.currentIndex());
    settings.setValue(hlKeyWordStyleKey,hlKeyWordStyle.isChecked());
    settings.setValue(hlKeyWordWeightKey,hlKeyWordWeight.isChecked());
    settings.setValue(hlKeyWordColorKey,hlKeyWordColor.currentIndex());
    settings.setValue(hlPreProcStyleKey,hlPreProcStyle.isChecked());
    settings.setValue(hlPreProcWeightKey,hlPreProcWeight.isChecked());
    settings.setValue(hlPreProcColorKey,hlPreProcColor.currentIndex());
    settings.setValue(hlQuoteStyleKey,hlQuoteStyle.isChecked());
    settings.setValue(hlQuoteWeightKey,hlQuoteWeight.isChecked());
    settings.setValue(hlQuoteColorKey,hlQuoteColor.currentIndex());
    settings.setValue(hlLineComStyleKey,hlLineComStyle.isChecked());
    settings.setValue(hlLineComWeightKey,hlLineComWeight.isChecked());
    settings.setValue(hlLineComColorKey,hlLineComColor.currentIndex());
    settings.setValue(hlBlockComStyleKey,hlBlockComStyle.isChecked());
    settings.setValue(hlBlockComWeightKey,hlBlockComWeight.isChecked());
    settings.setValue(hlBlockComColorKey,hlBlockComColor.currentIndex());

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
    hlNumStyle.setChecked(hlNumStyleBool);
    hlNumWeight.setChecked(hlNumWeightBool);
    hlNumColor.setCurrentIndex(hlNumColorIndex);
    hlFuncStyle.setChecked(hlFuncStyleBool);
    hlFuncStyle.setChecked(hlFuncWeightBool);
    hlFuncColor.setCurrentIndex(hlFuncColorIndex);
    hlKeyWordStyle.setChecked(hlKeyWordStyleBool);
    hlKeyWordWeight.setChecked(hlKeyWordWeightBool);
    hlKeyWordColor.setCurrentIndex(hlKeyWordColorIndex);
    hlPreProcStyle.setChecked(hlPreProcStyleBool);
    hlPreProcWeight.setChecked(hlPreProcWeightBool);
    hlPreProcColor.setCurrentIndex(hlPreProcColorIndex);
    hlQuoteStyle.setChecked(hlQuoteStyleBool);
    hlQuoteWeight.setChecked(hlQuoteWeightBool);
    hlQuoteColor.setCurrentIndex(hlQuoteColorIndex);
    hlLineComStyle.setChecked(hlLineComStyleBool);
    hlLineComWeight.setChecked(hlLineComWeightBool);
    hlLineComColor.setCurrentIndex(hlLineComColorIndex);
    hlBlockComStyle.setChecked(hlBlockComStyleBool);
    hlBlockComWeight.setChecked(hlBlockComWeightBool);
    hlBlockComColor.setCurrentIndex(hlBlockComColorIndex);

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
    this->setWindowTitle(QString(PropellerIdeGuiKey) +" Preferences");

    tabSpacesStr = tabspaceLedit.text();
    hlNumStyleBool = hlNumStyle.isChecked();
    hlNumWeightBool = hlNumWeight.isChecked();
    hlNumColorIndex = hlNumColor.currentIndex();
    hlFuncStyleBool = hlFuncStyle.isChecked();
    hlFuncWeightBool = hlFuncStyle.isChecked();
    hlFuncColorIndex = hlFuncColor.currentIndex();
    hlKeyWordStyleBool = hlKeyWordStyle.isChecked();
    hlKeyWordWeightBool = hlKeyWordWeight.isChecked();
    hlKeyWordColorIndex = hlKeyWordColor.currentIndex();
    hlPreProcStyleBool = hlPreProcStyle.isChecked();
    hlPreProcWeightBool = hlPreProcWeight.isChecked();
    hlPreProcColorIndex = hlPreProcColor.currentIndex();
    hlQuoteStyleBool = hlQuoteStyle.isChecked();
    hlQuoteWeightBool = hlQuoteWeight.isChecked();
    hlQuoteColorIndex = hlQuoteColor.currentIndex();
    hlLineComStyleBool = hlLineComStyle.isChecked();
    hlLineComWeightBool = hlLineComWeight.isChecked();
    hlLineComColorIndex = hlLineComColor.currentIndex();
    hlBlockComStyleBool = hlBlockComStyle.isChecked();
    hlBlockComWeightBool = hlBlockComWeight.isChecked();
    hlBlockComColorIndex = hlBlockComColor.currentIndex();

    autoCompleteEnableSaved = autoCompleteEnable.isChecked();
    spinSuggestEnableSaved = spinSuggestEnable.isChecked();

    this->show();
}

QString Preferences::getSpinLibraryString()
{
    return this->lineEditLibrary.text();
}
