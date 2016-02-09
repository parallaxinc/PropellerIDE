#include "propterm.h"
#include "console.h"

#include <QDebug>
#include <QIntValidator>

Q_LOGGING_CATEGORY(terminal, "ide.terminal")

PropTerm::PropTerm(PropellerManager * manager,
                   const QString & portname,
                   QWidget *parent)
: QWidget(parent)
{
    ui.setupUi(this);

    this->manager = manager;
    session = new PropellerSession(manager, portname);

    ui.baudRate->setValidator(new QIntValidator(0, 1000000, this));

    timeout = 100;

    rxTimeout.setSingleShot(true);
    txTimeout.setSingleShot(true);

    toggleRxLight(false);
    toggleTxLight(false);

    updatePorts();
    ui.console->clear();

    connect(&rxTimeout,     SIGNAL(timeout()),                              this,       SLOT(turnOffRxLight()));
    connect(&txTimeout,     SIGNAL(timeout()),                              this,       SLOT(turnOffTxLight()));
    connect(&busyTimeout,   SIGNAL(timeout()),                              this,       SLOT(handleBusy()));

    connect(manager,        SIGNAL(portListChanged()),                      this,       SLOT(updatePorts()));
    connect(session,        SIGNAL(sendError(const QString &)),             this,       SLOT(handleError()));
    connect(session,        SIGNAL(baudRateChanged(qint32)),                this,       SLOT(setWidgetBaudRate(qint32)));

    connect(session,        SIGNAL(deviceStateChanged(bool)),               this,       SLOT(handleDeviceStateChanged(bool)));
    connect(session,        SIGNAL(deviceAvailableChanged(bool)),           this,       SLOT(handleAvailable(bool)));

    connect(ui.console,     SIGNAL(getData(QByteArray)),                    this,       SLOT(writeData(QByteArray)));
    connect(ui.echo,        SIGNAL(toggled(bool)),                          ui.console, SLOT(setEcho(bool)));

    connect(ui.sendLineEdit,SIGNAL(returnPressed()),                        this,       SLOT(sendDataLine()));
    connect(ui.sendButton,  SIGNAL(pressed()),                              this,       SLOT(sendDataLine()));

    connect(ui.port,        SIGNAL(currentIndexChanged(const QString &)),   this,       SLOT(portChanged()));
    connect(ui.baudRate,    SIGNAL(currentIndexChanged(const QString &)),   this,       SLOT(setDeviceBaudRate(const QString &)));

    connect(ui.clear,       SIGNAL(clicked()),                              ui.console, SLOT(clear()));
    connect(ui.reset,       SIGNAL(clicked()),                              this,       SLOT(reset()));
    connect(ui.activeButton,SIGNAL(toggled(bool)),                          this,       SLOT(handleDevicePower(bool)));

    title = tr("Terminal");

    open();
}

PropTerm::~PropTerm()
{
    close();
    delete session;
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


void PropTerm::setEnabled(bool enabled)
{
    ui.console->enable(enabled);
    ui.console->setEnabled(enabled);
    ui.sendButton->setEnabled(enabled);
    ui.sendLineEdit->setEnabled(enabled);
    ui.baudRate->setEnabled(enabled);
    ui.reset->setEnabled(enabled);
    ui.clear->setEnabled(enabled);

    disconnect(ui.activeButton,SIGNAL(toggled(bool)),  this, SLOT(handleDevicePower(bool)));
    ui.activeButton->setChecked(enabled);
    connect(ui.activeButton,SIGNAL(toggled(bool)),  this, SLOT(handleDevicePower(bool)));

    session->setPaused(!enabled);
}

void PropTerm::open()
{
    setEnabled(true);
    ui.activeLight->setPixmap(QPixmap(":/icons/propterm/led-green.png"));

    portChanged();
    setDeviceBaudRate(ui.baudRate->currentText());

    connect(session, SIGNAL(readyRead()), this, SLOT(readData()));
}

void PropTerm::closed()
{
    setEnabled(false);
    ui.activeLight->setPixmap(QPixmap(":/icons/propterm/led-off.png"));

    disconnect(session, SIGNAL(readyRead()), this, SLOT(readData()));
}

void PropTerm::handleAvailable(bool available)
{
    if (available)
        free();
    else
        busy();
}

void PropTerm::busy()
{
    closed();
    ui.activeLight->setPixmap(QPixmap(":/icons/propterm/led-yellow.png"));
    busyTimeout.start(75);
}

void PropTerm::free()
{
    busyTimeout.stop();
    open();
}

void PropTerm::reset()
{
    toggleTxLight(true);
    session->reset();
}

void PropTerm::portChanged()
{
    if (ui.port->currentText() != session->portName())
    {
        qCDebug(terminal) << "switch to port" << ui.port->currentText();
        session->setPortName(ui.port->currentText());
        setWidgetBaudRate(session->baudRate());
        if (session->portName().isEmpty())
        {
            setWindowTitle(tr("%1").arg(title));
        }
        else
        {
            setWindowTitle(tr("%1 - %2").arg(session->portName()).arg(title));
        }
    }
}

void PropTerm::setDeviceBaudRate(const QString & text)
{
    bool ok;
    qint32 baud = text.toInt(&ok);
    if (!ok)
    {
        qCCritical(terminal) << "Baud rate invalid:" << baud;
        return;
    }

    session->setBaudRate(baud);
}

void PropTerm::setWidgetBaudRate(qint32 baudrate)
{
    ui.baudRate->setCurrentIndex(ui.baudRate->findText(QString::number(baudrate)));
}

void PropTerm::sendDataLine()
{
    QByteArray data = ui.sendLineEdit->text().toLocal8Bit();
    data.append(13);
    writeData(data);
    ui.sendLineEdit->clear();
}

void PropTerm::writeData(const QByteArray & data)
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
//    qCDebug(terminal) << "THERE WAS AN ERROR";
    closed();
}

void PropTerm::handleDevicePower(bool enabled)
{
    if (enabled)
        manager->openPort(session->portName());
    else
        manager->closePort(session->portName());
}

void PropTerm::handleDeviceStateChanged(bool enabled)
{
    if (enabled)
        open();
    else
        closed();
}

void PropTerm::handleBusy()
{
    if (busytoggle)
    {
        ui.activeLight->setPixmap(QPixmap(":/icons/propterm/led-yellow.png"));
        busytoggle = false;
    }
    else
    {
        ui.activeLight->setPixmap(QPixmap(":/icons/propterm/led-off.png"));
        busytoggle = true;
    }
}

