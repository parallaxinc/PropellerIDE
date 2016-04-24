#include "application.h"

#include <QFileOpenEvent>
#include <QDebug>

Application::Application(int &argc, char **argv) : QApplication(argc,argv)
{
}

Application::~Application()
{
}

bool Application::event(QEvent *event)
{
    switch(event->type())
    {
        case QEvent::FileOpen:
            loadFile( ((QFileOpenEvent *)event)->file() );
            emit fileOpened( ((QFileOpenEvent *)event)->file() );
            return true;
        default:
            return QApplication::event(event);
    }
}

void Application::loadFile(const QString & filename)
{
    qDebug() << "File Name:" << filename;
}
