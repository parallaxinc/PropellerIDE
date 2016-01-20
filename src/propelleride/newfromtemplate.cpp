#include "newfromtemplate.h"

#include <QDebug>
#include <QGridLayout>

NewFromTemplate::NewFromTemplate(
        QStringList paths,
        QWidget * parent)
: QDialog(parent)
{
    ui.setupUi(this);
    layout = NULL;

    QStringList temps;
    temps << "Basic_Terminal.spin" << "More_Files.spin"
        << "Serial_Terminal.spin"
        << "Serial_Terminal2.spin"
        << "Serial_Terminal3.spin";

    addTemplates("spin",temps);

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
    qDebug() << "RESIZE";
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

    qDebug() << "ADD" << row << col << width + newwidth << ui.scrollArea->width();
    layout->addWidget(widget, row, col);
}

void NewFromTemplate::addTemplates(QString category, QStringList templates)
{
    _category = category;
    ui.listWidget->addItem(category);

    QList<TemplateIcon *> newtemplates;

    foreach (QString t, templates)
    {
        newtemplates.append(new TemplateIcon(t));
    }

    _templates[category] = newtemplates;
}
