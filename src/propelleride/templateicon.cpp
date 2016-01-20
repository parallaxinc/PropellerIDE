#include "templateicon.h"

#include <QDebug>

TemplateIcon::TemplateIcon(QString filename,
        QIcon icon,
        QWidget * parent)
: QWidget(parent)
{
    ui.setupUi(this);

    _filename = filename;
    ui.filename->setText(filename.replace("_"," ").remove(".spin"));
    _icon = icon;
}

TemplateIcon::~TemplateIcon()
{
}

void TemplateIcon::enterEvent(QEvent * e)
{
    Q_UNUSED(e);
    setColor(QColor("#97C9FD"));
}

void TemplateIcon::leaveEvent(QEvent * e)
{
    Q_UNUSED(e);
    setColor(QColor("#FFFFFF"));
}


void TemplateIcon::mousePressEvent(QMouseEvent * e)
{
    Q_UNUSED(e);
    setColor(QColor("#2D83DE"));
}

void TemplateIcon::mouseReleaseEvent(QMouseEvent * e)
{
    Q_UNUSED(e);

}

void TemplateIcon::mouseDoubleClickEvent(QMouseEvent * e)
{
    Q_UNUSED(e);
    qDebug() << "DOUBLE CLICK" << _filename;

}

void TemplateIcon::setColor(QColor color)
{
    QPalette p = palette();
    p.setColor(QPalette::Window, color);
    setPalette(p);
}
