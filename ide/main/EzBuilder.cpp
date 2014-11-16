#include "EzBuilder.h"

EzBuilder::EzBuilder(QObject *parent) : QObject(parent)
{
}

void EzBuilder::setParameters(QString comp, QString incl, QString compPath, QString projFile, QString compstat)
{
    compilerStr = comp;
    compilerPath = compPath;
    includesStr = incl;
    projectFile = projFile;
    compileResult = compstat;
}

void EzBuilder::setObjects(QLabel *stat, QLabel *progsize, QProgressBar *progbar, QComboBox *ports)
{
    msgLabel = stat;
    sizeLabel = progsize;
    progress = progbar;
    cbPort = ports;
}

QString EzBuilder::filePathName(QString fileName)
{
    QString rets;
    if(fileName.lastIndexOf("/") > -1)
        rets = fileName.mid(0,fileName.lastIndexOf("/")+1);
    else
        rets = fileName;
    return rets;
}

int EzBuilder::runCompiler(QString copts)
{
    // satisfy warnings only
    if(copts.length())
        return 0;
    return 0;
}

void EzBuilder::compilerError(QProcess::ProcessError error)
{
    // satisfy warnings only
    if(error != QProcess::Crashed) return;
}

void EzBuilder::compilerFinished(int exitCode, QProcess::ExitStatus status)
{
    // satisfy warnings only
    if(exitCode) return;
    if(status != QProcess::CrashExit) return;
}

void EzBuilder::procReadyRead()
{
}

