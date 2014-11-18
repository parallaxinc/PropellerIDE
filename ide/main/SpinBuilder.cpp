#include "qtversion.h"

#include "SpinBuilder.h"
#include "Sleeper.h"

SpinBuilder::SpinBuilder() : EzBuilder(0)
{
}

void SpinBuilder::setLoader(QString loader)
{
    this->loader = loader;
}

int  SpinBuilder::checkCompilerInfo()
{
    QMessageBox mbox(QMessageBox::Critical,tr("Build Error"),"",QMessageBox::Ok);
    if(compilerStr.length() == 0) {
        mbox.setInformativeText(tr("Please specify compiler application in Preferences."));
        mbox.exec();
        return -1;
    }
/*
    if(includesStr.length() == 0) {
        mbox.setInformativeText(tr("Please specify a library path in Preferences."));
        mbox.exec();
        return -1;
    }
    if(loader.length() == 0) {
        mbox.setInformativeText(tr("Please specify the loader in Preferences."));
        mbox.exec();
        return -1;
    }
 */
    return 0;
}

QStringList SpinBuilder::getCompilerParameters()
{
    QString srcpath = projectFile;
    srcpath = QDir::fromNativeSeparators(srcpath);
    srcpath = filePathName(srcpath);

    QStringList args;
    if(srcpath.length()) {
        args.append(("-L"));
        args.append(srcpath);
    }
    if(includesStr.length()) {
        args.append(("-L"));
        args.append(includesStr);
    }
    args.append(projectFile);

    return args;
}

int  SpinBuilder::loadProgram(QString copts)
{
    int exitCode = 0;
    int exitStatus = 0;

    // use the projectFile instead of the current tab file
    QString srcpath = projectFile;
    srcpath = QDir::fromNativeSeparators(srcpath);
    srcpath = filePathName(srcpath);

    portName = cbPort->itemText(cbPort->currentIndex());    // TODO should be itemToolTip

    QStringList optslist = copts.split(" ");
    QStringList args;
    foreach (QString s, optslist) {
        args.append(s);
    }
    args.append(projectFile.replace(".spin",".binary"));
    //args.append("-p");
    //args.append(portName);

    qDebug() << args;

    QMessageBox mbox;
    mbox.setStandardButtons(QMessageBox::Ok);

    QProcess proc(this);
    this->proc = &proc;

    connect(this->proc, SIGNAL(readyReadStandardOutput()),this,SLOT(procReadyRead()));
    connect(this->proc, SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(compilerFinished(int,QProcess::ExitStatus)));
    proc.setProcessChannelMode(QProcess::MergedChannels);

    proc.setWorkingDirectory(filePathName(loader));

    procDone = false;
    receiving = false;
    proc.start(loader,args);

    if(!proc.waitForStarted()) {
        mbox.setInformativeText(tr("Could not start loader. Please check Preferences."));
        mbox.exec();
        goto loaderror;
    }

    progress->setValue(10);


    /* process Qt application events until procDone
     */
    while(procDone == false) {
        QApplication::processEvents();
        Sleeper::ms(5);
    }

    disconnect(this->proc, SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(compilerFinished(int,QProcess::ExitStatus)));

    progress->setValue(20);

    exitCode = proc.exitCode();
    exitStatus = proc.exitStatus();

    mbox.setInformativeText(compileResult);
    //msgLabel->setText("");

    if(exitStatus == QProcess::CrashExit)
    {
        mbox.setText(tr("Loader Crashed"));
        mbox.exec();
        goto loaderror;
    }
    progress->setValue(30);
    if(exitCode != 0)
    {
        mbox.setInformativeText(compileResult);
        mbox.setText(tr("Loader Error"));
        mbox.exec();
        goto loaderror;
    }
    progress->setValue(40);
    if(compileResult.indexOf("error") > -1)
    { // just in case we get an error without exitCode
        mbox.setInformativeText(compileResult);
        mbox.setText(tr("Loader Error"));
        mbox.exec();
        goto loaderror;
    }

    //msgLabel->setText(msgLabel->text()+" Load Complete");
    progress->setValue(100);
    progress->setVisible(false);
    disconnect(this->proc, SIGNAL(readyReadStandardOutput()),this,SLOT(procReadyRead()));
    return 0;

    loaderror:
    msgLabel->setText(msgLabel->text()+" Load Failed");
    progress->setValue(100);
    progress->setVisible(false);
    disconnect(this->proc, SIGNAL(readyReadStandardOutput()),this,SLOT(procReadyRead()));
    return 1;
}

