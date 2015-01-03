#pragma once

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QString>
#include <QMutex>
#include <QByteArray>
#include <QTimerEvent>

#include "Console.h"
#include "StatusDialog.h"
#include "qext/qextserialport.h"

class PortListener : public QThread
{
Q_OBJECT
public:
    using QObject::timerEvent;
    PortListener(QObject *parent, Console *term);
    void init(const QString & portName, BaudRateType baud);
    void setDtr(bool enable);
    bool open();
    void close();
    bool isOpen();

    void run();
    void start();
    void stop();

    void setLoadEnable(bool enable);
    void setTerminalWindow(Console *editor);

    QString         getPortName();
    BaudRateType    getBaudRate();
    QextSerialPort  *getPort();
    int             getFileHandle();

private:
    QMutex          rxqMutex;
    Console         *textEditor;
    QextSerialPort  *port;
    QTimer          *timer;

public slots:
    void onReadyRead();
    void onReadyRead(QByteArray arr);
    void send(QByteArray bytes);
    void appendConsole(QString text);
    void timerEvent();

signals:
    void readyRead();
    void readyRead(QByteArray arr);


public:
/*
    enum DOWNLOAD {
        DOWNLOAD_RUN_BINARY      =  1,
        DOWNLOAD_EEPROM          =  2,
        DOWNLOAD_RUN_EEPROM      =  3,
        DOWNLOAD_SHUTDOWN        =  4
    };
*/
    /**
     * find a propeller on port
     * @param hSerial - file handle to serial port
     * @param sparm - pointer to DCB serial control struct
     * @param port - pointer to com port name
     * @returns 1 if found, 0 if not found, or -1 if port not opened.
     */
    int findprop(const char* port);

    /**
     * @brief preset uses findProp to reset board
     * @return non-zero on error
     */
    int preset(void);

    void setStatusDialog(StatusDialog *dialog);

    int  isDevice(QString port);

    enum { RESET_BY_DTR = 1 };
    enum { RESET_BY_RTS = 2 };

    void setDtrReset() {
        resetType = RESET_BY_DTR;
    }
    void setRtsReset() {
        resetType = RESET_BY_RTS;
    }

private:

    int resetType;

    enum { RXSIZE = (1<<10)-1 };

    QByteArray rxqueue;
    bool running;
    StatusDialog *statusDialog;

    /**
     * receive a buffer with a timeout
     * @param buff - char pointer to buffer
     * @param n - number of bytes in buffer to read
     * @param timeout - timeout in milliseconds
     * @returns number of bytes read or SERIAL_TIMEOUT
     */
    int rx_timeout(char* buff, int n, int timeout);

    /**
     * transmit a buffer
     * @param buff - char pointer to buffer
     * @param n - number of bytes in buffer to send
     * @returns zero on failure
     */
    int tx(char* buff, int n);

    /**
     * hwreset ... resets Propeller hardware using DTR or RTS
     * @param sparm - pointer to DCB serial control struct
     * @returns void
     */
    void hwreset(void);

    QString shortFileName(QString fileName);

private:

    char LFSR; // 'P'

    int pload_verbose;
    int pload_delay;
    bool findReceiveEnabled;
    bool loadReceiveEnabled;

    enum { SHUTDOWN_CMD             =  0 };
    enum { SERIAL_TIMEOUT           = -1 };
    enum { PLOAD_RESET_DEVICE       =  0 };
    enum { PLOAD_NORESET            =  1 };
    enum { PLOAD_STATUS_OK          =  0 };
    enum { PLOAD_STATUS_OPEN_FAILED = -1 };
    enum { PLOAD_STATUS_NO_PROPELLER= -2 };

    /**
     * update the LFSR byte
     */
    int iterate(void);

    /**
     * getBit ... get bit from serial stream
     * @param hSerial - file handle to serial port
     * @param status - pointer to transaction status 0 on error
     * @returns bit state 1 or 0
     */
    int getBit(int* status, int timeout);

    /**
     * hwfind ... find propeller using sync-up sequence.
     * @param hSerial - file handle to serial port
     * @returns zero on failure
     */
    int hwfind(int retry);

    /**
     * read all data until empty.
     */
    void flushPort();

};
