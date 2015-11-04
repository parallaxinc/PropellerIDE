#pragma once

#include <QWidget>
#include <QTimer>

#include "propellersession.h"

#include "ui_propterm.h"

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

    void message(QString text);
    void error(QString text);

    int timeout;
    QTimer rxTimeout;
    QTimer txTimeout;

public:
    explicit PropTerm(PropellerManager * manager, QWidget *parent = 0);
    ~PropTerm();

public slots:
    void setFont(const QFont & font);

private slots:
    void open();
    void close();
    void writeData(const QByteArray &data);
    void readData();

    void handleError();
    void handleEnable(bool checked);
    void portChanged();
    void baudRateChanged(const QString & text);

    void sendDataLine();

    void toggleTxLight(bool enabled);
    void toggleRxLight(bool enabled);

    void turnOffTxLight();
    void turnOffRxLight();

    void updatePorts();

};
