#pragma once

#include "ui_templateicon.h"

class TemplateIcon : public QWidget 
{
    Q_OBJECT

    Ui::TemplateIcon ui;

    QString _filename;
    QIcon _icon;
    QColor _color;

private slots:

public:
    explicit TemplateIcon(QString filename = QString(), QIcon icon = QIcon(), QWidget * parent = 0);
    ~TemplateIcon();

public slots:
    void setColor(QColor color);

protected:
    virtual void enterEvent(QEvent * e);
    virtual void leaveEvent(QEvent * e);
    virtual void mousePressEvent(QMouseEvent * e);
    virtual void mouseReleaseEvent(QMouseEvent * e);
    virtual void mouseDoubleClickEvent(QMouseEvent * e);
};
