#pragma once

#include <QObject>
#include <QString>
#include <QLabel>
#include <QComboBox>
#include <QProcess>
#include <QMutex>
#include <QDebug>

class Builder : public QObject
{
    Q_OBJECT
public:
    explicit Builder(QObject *parent = 0);

    void setParameters(QString comp, QString incl, QString projFile, QString compstat);

    virtual int  runCompiler(QString copts);

signals:
    void compilerErrorInfo(QString file, int line);
    void terminalReceived(QString text);

public slots:
    virtual void compilerError(QProcess::ProcessError error);
    virtual void compilerFinished(int exitCode, QProcess::ExitStatus status);
    virtual void procReadyRead();

public:
    QString compilerStr;
    QString includesStr;

    QString projectFile;
    QString portName;

    QProcess * proc;

    int progMax;
    int progCount;

    QString         compileResult;

    QMutex      procMutex;
    bool        procDone;
};
