#ifndef TERMINAL_H
#define TERMINAL_H

#include "qtversion.h"

#include "console.h"
#include "PortListener.h"

class Terminal : public QDialog
{
    Q_OBJECT
public:
    explicit Terminal(QWidget *parent);
    Console *getEditor();
    void setPortListener(PortListener *listener);
    QString getPortName();
    void setPortName(QString name);
    void setPosition(int x, int y);
    void print(QByteArray str);
    void accept();
    void reject();

public slots:
    void clearScreen();
    void echoClicked();

#if 0
    void baudRateChange(int index);
    void echoOnChange(bool value);
    void toggleEnable();
    void setPortEnabled(bool value);
    void clearScreen();
    void showOptions();
#endif
    void copyFromFile();
    void cutFromFile();
    void pasteToFile();

private:
    PortListener *portListener;

    Console    *termEditor;
    QLabel      portLabel;
    QCheckBox   echoOnBox;
};

#endif // TERMINAL_H
