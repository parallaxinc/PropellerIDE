#include "graphline.h"

GraphLine::GraphLine(QWidget *parent) :
    QWidget(parent)
{
    QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));

    time = QTime::currentTime();

    background = QBrush(QColor(250,240,220));
    lineBrush = QBrush(Qt::black);
    linePen   = QPen(Qt::black);
    linePen.setWidth(2);

    textPen = QPen(Qt::black);
    textFont.setPixelSize(18);

    linetype[0] = 0;
    lines[0] = true;
    for(int n = 1; n < 4; n++) {
        lines[n] = false;
        linetype[n] = 0;
    }

    tools = new QToolBar(tr("Line Graph Tools"), this);

    enLine0 = new QAction("One Line", tools);
    enLine0->setToolTip(tr("Show only one line as Y axis."));
    enLine0->setCheckable(true);
    enLine0->setChecked(true);
    enLine1 = new QAction("Enable XY", tools);
    enLine1->setToolTip(tr("Enable X and Y axis (X,Y)."));
    enLine1->setCheckable(true);
    enLine1->setChecked(true);
    enLine2 = new QAction("Enable Y2", tools);
    enLine2->setToolTip(tr("Enable X and Y2 (X,Y1,Y2)."));
    enLine2->setCheckable(true);
    enLine3 = new QAction("Enable Y3", tools);
    enLine3->setToolTip(tr("Enable X and Y3 (X,Y1,Y2,Y3)."));
    enLine3->setCheckable(true);

#if 0
    sum = new QAction("+/-", tools);
    sum->setCheckable(true);
    mul = new QAction("+/-", tools);
    mul->setCheckable(true);
#endif

    plm = new QAction("+/-", tools);
    plm->setCheckable(true);
    log = new QAction("Log", tools);
    log->setCheckable(true);
    hlc = new QAction("HLC", tools);
    hlc->setCheckable(true);
    pause = new QAction("Pause", tools);
    pause->setCheckable(true);
    statusText = "Status: 0";
    status = new QAction(new QLabel(statusText));
    status->setDisabled(true);
    status->setText(statusText);
    status->setVisible(false);

    tools->addAction(enLine0);
    tools->addAction(enLine1);
    tools->addAction(enLine2);
    tools->addAction(enLine3);
#if 0
    // make per line context popup menu
    tools->addSeparator();
    tools->addAction(plm);
    tools->addAction(log);
    tools->addAction(hlc);
#endif
    tools->addSeparator();
    tools->addAction(tr("Field"), this, SLOT(setDelimiter()));
    tools->addSeparator();
    tools->addAction(pause);
    tools->addAction(tr("Help"), this, SLOT(showHelp()));
    tools->addAction(status);
    delimiter = "";
    xdelimiter = "";

    displayTimer = new QTimer(this);
    connect(displayTimer, SIGNAL(timeout()), this, SLOT(animate()));
    displayTimer->start(GraphWidget::RunDelay);
}

void GraphLine::showHelp()
{
    QDialog d(this);
    d.setWindowFlags(Qt::Window|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);
    d.setWindowTitle(tr("Line Graph Help"));
    QVBoxLayout lay;
    QLabel label;

    d.setLayout(&lay);
    lay.addWidget(&label);

    label.setText("\
        <html><body> \
        <h2>Line Graph</h2> \
        Displays a sequence of values over time. Capture is enabled at startup. \
        <p>The expected serial data format is one of these: (Y)(Y)..., \
        <br/>(X,Y)(X,Y)..., (X,Y,Y)(X,Y,Y)..., or (X,Y,Y,Y)(X,Y,Y,Y)....\
        <p>Types: signed integers, binary, hexadecimal or floating point numbers. \
        <p>Press 'Pause' stop capturing data. \
        </body></html>"
    );
#if 0
/*
        <br/>Press '+/-' to enable +/0/- axis tracking. \
        <br/>Press 'LOG' to enable logaritmic scaling. \
        <br/>Press 'HLC' to enable candlestick scaling. \
*/
#endif
    d.exec();
}

