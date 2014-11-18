/*
    SpinModel.cpp - Provides a simple Spin tree model.
*/

#include <QtWidgets>

#include "treemodel.h"
#include "Preferences.h"
#include "SpinModel.h"

SpinModel::SpinModel(const QString name, QObject *parent) : TreeModel(name, parent)
{
}

SpinModel::~SpinModel()
{
    symbolHash.clear();
}

// section heads not used at the moment
bool SpinModel::isSectionHead(QString s, QString nocmp)
{
    QRegExp con("^[ ]*(con) ([^\n]*)");
    QRegExp con2("^[ ]*(con)([^\n]*)");
    QRegExp obj("^[ ]*(obj) ([^\n]*)");
    QRegExp obj2("^[ ]*(obj)([^\n]*)");
    QRegExp var("^[ ]*(var) ([^\n]*)");
    QRegExp var2("^[ ]*(var)([^\n]*)");
    QRegExp pub("^[ ]*(pub) ([^\n]*)");
    QRegExp pri("^[ ]*(pri) ([^\n]*)");
    QRegExp dat("^[ ]*(dat) ([^\n]*)");
    QRegExp dat2("^[ ]*(dat)([^\n]*)");

    con.setCaseSensitivity(Qt::CaseInsensitive);
    con2.setCaseSensitivity(Qt::CaseInsensitive);
    obj.setCaseSensitivity(Qt::CaseInsensitive);
    obj2.setCaseSensitivity(Qt::CaseInsensitive);
    var.setCaseSensitivity(Qt::CaseInsensitive);
    var2.setCaseSensitivity(Qt::CaseInsensitive);
    pub.setCaseSensitivity(Qt::CaseInsensitive);
    pri.setCaseSensitivity(Qt::CaseInsensitive);
    dat.setCaseSensitivity(Qt::CaseInsensitive);
    dat2.setCaseSensitivity(Qt::CaseInsensitive);

    if(!s.contains(nocmp,Qt::CaseInsensitive)) {
        if((con.indexIn(s) & con2.indexIn(s)) > -1) {
            return true;
        }
        else
        if((obj.indexIn(s) & obj2.indexIn(s)) > -1) {
            return true;
        }
        else
        if((var.indexIn(s) & var2.indexIn(s)) > -1) {
            return true;
        }
        else
        if((dat.indexIn(s) & dat2.indexIn(s)) > -1) {
            return true;
        }
        else
        if(pub.indexIn(s) > -1) {
            return true;
        }
        else
        if(pri.indexIn(s) > -1) {
            return true;
        }
    }
    return false;
}

QString SpinModel::removeComments(QString text)
{
    QString rs = text;
    QString s;
    QRegExp cx("(\\{\\{)");

    int pos = -1;
    while((pos = cx.indexIn(rs)) > -1) {
        int pos2 = pos;
        pos2 = QRegExp("\\}\\}").indexIn(rs, pos2);
        if(pos2 > pos) {
            rs = rs.remove(pos,pos2-pos+2);
        }
    }

    QRegExp cx2("(\\{)");
    while((pos = cx2.indexIn(rs)) > -1) {
        int pos2 = pos;
        pos2 = QRegExp("\\}").indexIn(rs, pos2);
        if(pos2 > pos) {
            rs = rs.remove(pos,pos2-pos+1);
        }
    }

    QRegExp cx3("'[^\r^\n]*");
    while(cx3.indexIn(rs) > -1) {
        s = rs;
        s = s.remove(cx3);
        rs = s;
    }
    return rs;
}

// includes not used at the moment
void SpinModel::includes(QString filePath, QString incPath, QString text)
{
    QRegExp rx("(obj)([^\r^\n]*)");
    QRegExp rx2("(obj)([^\r^\n]*)");

    QString inc;
    QString cap;
    QString s;
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    rx2.setCaseSensitivity(Qt::CaseInsensitive);

    text = removeComments(text);
    QStringList st = text.split('\n');

    int len = st.length();
    int gotit = -1;

    for(int n = 0; n < len; n++)
    {
        s = st.at(n);

        if(gotit > -1) {
            if(isSectionHead(s, "obj")) {
                gotit = -1;
            }
            else {
                //qDebug() << "obj " << s;
            }
        }
        else {
            gotit = rx.indexIn(s) & rx2.indexIn(s);
        }

        if(gotit > -1) {
            QList<QVariant> clist;
            inc = rx.cap(1);
            cap = rx.cap(2);
            if(cap != "" && cap.indexOf("\"") > -1) {
                QStringList caps = cap.split("\"");
                cap = caps.at(1);
                cap = cap.trimmed();
                clist << cap;
                if(!isDuplicate(rootItem, cap)) {
                    rootItem->appendChild(new TreeItem(clist, rootItem));
                }
            }
            else {
                if(s.indexOf(":") > -1) {
                    s = s.mid(s.indexOf(":")+1);
                    if(s.indexOf("\"") > -1) {
                        s = s.mid(s.indexOf("\"")+1);
                        if(s.indexOf("\"") > -1) {
                            s = s.mid(0,s.indexOf("\""));
                        }
                    }
                    s = s.trimmed();
                    if(s.length() > 0) {
                        if(!s.endsWith(".spin", Qt::CaseInsensitive)) {
                            s += ".spin";
                        }
                        clist << s;
                        if(!isDuplicate(rootItem, s)) {
                            rootItem->appendChild(new TreeItem(clist, rootItem));
                        }
                    }
                    cap = s;
                }
            }

            QString newPath = filePath.mid(0,(filePath.lastIndexOf("/")+1))+cap;
            QString newInc = incPath+cap;
            if(QFile::exists(newPath) == true)
            {
                QString filename = newPath;
                QFile myfile(filename);
                if (myfile.open(QFile::ReadOnly | QFile::Text))
                {
                    QTextStream in(&myfile);
                    text = in.readAll();
                    //text = myfile.readAll();
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
                    QTextStream in(&myfile);
                    text = in.readAll();
                    //text = myfile.readAll();
                    myfile.close();
                    includes(filename, incPath, text);
                }
            }
        }
    }
}

