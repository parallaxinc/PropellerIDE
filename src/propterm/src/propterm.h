#pragma once

#include <QWidget>
#include <QTimer>
#include <QLoggingCategory>

#include <PropellerSession>
#include "ui_propterm.h"

Q_DECLARE_LOGGING_CATEGORY(terminal)

class Console;

class PropTerm : public QWidget
{
    Q_OBJECT

private:
    Ui::PropTerm ui;
    Console console;
    PropellerManager * manager;
    PropellerSession * session;

    QString title;

    int timeout;
    QTimer rxTimeout;
    QTimer txTimeout;
    QTimer busyTimeout;
    bool busytoggle;
    bool _open;

public:
    explicit PropTerm(PropellerManager * manager,
                      const QString & portname = QString(),
                      QWidget *parent = 0);
    ~PropTerm();
    bool isOpen();

public slots:
    void setFont(const QFont & font);
    void setEnabled(bool enabled);

private slots:
    void open();
    void closed();
    void handleAvailable(bool available);
    void free();
    void busy();
    void reset();
    void writeData(const QByteArray & data);
    void readData();

    void handleError();
    void handleDeviceStateChanged(bool enabled);
    void handleDevicePower(bool enabled);
    void handleBusy();

    void portChanged();
    void setDeviceBaudRate(const QString & text);
    void setWidgetBaudRate(qint32 baudrate);

    void sendDataLine();

    void toggleTxLight(bool enabled);
    void toggleRxLight(bool enabled);

    void turnOffTxLight();
    void turnOffRxLight();

    void updatePorts();

};