void GraphLine::setCheckDelimiter(QString s, bool ok, QString &del)
{
    if(ok) {
        QRegExp r(s);
        if(s.isEmpty()) {
            del = s;
            return;
        }
        s.replace("*","");
        r.setPatternSyntax(QRegExp::Wildcard);
        if(r.isValid() != true) qDebug() << "Invalid?";
        if(r.indexIn(s) < 0) {
            QMessageBox::critical(this,tr("Invalid Expression"),
                tr("One or more characters in the expression is not valid.")+" \n"+tr("Please try another delimiter string like: () or (,)"));
            return;
        }
        if(s.compare("*") == 0) {
            QMessageBox::critical(this,tr("Invalid Expression"),
                tr("Symbol * is a wildcard.")+" "+tr("It must be followed by a non-wildcard character.")+" \n"+tr("Please try another delimiter string like: () or (,)"));
            return;
        }
        del = s;
    }
}

void GraphLine::setDelimiter()
{
    bool ok = true;
    QString s;
    QInputDialog d(this);
    pause->setChecked(true);
    d.setToolTip(tr("Delimiter string examples")+" '()' '(,)' '(,,)' '(,,,)' '(,,,,)'");
    s = d.getText(this, tr("Field Parser."),
            tr("Enter a field delimiter string.")+" \n"+d.toolTip(),
            QLineEdit::Normal, delimiter, &ok, Qt::Window|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);
    setCheckDelimiter(s, ok, delimiter);
    pause->setChecked(false);
}

void GraphLine::resetGraph()
{
    validq.clear();
    lineq0.clear();
    lineq1.clear();
    lineq2.clear();
    lineq3.clear();
    linetype[0] = 0;
    lines[0] = true;
    for(int n = 1; n < 4; n++) {
        lines[n] = false;
        linetype[n] = 0;
    }
}

#define USE_QREGEXP

void GraphLine::queueGraph(QString value)
{
    bool ok = false;
    int num = 0;

    int pos = 0;
    int count = 0;

    QString s;
    QStringList list;

    foreach(QChar c, value) {
        if(c == 0 || c == 16) {
            resetGraph();
            return;
        }
        if(!c.isPrint())
            value.remove(c);
    }

    if(pause->isChecked()) {
        return;
    }

    /*
     * We use a delimiter string to build an expression: (,) becomes (.*,.*)
     * Must have an open and close delimiter otherwise data is not reliable.
     * Would (,,) work? Yes. Implemented.
     */
#ifdef USE_QREGEXP
    QRegExp ry;
    QRegExp rp;

    s = "(.*\\))";
    rp.setPattern(s);
    rp.setMinimal(true);

    s = "(\\(.*\\))";
    ry.setPattern(s);
    ry.setMinimal(true);
#endif

    for(;;) {
#ifdef USE_QREGEXP
        // QRegExp has a memory leak.
        int hit  = ry.indexIn(value, pos);
        int miss = rp.indexIn(value, pos);
        if(hit > -1 || miss > -1) {
            if(hit != miss) {
                pos = miss;
                count++;
                pos += rp.matchedLength();
                queueLine("0", 0, &validq);
                continue;
            }
            pos = hit;
            // Get a single data element with delimiters.
            s = ry.cap();
        }
        else {
            break;
        }
#else
        int hit  = -1;
        int miss = -1;
        int open = -1;
        int close= -1;
        int n = pos;

        for(; n < value.count(); n++) {
            if(value[n] == '(') {
                open = n;
            }
            if(value[n] == ')') {
                close = n;
                if(open > -1) {
                    hit = (close-open > 0) ? n : -1;
                }
                miss= n;
                break;
            }
        }

        if(hit > -1 || miss > -1) {
            if(hit != miss) {
                pos = miss+1;
                count++;
                queueLine("0", 0, &validq);
                continue;
            }
            pos = hit+1;
            // Get a single data element with delimiters.
            s = value.mid(open,close-open+1);
        }
        else {
            break;
        }
#endif
        // Strip out the delimiters.
        // normally do this, but doesn't work with hit != miss ...
        // s = s.replace(ry,"\\1");
        if(s.at(0) == '(')  s = s.mid(1);
        if(s.endsWith(")")) s = s.left(s.length()-1);

        list = s.split(",");

        if(list.count() != 0) {
            queueLine("1", -1, &validq);
        }

        if(list.count() == 1) {
            queueLine(QString(list[0]), 0, &lineq0);
        }
        else if(list.count() == 2) {
            queueLine(QString(list[0]), 0, &lineq0);
            queueLine(QString(list[1]), 1, &lineq1);
        }
        else if(list.count() == 3) {
            queueLine(QString(list[0]), 0, &lineq0);
            queueLine(QString(list[1]), 1, &lineq1);
            queueLine(QString(list[2]), 2, &lineq2);
        }
        else if(list.count() == 4) {
            queueLine(QString(list[0]), 0, &lineq0);
            queueLine(QString(list[1]), 1, &lineq1);
            queueLine(QString(list[2]), 2, &lineq2);
            queueLine(QString(list[3]), 3, &lineq3);
        }

        updateGraph = true;

        count++;
#ifdef USE_QREGEXP
        pos += ry.matchedLength();
#endif
    }
    //qDebug() << "cap count" << count;
    s = status->text();
    s = s.mid(s.indexOf(":")+1);

    num = s.toInt(&ok);
    num += count;

    // ugh, this causes memory leaks ....
    statusText = "Status: "+QString("%1").arg(num);
}

