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
    if (compiler)
    {
        delete compiler;
        compiler = NULL;
    }
}

void BuildManager::showStatus()
{
    setStage(0);
    show();
    raise();
    timer.stop();
    ui.plainTextEdit->clear();
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

void BuildManager::compilerFinished(bool success)
{
    disconnect(compiler,   SIGNAL(highlightLine(const QString &, int, int, const QString &)),
               this,       SIGNAL(highlightLine(const QString &, int, int, const QString &)));
    disconnect(compiler,   SIGNAL(finished(bool)),
               this,       SLOT(compilerFinished(bool)));

    if (compiler)
    {
        delete compiler;
        compiler = NULL;
    }

    if (!success)
    {
        setText(tr("Build failed!"));
        emit finished();
    }
    else
    {
        setText(tr("Build successful!"));

        if (config.load)
        {
            if (!load())
                emit finished();
        }
        else
        {
            emit finished();
        }
    }
}

void BuildManager::setTextColor(QColor color)
{
    QTextCharFormat tf = ui.plainTextEdit->currentCharFormat();
    tf.setForeground(color);
    ui.plainTextEdit->setCurrentCharFormat(tf);
}

void BuildManager::print(const QString & text, QColor color)
{
    QTextCharFormat tf = ui.plainTextEdit->currentCharFormat();
    QBrush oldcolor = tf.foreground();

    tf.setForeground(color);
    ui.plainTextEdit->setCurrentCharFormat(tf);

    ui.plainTextEdit->appendPlainText(text);

    tf.setForeground(oldcolor);
    ui.plainTextEdit->setCurrentCharFormat(tf);
}

//void BuildManager::procReadyRead()
//{
//    foreach (QString line, lines)
//    {
//        if (line.contains("Program size is") || line.contains("Bit fe") || line.contains("DOWNLOAD COMPLETE"))
//        {
//            setTextColor(Qt::darkGreen);
//        }
//        else if (line.contains("error", Qt::CaseInsensitive))
//        {
//            setTextColor(Qt::red);
//        }
//
//        ui.plainTextEdit->appendPlainText(line);
//    }
//
//    QScrollBar *sb = ui.plainTextEdit->verticalScrollBar();
//    sb->setValue(sb->maximum());
//}
//
//void BuildManager::runProcess(const QString & programName, const QStringList & programArgs)
//{
//    setTextColor(Qt::black);
//}


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
        QFile file(config.binary);
        if (!file.open(QIODevice::ReadOnly))
        {
            qCCritical(logbuildmanager) << "Couldn't open file:" << config.binary;
            return false;
        }

        image = PropellerImage(file.readAll(), config.binary);
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
    if (compiler)
        return;

    compiler = new ExternalCompiler();

    connect(compiler,   SIGNAL(finished(bool)),
            this,       SLOT(compilerFinished(bool)));
    connect(compiler,   SIGNAL(highlightLine(const QString &, int, int, const QString &)),
            this,       SIGNAL(highlightLine(const QString &, int, int, const QString &)));

    compiler->build(config.file);

//    showStatus();
//
//    QStringList args;
//
//    foreach (QString include, config.includes)
//    {
//        if (include.size() > 0)
//            args.append("-L" + include);
//    }
//
//    QString actionstring = tr("Building '%1'...")
//        .arg(QFileInfo(config.file).fileName());
//
//    setStage(1);
//    print(actionstring, Qt::darkYellow);
//    setText(actionstring);
//
//    args.append(config.file);
//
//    runProcess(config.compiler, args);
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