void SpinModel::addObjects(QString filePath, QString incPath, QString text, int level)
{
    QRegExp endline("\r\n|\n|\r|\n\r");
    QStringList st = text.split(endline, QString::SkipEmptyParts);
    QString s;
    QString cap;

    int len = st.length();

    for(int n = 0; n < len; n++)
    {
        s = st.at(n);

        if(s.indexOf(":") > -1) {
            QList<QVariant> clist;
            s = s.mid(s.indexOf(":")+1);
            if(s.indexOf("\"") > -1) {
                s = s.mid(s.indexOf("\"")+1);
                if(s.indexOf("\"") > -1) {
                    s = s.mid(0,s.indexOf("\""));
                }
            }
            s = s.trimmed();
            if(s.length() > 0) {
                if(!s.endsWith(".spin", Qt::CaseInsensitive)) {
                    s += ".spin";
                }
                QString t = "";
                if(level) {
                    for(int n = level; n > 0; n--)
                        t += "  ";
                }
                clist << t+s;
                if(!isDuplicate(rootItem, s)) {
                    rootItem->appendChild(new TreeItem(clist, rootItem));
                }
            }
            cap = s;

            QString newPath = filePath.mid(0,(filePath.lastIndexOf("/")+1));
            QFile file;
            if(file.exists(newPath+cap))
            {
                QString filename = newPath+cap;
                QFile myfile(filename);
                if (myfile.open(QFile::ReadOnly))
                {
                    QTextStream in(&myfile);
                    text = in.readAll();
                    myfile.close();
                    addFileReferences(filename, incPath, text, level+1);
                }
            }
            else if(file.exists(incPath+cap))
            {
                QString filename = incPath+cap;
                QFile myfile(filename);
                if (myfile.open(QFile::ReadOnly))
                {
                    QTextStream in(&myfile);
                    text = in.readAll();
                    myfile.close();
                    addFileReferences(filename, incPath, text, level+1);
                }
            }

        }
    }

}

void SpinModel::addFileReferences(QString filePath, QString incPath, QString text, int level)
{
    QRegExp endline("\r\n|\n|\r|\n\r");
    QStringList lines = text.split(endline);

    text = removeComments(text);

    QRegExp sections("(obj|con|dat|pub|pri|var)[ \t\r\n]");
    //QRegExp sections("obj|con|dat|pub|pri|var");
    sections.setCaseSensitivity(Qt::CaseInsensitive);

    QRegExp dx("(pub|pri) ([^\n]*)");
    dx.setCaseSensitivity(Qt::CaseInsensitive);

    int start = text.indexOf(sections);
    int length = 0;
    int malen;
    QString s;
    QString sec;

    while(start > -1 && length > -1) {
        malen = sections.matchedLength();
        length = text.indexOf(sections,start+malen);
        if(length < 0)
            s = text;
        else
            s = text.mid(start,length-start);
        int spos = s.indexOf(sections);
        if(spos > -1) {
            sec = s.mid(spos,3);
            if(sec.compare("obj",Qt::CaseInsensitive) == 0) {
                addObjects(filePath, incPath, s.mid(3), level);
            }
            else {
                /*
                 * Some spin code has lots of "." coding. Bad coding practice, but is valid.
                 * The parser spends lots of time on that, so we have a special
                 * case to handle that better here.
                 */
                QRegExp qt1("\\\".\\\"");
                while(qt1.indexIn(text) > -1) {
                    text = text.remove(qt1);
                }

                /*
                 * Handle longer strings here.
                 */
                if(s.contains("\"")) {
                    QRegExp cx2("(\\\".*)");
                    int pos, pos2;
                    while((pos = cx2.indexIn(text)) > -1) {
                        pos2 = pos;
                        pos2 = QRegExp("\\\"").indexIn(text, pos2+1);
                        if(pos2 > pos) {
                            text = text.remove(pos,pos2-pos+1);
                            break;
                        }
                    }
                    start = text.indexOf(sections);
                    malen = sections.matchedLength();
                    length = text.indexOf(sections,start+malen);

                    // do search again if more to do
                    if(length > 0)
                        continue;
                }

                QString inc;
                if(text.indexOf(dx) == 0) {
                    QRegExp methx("[:|\r\n]");
                    QList<QVariant> clist;
                    inc = dx.cap(0);
                    if(inc.indexOf(methx) > -1) {
                        inc = inc.mid(0, inc.indexOf(methx));
                    }
                    inc = inc.trimmed();
                    QString t = "";
                    if(level) {
                        for(int n = level; n > 0; n--)
                            t += "  ";
                    }
                    clist << t+inc;

                    if(!isDuplicate(rootItem, inc)) {
                        TreeItem *item = rootItem->appendChild(new TreeItem(clist, rootItem, filePath));
                        for(int n = 0; n < lines.length(); n++) {
                            if(lines[n].indexOf(inc) > -1) {
                                symbolHash.insert(item->file()+"::"+inc, filePath+QString("::%1").arg(n));
                                break;
                            }
                        }
                    }
                }
            }
        }
        //qDebug() << "cap 0" << sections.cap(0) << "cap 1" << sections.cap(1) << "captured" << s;
        text = text.mid(length);
        start = text.indexOf(sections);
        //qDebug() << "start" << start << "length" << length << s;
    }
}