/**
 * @brief GraphLine::queueLine
 * @param str  Input string parsed from a serial device.
 * @param linenum  Line to stuff.
 * @param lineptr [out] Line list.
 * @return
 */
int GraphLine::queueLine(QString str, int linenum, QList<int> *lineptr)
{
    int rc = 0;
    int yval = 0;
    int ival = 0;
    float fval = 0;
    float fmul = 100.0;

    if(linenum > -1)
        lines[linenum] = true;

    // y = list[0];
    rc = stringToValue(str, &ival, &fval);
    if(rc == GraphWidget::TypeNone) {
        qDebug() << "stringToValue conversion error" << lineptr;
    }
    else if(rc == GraphWidget::TypeFloat) {
        // scale xfmul for floats
        yval = (int)((fval*fmul));
    }
    else if(rc == GraphWidget::TypeBin) {  // base 2
        yval = (int)ival*50.0;
    }
    else if(rc == GraphWidget::TypeDec) { // decimal
        yval = (int)ival;
    }
    else if(rc == GraphWidget::TypeHex) { // hex
    }

    if(rc != 0) {
        if(lineptr->count() < maxlinex) {
            lineptr->push_back(yval);
        }
        else {
            lineptr->pop_front();
            lineptr->push_back(yval);
        }
    }
    linetype[linenum] = rc;
    return rc;
}

/**
 * @brief GraphLine::stringToValue
 * @param vs Input QString.
 * @param ival [out] integer value.
 * @param fval [out] float value.
 * @return Type of number: error 0, float 1, int 10, binary int 2, hex int 16
 */
int GraphLine::stringToValue(QString vs, int *ival, float *fval)
{
    int rc = 0;
    bool ok = false;

    if(vs.contains(".") && !vs.contains("e")) {
        *fval = vs.toFloat(&ok);
        if(ok) rc = 1;
    }
    else if(vs.indexOf("0b")==0 || vs.indexOf("%")==0) {
        *ival = vs.toInt(&ok, 2);
        if(ok) rc = 2;
    }
    else if(vs.indexOf("0x")==0 || vs.indexOf("$")==0) {
        *ival = vs.toInt(&ok, 16);
        if(ok) rc = 16;
    }
    else {
        *ival = vs.toInt(&ok, 10);
        if(ok) rc = 10;
    }
    return rc;
}

QToolBar *GraphLine::getTools()
{
    return tools;
}

void GraphLine::animate()
{
    elapsed = (elapsed + qobject_cast<QTimer*>(sender())->interval());

    if(updateGraph) {
        repaint();
        updateGraph = false;
    }
}

void GraphLine::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    paint(&painter, event, elapsed);
    painter.end();
}

