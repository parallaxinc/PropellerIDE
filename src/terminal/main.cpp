#include <QApplication>

#include "propterm.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PropTerm w;
    w.show();
    return a.exec();
}
