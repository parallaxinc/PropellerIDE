#include "properties.h"

Properties::Properties(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(QString(PropellerIdeGuiKey)+tr(" Properties"));

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

void Properties::configSettings()
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

void Properties::cleanSettings()
{
    QSettings settings(publisherKey, PropellerIdeGuiKey);
    QStringList list = settings.allKeys();

    foreach(QString key, list) {
        if(key.indexOf(PropellerIdeGuiKey) == 0) {
            settings.remove(key);
        }
    }
    // mac doesn't filter settings by publisher, etc...
    //settings.setValue(useKeys,1);
    //list = settings.allKeys(); // debug only
    settings.setValue(useKeys,0);
}

void Properties::setupOptions()
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
    tabWidget.addTab(frame,"General");

    QSettings settings(publisherKey, PropellerIdeGuiKey,this);
    QVariant enac = settings.value(enableAutoComplete,true);
    QVariant enss = settings.value(enableSpinSuggest,true);

    autoCompleteEnable.setChecked(enac.toBool());
    edlayout->addRow(new QLabel(tr("Enable AutoComplete")), &autoCompleteEnable);

    spinSuggestEnable.setChecked(enss.toBool());
    edlayout->addRow(new QLabel(tr("Enable Code Suggestion")), &spinSuggestEnable);

#if ENABLE_PORT_BOARD_NAMES
    portBoardNameEnable.setChecked(false);
    portBoardNameEnable.setDisabled(true);
    otlayout->addRow(new QLabel(tr("Enable Port Board Names")), &portBoardNameEnable);
#endif

    QVariant tabsv = settings.value(tabSpacesKey,"4");
    if(tabsv.canConvert(QVariant::String)) {
        tabspaceLedit.setText(tabsv.toString());
    }
    else {
        tabspaceLedit.setText("4");
    }
    edlayout->addRow(new QLabel(tr("Editor Tab Space Count")), &tabspaceLedit);

    clearSettingsButton.setText("Clear Settings");
    clearSettingsButton.setToolTip(tr("Clear Settings on Exit"));
    connect(&clearSettingsButton,SIGNAL(clicked()), this, SLOT(cleanSettings()));
    otlayout->addRow(new QLabel(tr("Clear Settings on Exit")), &clearSettingsButton);

    fontButton.setText(tr("Set Editor Font"));
    connect(&fontButton,SIGNAL(clicked()),this,SLOT(showFontDialog()));
    edlayout->addRow(new QLabel(tr("Set Editor Font")), &fontButton);
}

void Properties::showFontDialog()
{
    emit openFontDialog();
}

int Properties::getTabSpaces()
{
    bool ok;
    int count = tabspaceLedit.text().toInt(&ok);
    if(ok) return count;
    return 2;
}

bool Properties::getAutoCompleteEnable()
{
    return autoCompleteEnable.isChecked();;
}

bool Properties::getSpinSuggestEnable()
{
    return spinSuggestEnable.isChecked();
}

bool Properties::getPortBoardNameEnable()
{
#if ENABLE_PORT_BOARD_NAMES
    return portBoardNameEnable.isChecked();
#else
    return false;
#endif
}

QLineEdit *Properties::getTabSpaceLedit()
{
    return &tabspaceLedit;
}

