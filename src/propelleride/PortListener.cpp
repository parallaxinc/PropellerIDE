
#include "PortListener.h"
#include <QtDebug>

#undef  PORTDEBUG
//#define PORTDEBUG

#define RESPWAIT    (5)
#define TWAIT(ms)   ((ms)/RESPWAIT)
#define JUNKWAIT    (TWAIT(10))  // want about 10ms
#define GETBITWAIT  (TWAIT(110)) // want about 110 ms (90 to 300+ at RESPWAIT=2)

#define USE_PORT_TIMER

PortListener::PortListener(QObject *parent, Console *term) : QThread(parent)
{
    findReceiveEnabled = false;
    loadReceiveEnabled = false;
    pload_verbose = 0;
    pload_delay = 0;
    resetType = RESET_BY_DTR;
    textEditor = term;
    connect(term, SIGNAL(sendSerial(QByteArray)), this, SLOT(send(QByteArray)));

    timer = new QTimer(this);
    this->port = new QSerialPort();
    connect(this, SIGNAL(readyRead(QByteArray)), this, SLOT(onReadyRead(QByteArray)));
    port->setPortName(""); // empty for now
}

void PortListener::init(const QString & portName, QSerialPort::BaudRate baud)
{
    port->setPortName(portName);
    port->setBaudRate(baud);
}

QString PortListener::getPortName()
{
    return QSerialPortInfo(port->portName()).systemLocation();
}

int PortListener::getBaudRate()
{
    return port->baudRate();
}

QSerialPort *PortListener::getPort()
{
    return port;
}

void PortListener::setDtr(bool enable)
{
    this->port->setDataTerminalReady(enable);
}

bool PortListener::open()
{
    if(!textEditor) // no text editor, no open
        return false;

    if(port == NULL)
        return false;

    bool rc = port->open(QIODevice::ReadWrite);
    if(rc) {
        start();
    }
    return rc;
}

void PortListener::close()
{
    if(port == NULL)
        return;

    running = false;
    flushPort();

    if(port->isOpen())
        port->close();
}

bool PortListener::isOpen()
{
    return port->isOpen();
}

void PortListener::send(QByteArray data)
{
    port->write(data.constData(),1);
}

void PortListener::setLoadEnable(bool enable)
{
    loadReceiveEnabled = enable;
}

void PortListener::setTerminalWindow(Console *editor)
{
    textEditor = editor;
}

void PortListener::appendConsole(QString text)
{
    if(!textEditor)
        return;
    QByteArray arr(text.toLocal8Bit());
    int size = text.length();
    if(textEditor->getHexMode()) {
        for(int n = 0; n < size; n++)
            textEditor->dumphex((int)arr[n]);
        return;
    }
    else {
        for(int n = 0; n < size; n++)
            textEditor->update((char)arr[n]);
        arr.clear();
    }
    QApplication::processEvents();
}

/*
 * Ok, this works for polling for most cases but fails a stress test
 * with gobs of data coming in. We really need event driven, and
 * now that Qt has QSerialPort, we can test with that. Maybe not today :)
 */

void PortListener::onReadyRead()
{
    QByteArray bytes = port->readAll();
    onReadyRead(bytes);
}

void PortListener::onReadyRead(QByteArray arr)
{
    int size = 0;

    /* Sometimes the runner thread gets killed,
     * but somehow onReadyRead is still queued.
     * If not running, return to not hog CPU.
     */
    if(!running) {
        return;
    }
    size = arr.count(); // buffer size

    if(findReceiveEnabled) {
        char *bytes = arr.data();
        for(int n = 0; n < size; n++) {
            rxqMutex.lock();
            rxqueue.append(bytes[n]);
            rxqMutex.unlock();
        }
        return;
    }

    if(!textEditor->enabled()) {
        return;
    }

    else {
        if(textEditor->getHexMode()) {
            for(int n = 0; n < size; n++) {
                textEditor->dumphex((int)arr[n]);
            }
        }
        else {
            for(int n = 0; n < size; n++) {
                textEditor->update((char)arr[n]);
            }
            arr.clear();
        }
        textEditor->repaint(); // yay, all done! Keep this so screen updates.
    }
}

