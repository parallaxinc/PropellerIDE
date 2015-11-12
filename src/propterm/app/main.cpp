#include <QApplication>

#include <PropTerm>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PropellerManager manager;
    manager.enablePortMonitor(true);
    PropTerm w(&manager);
    w.show();
    return a.exec();
}
