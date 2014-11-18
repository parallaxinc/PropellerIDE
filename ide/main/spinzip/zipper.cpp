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

bool Zipper::makeZip(QString fileName, QStringList fileTree, QString libPath, StatusDialog *stat)
{
    statusDialog = stat;
    spinLibPath = libPath;
    zipSpinProjectTree(fileName, fileTree);
    return true;
}

QString Zipper::getZipDestination(QString fileName)
{
    /*
     * ask the user for destination ... start within existing location
     */
    QString dstName     = fileName;
    dstName             = dstName.mid(0,dstName.lastIndexOf("."));

    /* Enumerate user input until it's unique */
    int num = 0;
    QString serial("");
    if(QFile::exists(dstName+serial+".zip")) {
        do {
            num++;
            serial = QString("(%1)").arg(num);
        } while(QFile::exists(dstName+serial+".zip"));
    }

    QFileDialog dialog;
    QString afilter("Zip File (*.zip)");

    QString dstPath = dialog.getSaveFileName(0, tr("Zip"), dstName+serial+".zip", afilter);
    return dstPath;
}

void Zipper::zipSpinProjectTree(QString fileName, QStringList fileTree)
{
    /*
     * method assumes that fileName is the top of the Spin tree
     * - source files come from local source & library directory
     */
    if(fileName.isEmpty())              /* no empty .zip files, please! */
    {
        QMessageBox::critical(
                    0,tr("No Spin file"),
                    tr("Can't \"Zip\" from an empty file.")+"\n"+
                    tr("Please create a new file or open an existing one."),
                    QMessageBox::Ok);
        return;
    }

    QString spinCodePath    = filePathName(fileName);
    QDir spinPath(spinCodePath);

    if(spinPath.exists() == false) {
        QMessageBox::critical(
                0,tr("Spin folder not Found."),
                tr("Can't \"Zip\" from a non-existing folder."),
                QMessageBox::Ok);
        return;
    }

    /*
     * create a new archive (zip) file
     */
    QString dstName     = getZipDestination(fileName);
    if (dstName.length() < 1) {
        return;
    }
    QString sfile = shortFileName(dstName);
    qDebug() << "dstName: " << sfile;
    statusDialog->init("Zipping Spin", sfile);

    QString source = sfile.left(sfile.indexOf(".",Qt::CaseInsensitive));

    ZipWriter zip(dstName);
    if(!zip.isWritable()) {
        QMessageBox::critical(0, tr("Zip File Error"),
            tr("Can't create zip file")+"\n"+dstName);
        return;
    }
    foreach(QString entry, fileTree) {
        QString name;
        if(QFile::exists(spinCodePath+"/"+entry)) {
            name = spinCodePath+"/"+entry;
        }
        else if(QFile::exists(spinLibPath+"/"+entry)) {
            name = spinLibPath+"/"+entry;
        }
        else {
            QMessageBox::critical(0, tr("Zip File Error"),
                tr("Can't add to zip file")+"\n"+entry);
            zip.close();
            return;
        }
        QFile file(name);
        if(file.open(QFile::ReadOnly)) {
            zip.addFile(source+"/"+entry, file.readAll());
            file.close();
        }
    }
    zip.close();

    statusDialog->stop(4);
}

QStringList Zipper::directoryTreeList(QString folder)
{
    QStringList list;
    QDir dir(folder);
    foreach(QString entry, dir.entryList(QDir::AllEntries, QDir::DirsFirst)) {
        if(entry.compare(".") == 0) continue;
        if(entry.compare("..") == 0) continue;
        QString name = dir.path()+"/"+entry;
        if(QFile::exists(name) && !QFileInfo(name).isFile()) {
            if(entry.endsWith("/") == false)
                entry.append("/");
            list.append(entry);
            QStringList dlist = directoryTreeList(name);
            foreach(QString s, dlist)
                list.append(entry+s);
        }
        else {
            list.append(entry);
        }
    }
    return list;
}

bool Zipper::createFolderZip(QString source, QString dstZipFile)
{
    ZipWriter zip(dstZipFile);
    if(!zip.isWritable())
        return false;

    QStringList list = directoryTreeList(source);
    foreach(QString entry, list) {
        if(entry.compare(".") == 0) continue;
        if(entry.compare("..") == 0) continue;
        if(entry.endsWith("/")) {
            zip.addDirectory(entry);
        } else {
            QFile file(source+"/"+entry);
            if(file.open(QFile::ReadOnly)) {
                zip.addFile(entry, file.readAll());
                file.close();
            }
        }
    }
    zip.close();
    return true;
}

QString Zipper::filePathName(QString fileName)
{
    QString rets;
    if(fileName.lastIndexOf("/") > -1)
        rets = fileName.mid(0,fileName.lastIndexOf("/")+1);
    else
        rets = fileName;
    return rets;
}

QString Zipper::shortFileName(QString fileName)
{
    QString rets;
    if(fileName.indexOf("/") > -1)
        rets = fileName.mid(fileName.lastIndexOf("/")+1);
    else
        rets = fileName;
    return rets;
}

