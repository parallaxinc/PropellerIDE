#pragma once

#include <QTimer>
#include <QStringList>

#include <QList>
#include <QSerialPortInfo>


class PortMonitor : public QObject
{
    Q_OBJECT

    QTimer timer;
    QStringList ports;

    QStringList enumeratePorts()
    {
        QStringList list;
        QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
        foreach(QSerialPortInfo port, ports)
        {
            list.append(port.portName());
        }
        return list;
    }

private slots:
    void checkPorts()
    {
        QStringList newports = enumeratePorts();

        newports = enumeratePorts();

        if(ports != newports)
        {
            emit portChanged();
            ports = newports;
        }
    }

public:
    explicit PortMonitor(QObject *parent = 0)
        : QObject(parent)
    {
        connect(&timer, SIGNAL(timeout()), this, SLOT(checkPorts()));
        timer.start(200);
    }

    ~PortMonitor()
    {
        timer.stop();
        disconnect(&timer, SIGNAL(timeout()), this, SLOT(checkPorts()));
    }

signals:
    void portChanged();

};

