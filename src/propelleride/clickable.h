#pragma once

#include <QLabel>
#include <QMouseEvent>
#include <QString>

class ClickableLabel : public QLabel
{
    Q_OBJECT
  
public:
    explicit ClickableLabel(QWidget * parent = 0 )
        : QLabel(parent)
    {
    }

    ~ClickableLabel()
    {
    }
           
signals:
    void clicked();

protected:
    void mousePressEvent ( QMouseEvent * event )
    {
        event->accept();
        emit clicked();
    }
};

