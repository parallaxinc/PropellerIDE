#include <QApplication>
#include <QDebug>
#include "mainwindow.h"

#include <QMessageBox>
#include <QStyleFactory>
#include <QTranslator>
#include <QCommandLineParser>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Parallax");
    QCoreApplication::setOrganizationDomain("www.parallax.com");
#ifdef VERSION
    QCoreApplication::setApplicationVersion(VERSION);
#endif
    QCoreApplication::setApplicationName("PropellerIDE");


    // init translations
//    QTranslator translator;
//    translator.load("translations/propelleride_fake");
//    app.installTranslator(&translator);

    // init styles
#if defined(Q_OS_WIN32)
    QStringList styles = QStyleFactory::keys();
    qDebug() << "Available window styles" << styles;
    if(styles.contains("WindowsVista")) {
        QApplication::setStyle("WindowsVista");
    }
#endif

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

    MainWindow w;
    w.openFiles(parser.positionalArguments());
    w.show();
    return app.exec();
}
