#include "clock.h"

Clock::Clock(QWidget *parent) :
    QWidget(parent)
{
    QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));

    time = QTime::currentTime();

    background = QBrush(Qt::black);
    circleBrush = QBrush(gradient);
    circlePen = QPen(Qt::black);
    circlePen.setWidth(1);
    lineBrush = QBrush(Qt::white);
    linePen   = QPen(Qt::white);
    linePen.setWidth(1);
    circlePen.setWidth(1);

    textPen = QPen(Qt::green);
    textFont.setPixelSize(50);

    //tools = new QToolBar(tr("Tools"),this);

    displayTimer = new QTimer(this);
    connect(displayTimer, SIGNAL(timeout()), this, SLOT(animate()));
    displayTimer->start(GraphWidget::RunDelay);
}

QToolBar *Clock::getTools()
{
    return 0;
}

void Clock::animate()
{
    //elapsed = (elapsed + qobject_cast<QTimer*>(sender())->interval());
    QDateTime mytime;
    qint64 ms = mytime.toTime_t()*1000;
    elapsed = abs(ms);
    repaint();
}

void Clock::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    paint(&painter, event, elapsed);
    painter.end();
}

void Clock::paint(QPainter *painter, QPaintEvent *event, int elapsed)
{
    int wd = this->width();
    int ht = this->height();
    painter->fillRect(event->rect(), background);
    painter->translate((wd>>1),(ht>>1)-25);

    painter->save();
    painter->setBrush(lineBrush);
    painter->setPen(linePen);
    painter->rotate(0);

    painter->setBrush(circleBrush);
    painter->setPen(linePen);
    painter->drawEllipse(QRectF(-6.0, -6.0, 12.0, 12.0));

    time = QTime::currentTime();

    for (int n = 0; n < 60; n++)
    {
        qreal radius = 160.0; // * factor;
        qreal hstart = -radius/15.0;
        qreal hwdt = 4;
        qreal hend = radius/10;

        painter->setBrush(lineBrush);

        if(time.second() == n) {
            painter->setBrush(lineBrush);
            painter->setPen(linePen);
            painter->drawEllipse(-hwdt/2.0, hstart+2, hwdt, (qreal)(-radius+hend));
        }
        hwdt += 2.0;
        hend *= 2.0;
        if(time.minute() == n) {
            painter->setBrush(lineBrush);
            painter->setPen(linePen);
            painter->drawEllipse(-hwdt/2.0, hstart, hwdt, (qreal)(-radius+hend));
        }
        hwdt += 2.0;
        hend *= 2.0;
        if(time.hour()%12 == n/5 && !(n%5)) {
            painter->setBrush(circleBrush);
            painter->setPen(linePen);
            painter->drawEllipse(-hwdt/2.0, hstart, hwdt, (qreal)(-radius+hend));
        }

        painter->setBrush(lineBrush);
        painter->setPen(linePen);
        painter->drawEllipse(QRectF(radius+2.0, -0.5, 4.0, 1.0));

        if(!(n % 5)) {
            painter->setBrush(circleBrush);
            painter->setPen(circlePen);
            painter->drawEllipse(QRectF(radius, -3.0, 10.0, 6.0));
        }

        painter->rotate(6);
    }
    painter->restore();

    painter->setPen(textPen);
    painter->setFont(textFont);
    painter->drawText(
        QRect(-200, 185, 400, 50),
        Qt::AlignBottom|Qt::AlignHCenter,
        QString("%1:%2:%3 %4")
            .arg(time.hour(),2,10,QChar(QLatin1Char('0')))
            .arg(time.minute(),2,10,QChar(QLatin1Char('0')))
            .arg(time.second(),2,10,QChar(QLatin1Char('0')))
            .arg(time.msec(),3,10,QChar(QLatin1Char('0')))
    );

}
