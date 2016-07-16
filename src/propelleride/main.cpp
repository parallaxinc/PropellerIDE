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
bool initCompilers();
bool initTranslations();
bool initStyles();
bool initFonts();

QSplashScreen * splash;

QCommandLineParser parser;

QCommandLineOption optLogFile(QStringList() << "f" << "file",
                              QObject::tr("Log debug messages to FILE"),
                              "FILE");

QCommandLineOption optPath(QStringList() << "p" << "path",
                           QObject::tr("Add PATH to list of search paths for external programs"),
                           "PATH");

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qInstallMessageHandler(messageHandler);

    qApp->setOrganizationName("Parallax");
    qApp->setOrganizationDomain("www.parallax.com");
    qApp->setApplicationName("PropellerIDE");

#ifdef VERSION
    qApp->setApplicationVersion(VERSION);
#else
    qApp->setApplicationVersion("0.0.0 (dev)");
#endif

    QString description = QObject::tr("An easy-to-use, cross-platform IDE for the Parallax Propeller");

    parser.setApplicationDescription(qApp->applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(optLogFile);
    parser.addOption(optPath);
    parser.addPositionalArgument("Source files", QObject::tr("Source files to open."), "OBJECTS...");
    parser.setApplicationDescription("\n" + description);
    parser.process(app);

    if (!parser.value(optLogFile).isEmpty())
    {
        QString logfile = parser.value(optLogFile);
        setLogFileName(logfile);
    }

    QSettings settings;
    if (settings.status() == QSettings::AccessError)
    {
        QMessageBox::critical(0,
                              QObject::tr("Can't open application settings!"),
                              QObject::tr("Unable to open the PropellerIDE settings stored at:\n\n%1\n\nTry deleting the file and restarting PropellerIDE.").arg(settings.fileName()));
        qCritical() << "can't access:" << settings.fileName() << ". Is it writable?";
        return 1;
    }

    printDebugInfo();

    QPixmap pixmap(":/icons/splash.png");
    splash = new QSplashScreen(pixmap);

    splash->show();
    app.processEvents();

    initLanguages();
    initCompilers();
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
    qDebug() << qPrintable(qApp->applicationName())
             << "-"
             << qPrintable(parser.applicationDescription().remove("\n"));

    qDebug() << "Version:"
             << qPrintable(qApp->applicationVersion());

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

bool initCompilers()
{
    updateSplash(splash, QObject::tr("Loading compilers..."));

    Language language;
    QDirIterator it(":/config", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString file = it.next();
        QString filename = QFileInfo(file).fileName();

        if (!filename.startsWith("compiler."))
            continue;

        filename.remove("compiler.");
        ExternalCompiler::add(filename, file);
    }

    foreach (QString path, parser.values(optPath))
    {
        if (QDir(path).exists())
        {
            ExternalCompiler::addPath(path);
        }
        else
        {
            qWarning() << "Path provided with '-p' does not exist:"
                        << path;
        }
    }

    ExternalCompiler::addPath(QCoreApplication::applicationDirPath());

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
