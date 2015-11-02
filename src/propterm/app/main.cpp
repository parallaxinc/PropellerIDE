#include <QApplication>

#include "propterm.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PropellerManager manager;
    PropTerm w(&manager);
    w.show();
    return a.exec();
}
