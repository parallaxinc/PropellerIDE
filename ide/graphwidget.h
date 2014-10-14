#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QBoxLayout>
#include <QPushButton>
#include <QToolBar>
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
#include <QToolTip>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QLabel>
#include <QList>

#define TabConsole      "Console"
#define TabBarGraph     "Bars"
#define TabLineGraph    "Lines"
#define TabPieGraph     "Pie"
#define TabPoleGraph    "Polar"
#define TabRadarGraph   "Radar"
#define TabScatGraph    "Scatter"
#define TabSurfaceGraph "Surface"
#define TabClock        "Clock"

class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *graph, QToolBar *tools, QWidget *parent = 0);

    enum { RunDelay = 100 };

    enum { TypeNone = 0 };
    enum { TypeFloat = 1 };
    enum { TypeBin = 2 };
    enum { TypeDec = 10 };
    enum { TypeHex = 16 };
    enum { TypeText = 100 };

private:
    QVBoxLayout *mainLayout;

signals:

public slots:

};

#endif // GRAPHWIDGET_H
