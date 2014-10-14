#include <QApplication>
#include <qdebug.h>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#if defined(Q_OS_WIN32)
    QStringList styles = QStyleFactory::keys();
    qDebug() << "Available window styles" << styles;
    if(styles.contains("Fusion")) {
        QApplication::setStyle("Fusion");
    }
#endif
    a.setWindowIcon(QIcon(":/images/blks.png"));
    MainWindow w;
    w.init();
    if(argc > 1) {
        QString s = QString(argv[1]);
        if(s.contains(QDir::toNativeSeparators(QDir::tempPath())) &&
           s.contains(".zip",Qt::CaseInsensitive)) {
            QMessageBox::critical(&w, w.tr("Cannot Open from Zip"),
                w.tr("The file is in a zipped archive. Unzip to")+"\n"+
                w.tr("a folder first, and open from there instead."));
        }
        else {
            s = s.mid(s.lastIndexOf("."));
            w.openFile(QString(argv[1]));
        }
    }
    w.show();
    return a.exec();
}
