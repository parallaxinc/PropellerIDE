#include "StatusDialog.h"

StatusDialog::StatusDialog(QWidget *parent) : QDialog(parent, 0)
{
    QVBoxLayout *vlayout = new QVBoxLayout();
    QHBoxLayout *hlayout = new QHBoxLayout();

    for(int j = 0; j < 5; j++) {
        QLabel *r = new QLabel();
        r->setPixmap(QPixmap(":/icons/status.png"));
        //r->setBackgroundRole(QPalette::Base);
        r->setEnabled(false);
        bump.append(r);
        hlayout->addWidget(r);
    }

    messageLabel = new QLabel(this);
    QFont mfont = this->font();
    mfont.setPixelSize(14);
    messageLabel->setFont(mfont);
    vlayout->addWidget(messageLabel);
    vlayout->addLayout(hlayout);

    //connect(&thread, SIGNAL(nextBump()), this, SLOT(nextBump()));
    //connect(&thread, SIGNAL(hideit()), this, SLOT(hide()));

    displayTimer = new QTimer(this);
    connect(displayTimer, SIGNAL(timeout()), this, SLOT(animate()));

    this->setWindowTitle("Status Message Dialog");
    this->setStyleSheet("QDialog { background-color: white; }");
    this->setLayout(vlayout);
}

StatusDialog::~StatusDialog()
{
}

void StatusDialog::init(const QString title, const QString message)
{
    QApplication::processEvents();
    this->setWindowTitle(title);

    QWidget *win = this->parentWidget();
    int w = 200;
    int h = 60;
    QRect   rect(win->x()+win->width()/2-w/2, win->y()+win->height()/2-h/2, w, h);

    this->setGeometry(rect);
    this->setMaximumWidth(w);

    messageLabel->setText(message);

    this->setWindowFlags(Qt::Tool);
    this->show();

    QApplication::processEvents();

    index = -1;
    //thread.startit();
    displayTimer->start(200);

    this->show();
    QApplication::processEvents();
}

void StatusDialog::addMessage(const QString message)
{
    messageLabel->setText(messageLabel->text() + message);
}

void StatusDialog::setMessage(const QString message)
{
    messageLabel->setText(message);
}

QString StatusDialog::getMessage()
{
    return messageLabel->text();
}

void StatusDialog::stop()
{
    displayTimer->stop();
    hide();
}

void StatusDialog::nextBump()
{
    for(int n = 0; n < bump.count(); n++) {
        bump[n]->setEnabled(false);
    }
    index = (index+1) % bump.count();
    bump[index]->setEnabled(true);
}

void StatusDialog::animate()
{
    nextBump();
    QApplication::processEvents();
}

StatusDialogThread::StatusDialogThread(QObject *parent) : QThread(parent)
{
    nextDelay = 300;
}

void StatusDialogThread::startit()
{
    running = -1;
    start();
}

void StatusDialogThread::stop(int count)
{
    running = count;
}

void StatusDialogThread::run()
{
    while(--running > -1) {
        emit nextBump();
        QApplication::processEvents();
        msleep(nextDelay);
    }
    emit hideit();
}

bool StatusDialog::isRunning()
{
    return thread.isRunning();
}

