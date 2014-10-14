#include "qtversion.h"

#include "XBasicBuilder.h"

XBasicBuilder::XBasicBuilder() : EzBuilder(0)
{
}

int  XBasicBuilder::checkCompilerInfo()
{
    QMessageBox mbox(QMessageBox::Critical,tr("Build Error"),"",QMessageBox::Ok);
    if(compilerStr.length() == 0) {
        mbox.setInformativeText(tr("Please specify compiler application in properties."));
        mbox.exec();
        return -1;
    }
    if(includesStr.length() == 0) {
        mbox.setInformativeText(tr("Please specify include path in properties."));
        mbox.exec();
        return -1;
    }
    return 0;
}

QStringList XBasicBuilder::getCompilerParameters(QString copts)
{
    // use the projectFile instead of the current tab file
    QString srcpath = projectFile;
    //QString srcpath = this->editorTabs->tabToolTip(this->editorTabs->currentIndex());
    srcpath = QDir::fromNativeSeparators(srcpath);
    srcpath = filePathName(srcpath);

    portName = cbPort->itemText(cbPort->currentIndex());    // TODO should be itemToolTip
    //boardName = cbBoard->itemText(cbBoard->currentIndex());

    QStringList args;
    args.append(("-d")); // tell compiler to insert a delay for terminal startup
    //args.append(("-b"));
    //args.append(boardName);
    args.append(("-p"));
    args.append(portName);
    args.append(("-I"));
    args.append(includesStr);
    args.append(("-I"));
    args.append(srcpath);
    args.append(projectFile);
    args.append(copts);

    qDebug() << args;
    return args;
}

int  XBasicBuilder::runCompiler(QString copts)
{
    if(checkCompilerInfo()) {
        return -1;
    }

    QString result;
    int exitCode = 0;
    int exitStatus = 0;

    QStringList args = getCompilerParameters(copts);

    QMessageBox mbox;
    mbox.setStandardButtons(QMessageBox::Ok);

    QProcess proc(this);
    this->proc = &proc;

    connect(this->proc, SIGNAL(readyReadStandardOutput()),this,SLOT(procReadyRead()));
    proc.setProcessChannelMode(QProcess::MergedChannels);

    proc.setWorkingDirectory(compilerPath);

    proc.start(compilerStr,args);

    if(!proc.waitForStarted()) {
        mbox.setInformativeText(tr("Could not start compiler."));
        mbox.exec();
        goto error;
    }

    progress->setValue(10);

    if(!proc.waitForFinished()) {
        mbox.setInformativeText(tr("Error waiting for compiler to finish."));
        mbox.exec();
        goto error;
    }

    progress->setValue(20);

    exitCode = proc.exitCode();
    exitStatus = proc.exitStatus();

    mbox.setInformativeText(compileResult);
    msgLabel->setText("");

    if(exitStatus == QProcess::CrashExit)
    {
        mbox.setText(tr("Compiler Crashed"));
        mbox.exec();
        goto error;
    }
    progress->setValue(30);
    if(exitCode != 0)
    {
        if(compileResult.toLower().indexOf("helper") > 0) {
            mbox.setInformativeText(result +
                 "\nDid you set the right board type?" +
                 "\nHUB and C3 set 80MHz clock." +
                 "\nHUB96 and SSF set 96MHz clock.");
        }
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

    msgLabel->setText("Build Complete");
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

void XBasicBuilder::compilerError(QProcess::ProcessError error)
{
    qDebug() << error;
}

void XBasicBuilder::compilerFinished(int exitCode, QProcess::ExitStatus status)
{
    qDebug() << exitCode << status;
}

void XBasicBuilder::procReadyRead()
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
    if(bytes.contains("bytes")) {
        for (int n = 0; n < lines.length(); n++) {
            QString line = lines[n];
            if(line.length() > 0) {
                if(line.indexOf("\r") > -1) {
                    QStringList more = line.split("\r",QString::SkipEmptyParts);
                    lines.removeAt(n);
                    for(int m = more.length()-1; m > -1; m--) {
                        QString ms = more.at(m);
                        if(ms.contains("bytes",Qt::CaseInsensitive))
                            lines.insert(n,more.at(m));
                        if(ms.contains("loading",Qt::CaseInsensitive))
                            lines.insert(n,more.at(m));
                    }
                }
            }
        }
    }
    int vmsize = 1916;

    bool isCompileSz = bytes.contains(" entry") && bytes.contains(" base") &&
                       bytes.contains(" file offset") && bytes.contains(" size");
    if(isCompileSz) {
        for (int n = 0; n < lines.length(); n++) {
            QString line = lines[n];
            if(line.endsWith(" size")) {
                QStringList sl = line.split(" ", QString::SkipEmptyParts);
                if(sl.count() > 1) {
                    bool ok;
                    int num = QString(sl[0]).toInt(&ok, 16);
                    sizeLabel->setText(QString::number(vmsize+num)+tr(" Total Bytes"));
                    return;
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
                msgLabel->setText(line+eol);
                progress->setValue(0);
            }
            else
            if(line.contains("loading image",Qt::CaseInsensitive)) {
                progMax = 0;
                progress->setValue(0);
                msgLabel->setText(line+eol);
            }
            else
            if(line.contains("writing",Qt::CaseInsensitive)) {
                progMax = 0;
                progress->setValue(0);
            }
            else
            if(line.contains("Download OK",Qt::CaseInsensitive)) {
                progress->setValue(100);
                msgLabel->setText(line+eol);
            }
            else
            if(line.contains("sent",Qt::CaseInsensitive)) {
                msgLabel->setText(line+eol);
            }
            else
            if(line.contains("remaining",Qt::CaseInsensitive)) {
                if(progMax == 0) {
                    QString bs = line.mid(0,line.indexOf(" "));
                    progMax = bs.toInt();
                    // include VM size
                    sizeLabel->setText(QString::number(vmsize+progMax)+tr(" Total Bytes"));
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
                msgLabel->setText(line);
            }
        }
    }

}
