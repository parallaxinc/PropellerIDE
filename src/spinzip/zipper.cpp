#include "zipper.h"

#include "zipreader.h"
#include "zipwriter.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

Zipper::Zipper(QObject *parent) :
    QObject(parent)
{
}

QString Zipper::getZipDestination(QString fileName)
{
    return QFileDialog::getSaveFileName(
            0,
            tr("Create Project Archive"),
            QFileInfo(fileName).path()+"/"+QFileInfo(fileName).completeBaseName()+".zip",
            tr("Zip File (*.zip)")
            );
}

bool Zipper::makeZip(QString fileName, QStringList files)
{
    if(fileName.isEmpty())
    {
        QMessageBox::critical(
                    0,tr("No Spin file"),
                    tr("Can't \"Zip\" from an empty file.")+"\n"+
                    tr("Please create a new file or open an existing one."),
                    QMessageBox::Ok);
        return false;
    }

    QString dstName = getZipDestination(fileName);
    if (dstName.length() < 1) {
        return false;
    }
    QString foldername = QFileInfo(dstName).completeBaseName();

    ZipWriter zip(dstName);
    if(!zip.isWritable())
    {
        QMessageBox::critical(
                0,
                tr("Zip File Error"),
                tr("Can't create zip file: %1").arg(dstName)
                );
        return false;
    }

    foreach(QString f, files)
    {
        if(QFile::exists(f))
        {
            QFile file(f);
            if(file.open(QFile::ReadOnly))
            {
                zip.addFile(foldername+"/"+QFileInfo(f).fileName(), file.readAll());
                file.close();
            }
        }
        else
        {
            QMessageBox::critical(
                    0,
                    tr("Zip File Error"),
                    tr("Can't add file to zip archive: %1").arg(f)
                    );
            zip.close();
            return false;
        }
    }
    zip.close();
    return true;
}
