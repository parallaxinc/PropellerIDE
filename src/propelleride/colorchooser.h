#pragma once

#include <QLabel>

class ColorChooser : public QLabel
{
    Q_OBJECT

public:
    explicit ColorChooser(QWidget *parent = 0);
    ~ColorChooser();
    
    int _key;
    QColor _color;

public slots:
    void setKey(int key);
    void setColor(QColor color);
    void updateColor();

signals:
    void sendColor(int, const QColor &);

protected:
    void mousePressEvent(QMouseEvent *event);
    
};