void Properties::setupFolders()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QFrame *box = new QFrame();
    box->setLayout(layout);
    tabWidget.addTab(box,"System");

    int minledit = 300;

    // Compiler Path Selector
    QLabel *spinCmpLabel = new QLabel("Compiler");
    spinCmpLabel->setMinimumWidth(70);

    QHBoxLayout *spincLayout = new QHBoxLayout();
    spinLeditCompiler = new QLineEdit(this);
    spinLeditCompiler->setMinimumWidth(minledit);
    spinLeditCompiler->setToolTip("Must add a Spin compiler.");
    spincLayout->addWidget(spinCmpLabel);
    spincLayout->addWidget(spinLeditCompiler);
    QPushButton *spinBtnCompilerBrowse = new QPushButton(tr("Browse"), this);
    spincLayout->addWidget(spinBtnCompilerBrowse);

    layout->addLayout(spincLayout);


    // Loader Path Selector
    QLabel *spinLoadLabel = new QLabel("Loader");
    spinLoadLabel->setMinimumWidth(70);

    QHBoxLayout *spinLoadLayout = new QHBoxLayout();
    spinLoadLedit = new QLineEdit(this);
    spinLoadLedit->setMinimumWidth(minledit);
    spinLoadLedit->setToolTip("Must add a loader program.");
    spinLoadLayout->addWidget(spinLoadLabel);
    spinLoadLayout->addWidget(spinLoadLedit);
    QPushButton *spinLoaderBtnBrowse = new QPushButton(tr("Browse"), this);
    spinLoadLayout->addWidget(spinLoaderBtnBrowse);

    layout->addLayout(spinLoadLayout);


    // Library Path Selector
    QLabel *spinIncLabel = new QLabel("Library");
    spinIncLabel->setMinimumWidth(70);

    QHBoxLayout *spiniLayout = new QHBoxLayout();
    spinLeditIncludes = new QLineEdit(this);
    spinLeditIncludes->setMinimumWidth(minledit);
    spinLeditIncludes->setToolTip("Add Spin library folder here.");
    spiniLayout->addWidget(spinIncLabel);
    spiniLayout->addWidget(spinLeditIncludes);
    QPushButton *spinBtnIncludesBrowse = new QPushButton(tr("Browse"), this);
    spiniLayout->addWidget(spinBtnIncludesBrowse);

    layout->addLayout(spiniLayout);

    connect(spinBtnCompilerBrowse, SIGNAL(clicked()), this, SLOT(spinBrowseCompiler()));
    connect(spinBtnIncludesBrowse, SIGNAL(clicked()), this, SLOT(spinBrowseIncludes()));

    QSettings settings(publisherKey, PropellerIdeGuiKey,this);

    QString compiler("/openspin");
#ifdef Q_OS_WIN
    QString library("/spin/");
#else
    QString library("/../spin/");
#endif
    compiler = QApplication::applicationDirPath()+compiler;
    library  = QApplication::applicationDirPath()+library;

#ifdef  Q_OS_WIN
    compiler += ".exe";
#endif

    QVariant compv = settings.value(spinCompilerKey, compiler);
    QVariant incv = settings.value(spinIncludesKey, library);

    if(compv.canConvert(QVariant::String)) {
        QString s = compv.toString();
        s = QDir::fromNativeSeparators(s);
        if(s.length() > 0) {
            spinLeditCompiler->setText(s);
        }
        else {
            spinLeditCompiler->setText(compiler);
        }
    }
    else {
        spinLeditCompiler->setText(compiler);
    }

    if(incv.canConvert(QVariant::String)) {
        QString s = incv.toString();
        s = QDir::fromNativeSeparators(s);
        if(s.length() > 0) {
            spinLeditIncludes->setText(s);
        }
        else {
            spinLeditIncludes->setText(library);
        }
    }
    else {
        spinLeditIncludes->setText(library);
    }

    settings.setValue(spinCompilerKey,spinLeditCompiler->text());
    settings.setValue(spinIncludesKey,spinLeditIncludes->text());

#ifdef EXTERNAL_SPIN_LOADER
    QString loader("/p1load");
#ifdef Q_OS_WIN
    loader += ".exe";
#endif
    loader = QApplication::applicationDirPath()+loader;

    connect(spinLoaderBtnBrowse, SIGNAL(clicked()), this, SLOT(spinBrowseLoader()));
    QVariant loadv = settings.value(spinLoaderKey,loader);

    if(loadv.canConvert(QVariant::String)) {
        QString s = loadv.toString();
        s = QDir::fromNativeSeparators(s);
        if(s.length() > 0) {
            spinLoadLedit->setText(s);
        }
        else {
            spinLoadLedit->setText(loader);
        }
    }
    else {
        spinLoadLedit->setText(loader);
    }

    settings.setValue(spinLoaderKey,spinLoadLedit->text());
#endif

}

void Properties::addHighlights(QComboBox *box, QVector<PColor*> pcolor)
{
    for(int n = 0; n < pcolor.count(); n++) {
        QPixmap pixmap(20,20);
        pixmap.fill(pcolor.at(n)->getValue());
        QIcon icon(pixmap);
        box->addItem(icon, static_cast<PColor*>(propertyColors[n])->getName());
    }
}

