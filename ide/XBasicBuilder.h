#ifndef XBASICBUILDER_H
#define XBASICBUILDER_H

#include <QtCore>
#include "EzBuilder.h"

class XBasicBuilder : public EzBuilder
{
public:
    XBasicBuilder();
    int runCompiler(QString copts);

    void compilerError(QProcess::ProcessError error);
    void compilerFinished(int exitCode, QProcess::ExitStatus status);
    void procReadyRead();

private:
    int  checkCompilerInfo();
    QStringList getCompilerParameters(QString copts);
};

#endif // XBASICBUILDER_H
