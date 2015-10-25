#include "propterm.h"
#include "console.h"

#include <QDebug>


PropTerm::PropTerm(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);

    connect(&device, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(ui.console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));

    connect(ui.sendLineEdit, SIGNAL(returnPressed()), this, SLOT(sendDataLine()));
    connect(ui.sendButton, SIGNAL(pressed()), this, SLOT(sendDataLine()));

    connect(ui.port, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(portChanged(const QString &)));
    connect(ui.baudRate, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(baudRateChanged(const QString &)));

    connect(ui.clear, SIGNAL(clicked()), ui.console, SLOT(clear()));
    connect(ui.enable, SIGNAL(toggled(bool)), this, SLOT(handleEnable(bool)));
    connect(ui.enable, SIGNAL(toggled(bool)), ui.console, SLOT(enable(bool)));

    connect(ui.checkEcho, SIGNAL(clicked(bool)), ui.console, SLOT(setEchoEnabled(bool)));
    ui.checkEcho->setChecked(true);

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
    text = "[PropellerTerminal]: "+text;
    fprintf(stderr, "%s\n", qPrintable(text));
    fflush(stderr);
}

void PropTerm::error(QString text)
{
    message("ERROR: "+text);
}

void PropTerm::openSerialPort()
{
    message("Opening port "+ui.port->currentText());
    ui.console->setEnabled(true);

    device.setPortName(ui.port->currentText());
    device.setBaudRate(115200);
    if (!device.open())
    {
        error("ERROR: Failed to open device");
        return;
    }

    device.reset();
//    device.setBaudRate(ui.baudRate->currentText().toInt());

    message(QString("Port open: %1, %2").arg(device.baudRate()).arg(device.portName()));
}

void PropTerm::closeSerialPort()
{
    device.close();
    ui.console->setEnabled(false);
}

void PropTerm::portChanged(const QString & text)
{
    if (ui.enable->isChecked())
    {
        closeSerialPort();
        openSerialPort();
    }
}

void PropTerm::baudRateChanged(const QString & text)
{
    bool ok;
    int baud = text.toInt(&ok);
    if (!ok)
    {
        error(QString("Failed to set baud rate: '%1'").arg(baud));
        return;
    }

    device.setBaudRate(baud);
    message(QString("Baud rate changed to '%1'").arg(baud));
}

void PropTerm::sendDataLine()
{
    QByteArray data = ui.sendLineEdit->text().toLocal8Bit();
    data.append(13);
    writeData(data);
    ui.sendLineEdit->clear();
}

void PropTerm::writeData(const QByteArray &data)
{
    device.write(data);
    if (ui.checkEcho->isChecked())
        ui.console->putData(data);
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

void PropTerm::handleEnable(bool checked)
{
    if (checked)
        openSerialPort();
    else
        closeSerialPort();
}
