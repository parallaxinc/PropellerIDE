#pragma once

#include <QStringList>

#include "ui_newfromtemplate.h"

#include "templateicon.h"
#include "language.h"

class NewFromTemplate : public QDialog 
{
    Q_OBJECT

    Language        language;

    QString _category;
    QHash<QString, QList<TemplateIcon *> > _templates;
    QGridLayout * layout;



    Ui::NewFromTemplate ui;

private slots:

public:
    explicit NewFromTemplate(
            QStringList paths = QStringList(),
            QWidget * parent = 0);
    ~NewFromTemplate();
    void addTemplates(QString category, QStringList templates);

public slots:

protected:
    void resizeEvent(QResizeEvent * e);

private:
    void addWidget(QWidget * widget);
    void rebuildLayout(QList<TemplateIcon *> templates);
};
