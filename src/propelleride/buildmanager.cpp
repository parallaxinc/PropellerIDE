#include "buildmanager.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QScrollBar>
#include <QThread>

BuildManager::BuildManager(QWidget *parent)
    : QFrame(parent)
{
    compiler = NULL;

    ui.setupUi(this);

    ui.activeText->setText(" ");
    setStage(0);

    connect(&timer, SIGNAL(timeout()),  this,   SLOT(hideStatus()));

    currentTheme = &Singleton<ColorScheme>::Instance();
    updateColors();
}

BuildManager::~BuildManager()
{
    cleanupCompiler();
}

void BuildManager::showStatus()
{
    setStage(0);
    setTextColor(Qt::black);
    show();
    raise();
    timer.stop();
}

void BuildManager::hideStatus()
{
    hide();
}

void BuildManager::waitClose()
{
    timer.setSingleShot(true);
    timer.start(100);
}

void BuildManager::setFont(const QFont & font)
{
    ui.plainTextEdit->setFont(font);
}

void BuildManager::setConfiguration(BuildManager::Configuration config)
{
    this->config = config;
}

void BuildManager::setTextColor(QColor color)
{
    QTextCharFormat tf = ui.plainTextEdit->currentCharFormat();
    tf.setForeground(color);
    ui.plainTextEdit->setCurrentCharFormat(tf);
}

void BuildManager::print(const QString & text)
{
    ui.plainTextEdit->appendPlainText(text);
    fprintf(stdout, "%s", qPrintable(text));
    fflush(stdout);
}

bool BuildManager::load(const QByteArray & binary)
{
    setStage(2);
    setTextColor(Qt::darkYellow);

    PropellerLoader loader(config.manager, config.port);

    connect(&loader,    SIGNAL(success()), this, SLOT(loadSuccess()));
    connect(&loader,    SIGNAL(failure()), this, SLOT(loadFailure()));
    connect(&loader,    SIGNAL(finished()), this, SIGNAL(finished()));

    connect(&loader,            SIGNAL(statusChanged(const QString &)),
            this,               SLOT(setText(const QString &)));

    connect(&loader,            SIGNAL(statusChanged(const QString &)),
            this,               SIGNAL(statusChanged(const QString &)));

    connect(&loader,            SIGNAL(statusChanged(const QString &)),
            ui.plainTextEdit,   SLOT(appendPlainText(const QString &)));

    PropellerImage image;

    if (!binary.isEmpty())
    {
        image = PropellerImage(binary);
    }
    else
    {
        QFile file(config.file);
        if (!file.open(QIODevice::ReadOnly))
        {
            qCritical() << "Couldn't open file:" << config.file;
            return false;
        }

        image = PropellerImage(file.readAll(), config.file);
    }

    loader.upload(image, config.write, true, true);
    emit finished();

    waitClose();
    return true;
}

void BuildManager::loadSuccess()
{
    setStage(3);
    setTextColor(Qt::darkGreen);
}

void BuildManager::loadFailure()
{
    setTextColor(Qt::red);
}

void BuildManager::build()
{
    ui.plainTextEdit->clear();

    language.loadExtension(QFileInfo(config.file).suffix());
    compilersteps = language.listBuildSteps();

    if (compilersteps.isEmpty())
    {
        qWarning() << "No compiler steps defined for language:" << language.name();
        compilerFinished(false);
        return;
    }

    runCompilerStep();
}

void BuildManager::runCompilerStep()
{
    if (compilersteps.isEmpty())
        return;

    QString step = compilersteps.takeFirst();
    runCompiler(step);
}

void BuildManager::runCompiler(QString name)
{
    if (compiler)
        return;

    compiler = new ExternalCompiler(name);

    connect(compiler,   SIGNAL(finished(bool)),
            this,       SLOT(compilerFinished(bool)));
    connect(compiler,   SIGNAL(highlightLine(const QString &, int, int, const QString &)),
            this,       SIGNAL(highlightLine(const QString &, int, int, const QString &)));
    connect(compiler,   SIGNAL(print(const QString &)),
            this,       SLOT(print(const QString &)));

    config.file = compiler->build(config.file,
                                  config.includes);

    if (config.file.isEmpty())
        return;

    showStatus();
}

void BuildManager::cleanupCompiler()
{
    if (compiler)
    {
        disconnect(compiler,   SIGNAL(print(const QString &)),
                   this,       SLOT(print(const QString &)));
        disconnect(compiler,   SIGNAL(highlightLine(const QString &, int, int, const QString &)),
                   this,       SIGNAL(highlightLine(const QString &, int, int, const QString &)));
        disconnect(compiler,   SIGNAL(finished(bool)),
                   this,       SLOT(compilerFinished(bool)));

        delete compiler;
        compiler = NULL;
    }
}

void BuildManager::compilerFinished(bool success)
{
    cleanupCompiler();

    if (!success)
    {
        setText(tr("Build failed!"));
        emit finished();
    }
    else
    {
        setText(tr("Build successful!"));

        if (!compilersteps.isEmpty())
        {
            runCompilerStep();
            return;
        }

        if (config.load)
        {
            if (!load())
                emit finished();
        }
        else
        {
            setStage(1);
//            hideStatus();
            emit finished();
        }
    }
}

void BuildManager::updateColors()
{
    ui.plainTextEdit->setFont(currentTheme->getFont());
}

void BuildManager::setText(const QString & text)
{
    ui.activeText->setText(text);
}

void BuildManager::keyPressEvent(QKeyEvent * event)
{
    if(event->key() == Qt::Key_Escape)
    {
        event->ignore();
    }
}

void BuildManager::setBuild(bool active)
{
    ui.iconBuild->setEnabled(active);
}

void BuildManager::setDownload(bool active)
{
    ui.arrow1->setEnabled(active);
    ui.iconDownload->setEnabled(active);
}

void BuildManager::setRun(bool active)
{
    ui.arrow2->setEnabled(active);
    ui.iconRun->setEnabled(active);
}

void BuildManager::setStage(int stage)
{
    if (stage > 0)
        setBuild(true);
    else
        setBuild(false);

    if (stage > 1)
        setDownload(true);
    else
        setDownload(false);

    if (stage > 2)
        setRun(true);
    else
        setRun(false);
}

