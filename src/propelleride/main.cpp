#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QStyleFactory>
#include <QTranslator>
#include <QCommandLineParser>
#include <QObject>
#include <QSplashScreen>
#include <Qt>
#include <QString>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QPixmap pixmap(":/icons/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    QCoreApplication::setOrganizationName("Parallax");
    QCoreApplication::setOrganizationDomain("www.parallax.com");
#ifdef VERSION
    QCoreApplication::setApplicationVersion(VERSION);
#endif
    QCoreApplication::setApplicationName("PropellerIDE");

    splash.showMessage(QObject::tr("Loading translations..."),Qt::AlignLeft | Qt::AlignBottom, Qt::white);
    splash.repaint();
    app.processEvents();

    // init translations
//    QTranslator translator;
//    translator.load("translations/propelleride_fake");
//    app.installTranslator(&translator);

    splash.showMessage(QObject::tr("Loading styles..."),Qt::AlignLeft | Qt::AlignBottom, Qt::white);
    splash.repaint();
    app.processEvents();

    // init styles
#if defined(Q_OS_WIN32)
    QStringList styles = QStyleFactory::keys();
    qDebug() << "Available window styles" << styles;
    if(styles.contains("WindowsVista")) {
        QApplication::setStyle("WindowsVista");
    }
#endif

    splash.showMessage(QObject::tr("Loading fonts..."),Qt::AlignLeft | Qt::AlignBottom, Qt::white);
    splash.repaint();
    app.processEvents();

    // init fonts
    QDirIterator it(":/fonts", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QFontDatabase::addApplicationFont(it.next());
    }

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("Source files", QCoreApplication::translate("main", "Source files to open."), "OBJECTS...");
    parser.setApplicationDescription(
            "\n"+QCoreApplication::applicationName()
+ QObject::tr(" is an easy-to-use, cross-platform development tool for the\n\
Parallax Propeller microcontroller.\n\n\
Write code, download programs to your Propeller board, and\n\
debug your applications with the built-in serial terminal.")
           );
    parser.process(app);

    splash.showMessage(QObject::tr("Loading editor..."),Qt::AlignLeft | Qt::AlignBottom, Qt::white);
    splash.repaint();
    app.processEvents();

    MainWindow w;

    splash.showMessage(QObject::tr("Loading previous session..."),Qt::AlignLeft | Qt::AlignBottom, Qt::white);
    splash.repaint();
    app.processEvents();

    w.openFiles(parser.positionalArguments());

    w.show();
    splash.finish(&w);
    return app.exec();
}
