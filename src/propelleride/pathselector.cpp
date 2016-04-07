#include "pathselector.h"

PathSelector::PathSelector(QString language,
        QString compiler,
        QStringList includes,
        QWidget *parent)
: QWidget(parent)
{
    ui.setupUi(this);

    this->language = language;

    ui.name->setText(language);

    setDefaultCompiler(compiler);
    setDefaultIncludes(includes);
    restore();

    load();
    save();

    connect(ui.deletePath,  SIGNAL(clicked()),  this,   SLOT(deletePath()));
    connect(ui.addPath,     SIGNAL(clicked()),  this,   SLOT(addPath()));
    connect(ui.browse,      SIGNAL(clicked()),  this,   SLOT(browse()));
}

PathSelector::~PathSelector()
{
}

void PathSelector::setCompiler(const QString & path)
{
    compiler = path;
    ui.compiler->setText(path);
}

void PathSelector::setIncludes(const QStringList & paths)
{
    QStringList p = paths;
    includes.clear();
    ui.includes->clear();
    foreach (QString i, p)
    {
        includes.append(i);

        QListWidgetItem * item = new QListWidgetItem(i);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui.includes->addItem(item);
    }
}

void PathSelector::setDefaultCompiler(QString path)
{
    defaultcompiler = path;
}

void PathSelector::setDefaultIncludes(QStringList paths)
{
    defaultincludes = paths;
}

void PathSelector::browse()
{
    QString path = QFileDialog::getOpenFileName(this,
                tr("Browse For Compiler"), "");

    if(path.isEmpty()) return;

    setCompiler(path);
}

void PathSelector::addPath()
{
    int i = ui.includes->currentRow();

    QString path = QFileDialog::getExistingDirectory(this,
        tr("Add Includes"), "", QFileDialog::ShowDirsOnly);

    if(path.isEmpty()) return;

    QListWidgetItem * item = new QListWidgetItem(path);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    if (i+1 >= ui.includes->count())
        ui.includes->addItem(item);
    else
        ui.includes->insertItem(i+1, item);
}

void PathSelector::deletePath()
{
    int i = ui.includes->currentRow();

    QListWidgetItem * item;
    item = ui.includes->takeItem(i);
    delete item;
}

void PathSelector::restore()
{
    setCompiler(defaultcompiler);
    setIncludes(defaultincludes);
}

void PathSelector::reject()
{
    setCompiler(compiler);
    setIncludes(includes);
}

void PathSelector::accept()
{
    compiler = ui.compiler->text();

    includes.clear();
    for (int i = 0; i < ui.includes->count(); i++)
    {
        includes.append(ui.includes->item(i)->text());
    }

    save();
}

void PathSelector::save()
{
    QSettings settings;
    settings.beginGroup("Paths");
    settings.beginGroup(language);

    settings.setValue("compiler",ui.compiler->text());
    settings.setValue("includes",includes);

    settings.endGroup();
    settings.endGroup();
}

void PathSelector::load()
{
    QSettings settings;
    settings.beginGroup("Paths");
    settings.beginGroup(language);

    QString cmp = settings.value("compiler", defaultcompiler).toString();
    setCompiler(cmp);

    QStringList inc = settings.value("includes", defaultincludes).toStringList();
    setIncludes(inc);

    settings.endGroup();
    settings.endGroup();
}

