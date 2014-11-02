#include <QString>
#include <QFile>
#include <QDebug>

#include "osint.h"
#include "p1load.h"
#include "p1loader.h"

P1Loader::P1Loader()
{
}

int P1Loader::open(QString portname, int baud)
{
    return ::OpenPort(portname.toLatin1().data(), baud);
}

void P1Loader::close()
{
    ::ClosePort();
}

#if 1
int  P1Loader::loadFile(QString filename, int operation)
{
    int rc = 0;
    int lc = 0;
    int version;
    QByteArray barry;
    QFile file(filename);
    if(file.open(QFile::ReadOnly)) {
        barry = file.readAll();
        file.close();

        lc = ::HardwareFound(&version);
        if(!lc) {
            // give it a second chance
            lc = ::HardwareFound(&version);
        }
        if(!lc) {
            qDebug() << "Propeller not found.";
            rc = 1;
        }
        else if(::LoadSpinBinary(operation, barry.data(), barry.length()) != 0) {
            qDebug() << filename << "Load failed\n";
            rc = 2;
        }
    }
    else {
        rc = -1;
    }
    return rc;
}
#else
int  P1Loader::loadFile(QString filename, int operation)
{
    int rc = 0;
    int version;
    QByteArray barry;
    QFile file(filename);
    if(file.open(QFile::ReadOnly)) {
        barry = file.readAll();
        file.close();
        if(!::HardwareFound(&version)) {
            qDebug() << "Propeller not found.";
            rc = 3;
        }
        else if(::LoadSpinBinary(operation, barry.data(), barry.length()) != 0) {
            qDebug() << filename << "Load failed\n";
            rc = 4;
        }
    }
    else {
        rc = -1;
    }
    return rc;
}
#endif

int P1Loader::load(QString filename, int operation, int fileid)
{
    int rc = 0;
    if(fileid == -1) {
        qDebug("Error port must be opened.\n");
        rc = 1;
    }
    else {
#ifdef Q_OS_WIN
        serial_sethandle(fileid);
#endif
        rc = loadFile(filename, operation);
        if(rc < 0) {
            qDebug() << "Can't open file" << filename;
            rc = 2;
        }
    }
    return rc;
}

int P1Loader::load(QString filename, int operation, QString portname, int baud)
{
    int rc = 0;
    if((rc = open(portname.toLatin1().data(), baud))) {
        if(rc == CHECK_PORT_OPEN_FAILED)
            qDebug() << "Error opening port" << portname << "at" << baud << "bps";
        else
            qDebug() << "Propeller not found on" << portname;
        rc = 1;
    }
    else if((rc |= loadFile(filename, operation))) {
        qDebug() << filename << "Load failed\n";
    }
    close();
    return rc;
}
