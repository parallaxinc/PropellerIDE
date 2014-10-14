#ifndef EZBUILDER_H
#define EZBUILDER_H

#include "qtversion.h"

#include <QObject>
#include <QtCore>

class EzBuilder : public QObject
{
    Q_OBJECT
public:
    explicit EzBuilder(QObject *parent = 0);

    void setParameters(QString comp, QString incl, QString compPath, QString projFile, QString compstat);
    void setObjects(QLabel *stat, QLabel *progsize, QProgressBar *progbar, QComboBox *ports);

    QString filePathName(QString file);

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
    QString compilerPath;

    QString projectFile;
    QString portName;

    QComboBox *cbPort;
    QProcess  *proc;

    int progMax;
    int progCount;

    QString         compileResult;
    QLabel          *sizeLabel;
    QLabel          *msgLabel;
    QProgressBar    *progress;

    QMutex      procMutex;
    bool        procDone;
};

#endif // EZBUILDER_H
