#ifndef TERMINAL2_H
#define TERMINAL2_H

class Terminal;

#include <QDialog>
#include <QWidget>
#include <QString>

#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QTabWidget>

#include "console.h"
#include "PortListener.h"
#include "termprefs.h"

class Terminal : public QDialog
{
    Q_OBJECT
public:
    explicit Terminal(QWidget *parent);
    void setPortListener(PortListener *listener);
    QString getPortName();
    void setPortName(QString name);
    void setPosition(int x, int y);
    void accept();
    void reject();

    BaudRateType getBaud();
    int  getBaudRate();
    bool setBaudRate(int baud);
    void setEchoOn(bool echoOn);
    void setPortLabelEnable(bool enable);

signals:
    void enablePortCombo();
    void disablePortCombo();

private:
    void init();

public slots:
    void baudRateChange(int index);
    void echoOnChange(bool value);
    void toggleEnable();
    void setPortEnabled(bool value);
    void clearScreen();
    void copyFromFile();
    void cutFromFile();
    void pasteToFile();
    void showOptions();

public:
    Console *getEditor();
private:
    Console     *termEditor;
    TermPrefs   *options;
    QComboBox   *comboBoxBaud;
    QCheckBox   *cbEchoOn;
    QLabel      portLabel;

private:
    QPushButton     *buttonEnable;
    PortListener    *portListener;
};

#endif // TERMINAL2_H
