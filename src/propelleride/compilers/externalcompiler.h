#pragma once

#include "compiler.h"

#include <QObject>
#include <QProcess>
#include <QRegularExpression>

class ExternalCompiler : public Compiler
{
    Q_OBJECT

    QProcess * proc;

    QRegularExpression re_success;
    QRegularExpression re_error;

public:
    explicit ExternalCompiler(QObject * parent = 0) Q_DECL_OVERRIDE;
    virtual ~ExternalCompiler() Q_DECL_OVERRIDE;

    void build(QString filename) Q_DECL_OVERRIDE;

signals:
    void finished(bool success);
    void highlightLine(const QString & file,
            int line,
            int col,
            const QString & text);

private slots:

    void cleanup();
    void finish(int exitCode, QProcess::ExitStatus status);
    void readOutput();
    void handleError(QProcess::ProcessError e);

};
