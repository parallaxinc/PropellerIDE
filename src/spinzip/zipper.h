#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

extern "C" {
    extern int minizip(const char *zipFileName, const char *fileListCSV);
    extern int miniunzip(const char *zipFile, const char *extractToFile);
}

class Zipper : public QObject
{
    Q_OBJECT

    QString getZipDestination(QString fileName);

public:
    explicit Zipper(QObject *parent = 0);
    bool makeZip(QString fileName, QStringList files);

};
