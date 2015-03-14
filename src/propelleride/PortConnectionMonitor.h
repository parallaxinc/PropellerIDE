#pragma once

#include <QThread>
#include <QObject>
#include <QString>
#include <QStringList>

class PortConnectionMonitor : public QThread
{
    Q_OBJECT
public:
    explicit PortConnectionMonitor(QObject *parent = 0);
    void start();
    void stop();
    QStringList enumeratePorts();
    void run();

signals:
    void portChanged();

private:
    bool running;

};