void GraphLine::paint(QPainter *painter, QPaintEvent *event, int)
{
    int n = 0;
    int x = 0;
    int y = 0;
    int xo = 0;
    int yo = 0;

    int wd = this->width();
    int ht = this->height();

    int margin = 50;
    int ytop = margin;
    int ybot = ht-margin;
    int xbeg = margin;
    int xend = wd-margin;
    int xdiv = (xend-xbeg)/10;
    int ydiv = (ybot-ytop)/10;

    int linecount = 0;

    painter->fillRect(event->rect(), background);

    painter->save();

    painter->setBrush(lineBrush);
    painter->setPen(linePen);
    painter->rotate(0);

    time = QTime::currentTime();

    maxlinex = xend-margin; // window width-2*margin

    painter->drawLine(xbeg, ytop, xbeg, ybot); // y axis
    painter->drawLine(xbeg, ybot, xend, ybot); // x-axis

    painter->setPen(textPen);
    painter->setFont(textFont);

    linecount  = (lines[0] && enLine0->isChecked() && lineq0.count() > 0) ? 1 : 0;
    linecount += (lines[1] && enLine1->isChecked() && lineq1.count() > 0) ? 1 : 0;
    linecount += (lines[2] && enLine2->isChecked() && lineq2.count() > 0) ? 1 : 0;
    linecount += (lines[3] && enLine3->isChecked() && lineq3.count() > 0) ? 1 : 0;

    /* for now Y axis is just %
    */
    n = 0;
    for(y = ybot; y > ytop; y -= ydiv, n += 10) {
        QRect r(10, y-10, 40, 30);
        if(n > 0) painter->drawText(r, Qt::AlignCenter|Qt::AlignJustify, QString("%1").arg(n));
    }

    if(linecount == 0) {
        n = 0;
        for(x = xbeg; x < xend; x += xdiv, n += 10) {
            QRect r(x-30, ybot, 50, 30);
            painter->drawText(r, Qt::AlignBottom|Qt::AlignHCenter, QString("%1").arg(n));
        }
    }

    if(linecount == 1) {
        yo = lineq0[0];
        xo = 0;
        if(!(lines[1] && lines[2] && lines[3])) {
            n = 0;
            for(x = 1; x < lineq0.count() && x < maxlinex; x++, n++) {
                y = lineq0[x];
                if(validq[x] != 0) {
                    painter->drawLine(xbeg+xo,ybot-yo,xbeg+x,ybot-y);
                    yo = y;
                    xo = x;
                }
            }
            n = 0;
            for(x = xbeg; x < xend; x += xdiv, n += 10) {
                QRect r(x-30, ybot, 50, 30);
                painter->drawText(r, Qt::AlignBottom|Qt::AlignHCenter, QString("%1").arg(n));
            }
        }
        else {
            n = 0;
            for(x = xbeg; x < lineq0.count() && x < maxlinex; x += xdiv, n += 10) {
                QRect r(x-30, ybot+1, 50, 30);
                painter->fillRect(r, background);
                painter->drawText(r, Qt::AlignBottom|Qt::AlignHCenter, QString("%1").arg(lineq0[n]));
            }
        }
    }

    if(linecount == 2) {
        int len = 0;
        int xscale = 2;
        yo = lineq1[0];
        xo = lineq0[0];
        n = 0;
        for(x = 1; x < lineq1.count() && x < maxlinex; x++, n++) {
            y = lineq1[x];
            if(validq[x] != 0) {
                painter->drawLine(xbeg+xo*xscale,ybot-yo,xbeg+lineq0[x]*xscale,ybot-y);
                yo = y;
                xo = lineq0[x];
                /*
                if(!(x % (maxlinex/10))) {
                    QRect r(xo*xscale+30, ybot+1, 30, 30);
                    painter->fillRect(r, background);
                    painter->drawText(r, Qt::AlignBottom|Qt::AlignHCenter, QString("%1").arg(xo));
                }
                */
            }
        }
        len = lineq0.count();
        for(x = 0; x < len; x+=len/10+1) {
            QRect r(x*xscale+30, ybot+1, 30, 30);
            painter->fillRect(r, background);
            painter->drawText(r, Qt::AlignBottom|Qt::AlignHCenter, QString("%1").arg(lineq0[x]));
        }
        QRect r(xo*xscale+2*30, ybot+1, 30, 30);
        painter->fillRect(r, background);
        painter->drawText(r, Qt::AlignBottom|Qt::AlignHCenter, QString("%1").arg(xo));

        /*
        yo = lineq0[0];
        xo = 0;
        n  = 0;
        for(x = xbeg; x < lineq0.count() && x < maxlinex; x += xdiv, n += 1) {
            if(!(lineq0[n] % 10)) {
            QRect r(lineq0[x]-30, ybot+1, 50, 30);
            painter->fillRect(r, background);
            painter->drawText(r, Qt::AlignBottom|Qt::AlignHCenter, QString("%1").arg(lineq0[n]));
        }
        QRect r(x-30, ybot, 50, 30);
        painter->drawText(r, Qt::AlignBottom|Qt::AlignHCenter, QString("%1").arg(n));
        */
    }

    // if(validq.count() > maxlinex-1) resetGraph();

    painter->restore();
}
