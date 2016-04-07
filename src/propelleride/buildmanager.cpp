#include "buildmanager.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QScrollBar>
#include <QThread>

BuildManager::BuildManager(QWidget *parent)
    : QFrame(parent)
{
    ui.setupUi(this);

    ui.activeText->setText(" ");
    setStage(0);

    connect(&timer, SIGNAL(timeout()),  this,   SLOT(hideStatus()));

    currentTheme = &Singleton<ColorScheme>::Instance();
    updateColors();
}

BuildManager::~BuildManager()
{
}

void BuildManager::showStatus()
{
    setStage(0);
    show();
    raise();
    timer.stop();
    ui.plainTextEdit->clear();
}

void BuildManager::hideStatus()
{
    hide();
}

void BuildManager::waitClose()
{
    timer.setSingleShot(true);
    timer.start(100);
}

void BuildManager::setFont(const QFont & font)
{
    ui.plainTextEdit->setFont(font);
}

void BuildManager::setConfiguration(BuildManager::Configuration config)
{
    this->config = config;
}

void BuildManager::handleCompilerError(QProcess::ProcessError e)
{
    qCritical() << "Build failure" << config.compiler;
    failure = true;
    timer.stop();

    QString errorstring;
    switch (e)
    {
        case QProcess::FailedToStart:
            errorstring = tr("Failed to start compiler: '%1'; check Preferences").arg(config.compiler);
            break;
        case QProcess::Crashed:
        case QProcess::Timedout:
        case QProcess::WriteError:
        case QProcess::ReadError:
        case QProcess::UnknownError:
            errorstring = ((QProcess *) sender())->errorString();
            break;
    }

    print("ERROR: "+errorstring,Qt::red);
    setText(tr("Build failed!"));
    QMessageBox::critical((QWidget *) parent(),
            tr("Build Failed"),
            tr("%1").arg(errorstring));

    emit buildError();
    emit finished();
}

void BuildManager::compilerFinished(int exitCode, QProcess::ExitStatus status)
{
    if (exitCode != 0 || status != QProcess::NormalExit)
    {
        failure = true;
        setText(tr("Build failed!"));
        emit finished();
    }
    else
    {
        setText(tr("Build successful!"));

        if (config.load)
        {
            if (!load())
                emit finished();
        }
        else
        {
            emit finished();
        }
    }
}

void BuildManager::setTextColor(QColor color)
{
    QTextCharFormat tf = ui.plainTextEdit->currentCharFormat();
    tf.setForeground(color);
    ui.plainTextEdit->setCurrentCharFormat(tf);
}

void BuildManager::print(const QString & text, QColor color)
{
    QTextCharFormat tf = ui.plainTextEdit->currentCharFormat();
    QBrush oldcolor = tf.foreground();

    tf.setForeground(color);
    ui.plainTextEdit->setCurrentCharFormat(tf);

    ui.plainTextEdit->appendPlainText(text);

    tf.setForeground(oldcolor);
    ui.plainTextEdit->setCurrentCharFormat(tf);
}

