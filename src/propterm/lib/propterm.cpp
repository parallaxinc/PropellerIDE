#include "propterm.h"
#include "console.h"

#include <QDebug>


PropTerm::PropTerm(PropellerManager * manager,
                   QWidget *parent)
: QWidget(parent)
{
    ui.setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    this->manager = manager;
    this->session = manager->session();

    timeout = 100;

    rxTimeout.setSingleShot(true);
    txTimeout.setSingleShot(true);

    toggleRxLight(false);
    toggleTxLight(false);

    connect(&rxTimeout, SIGNAL(timeout()), this, SLOT(turnOffRxLight()));
    connect(&txTimeout, SIGNAL(timeout()), this, SLOT(turnOffTxLight()));

    connect(manager, SIGNAL(portListChanged()), this, SLOT(updatePorts()));
    connect(session, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(ui.console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));

    connect(ui.sendLineEdit, SIGNAL(returnPressed()), this, SLOT(sendDataLine()));
    connect(ui.sendButton, SIGNAL(pressed()), this, SLOT(sendDataLine()));

    connect(ui.port, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(portChanged()));
    connect(ui.baudRate, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(baudRateChanged(const QString &)));

    connect(ui.clear, SIGNAL(clicked()), ui.console, SLOT(clear()));
    connect(ui.activeButton, SIGNAL(toggled(bool)), this, SLOT(handleEnable(bool)));
    connect(ui.activeButton, SIGNAL(toggled(bool)), ui.console, SLOT(enable(bool)));

    ui.echo->setChecked(true);

    title = tr("Propeller Terminal");
    setWindowTitle(title);

    updatePorts();
    ui.console->clear();
    openSerialPort();
}

PropTerm::~PropTerm()
{
    closeSerialPort();
    manager->endSession(session);
}

void PropTerm::setFont(const QFont & font)
{
    ui.console->setFont(font);
    ui.sendLineEdit->setFont(font);
}

void PropTerm::updatePorts()
{
    ui.port->clear();
    foreach(QString s, manager->listPorts())
    {
        ui.port->addItem(s);
    }
}

void PropTerm::turnOffRxLight()
{
    toggleRxLight(false);
}

void PropTerm::turnOffTxLight()
{
    toggleTxLight(false);
}

void PropTerm::toggleRxLight(bool enabled)
{
    if (enabled)
    {
        rxTimeout.start(timeout);
        ui.rxLight->setPixmap(QPixmap(":/icons/propterm/led-red.png"));
    }
    else
    {
        rxTimeout.stop();
        ui.rxLight->setPixmap(QPixmap(":/icons/propterm/led-off.png"));
    }
}

void PropTerm::toggleTxLight(bool enabled)
{
    if (enabled)
    {
        txTimeout.start(timeout);
        ui.txLight->setPixmap(QPixmap(":/icons/propterm/led-blue.png"));
    }
    else
    {
        txTimeout.stop();
        ui.txLight->setPixmap(QPixmap(":/icons/propterm/led-off.png"));
    }
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
    message("Port: "+ui.port->currentText());
    ui.console->setEnabled(true);
    ui.sendButton->setEnabled(true);
    ui.sendLineEdit->setEnabled(true);
    ui.activeLight->setPixmap(QPixmap(":/icons/propterm/led-green.png"));
    session->setPortName(ui.port->currentText());
    setWindowTitle(tr("%1 - %2").arg(session->portName()).arg(title));

    session->reset();
    baudRateChanged(ui.baudRate->currentText());
}

void PropTerm::closeSerialPort()
{
    ui.console->setEnabled(false);
    ui.sendButton->setEnabled(false);
    ui.sendLineEdit->setEnabled(false);
    ui.activeLight->setPixmap(QPixmap(":/icons/propterm/led-off.png"));
}

void PropTerm::portChanged()
{
    session->setPortName(ui.port->currentText());
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

    session->setBaudRate(baud);
    message(QString("Baud rate: %1").arg(baud));
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
    toggleTxLight(true);

    session->write(data);
    if (ui.echo->isChecked())
        ui.console->putData(data);
}

void PropTerm::readData()
{
    toggleRxLight(true);

    QByteArray data = session->readAll();
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
