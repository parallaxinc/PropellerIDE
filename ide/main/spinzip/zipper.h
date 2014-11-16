#ifndef ZIPPER_H
#define ZIPPER_H

#include "qtversion.h"

#include "StatusDialog.h"

extern "C" {
    extern int minizip(const char *zipFileName, const char *fileListCSV);
    extern int miniunzip(const char *zipFile, const char *extractToFile);
}

class Zipper : public QObject
{
    Q_OBJECT
public:
    explicit Zipper(QObject *parent = 0);
    bool makeZip(QString fileName, QStringList fileTree, QString libPath, StatusDialog *stat);

private:
    QString getZipDestination(QString fileName);
    void    zipSpinProjectTree(QString fileName, QStringList fileTree);
    QStringList directoryTreeList(QString folder);
    bool    createFolderZip(QString source, QString dstZipFile);
    QString filePathName(QString fileName);
    QString shortFileName(QString fileName);

    QString spinLibPath;
    StatusDialog *statusDialog;

signals:

public slots:

};

#endif // ZIPPER_H
