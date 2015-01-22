#include "Builder.h"

Builder::Builder(QObject *parent) : QObject(parent)
{
}

void Builder::setParameters(QString comp, QString incl, QString projFile, QString compstat)
{
    compilerStr = comp;
    includesStr = incl;
    projectFile = projFile;
    compileResult = compstat;
    qDebug() << "Setting build: "
	    << compilerStr 
	    << includesStr 
	    << projectFile 
	    << compileResult;
}

void Builder::setObjects(QLabel *stat, QLabel *progsize, QProgressBar *progbar, QComboBox *ports)
{
    msgLabel = stat;
    sizeLabel = progsize;
    progress = progbar;
    cbPort = ports;
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

