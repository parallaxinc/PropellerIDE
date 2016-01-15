#include "pathselector.h"

PathSelector::PathSelector(QWidget *parent)
: QWidget(parent)
{
    ui.setupUi(this);

//    connect(browseButton, SIGNAL(clicked()), parent, slot);
//
//    lineEdit->setText(defaultpath);
//
//    QSettings settings;
//    settings.beginGroup("Paths");
//    if ( settings.contains(key) )
//    {
//        load();
//    }
//    else
//    {
//        save();
//    }
//    settings.endGroup();
//
//    oldvalue = lineEdit->text();
}

PathSelector::~PathSelector()
{
}

//void PathSelector::browsePath(
//        QString const & pathlabel, 
//        QString const & pathregex,  
//        bool isfolder
//        )
//{
//    QString folder = lineEdit->text();
//    QString s;
//
//    if (isfolder) 
//        s = QFileDialog::getExistingDirectory(this,
//                pathlabel, folder, QFileDialog::ShowDirsOnly);
//    else
//        s = QFileDialog::getOpenFileName(this,
//                pathlabel, folder, pathregex);
//
//    qDebug() << "browsePath(" << pathlabel << "): " << s;
//
//    if(s.length() == 0)
//    {
//        return;
//    }
//    lineEdit->setText(s);
//}
//
//void PathSelector::restore()
//{
//    lineEdit->setText(oldvalue);
//}
//
//void PathSelector::save()
//{
//    QSettings settings;
//    settings.beginGroup("Paths");
//    settings.setValue(key,lineEdit->text());
//    settings.endGroup();
//}
//
//void PathSelector::load()
//{
//    QSettings settings;
//    settings.beginGroup("Paths");
//    QString s = settings.value(key, defaultpath).toString();
//    settings.endGroup();
//
//    if(!s.isEmpty())
//    {
//        lineEdit->setText(s);
//    }
//    else
//    {
//        lineEdit->setText(defaultpath);
//    }
//}
