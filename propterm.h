#pragma once

#include <QWidget>

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

public:
    explicit PropTerm(QWidget *parent = 0);
    ~PropTerm();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void writeData(const QByteArray &data);
    void readData();

    void handleError();
    void handleToggle(bool checked);
    void portChanged(const QString & text);
    void baudRateChanged(const QString & text);

private slots:
    void message(QString text);
    void error(QString text);

};
