#include "propterm.h"
#include "console.h"

#include <QDebug>

PropTerm::PropTerm(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);

    connect(&device, SIGNAL(sendError()),
            this, SLOT(handleError()));

    connect(&device, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(ui.console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));

    connect(ui.port, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(portChanged(const QString &)));
    connect(ui.baudRate, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(baudRateChanged(const QString &)));

    connect(ui.clear, SIGNAL(clicked()), ui.console, SLOT(clear()));
    connect(ui.enable, SIGNAL(toggled(bool)), this, SLOT(handleToggle(bool)));
    connect(ui.enable, SIGNAL(toggled(bool)), ui.console, SLOT(enable(bool)));

    connect(ui.echo, SIGNAL(clicked(bool)), ui.console, SLOT(setEchoEnabled(bool)));
    ui.echo->setChecked(true);

    foreach (QString s, PropellerDevice::list())
    {
        qDebug() << s;
        ui.port->addItem(s);
    }

    ui.console->clear();
}

PropTerm::~PropTerm()
{
    closeSerialPort();
}

void PropTerm::message(QString text)
{
    qDebug() << "[PropellerManager]" << qPrintable(device.portName())
             << ":" << qPrintable(text);
}

void PropTerm::error(QString text)
{
    message("ERROR: "+text);
}


void PropTerm::openSerialPort()
{
    qDebug() << "Opening port" << ui.port->currentText();
    ui.console->setEnabled(true);

    device.setPortName(ui.port->currentText());
    device.setBaudRate(115200);
    if (!device.open())
        qDebug() << "ERROR: Failed to open device";

    device.reset();
//    device.setBaudRate(ui.baudRate->currentText().toInt());

    qDebug() << "Port open" << device.baudRate() << device.portName();
}

void PropTerm::closeSerialPort()
{
    device.close();
    ui.console->setEnabled(false);
}

void PropTerm::portChanged(const QString & text)
{
    qDebug() << "Port changed" << text;
    if (ui.enable->isChecked())
    {
        qDebug() << "Was enabled!";
        closeSerialPort();
        openSerialPort();
    }
}

void PropTerm::baudRateChanged(const QString & text)
{
    int baud = text.toInt();
    qDebug() << "Baud rate changed" << baud;
    device.setBaudRate(115200);
}

void PropTerm::writeData(const QByteArray &data)
{
    device.write(data);
}

void PropTerm::readData()
{
    QByteArray data = device.readAll();
    ui.console->putData(data);
}

void PropTerm::handleError()
{
    closeSerialPort();
}

void PropTerm::handleToggle(bool checked)
{
    if (checked)
        openSerialPort();
    else
        closeSerialPort();
}
