#pragma once

#include "compiler.h"

#include <QProcess>
#include <QRegularExpression>
#include <QHash>

class ExternalCompiler : public Compiler
{
    Q_OBJECT

    static QHash<QString, QString> _lookup;

    QProcess * proc;

    QString arg_exe;
    QString arg_flags;
    QString arg_library;
    QString arg_output;

    QString pattern_in;
    QString pattern_out;
    QString pattern_ret;

    QRegularExpression re_success;
    QRegularExpression re_error;

public:
    explicit ExternalCompiler(QString name,
                              QObject * parent = 0) Q_DECL_OVERRIDE;
    virtual ~ExternalCompiler() Q_DECL_OVERRIDE;

    QString build(QString filename,
                  QStringList libraries = QStringList()) Q_DECL_OVERRIDE;

    static void add(QString name, QString filename);
    void load(QString filename);
    void save(QString filename);

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
