#include "ReferenceTree.h"


ReferenceTree::ReferenceTree(
        const QString & name,
        ColorScheme::Color colorkey,
        QWidget *parent) :
    QTreeView(parent)
{
    this->setToolTip(name);

    color = colorkey;
    currentTheme = &Singleton<ColorScheme>::Instance();
}

ReferenceTree::~ReferenceTree()
{
}

//void ReferenceTree::rebuildTree(QString fileName, QString text)
//{
//    QString s = this->shortFileName(fileName);
//    basicPath = filePathName(fileName);
//
//    // our startup strategy should change so that we have a tree and it starts collapsed.
//
//    if(referenceModel != NULL) {
//        delete referenceModel;
//    }
//    if(fileName.endsWith(".spin",Qt::CaseInsensitive)) {
//        referenceModel = new TreeModel(s, this);
//        if(text.length() > 0) {
//            rebuildSpinTree(fileName, spinIncludes, "", 0); // start at top object
//        }
//    }
//    else {
//        referenceModel = new TreeModel(s, this);
//    }
//
//    referenceTree->setWindowTitle(s);
//    referenceTree->setModel(referenceModel);
//    referenceTree->show();
//}
//
//void ReferenceTree::rebuildSpinTree(QString fileName, QString includes, QString objname, int level)
//{
//    QString path = filePathName(fileName);
//
//    QStringList mlist = spinParser.spinMethods(fileName, objname);
//
//    // move objects to end of the list
//    for(int n = mlist.count()-1; n > -1; n--) {
//        if(mlist[n].at(0) == 'o') {
//            mlist.insert(mlist.count(),mlist[n]);
//            mlist.removeAt(n);
//        }
//    }
//    // display all
//    for(int n = 0; n < mlist.count(); n ++) {
//        QString s = mlist[n];
//
//        if(s.at(0) == 'o') {
//
//            /* get the file name */
//            QString file = s.mid(s.indexOf(":")+1);
//            file = file.mid(0, file.indexOf("\t"));
//            file = file.trimmed();
//            if(file.startsWith("\""))
//                file = file.mid(1);
//            if(file.endsWith("\""))
//                file = file.mid(0,file.length()-1);
//            file = file.trimmed();
//            if(file.endsWith(".spin",Qt::CaseInsensitive) == false)
//                file += ".spin";
//
//            /* prepend path info to new file if found*/
//            if(QFile::exists(file)) {
//                referenceModel->addRootItem(file, file);
//            }
//            else if(QFile::exists(path+file)) {
//                referenceModel->addRootItem(file, path+file);
//                file = path+file;
//            }
//            else if(QFile::exists(includes+file)) {
//                referenceModel->addRootItem(file, includes+file);
//                file = includes+file;
//            }
//            else {
//                qDebug() << "updateSpinReferenceTree can't find file" << file;
//            }
//
//            /* get the object name */
//            QString obj = s.mid(s.indexOf("\t")+1);
//            obj = obj.mid(0,obj.indexOf(":"));
//            obj = obj.trimmed();
//
//            updateSpin(file, includes, obj, level+1);
//        }
//        else {
//            int line = 0;
//            QString sl = s.mid(s.lastIndexOf("\t")+1);
//            line = sl.toInt();
//            s = s.mid(s.indexOf("\t")+1);
//            s = s.mid(0,s.indexOf("\t"));
//            if(s.indexOf(":") != -1)
//                s = s.mid(0, s.indexOf(":"));
//            if(s.indexOf("|") != -1)
//                s = s.mid(0, s.indexOf("|"));
//            s = s.trimmed();
//
//            referenceModel->addSymbolInfo(s, fileName, line);
//
//            for(int n = 0; n < level; n++)
//                s = "    "+s;
//            //methods.append(s);
//            referenceModel->addRootItem(s, fileName);
//        }
//    }
//}


void ReferenceTree::updateColors()
{
    QFont font = this->font();
    font.setItalic(true);
    this->setFont(font);

    QPalette p = this->palette();
    p.setColor(QPalette::Text, currentTheme->getColor(ColorScheme::SyntaxText));
    p.setColor(QPalette::Base, currentTheme->getColor(color));
    this->setPalette(p);
}

void ReferenceTree::updateFonts()
{
    this->setFont(currentTheme->getFont());
}
