#pragma once

#include <QStringList>

#include "ui_newfromtemplate.h"

#include "templateicon.h"

class NewFromTemplate : public QDialog 
{
    Q_OBJECT

    QString _category;
    QHash<QString, QList<TemplateIcon *> > _templates;
    QGridLayout * layout;

    QString _selected;

    Ui::NewFromTemplate ui;

private slots:
    void templateSelected(const QString & filename);

public:
    explicit NewFromTemplate(
            QStringList paths = QStringList(),
            QWidget * parent = 0);
    ~NewFromTemplate();
    void addTemplates(QString category, QStringList templates);
    QString selectedTemplate();

public slots:

protected:
    void resizeEvent(QResizeEvent * e);

private:
    void addWidget(QWidget * widget);
    void rebuildLayout(QList<TemplateIcon *> templates);
};
