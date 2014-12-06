#pragma once

class TermPrefs;

#include <QtGui>
#include "Terminal.h"
#include "Console.h"
#include "Preferences.h"
#include "colors.h"


namespace Ui {
    class TermPrefs;
}

class TermPrefs : public QDialog
{
    Q_OBJECT
public:
    TermPrefs(Terminal *term);
    virtual ~TermPrefs();
    void addColors(QComboBox *box, QVector<PColor*> pcolor);
    void saveSettings();
    void readSettings();

    int  getBaudRate();
    void saveBaudRate(int baud);
    bool getEchoOn();
    void saveEchoOn(bool echoOn);
    int  getMaxRows();

    void showDialog();

public slots:
    void hexDump(bool hex);
    void chooseFont();
    void resetSettings();
    void accept();
    void reject();

private:
    Ui::TermPrefs  *ui;
    Terminal       *terminal;
    Console        *serialConsole;
    QSettings      *settings;
    QStringList     settingNames;

    QVector<PColor*> propertyColors;

    QString         styleText;
};
