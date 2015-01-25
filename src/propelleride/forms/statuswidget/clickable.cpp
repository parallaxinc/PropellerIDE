#include "clickable.h"

ClickableLabel::ClickableLabel(QWidget * parent ) :
    QLabel(parent)
{
}

ClickableLabel::~ClickableLabel()
{
}

void ClickableLabel::mousePressEvent ( QMouseEvent * event )
{
    emit clicked();
}
