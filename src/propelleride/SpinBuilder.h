#pragma once

#include "Builder.h"

#include <QStringList>
#include <QDialog>
#include <QPlainTextEdit>

class SpinBuilder : public Builder
{
public:
    SpinBuilder();
    virtual ~SpinBuilder();

    void setLoader(QString loader);
    int loadProgram(QString copts);
    int runCompiler(QString copts);

    void compilerFinished(int exitCode, QProcess::ExitStatus status);
    void procReadyRead();

private:
    QString loader;
    QStringList getCompilerParameters();
    QDialog * console;
    QPlainTextEdit * consoleEdit;
};
