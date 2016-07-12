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
#include <QSettings>

#if QT_VERSION >= 0x050400
#include <QSysInfo>
#endif

#include "logging.h"

#include "mainwindow.h"

void updateSplash(QSplashScreen * splash, const QString & text)
{
    QString t = QObject::tr("Version %1\n").arg(qApp->applicationVersion());
    t += text;
    splash->showMessage(t,Qt::AlignLeft | Qt::AlignBottom, Qt::white);
    splash->repaint();
    qApp->processEvents();
}

void printDebugInfo();
bool initLanguages();
bool initTranslations();
bool initStyles();
bool initFonts();

QSplashScreen * splash;
QCommandLineParser parser;

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageHandler);

    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Parallax");
    QCoreApplication::setOrganizationDomain("www.parallax.com");
    QCoreApplication::setApplicationName("PropellerIDE");

#ifdef VERSION
    QCoreApplication::setApplicationVersion(VERSION);
#else
    QCoreApplication::setApplicationVersion("0.0.0 (dev)");
#endif

    QSettings settings;
    if (settings.status() == QSettings::AccessError)
    {
        QMessageBox::critical(0, QObject::tr("Can't open application settings!"), QObject::tr("Unable to open the PropellerIDE settings stored at:\n\n%1\n\nTry deleting the file and restarting PropellerIDE.").arg(settings.fileName()));
        qCCritical(logmain) << "can't access:" << settings.fileName() << ". Is it writable?";
        return 1;
    }

    QString description = QObject::tr("An easy-to-use, cross-platform IDE for the Parallax Propeller");

    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("Source files", QObject::tr("Source files to open."), "OBJECTS...");
    parser.setApplicationDescription("\n" + description);
    parser.process(app);

    printDebugInfo();

    QPixmap pixmap(":/icons/splash.png");
    splash = new QSplashScreen(pixmap);

    splash->show();
    app.processEvents();

    initLanguages();
    initTranslations();
    initStyles();
    initFonts();

    updateSplash(splash, QObject::tr("Loading editor..."));

    MainWindow w;

    updateSplash(splash, QObject::tr("Loading previous session..."));

    w.openFiles(parser.positionalArguments());

    w.show();

    splash->finish(&w);
    delete splash;
    splash = NULL;

    return app.exec();
}


void printDebugInfo()
{
    qDebug() << qPrintable(QCoreApplication::applicationName())
             << "-"
             << qPrintable(parser.applicationDescription().remove("\n"));

    qDebug() << "Version:"
             << qPrintable(QCoreApplication::applicationVersion());

#if QT_VERSION >= 0x050400
    qDebug() << "Arch:" 
             << qPrintable(QSysInfo::buildAbi());

    qDebug() << "OS:"
#if defined(Q_OS_WIN)
             << "Windows,"
#elif defined(Q_OS_MAC)
             << "Mac,"
#elif defined(Q_OS_LINUX)
             << "Linux,"
#else
#error "Unsupported platform"
#endif
             << qPrintable(QSysInfo::prettyProductName());
#endif
}

bool initLanguages()
{
    updateSplash(splash, QObject::tr("Loading languages..."));

    Language language;
    QDirIterator it(":/languages", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString filename = it.next();
        QString name = QFileInfo(filename).baseName();
        language.load(name, filename);
    }

    return true;
}

bool initTranslations()
{
    updateSplash(splash, QObject::tr("Loading translations..."));

//    QTranslator translator;
//    translator.load("translations/propelleride_fake");
//    app.installTranslator(&translator);

    return true;
}

bool initStyles()
{
    updateSplash(splash, QObject::tr("Loading styles..."));

#if defined(Q_OS_WIN32)
    QStringList styles = QStyleFactory::keys();
    if(styles.contains("WindowsVista")) {
        QApplication::setStyle("WindowsVista");
    }

    qDebug() << "Window styles" << styles;
#endif

    return true;
}


bool initFonts()
{
    updateSplash(splash, QObject::tr("Loading fonts..."));

    QDirIterator it(":/fonts", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QFontDatabase::addApplicationFont(it.next());
    }

    return true;
}
