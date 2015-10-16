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
#include <QtGlobal>

#include "mainwindow.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
        case QtDebugMsg:
            fprintf(stderr, "[%s]: %s\n", context.function, localMsg.constData());
            break;
        case QtWarningMsg:
            fprintf(stderr, "[WARNING]: (%s:%u, %s): %s\n", context.file, context.line, context.function, localMsg.constData());
            break;
        case QtCriticalMsg:
            fprintf(stderr, "[CRITICAL]: (%s:%u, %s): %s\n", context.file, context.line, context.function, localMsg.constData());
            break;
        case QtFatalMsg:
            fprintf(stderr, "[FATAL]: (%s:%u, %s): %s\n", context.file, context.line, context.function, localMsg.constData());
            abort();
    }
}

void updateSplash(QSplashScreen * splash, const QString & text)
{
    QString t = QObject::tr("Version %1\n").arg(qApp->applicationVersion());
    t += text;
    splash->showMessage(t,Qt::AlignLeft | Qt::AlignBottom, Qt::white);
    splash->repaint();
    qApp->processEvents();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Parallax");
    QCoreApplication::setOrganizationDomain("www.parallax.com");
#ifdef VERSION
    QCoreApplication::setApplicationVersion(VERSION);
#else
    QCoreApplication::setApplicationVersion("DEV");
#endif
    QCoreApplication::setApplicationName("PropellerIDE");

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("Source files", QObject::tr("Source files to open."), "OBJECTS...");
    parser.setApplicationDescription(
            "\n"+qApp->applicationName()
+ QObject::tr(" is an easy-to-use, cross-platform development tool for the\n\
Parallax Propeller microcontroller.\n\n\
Write code, download programs to your Propeller board, and\n\
debug your applications with the built-in serial terminal.")
           );
    parser.process(app);


    QPixmap pixmap(":/icons/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    updateSplash(&splash, QObject::tr("Loading translations..."));

    // init translations
//    QTranslator translator;
//    translator.load("translations/propelleride_fake");
//    app.installTranslator(&translator);

    updateSplash(&splash, QObject::tr("Loading styles..."));

    // init styles
#if defined(Q_OS_WIN32)
    QStringList styles = QStyleFactory::keys();
    qDebug() << "Available window styles" << styles;
    if(styles.contains("WindowsVista")) {
        QApplication::setStyle("WindowsVista");
    }
#endif

    updateSplash(&splash, QObject::tr("Loading fonts..."));

    // init fonts
    QDirIterator it(":/fonts", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QFontDatabase::addApplicationFont(it.next());
    }

    updateSplash(&splash, QObject::tr("Loading editor..."));

    MainWindow w;

    updateSplash(&splash, QObject::tr("Loading previous session..."));

    w.openFiles(parser.positionalArguments());

    w.show();
    splash.finish(&w);
    return app.exec();
}
