#ifndef GRAPHLINE_H
#define GRAPHLINE_H

#include "graphwidget.h"

class GraphLine : public QWidget
{
    Q_OBJECT
public:
    explicit GraphLine(QWidget *parent = 0);
    QToolBar *getTools();

public:
    void paint(QPainter *painter, QPaintEvent *event, int = 0);
    int stringToValue(QString vs, int *ival, float *fval);
    int queueLine(QString str, int linenum, QList<int> *lineptr);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QBrush  background;
    QBrush  lineBrush;
    QFont   textFont;
    QPen    linePen;
    QPen    textPen;

    QTime   time;
    int elapsed;

    QTimer  *displayTimer;
    QToolBar *tools;

    QToolTip *tip;

    QAction *pause;
    QAction *plm;
    QAction *log;
    QAction *hlc;
    QAction *status;

    bool    updateGraph;

    // hmm ... looks like need a line class
    //
    QAction *enLine0;
    QAction *enLine1;
    QAction *enLine2;
    QAction *enLine3;

    bool    lines[4];
    int     linetype[4];

    QList<int> lineq0;
    QList<int> lineq1;
    QList<int> lineq2;
    QList<int> lineq3;

    QList<int> validq;

    int     maxlinex;
    int     lastx;

    QString statusText;
    QString delimiter;
    QString xdelimiter;


signals:

public slots:
    void animate();
    void queueGraph(QString value);
    void showHelp();
    void setDelimiter();
    void setCheckDelimiter(QString s, bool ok, QString &del);
    void resetGraph();
};

#endif // GRAPHLINE_H

