#include "newfromtemplate.h"

#include <QDebug>
#include <QDirIterator>
#include <QGridLayout>

NewFromTemplate::NewFromTemplate(
        QStringList paths,
        QWidget * parent)
: QDialog(parent)
{
    ui.setupUi(this);

    layout = NULL;

    QStringList temps;

    foreach (QString path, paths)
    {
        QDirIterator it(path + "/templates", QStringList() << "*.spin");
        while (it.hasNext())
        {
            temps.append(it.next());
        }
    }

    temps.sort();

    addTemplates("Spin",temps);

    rebuildLayout(_templates[_category]);
}

NewFromTemplate::~NewFromTemplate()
{
}

void NewFromTemplate::rebuildLayout(QList<TemplateIcon *> templates)
{
    if (layout != NULL)
    {
        foreach (TemplateIcon * t, templates)
        {
            layout->removeWidget(t);
        }
        delete layout;
    }

    layout = new QGridLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    ui.grid->setLayout(layout);

    layout->invalidate();

    foreach (TemplateIcon * t, templates)
    {
        addWidget(t);
    }

    layout->update();
}


void NewFromTemplate::resizeEvent(QResizeEvent * e)
{
    Q_UNUSED(e);
    rebuildLayout(_templates[_category]);
}

void NewFromTemplate::addWidget(QWidget * widget)
{
    int row = 0, col = 0;
    int width = 0;
    int newwidth = widget->geometry().width();

    while (layout->itemAtPosition(row, col))
    {
        width += layout->itemAtPosition(row, col)->geometry().width();

        if (width + newwidth >= ui.scrollArea->width())
        {
            width = 0;
            col = 0;
            row++;
        }
        else
        {
            col++;
        }
    }

    layout->addWidget(widget, row, col);
}

void NewFromTemplate::addTemplates(QString category, QStringList templates)
{
    _category = category;
    ui.listWidget->addItem(category);

    QList<TemplateIcon *> newtemplates;

    foreach (QString t, templates)
    {
        TemplateIcon * tmp = new TemplateIcon(t);
        connect(tmp,    SIGNAL(templateSelected(const QString &)),
                this,   SLOT(templateSelected(const QString &)));
        newtemplates.append(tmp);
    }

    _templates[category] = newtemplates;
}


QString NewFromTemplate::selectedTemplate()
{
    return _selected;
}

void NewFromTemplate::templateSelected(const QString & filename)
{
    _selected = filename;
    close();
}
