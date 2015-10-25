#pragma once

#include <QWidget>
#include <QTimer>

#include "propellersession.h"
#include "propellerdevice.h"

#include "ui_propterm.h"

class Console;

class PropTerm : public QWidget
{
    Q_OBJECT

private:
    Ui::PropTerm ui;
    Console console;
    PropellerDevice device;

    void message(QString text);
    void error(QString text);

    int timeout;
    QTimer rxTimeout;
    QTimer txTimeout;

public:
    explicit PropTerm(QWidget *parent = 0);
    ~PropTerm();

    void setFont(QFont font);

private slots:
    void openSerialPort();
    void closeSerialPort();
    void writeData(const QByteArray &data);
    void readData();

    void handleError();
    void handleEnable(bool checked);
    void portChanged(const QString & text);
    void baudRateChanged(const QString & text);

    void sendDataLine();

    void toggleTxLight(bool enabled);
    void toggleRxLight(bool enabled);

    void turnOffTxLight();
    void turnOffRxLight();

};
