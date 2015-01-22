#include <Qt>
#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include <QByteArray>
#include <QDebug>
#include <QFileInfo>
#include <QHBoxLayout>

#include "SpinBuilder.h"
#include "Sleeper.h"

SpinBuilder::SpinBuilder() : Builder(0)
{
    console = new QDialog;
    console->setWindowFlags(Qt::WindowStaysOnTopHint);
    consoleEdit = new QPlainTextEdit; 
    consoleEdit->setReadOnly(true);
    QHBoxLayout * layout = new QHBoxLayout;
    layout->addWidget(consoleEdit);
    console->setLayout(layout);
    console->show();

    proc = new QProcess;

    connect(proc, SIGNAL(readyReadStandardOutput()),this,SLOT(procReadyRead()));
    connect(proc, SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(compilerFinished(int,QProcess::ExitStatus)));
}

SpinBuilder::~SpinBuilder()
{
    delete proc;
    delete consoleEdit;
    delete console;
}

void SpinBuilder::setLoader(QString loader)
{
    this->loader = loader;
}

int  SpinBuilder::loadProgram(QString copts)
{
    QStringList optslist = copts.split(" ");
    QStringList args;
    foreach (QString s, optslist) {
        args.append(s);
    }
    args.append(QDir::toNativeSeparators(
                            projectFile.replace(".spin",".binary")
                            ));
    qDebug() << args;

    QMessageBox mbox;
    mbox.setStandardButtons(QMessageBox::Ok);

    proc->setProcessChannelMode(QProcess::MergedChannels);

    procDone = false;
    proc->start(QDir::toNativeSeparators(loader),args);

    QTextCharFormat tf = consoleEdit->currentCharFormat();
    tf.setForeground(Qt::black);
    consoleEdit->setCurrentCharFormat(tf);

    if(!proc->waitForStarted()) {
        mbox.setInformativeText(tr("Could not start loader. Please check Preferences."));
        mbox.exec();
        goto loaderror;
    }

    /* process Qt application events until procDone
     */
    while(procDone == false) {
        QApplication::processEvents();
        Sleeper::ms(5);
    }

    mbox.setInformativeText(compileResult);

    if(proc->exitStatus() == QProcess::CrashExit)
    {
        mbox.setText(tr("Loader Crashed"));
        mbox.exec();
        goto loaderror;
    }
    if(proc->exitCode())
    {
        mbox.setInformativeText(compileResult);
        mbox.setText(tr("Loader Error"));
        mbox.exec();
        goto loaderror;
    }
    if(compileResult.indexOf("error") > -1)
    {
        mbox.setInformativeText(compileResult);
        mbox.setText(tr("Loader Error"));
        mbox.exec();
        goto loaderror;
    }

    return 0;

    loaderror:
    return 1;
}

int  SpinBuilder::runCompiler(QString copts)
{
    if(compilerStr.isEmpty()) {
        return 1;
    }

    QStringList args;
    if(includesStr.length()) {
        args.append(("-L"));
        args.append(QDir::toNativeSeparators(includesStr));
    }

    args.append(QDir::toNativeSeparators(projectFile));
    args.append(copts);

    qDebug() << args;

    QMessageBox mbox;
    mbox.setStandardButtons(QMessageBox::Ok);


    proc->setProcessChannelMode(QProcess::MergedChannels);

    procDone = false;
    proc->start(QDir::toNativeSeparators(compilerStr),args);
    QTextCharFormat tf = consoleEdit->currentCharFormat();
    tf.setForeground(Qt::black);
    consoleEdit->setCurrentCharFormat(tf);

    if(!proc->waitForStarted()) {
        mbox.setInformativeText(tr("Could not start compiler. Please check Preferences."));
        mbox.exec();
        goto error;
    }


    /* process Qt application events until procDone
     */
    while(procDone == false) {
        QApplication::processEvents();
        Sleeper::ms(5);
    }

    mbox.setInformativeText(compileResult);

    if (proc->exitStatus() == QProcess::CrashExit)
    {
        mbox.setText(tr("Compiler Crashed"));
        mbox.exec();
        goto error;
    }
    if(proc->exitCode())
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
        QRegExp err(" : error");
        err.setCaseSensitivity(Qt::CaseInsensitive);
        bool ok = false;
        foreach(QString s, list) {
            int pos = s.indexOf(err);
            if(pos > -1) {
                s = s.mid(0,s.indexOf(err));
                if(s.indexOf("(") > -1) {
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
        mbox.setInformativeText(compileResult);
        mbox.setText(tr("Compile Error"));
        mbox.exec();
        goto error;
    }
    if(compileResult.indexOf("error") > -1)
    {
        mbox.setText(tr("Compile Error"));
        mbox.exec();
        goto error;
    }

    return 0;

    error:

    return -1;
}

void SpinBuilder::compilerFinished(int exitCode, QProcess::ExitStatus status)
{
    if(procDone == true)
        return;

    procMutex.lock();
    procDone = true;
    procMutex.unlock();

    qDebug() << exitCode << status;
}

void SpinBuilder::procReadyRead()
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
        else if (line.contains("error"))
        {
            tf.setForeground(Qt::red);
            consoleEdit->setCurrentCharFormat(tf);
        }

        consoleEdit->appendPlainText(line);
    }
}
