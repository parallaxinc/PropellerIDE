#include "externalcompiler.h" 

#include <QDebug>
#include <QDir>
#include <QString>
#include <QRegularExpression>

ExternalCompiler::ExternalCompiler(QObject * parent)
    : Compiler(parent)
{
    proc = NULL;

    // openspin
    re_success = QRegularExpression("(Done)", QRegularExpression::CaseInsensitiveOption);
    re_error = QRegularExpression("(.*?)\\.spin\\(([0-9]+):([0-9]+)\\)[ \t]*:[ \t]*error[ \t]*:[ \t]*(.*)", QRegularExpression::CaseInsensitiveOption);

    // bstc
    re_success = QRegularExpression("(Program size is)", QRegularExpression::CaseInsensitiveOption);
    re_error = QRegularExpression("(.*?)\\s*\\(\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*\\)\\s*error\\s*:\\s*(.*)", QRegularExpression::CaseInsensitiveOption);

}

ExternalCompiler::~ExternalCompiler()
{
    cleanup();
}

void ExternalCompiler::build(QString filename)
{
    qDebug() << "Building project" << filename;

    QStringList arglist;
    arglist << "bstc"
            << filename;

    if (arglist.isEmpty())
    {
        qCritical() << "No compiler specified";
        return;
    }

    QString program = QDir::toNativeSeparators(arglist.takeFirst());

    QStringList args;
    for (int i = 0; i < arglist.size(); ++i)
    {
        args.append(QDir::toNativeSeparators(arglist.at(i)));
    }

    qDebug() << qPrintable(program);
    foreach (QString a, args)
    {
        qDebug() << "    " << qPrintable(a);
    }

    if (proc)
    {
        qCritical() << "Build already in progress!";
        return;
    }

    proc = new QProcess();
    proc->setProcessChannelMode(QProcess::MergedChannels);

    connect(proc,   SIGNAL(readyReadStandardOutput()),          this,   SLOT(readOutput()));
    connect(proc,   SIGNAL(error(QProcess::ProcessError)),      this,   SLOT(handleError(QProcess::ProcessError)));
    connect(proc,   SIGNAL(finished(int,QProcess::ExitStatus)), this,   SLOT(finish(int,QProcess::ExitStatus)));
    connect(this,   SIGNAL(finished(bool)),                     this,   SLOT(cleanup()));

    proc->start(program, args);
}

void ExternalCompiler::readOutput()
{
    QString output = QString(proc->readAllStandardOutput());
    if (output.isEmpty())
        return;

    output = output.replace("\r\n", "\n");
    QStringList lines = output.split("\n", QString::SkipEmptyParts);

    foreach (QString line, lines)
    {
        QRegularExpressionMatch m_error = re_error.match(line);
        QRegularExpressionMatch m_success = re_success.match(line);

        if (m_error.hasMatch())
        {
            qCritical() << qPrintable(QString("%1 (%2, %3): %4")
                            .arg(m_error.captured(1))
                            .arg(m_error.captured(2))
                            .arg(m_error.captured(3))
                            .arg(m_error.captured(4)));
            emit highlightLine(m_error.captured(1)+".spin",
                               m_error.captured(2).toInt()-1,
                               m_error.captured(3).toInt()-1,
                               m_error.captured(4));
            return;
        }
        else if (m_success.hasMatch())
        {
            qDebug() << m_success.captured(1);
        }

        qDebug() << "    " << line;
    }
}

void ExternalCompiler::finish(int exitCode, QProcess::ExitStatus status)
{
    if (exitCode != 0 || status != QProcess::NormalExit)
        emit finished(false);
    else
        emit finished(true);
}

void ExternalCompiler::handleError(QProcess::ProcessError e)
{
    QString errorstring;
    switch (e)
    {
        case QProcess::FailedToStart:
            errorstring = tr("Failed to start compiler: '%1'; check Preferences").arg("compiler");
            break;
        case QProcess::Crashed:
        case QProcess::Timedout:
        case QProcess::WriteError:
        case QProcess::ReadError:
        case QProcess::UnknownError:
            errorstring = proc->errorString();
            break;
    }

    qCritical() << errorstring;
//    print("ERROR: "+errorstring,Qt::red);
//    QMessageBox::critical((QWidget *) parent(),
//            tr("Build Failed"),
//            tr("%1").arg(errorstring));

    emit finished(false);
}

void ExternalCompiler::cleanup()
{
    if (proc)
        delete proc;
    proc = NULL;
}

