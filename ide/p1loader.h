#ifndef P1LOADER_H
#define P1LOADER_H

#include <QString>
#include "p1load.h"

class P1Loader
{
public:
    P1Loader();

    enum { DontClose = false };
    enum { ClosePort = true };
    enum { LoadRunEeprom = LOAD_TYPE_EEPROM_RUN };
    enum { LoadRunHubRam = LOAD_TYPE_RUN };

    int open(QString portname, int baud);
    void close();
    int loadFile(QString filename, int operation);

    int load(QString filename, int operation, int fileid);
    int load(QString filename, int operation, QString portname, int baud);
};

#endif // P1LOADER_H