void PortListener::run()
{
    while(running) {
        if(port->bytesAvailable() > 0) {
            emit readyRead(port->readAll());
        }
        QApplication::processEvents();
        msleep(30);
    }
}

void PortListener::timerEvent()
{
    if(port->bytesAvailable() > 0) {
        emit readyRead(port->readAll());
    }
}

void PortListener::start()
{
    running = true;
#ifdef USE_PORT_TIMER
    timer->start(20);
#else
    QThread::start();
#endif
}

void PortListener::stop()
{
    running = false;
#ifdef USE_PORT_TIMER
    timer->stop();
#endif
}

void PortListener::setStatusDialog(StatusDialog *dialog)
{
    statusDialog = dialog;
}

int PortListener::isDevice(QString portName)
{
    if(!statusDialog) {
        qDebug() << "Silly programmer. StatusDialog not initialized";
        return 0;
    }
    if(statusDialog->getMessage().contains("Propeller")) {
        statusDialog->addMessage("\n");
    }
    QString s("Propeller ");
    int rc = findprop(portName.toLatin1());
    if(!rc)  s += "not ";
    s += "found on "+portName;
    statusDialog->addMessage(s);
    return rc;

}

/**
 * receive a buffer with a timeout
 * @param buff - char pointer to buffer
 * @param n - number of bytes in buffer to read
 * @param timeout - timeout in milliseconds
 * @returns number of bytes read or SERIAL_TIMEOUT
 */
int PortListener::rx_timeout(char* buff, int n, int timeout)
{
    int size = 0;
    while(rxqueue.count() < 1) {
        msleep(RESPWAIT); // Give cpu more time to capture bytes.
        QApplication::processEvents();
        if(timeout-- < 0)
            break;
    }
    while(timeout > 0 && rxqueue.count() > 0) {
        if(size >= n)
            break;
        rxqMutex.lock();
        buff[size] = rxqueue.at(0);
        rxqueue.remove(0,1);
        rxqMutex.unlock();
        size++;
    }
    return size == 0 ? SERIAL_TIMEOUT : size;
}

/**
 * transmit a buffer
 * @param buff - char pointer to buffer
 * @param n - number of bytes in buffer to send
 * @returns zero on failure
 */
int PortListener::tx(char* buff, int n)
{
    int size = port->write((const char*)buff, n);
    port->flush();
    if(size != n) {
        qDebug() << "PortListener:tx failed. size" << size << "requested" << n;
    }
#ifdef PORTDEBUG
    else {
        //qDebug() << "TX" << size;
    }
#endif
    return (size == n) ? 1 : 0;
}

/**
 * hwreset ... resets Propeller hardware using DTR or RTS
 * @param sparm - pointer to DCB serial control struct
 * @returns void
 */
void PortListener::hwreset(void)
{
    this->flushPort();
    this->port->flush();
    if(this->resetType == RESET_BY_DTR) {
        port->setDataTerminalReady(true);
        msleep(10);
        port->setDataTerminalReady(false);
    }
    else {
        port->setRequestToSend(true);
        msleep(10);
        port->setRequestToSend(false);
    }
    msleep(90);
    this->flushPort();
}

int PortListener::iterate(void)
{
    int bit = LFSR & 1;
    LFSR = (char)((LFSR << 1) | (((LFSR >> 7) ^ (LFSR >> 5) ^ (LFSR >> 4) ^ (LFSR >> 1)) & 1));
    return bit;
}

/**
 * getBit ... get bit from serial stream
 * @param hSerial - file handle to serial port
 * @param status - pointer to transaction status 0 on error
 * @returns bit state 1 or 0
 */
int PortListener::getBit(int* status, int timeout)
{
    char mybuf[2];
    int rc = rx_timeout(mybuf, 1, timeout);
    if(status)
        *status = rc <= 0 ? 0 : 1;
    return *mybuf & 1;
}


/**
 * hwfind ... find propeller using sync-up sequence.
 * @param hSerial - file handle to serial port
 * @returns zero on failure
 */