void BuildManager::procReadyRead()
{
    QProcess * proc = (QProcess *) sender();

    QByteArray bytes = proc->readAllStandardOutput();
    if(bytes.length() == 0)
        return;

    bytes = bytes.replace("\r\n","\n");
    compileResult = QString(bytes);
    QStringList lines = QString(bytes).split("\n",QString::SkipEmptyParts);

    foreach (QString line, lines)
    {
        if (line.contains("Program size is") || line.contains("Bit fe") || line.contains("DOWNLOAD COMPLETE"))
        {
            setTextColor(Qt::darkGreen);
        }
        else if (line.contains("error", Qt::CaseInsensitive))
        {
            setTextColor(Qt::red);
        }

        ui.plainTextEdit->appendPlainText(line);
    }

    QScrollBar *sb = ui.plainTextEdit->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void BuildManager::runProcess(const QString & programName, const QStringList & programArgs)
{
    setTextColor(Qt::black);

    QStringList args;
    QString program = QDir::toNativeSeparators(programName);
    for (int i = 0; i < programArgs.size(); ++i)
    {
        args.append(QDir::toNativeSeparators(programArgs.at(i)));
    }

    qCDebug(logbuildmanager) << qPrintable(program);
    foreach (QString a, args)
    {
        qCDebug(logbuildmanager) << "    " << qPrintable(a);

    }

    QProcess * proc = new QProcess;
    proc->setProcessChannelMode(QProcess::MergedChannels);

    connect(proc,   SIGNAL(readyReadStandardOutput()),          this,   SLOT(procReadyRead()));
    connect(proc,   SIGNAL(finished(int,QProcess::ExitStatus)), this,   SLOT(compilerFinished(int,QProcess::ExitStatus)));
    connect(proc,   SIGNAL(error(QProcess::ProcessError)),      this,   SLOT(handleCompilerError(QProcess::ProcessError)));
    connect(proc,   SIGNAL(finished(int)),                      proc,   SLOT(deleteLater()));

    proc->start(program,args);
}


bool BuildManager::load(const QByteArray & binary)
{
    setStage(2);
    setTextColor(Qt::darkYellow);

    PropellerLoader loader(config.manager, config.port);

    connect(&loader,    SIGNAL(success()), this, SLOT(loadSuccess()));
    connect(&loader,    SIGNAL(failure()), this, SLOT(loadFailure()));
    connect(&loader,    SIGNAL(finished()), this, SIGNAL(finished()));

    connect(&loader,            SIGNAL(statusChanged(const QString &)),
            this,               SLOT(setText(const QString &)));

    connect(&loader,            SIGNAL(statusChanged(const QString &)),
            this,               SIGNAL(statusChanged(const QString &)));

    connect(&loader,            SIGNAL(statusChanged(const QString &)),
            ui.plainTextEdit,   SLOT(appendPlainText(const QString &)));

    PropellerImage image;

    if (!binary.isEmpty())
    {
        image = PropellerImage(binary);
    }
    else
    {
        QFile file(config.binary);
        if (!file.open(QIODevice::ReadOnly))
        {
            qCCritical(logbuildmanager) << "Couldn't open file:" << config.binary;
            return false;
        }

        image = PropellerImage(file.readAll(), config.binary);
    }

    loader.upload(image, config.write, true, true);
    emit finished();

    waitClose();
    return true;
}

void BuildManager::loadSuccess()
{
    setStage(3);
    setTextColor(Qt::darkGreen);
}

void BuildManager::loadFailure()
{
    setTextColor(Qt::red);
}

void BuildManager::build()
{
    failure = false;

    showStatus();

    QStringList args;

    foreach (QString include, config.includes)
    {
        if (include.size() > 0)
            args.append("-L" + include);
    }

    QString actionstring = tr("Building '%1'...")
        .arg(QFileInfo(config.file).fileName());

    setStage(1);
    print(actionstring, Qt::darkYellow);
    setText(actionstring);

    args.append(config.file);

    runProcess(config.compiler, args);
}

void BuildManager::getCompilerOutput()
{
    QChar c;
    // filter non-chars for the moment
    for(int n = compileResult.length()-1; n > -1; n--) {
        c = compileResult.at(n);
        if(!c.toLatin1())
            compileResult.remove(n,1);
    }
    /*
     * Error example:
     * \nC:/Propeller/EEPROM/eeloader.spin(57:3) : error : Expected an instruction or variable\nLine:\n  boo.start(BOOTADDR, size, eeSetup, eeClkLow, eeClkHigh)\nOffending Item: boo\n
     */
    QStringList list = compileResult.split(QRegExp("\r|\n|\r\n|\n\r"));
    QString file;
    int line = 0;
    QRegExp err("error");
    err.setCaseSensitivity(Qt::CaseInsensitive);
    bool ok = false;
    foreach(QString s, list)
    {
        int pos = s.indexOf(err);
        if(pos > -1)
        {
            s = s.mid(0,s.indexOf(err));
            if(s.indexOf("(") > -1)
            {
                file = s.left(s.indexOf("("));
                s = s.mid(s.indexOf("(")+1);
                s = s.left(s.indexOf(":"));
                line = s.toInt(&ok);
                line--; // line number is 1 based. highlight is 0 based.
            }
            break;
        }
    }
    if(ok)
        emit compilerErrorInfo(file, line);
}

void BuildManager::updateColors()
{
    ui.plainTextEdit->setFont(currentTheme->getFont());
}

void BuildManager::setText(const QString & text)
{
    ui.activeText->setText(text);
}

void BuildManager::keyPressEvent(QKeyEvent * event)
{
    if(event->key() == Qt::Key_Escape)
    {
        event->ignore();
    }
}

void BuildManager::setBuild(bool active)
{
    ui.iconBuild->setEnabled(active);
}

void BuildManager::setDownload(bool active)
{
    ui.arrow1->setEnabled(active);
    ui.iconDownload->setEnabled(active);
}

void BuildManager::setRun(bool active)
{
    ui.arrow2->setEnabled(active);
    ui.iconRun->setEnabled(active);
}

void BuildManager::setStage(int stage)
{
    if (stage > 0)
        setBuild(true);
    else
        setBuild(false);

    if (stage > 1)
        setDownload(true);
    else
        setDownload(false);

    if (stage > 2)
        setRun(true);
    else
        setRun(false);
}

