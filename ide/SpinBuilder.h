#ifndef SPINBUILDER_H
#define SPINBUILDER_H

#include <QtCore>
#include "EzBuilder.h"

class SpinBuilder : public EzBuilder
{
public:
    SpinBuilder();
    virtual ~SpinBuilder() {}

    void setLoader(QString loader);
    int loadProgram(QString copts);
    int runCompiler(QString copts);

    void compilerError(QProcess::ProcessError error);
    void compilerFinished(int exitCode, QProcess::ExitStatus status);
    void procReadyRead();

private:
    QString loader;
    int  checkCompilerInfo();
    QStringList getCompilerParameters();
    bool receiving;
};

#endif // SPINBUILDER_H
