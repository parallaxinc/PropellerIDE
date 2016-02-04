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
#include <QDateTime>

#include "logging.h"

#include "mainwindow.h"

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
        case QtDebugMsg:
            fprintf(stderr, "[\033[1;32mDEBUG\033[0m] \033[0;34m%s\033[0m: %s\n", context.category, localMsg.constData());
            break;
#ifdef QtInfoMsg
        case QtInfoMsg:
            fprintf(stderr, "[\033[1;32mINFO \033[0m] \033[0;34m%s\033[0m: %s\n", context.category, localMsg.constData());
            break;
#endif
        case QtWarningMsg:
            fprintf(stderr, "[\033[1;33mWARN \033[0m] \033[0;34m%s\033[0m: %s\n", context.category, localMsg.constData());
            break;
        case QtCriticalMsg:
            fprintf(stderr, "[\033[1;31mERROR\033[0m] \033[0;34m%s\033[0m: %s\n", context.category, localMsg.constData());
            break;
        case QtFatalMsg:
            fprintf(stderr, "[\033[1;31mFATAL\033[0m] \033[0;34m%s\033[0m: %s\n", context.category, localMsg.constData());
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

#ifndef Q_OS_WIN32
    qInstallMessageHandler(messageHandler);
#endif

    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Parallax");
    QCoreApplication::setOrganizationDomain("www.parallax.com");
    QCoreApplication::setApplicationName("PropellerIDE");

#ifdef VERSION
    QCoreApplication::setApplicationVersion(VERSION);
#else
    QCoreApplication::setApplicationVersion("0.0.0");
#endif

    QString description = QObject::tr("An easy-to-use, cross-platform IDE for the Parallax Propeller");

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("Source files", QObject::tr("Source files to open."), "OBJECTS...");
    parser.setApplicationDescription("\n" + description);
    parser.process(app);

    qCDebug(logmain) << qPrintable(QCoreApplication::applicationName())
                     << qPrintable(QString("v"+QCoreApplication::applicationVersion()))
                     << "-"
                     << qPrintable(description);

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
//    qDebug() << "Available window styles" << styles;
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
