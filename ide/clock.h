#ifndef CLOCK_H
#define CLOCK_H

#include <QWidget>

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QPainter>
#include <QPaintEvent>
#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QToolBar>

#include "graphwidget.h"

class Clock : public QWidget
{
    Q_OBJECT
public:
    explicit Clock(QWidget *parent = 0);
    QToolBar *getTools();

public:
    void paint(QPainter *painter, QPaintEvent *event, int elapsed);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QBrush  background;
    QBrush  circleBrush;
    QBrush  lineBrush;
    QFont   textFont;
    QPen    circlePen;
    QPen    linePen;
    QPen    textPen;

    QTime   time;
    int elapsed;

    QTimer  *displayTimer;
    QToolBar *tools;

signals:

public slots:
    void animate();
};

#endif // CLOCK_H

