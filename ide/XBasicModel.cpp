/*
    SpinModel.cpp - Provides a simple Spin tree model.
*/

#include "qtversion.h"

#include "treemodel.h"
#include "properties.h"
#include "XBasicModel.h"

XBasicModel::XBasicModel(const QString name, QObject *parent) : TreeModel(name, parent)
{
}

XBasicModel::~XBasicModel()
{
    if(rootItem)
        delete rootItem;
}


void XBasicModel::includes(QString filePath, QString incPath, QString text)
{
    QString inc, cap, s;
    QStringList st = text.split('\n');
    QRegExp rx("(include) ([^\n]*)");

    rx.setCaseSensitivity(Qt::CaseInsensitive);

    int len = st.length();
    for(int n = 0; n < len; n++)
    {
        s = st.at(n);
        int gotit = rx.indexIn(s);
        if(gotit > -1) {
            QList<QVariant> clist;
            inc = rx.cap(1);
            cap = rx.cap(2);
            if(cap != "" && cap.indexOf("\"") > -1) {
                QStringList caps = cap.split("\"");
                cap = caps.at(1);
                cap = cap.trimmed();
                clist << cap;
                if(!isDuplicate(rootItem, cap))
                    rootItem->appendChild(new TreeItem(clist, rootItem));
            }

            QString newPath = filePath.mid(0,(filePath.lastIndexOf("/")+1))+cap;
            QString newInc = incPath+cap;
            if(QFile::exists(newPath) == true)
            {
                QString filename = newPath;
                QFile myfile(filename);
                if (myfile.open(QFile::ReadOnly | QFile::Text))
                {
                    text = myfile.readAll();
                    myfile.close();
                    includes(filename, incPath, text);
                }
            }
            else if(QFile::exists(newInc) == true)
            {
                QString filename = newInc;
                QFile myfile(filename);
                if (myfile.open(QFile::ReadOnly | QFile::Text))
                {
                    text = myfile.readAll();
                    myfile.close();
                    includes(filename, incPath, text);
                }
            }
        }
    }
}

void XBasicModel::addFileReferences(QString filePath, QString incPath, QString text, int level)
{
    QString inc, cap, s;
    QStringList st = text.split('\n');
    QRegExp rx("(include) ([^\n]*)");
    rx.setCaseSensitivity(Qt::CaseInsensitive);

    QRegExp dx("def ([^\n]*)");
    dx.setCaseSensitivity(Qt::CaseInsensitive);

    int len = st.length();
    for(int n = 0; n < len; n++)
    {
        s = st.at(n);
        int gotdef = dx.indexIn(s);

        if(gotdef > -1) {
            QList<QVariant> clist;
            inc = dx.cap(0);
            if(level) {
                for(int n = level; n > 0; n--)
                    clist << "  ";
            }
            QString t = "";
            if(level) {
                for(int n = level; n > 0; n--)
                    t += "  ";
            }
            clist << t+inc;
            if(!isDuplicate(rootItem, inc))
                rootItem->appendChild(new TreeItem(clist, rootItem, filePath));
        }

        int gotit = rx.indexIn(s);
        if(gotit > -1) {
            QList<QVariant> clist;
            inc = rx.cap(1);
            cap = rx.cap(2);
            if(cap != "" && cap.indexOf("\"") > -1) {
                QStringList caps = cap.split("\"");
                cap = caps.at(1);
                cap = cap.trimmed();
                clist << cap;
                if(!isDuplicate(rootItem, cap))
                    rootItem->appendChild(new TreeItem(clist, rootItem));
            }

            QString newPath = filePath.mid(0,(filePath.lastIndexOf("/")+1));
            QFile file;
            if(file.exists(newPath+cap))
            {
                QString filename = newPath+cap;
                QFile myfile(filename);
                if (myfile.open(QFile::ReadOnly | QFile::Text))
                {
                    text = myfile.readAll();
                    myfile.close();
                    addFileReferences(filename, incPath, text, level+1);
                }
            }
            else if(file.exists(incPath+cap))
            {
                QString filename = incPath+cap;
                QFile myfile(filename);
                if (myfile.open(QFile::ReadOnly | QFile::Text))
                {
                    text = myfile.readAll();
                    myfile.close();
                    addFileReferences(filename, incPath, text, level+1);
                }
            }
        }
    }
}