int  SpinBuilder::runCompiler(QString copts)
{
    if(checkCompilerInfo()) {
        return -1;
    }

    int exitCode = 0;
    int exitStatus = 0;

    QStringList args = getCompilerParameters();
    args.append(copts);

    qDebug() << args;

    QMessageBox mbox;
    mbox.setStandardButtons(QMessageBox::Ok);

    QProcess proc(this);
    this->proc = &proc;

    connect(this->proc, SIGNAL(readyReadStandardOutput()),this,SLOT(procReadyRead()));
    connect(this->proc, SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(compilerFinished(int,QProcess::ExitStatus)));

    proc.setProcessChannelMode(QProcess::MergedChannels);

    proc.setWorkingDirectory(compilerPath);

    msgLabel->setText("");

    procDone = false;
    receiving = false;
    proc.start(compilerStr,args);

    if(!proc.waitForStarted()) {
        mbox.setInformativeText(tr("Could not start compiler. Please check Preferences."));
        mbox.exec();
        goto error;
    }

    progress->setValue(10);

    /* process Qt application events until procDone
     */
    while(procDone == false) {
        QApplication::processEvents();
        Sleeper::ms(5);
    }

    disconnect(this->proc, SIGNAL(readyReadStandardOutput()),this,SLOT(procReadyRead()));
    disconnect(this->proc, SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(compilerFinished(int,QProcess::ExitStatus)));

    progress->setValue(20);

    exitCode = proc.exitCode();
    exitStatus = proc.exitStatus();

    mbox.setInformativeText(compileResult);

    if(exitStatus == QProcess::CrashExit)
    {
        mbox.setText(tr("Compiler Crashed"));
        mbox.exec();
        goto error;
    }
    progress->setValue(30);
    if(exitCode != 0)
    {
        //qDebug() << "Compile Error" << compileResult;
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
        if(ok)
            emit compilerErrorInfo(file, line);
        mbox.setInformativeText(compileResult);
        mbox.setText(tr("Compile Error"));
        mbox.exec();
        goto error;
    }
    progress->setValue(40);
    if(compileResult.indexOf("error") > -1)
    { // just in case we get an error without exitCode
        mbox.setText(tr("Compile Error"));
        mbox.exec();
        goto error;
    }

    //msgLabel->setText("Build Complete");
    progress->setValue(100);
    progress->setVisible(false);
    disconnect(this->proc, SIGNAL(readyReadStandardOutput()),this,SLOT(procReadyRead()));

    return 0;

    error:

    sizeLabel->setText("Error");

    exitCode = proc.exitCode();
    exitStatus = proc.exitStatus();

    progress->setValue(100);
    progress->setVisible(false);
    disconnect(this->proc, SIGNAL(readyReadStandardOutput()),this,SLOT(procReadyRead()));
    return -1;
}

void SpinBuilder::compilerError(QProcess::ProcessError error)
{
    qDebug() << error;
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
    QByteArray bytes = this->proc->readAllStandardOutput();
    if(bytes.length() == 0)
        return;

#if defined(Q_OS_WIN32)
    QString eol("\r");
#else
    QString eol("\n");
#endif
    bytes = bytes.replace("\r\n","\n");
    compileResult = QString(bytes);
    QStringList lines = QString(bytes).split("\n",QString::SkipEmptyParts);

    // other external loader
    bool isCompileSz = bytes.contains("Program size is");
    if(isCompileSz) {
        for (int n = 0; n < lines.length(); n++) {
            QString line = lines[n];
            if(line.endsWith(" bytes")) {
                msgLabel->setText(line+eol);
            }
        }
    }

    if(bytes.contains("error")) {
        for (int n = 0; n < lines.length(); n++) {
            QString line = lines[n];
            if(line.length() > 0) {
                if(line.contains("error",Qt::CaseInsensitive)) {
                    msgLabel->setText(line);
                }
            }
        }
    }

    for (int n = 0; n < lines.length(); n++) {
        QString line = lines[n];
        if(line.length() > 0) {
            /*
            bool iserror = compileResult.contains("error:",Qt::CaseInsensitive);
            if(iserror && line.contains(" line ", Qt::CaseInsensitive)) {
                QStringList sl = line.split(" ", QString::SkipEmptyParts);
                if(sl.count() > 1) {
                    int num = QString(sl[sl.count()-1]).toInt();
                    QPlainTextEdit *ed = editors->at(editorTabs->currentIndex());
                    QTextCursor cur = ed->textCursor();
                    cur.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
                    cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, num);
                    cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
                    cur.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                    QApplication::processEvents();
                }
            }
            */
            if(line.contains("Propeller Version",Qt::CaseInsensitive)) {
                //msgLabel->setText(line+eol);
                progMax = 0;
                progress->setValue(0);
                progress->setVisible(true);
            }
            else
            if(line.contains("loading image",Qt::CaseInsensitive)) {
                progMax = 0;
                progress->setValue(0);
                progress->setVisible(true);
                //msgLabel->setText(line+eol);
            }
            else
            if(line.contains("writing",Qt::CaseInsensitive)) {
                progMax = 0;
                progress->setValue(0);
                progress->setVisible(true);
            }
            else
            if(line.contains("Download OK",Qt::CaseInsensitive)) {
                progress->setValue(100);
                //msgLabel->setText(line+eol);
            }
            else
            if(line.contains("sent",Qt::CaseInsensitive)) {
                //msgLabel->setText(line+eol);
                //sizeLabel->setText(line);
                line = line.trimmed();
                progress->setVisible(false);
                sizeLabel->setText(line.left(line.indexOf(" "))+tr(" bytes loaded"));
            }
            else
            if(line.contains("remaining",Qt::CaseInsensitive)) {
                if(progMax == 0) {
                    QString bs = line.mid(0,line.indexOf(" "));
                    progMax = bs.toInt();
                    // include VM size
                    progMax /= 1024;
                    progMax++;
                    progCount = 0;
                    if(progMax == 0) {
                        progress->setValue(100);
                    }
                }
                if(progMax != 0) {
                    progCount++;
                    progress->setValue(100*progCount/progMax);
                }
                //msgLabel->setText(line);
            }
        }
    }

#ifdef EXTERNAL_SPIN_LOADER
    if(bytes.contains("RECEIVED")) {
        receiving = true;
    }

    if(receiving) {
        QString s(bytes);
        if(bytes.contains("RECEIVED")) {
            s = s.mid(s.indexOf("RECEIVED")+8);
            while(s.length() > 0 && s.at(0).isSpace())
                s = s.mid(1);
        }
        if(s.length() > 0) {
            emit terminalReceived(QString(s));
            qDebug() << "RECEIVED" << s;
        }
    }
#endif
}
