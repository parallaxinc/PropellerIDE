#include "PortConnectionMonitor.h"

#include <QApplication>
#include <QList>
#include <QSerialPortInfo>

PortConnectionMonitor::PortConnectionMonitor(QObject *parent) :
    QThread(parent)
{
    start();
}

void PortConnectionMonitor::start()
{
    running = true;
    QThread::start();
}

void PortConnectionMonitor::stop()
{
    running = false;
    msleep(200); // let run finish. don't terminate it.
}

QStringList PortConnectionMonitor::enumeratePorts()
{
    QStringList myPortList;
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    foreach(QSerialPortInfo port, ports)
    {
        myPortList.append(port.portName());
    }
    return myPortList;
}

void PortConnectionMonitor::run()
{
    QStringList ports;
    QStringList newports = enumeratePorts();

    while(running)
    {
        newports = enumeratePorts();

        if(ports != newports)
        {
            emit portChanged();
            ports = newports;
        }
        msleep(200);
    }
}
