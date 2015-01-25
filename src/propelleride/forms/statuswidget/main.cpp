
#include <QApplication>
#include <QDebug>
#include "status.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    StatusDialog * widget = new StatusDialog;
    widget->show();

    return app.exec();
}
