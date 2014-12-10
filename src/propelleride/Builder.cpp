#include "Builder.h"

Builder::Builder(QObject *parent) : QObject(parent)
{
}

void Builder::setParameters(QString comp, QString incl, QString compPath, QString projFile, QString compstat)
{
    compilerStr = comp;
    compilerPath = compPath;
    includesStr = incl;
    projectFile = projFile;
    compileResult = compstat;
}

void Builder::setObjects(QLabel *stat, QLabel *progsize, QProgressBar *progbar, QComboBox *ports)
{
    msgLabel = stat;
    sizeLabel = progsize;
    progress = progbar;
    cbPort = ports;
}

QString Builder::filePathName(QString fileName)
{
    QString rets;
    if(fileName.lastIndexOf("/") > -1)
        rets = fileName.mid(0,fileName.lastIndexOf("/")+1);
    else
        rets = fileName;
    return rets;
}

int Builder::runCompiler(QString copts)
{
    // satisfy warnings only
    if(copts.length())
        return 0;
    return 0;
}

void Builder::compilerError(QProcess::ProcessError error)
{
    // satisfy warnings only
    if(error != QProcess::Crashed) return;
}

void Builder::compilerFinished(int exitCode, QProcess::ExitStatus status)
{
    // satisfy warnings only
    if(exitCode) return;
    if(status != QProcess::CrashExit) return;
}

void Builder::procReadyRead()
{
}

