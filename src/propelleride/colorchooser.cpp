#include "colorchooser.h"

#include <QColor>
#include <QMouseEvent>
#include <QColorDialog>

#include "colorscheme.h"

ColorChooser::ColorChooser(QWidget *parent) :
    QLabel(parent)
{
}

ColorChooser::~ColorChooser()
{
}

void ColorChooser::setKey(int key)
{
    _key = key;
}

void ColorChooser::setColor(QColor color)
{
    _color = color;
    QPalette p = this->palette();
    p.setColor(QPalette::Window, color);
    this->setPalette(p);
}

void ColorChooser::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        QColor color = QColorDialog::getColor(_color, this);
        if(color.isValid())
        {
            setColor(color);
            emit sendColor(_key, _color);
        }
    }
}

void ColorChooser::updateColor()
{
    setColor(Singleton<ColorScheme>::Instance().getColor((ColorScheme::Color) _key));
}
