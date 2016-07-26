#include "pathselector.h"

PathSelector::PathSelector(QString languagekey,
        QWidget * parent)
: QWidget(parent)
{
    ui.setupUi(this);

    language.loadKey(languagekey);

    if (!language.hasIncludes())
        ui.includeRow->hide();

    ui.name->setText(language.name());
    ui.compiler->clear();

    foreach (QStringList buildsteps, language.listAllBuildSteps())
        ui.compiler->addItem(buildsteps.join(" > "));

    restore();

    load();
    save();

    connect(ui.compiler,    SIGNAL(currentIndexChanged(int)),  this,   SLOT(setBuildSteps(int)));
    connect(ui.deletePath,  SIGNAL(clicked()),  this,   SLOT(deletePath()));
    connect(ui.addPath,     SIGNAL(clicked()),  this,   SLOT(addPath()));
}

PathSelector::~PathSelector()
{
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

void PathSelector::setDefaultIncludes(QStringList paths)
{
    defaultincludes = paths;
}

void PathSelector::setBuildSteps(int index)
{
    language.setBuilder(index);
    qDebug() << "COMPILER" << language.listBuildSteps();
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
    setIncludes(defaultincludes);
}

void PathSelector::reject()
{
    setIncludes(includes);
}

void PathSelector::accept()
{
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
    settings.beginGroup(language.key());

    settings.setValue("builder",ui.compiler->currentIndex());
    settings.setValue("includes",includes);

    settings.endGroup();
    settings.endGroup();
}

void PathSelector::load()
{
    QSettings settings;
    settings.beginGroup("Paths");
    settings.beginGroup(language.key());

    int builder = -1;
    if (language.builders())
        builder = settings.value("builder", 0).toInt();

    ui.compiler->setCurrentIndex(builder);
    language.setBuilder(builder);

    QStringList inc = settings.value("includes", defaultincludes).toStringList();
    setIncludes(inc);

    settings.endGroup();
    settings.endGroup();
}