void Properties::setupHighlight()
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

    /*
        hlEnableKey                 // not implemented

        hlNumStyleKey               // Numeric style normal = 0 italic = 1
        hlNumWeightKey              // bold weight checked
        hlNumColorKey               // color integer
        hlFuncStyleKey              // function style - see Numeric
        hlFuncWeightKey             // bold weight checked
        hlFuncColorKey              // color integer
        hlKeyWordStyleKey
        hlKeyWordWeightKey
        hlKeyWordColorKey
        hlPreProcStyleKey
        hlPreProcWeightKey
        hlPreProcColorKey
        hlQuoteStyleKey
        hlQuoteWeightKey
        hlQuoteColorKey
        hlLineComStyleKey
        hlLineComWeightKey
        hlLineComColorKey
        hlBlockComStyleKey
        hlBlockComWeightKey
        hlBlockComColorKey
      */

    bool checkBold = true;
#ifdef Q_WS_MAC
    checkBold = false;
#endif

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

Qt::GlobalColor Properties::getQtColor(int index)
{
    if(index > -1 && index < propertyColors.count()) {
        return static_cast<PColor*>(propertyColors.at(index))->getValue();
    }
    return Qt::black; // just return black on failure
}

void Properties::spinBrowseCompiler()
{
    QString folder = spinLeditCompiler->text();
    if(!folder.length()) folder = lastFolder;
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Select Spin Compiler"), folder, "Spin Compiler (openspin* openspin.*)");
    QString s = QDir::fromNativeSeparators(fileName);
    lastFolder = s.mid(0,s.lastIndexOf("/")+1);
    spinCompilerStr = spinLeditCompiler->text();
    if(s.length() > 0)
        spinLeditCompiler->setText(s);
    qDebug() << "browseSpinCompiler" << s;
}

void Properties::spinBrowseIncludes()
{
    QString folder = spinLeditIncludes->text();
    if(!folder.length()) folder = lastFolder;
    QString pathName = QFileDialog::getExistingDirectory(this,
            tr("Select Spin Library Path"), folder, QFileDialog::ShowDirsOnly);
    QString s = QDir::fromNativeSeparators(pathName);
    lastFolder = s.mid(0,s.lastIndexOf("/")+1);
    if(s.length() == 0)
        return;
    if(!s.endsWith("/")) {
        s += "/";
    }
    spinLeditIncludes->setText(s);
    qDebug() << "spinBrowseIncludes" << s;
}

void Properties::spinBrowseLoader()
{
#ifdef EXTERNAL_SPIN_LOADER
    QString folder = spinLoadLedit->text();
    if(!folder.length()) folder = lastFolder;
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Select Loader"), folder, "Spin Loader (*load*)");
    QString s = QDir::fromNativeSeparators(fileName);
    lastFolder = s.mid(0,s.lastIndexOf("/")+1);
    spinLoaderStr = spinLoadLedit->text();
    if(s.length() > 0)
        spinLoadLedit->setText(s);
    qDebug() << "spinBrowseLoader" << s;
#endif
}

void Properties::accept()
{
    QSettings settings(publisherKey, PropellerIdeGuiKey,this);
    settings.setValue(spinCompilerKey,spinLeditCompiler->text());
    settings.setValue(spinIncludesKey,spinLeditIncludes->text());
#ifdef EXTERNAL_SPIN_LOADER
    settings.setValue(spinLoaderKey,spinLoadLedit->text());
#endif

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

void Properties::reject()
{
    spinLeditCompiler->setText(spinCompilerStr);
    spinLeditIncludes->setText(spinIncludesStr);
#ifdef EXTERNAL_SPIN_LOADER
    spinLoadLedit->setText(spinLoaderStr);
#endif

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

void Properties::showProperties(QString lastDir)
{
    this->lastFolder = lastDir;
    spinCompilerStr = spinLeditCompiler->text();
    spinIncludesStr = spinLeditIncludes->text();
#ifdef EXTERNAL_SPIN_LOADER
    spinLoaderStr = spinLoadLedit->text();
#endif
    this->setWindowTitle(QString(PropellerIdeGuiKey) +" Properties");

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

QString Properties::getSpinLibraryString()
{
    return this->spinLeditIncludes->text();
}
