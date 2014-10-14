#ifndef STATUSDIALOG_H
#define STATUSDIALOG_H

#include "qtversion.h"

#include <QTimer>

class StatusDialogThread : public QThread
{
    Q_OBJECT
public:
    explicit StatusDialogThread(QObject *parent = 0);
    virtual ~StatusDialogThread() {}
    void run();
    void startit();
    void stop(int count = 0);
    bool isRunning() { return running > 0; }

signals:
    void nextBump();
    void hideit();

private:
    int running;
    int nextDelay;
};

class StatusDialog : public QDialog
{
    Q_OBJECT
public:
    StatusDialog(QWidget *parent = 0);
    virtual ~StatusDialog();

    void init(const QString title, const QString message);
    void addMessage(const QString message);
    void setMessage(const QString message);
    QString getMessage();

    void stop(int count = 0);
    bool isRunning();

public slots:
    void animate();
    void nextBump();

private:
    int index;
    StatusDialogThread thread;
    QLabel  *messageLabel;
    QList<QLabel*> bump;
    QTimer *displayTimer;
};

#endif // STATUSDIALOG_H
