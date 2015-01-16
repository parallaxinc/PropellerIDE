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

    app.setWindowIcon(QIcon(":/icons/PropHatAlpha.png"));
    QCoreApplication::setOrganizationName("Parallax");
    QCoreApplication::setOrganizationDomain("www.parallax.com");
    QCoreApplication::setApplicationVersion(VERSION);
    QCoreApplication::setApplicationName("PropellerIDE");


    // init translations
//    QTranslator translator;
//    translator.load("translations/propelleride_zn");
//    app.installTranslator(&translator);

    // init styles
#if defined(Q_OS_WIN32)
    QStringList styles = QStyleFactory::keys();
    qDebug() << "Available window styles" << styles;
    if(styles.contains("Fusion")) {
        QApplication::setStyle("Fusion");
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
    w.init();

    const QStringList args = parser.positionalArguments();
    for (int i = 0; i < args.size(); i++)
    {
        qDebug() << args.at(i);
        w.openFile(args.at(i));
    }

    w.show();
    return app.exec();
}