int PortListener::hwfind(int retry)
{
    int  n, ii, jj, rc, to;
    char mybuf[300];
    rc = 0;

    /* hwfind is recursive if we get a failure on the first try.
     * retry is set by caller and should never be more than one.
     */
    if(retry < 0)
        goto endHwFind;

    findReceiveEnabled = true;

    /* Do not pause after reset.
     * Propeller can give up if it does not see a response in 100ms of reset.
     */
    mybuf[0] = 0xF9;
    LFSR = 'P';  // P is for Propeller :)

    /* send the calibration pulse
     */
    if(tx(mybuf, 1) == 0)
        goto endHwFind;   // tx should never return 0, return error if it does.

    //if(rx_timeout(mybuf,10,10) > -1) { qDebug("Junk chars ..."); }

    /* Send the magic propeller LFSR byte stream.
     */
    for(n = 0; n < 250; n++)
        mybuf[n] = iterate() | 0xfe;
    if(tx(mybuf, 250) == 0)
        goto endHwFind;   // tx should never return 0, return error if it does.

    n = 0;
    while((jj = rx_timeout(mybuf,10,JUNKWAIT)) > -1) {
        n += jj;
    }
    if(n != 0) qDebug("Ignored %d bytes. \n", n);

    /* Send 258 0xF9 for LFSR and Version ID
     * These bytes clock the LSFR bits and ID from propeller back to us.
     */
    for(n = 0; n < 258; n++)
        mybuf[n] = 0xF9;
    if(tx(mybuf, 258) == 0)
        goto endHwFind;   // tx should never return 0, return error if it does.

    /*
     * Wait at least 100ms for the first response. Allow some margin.
     * Some chips may respond < 50ms, but there's no guarantee all will.
     * If we don't get it, we can assume the propeller is not there.
     */
    ii = getBit(&rc, GETBITWAIT);
    if(rc == 0) {
        if (pload_verbose)
            qDebug("Timeout waiting for first response bit. Propeller not found.");
        goto endHwFind;
    }

    // wait for response so we know we have a Propeller
    for(n = 1; n < 250; n++) {

        jj = iterate();
        //if (pload_verbose) { printf("%d:%d %3d ", ii, jj, n); fflush(stdout); }

        if(ii != jj) {
            /* if we get this far, we probably have a propeller chip
             * but the serial port is in a funny state. just retry.
             */
            qDebug("Lost HW contact. %d %x ... retry.", n, *mybuf & 0xff);
            for(n = 0; (n < 300) && (rx_timeout(mybuf,1,10) > -1); n++);
            hwreset();
            findReceiveEnabled = false;
            return hwfind(--retry);
        }
        to = 0;
        do {
            ii = getBit(&rc, GETBITWAIT);
        } while(rc == 0 && to++ < 100);
        //printf(" %d\n", rc);
        if(to > 100) {
            qDebug("Timeout waiting for bit-stream response.");
            goto endHwFind;
        }
    }

    rc = 0;
    for(n = 0; n < 8; n++) {
        rc >>= 1;
        rc += getBit(0, GETBITWAIT) ? 0x80 : 0;
    }

    if (pload_verbose) qDebug("Propeller Version ... %d", rc);

endHwFind:
    findReceiveEnabled = false;
    return rc;
}

void PortListener::flushPort()
{
    int size = 0;
    do {
        //msleep(5);
        QApplication::processEvents();
        size = port->bytesAvailable();
        //if (pload_verbose) qDebug("Flushing port %d", size);
        port->readAll();
    } while(size > 0);
}

/**
 * find a propeller on port
 * @param hSerial - file handle to serial port
 * @param sparm - pointer to DCB serial control struct
 * @param port - pointer to com port name
 * @returns non-zero on error
 */
int PortListener::findprop(const char* name)
{
    int version = 0;

    if (pload_verbose)
        qDebug() << "Checking for Propeller on port " << name;

    if(!port)
        return -1;

    bool portstate;
    portstate = port->isOpen();

    if(portstate == false) {
        bool rc = open();
        if(!rc) {
            close();
            qDebug() << "Can't open port" << name;
            return 0;
        }
    }

    hwreset();
    version = hwfind(1); // retry once

    if(portstate == false) close();

    if (pload_verbose) {
        if(version) {
            qDebug() << "Propeller Version" << version << "on" << name;
        } else {
            qDebug() << "Propeller not found on" << name;
        }
    }

    return version != 0 ? 1 : 0;
}
