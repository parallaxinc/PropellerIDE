#include "BuildManager.h"

BuildManager::BuildManager(QWidget *parent) : QWidget(parent)
{
    console = new Status(this);
    consoleEdit = console->getOutput();
    connect(&timer,SIGNAL(timeout()),console,SLOT(hide()));
}

BuildManager::~BuildManager()
{
    delete console;
}

void BuildManager::show()
{
    timer.stop();
    console->setStage(0);
    consoleEdit->clear();
    console->show();
}

void BuildManager::hide()
{
    console->hide();
}

void BuildManager::waitClose()
{
    timer.setSingleShot(true);
    timer.start(500);
}

void BuildManager::setFont(const QFont & font)
{
    consoleEdit->setFont(font);
}

void BuildManager::setParameters(
        QString comp,
        QString load,
        QString incl,
        QString projFile)
{
    compilerStr = comp;
    loader = load;
    includesStr = incl;
    projectFile = projFile;
    qDebug() << "Setting build: "
	    << compilerStr 
	    << includesStr 
	    << projectFile;
}

void BuildManager::compilerError(QProcess::ProcessError error)
{
    qDebug() << error;
    if(error != QProcess::Crashed) return;
}

void BuildManager::compilerFinished(int exitCode, QProcess::ExitStatus status)
{
    if(procDone == true)
        return;

    procMutex.lock();
    procDone = true;
    procMutex.unlock();

    qDebug() << exitCode << status;
}

void BuildManager::procReadyRead()
{
    QByteArray bytes = proc->readAllStandardOutput();
    if(bytes.length() == 0)
        return;

    bytes = bytes.replace("\r\n","\n");
    compileResult = QString(bytes);
    QStringList lines = QString(bytes).split("\n",QString::SkipEmptyParts);

    QTextCharFormat tf = consoleEdit->currentCharFormat();

    foreach (QString line, lines)
    {
        if (line.contains("Program size is") || line.contains("Bit fe"))
        {
            tf.setForeground(Qt::darkGreen);
            consoleEdit->setCurrentCharFormat(tf);
        }
        else if (line.contains("error", Qt::CaseInsensitive))
        {
            tf.setForeground(Qt::red);
            consoleEdit->setCurrentCharFormat(tf);
        }

        consoleEdit->appendPlainText(line);
    }

    QScrollBar *sb = consoleEdit->verticalScrollBar();
    sb->setValue(sb->maximum());
}

int BuildManager::runProcess(const QString & programName, const QStringList & programArgs)
{
    QStringList args;
    QString program = QDir::toNativeSeparators(programName);
    for (int i = 0; i < programArgs.size(); ++i)
    {
        args.append(QDir::toNativeSeparators(programArgs.at(i)));
    }
    qDebug() << program << args;

    proc = new QProcess;

    proc->setProcessChannelMode(QProcess::MergedChannels);

    procDone = false;

    connect(proc, SIGNAL(readyReadStandardOutput()),this,SLOT(procReadyRead()));
    connect(proc, SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(compilerFinished(int,QProcess::ExitStatus)));

    proc->start(QDir::toNativeSeparators(program),args);

    QTextCharFormat tf = consoleEdit->currentCharFormat();
    tf.setForeground(Qt::black);
    consoleEdit->setCurrentCharFormat(tf);

    if(!proc->waitForStarted())
    {
        QMessageBox::critical(this, tr("Error"),
                             tr("Could not start \"%1.\"\nPlease check Preferences.")
                             .arg(program));
        return 1;
    }

    while(!procDone)
    {
        QApplication::processEvents();
        QThread::msleep(5);
    }

    if(proc->exitStatus() == QProcess::CrashExit)
    {
        QMessageBox::critical(this, tr("Error"),
                             tr("%1 crashed.")
                             .arg(program));
        return 1;
    }

    if(proc->exitCode())
        return 1;

    disconnect(proc);
    delete proc;
    return 0;
}


int BuildManager::loadProgram(QString options)
{
    QStringList optslist, args;
    if (!options.isNull())
    {
        optslist = options.split(" ");
        foreach (QString s, optslist)
        {
            args.append(s);
        }
    }
    args.append(projectFile.replace(".spin",".binary"));

    console->setStage(2);
    console->setText(tr("Downloading %1...").arg(QFileInfo(projectFile).fileName()));

    int rc = runProcess(loader,args);
    if (rc)
    {
        console->showDetails();
    }
    else
    {
        console->setStage(3);
        console->setText(tr("Download complete!"));
        waitClose();
    }

    return rc;
}

int BuildManager::runCompiler(QString options)
{
    console->raise();
    QStringList args;

    if(includesStr.length()) {
        args.append(("-L"));
        args.append(includesStr);
    }

    console->setStage(1);
    console->setText(tr("Building %1...").arg(QFileInfo(projectFile).fileName()));

    args.append(projectFile);

    if (!options.isNull())
        args.append(options);

    int rc = runProcess(compilerStr,args);

    if (rc)
    {
        console->showDetails();
        getCompilerOutput();
        return 1;
    }
    return 0;
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

    qDebug() << "Error line: " << line;
    if(ok)
        emit compilerErrorInfo(file, line);
}
