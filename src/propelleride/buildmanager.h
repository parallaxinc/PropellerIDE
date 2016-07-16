#pragma once

#include <QString>
#include <QProcess>
#include <QTimer>
#include <QKeyEvent>
#include <Qt>

#include <PropellerLoader>

#include "compiler.h"
#include "logging.h"
#include "colorscheme.h"
#include "externalcompiler.h"
#include "language.h"

#include "ui_buildmanager.h"

class BuildManager : public QFrame
{
    Q_OBJECT

    Ui::buildManager ui;
    ColorScheme * currentTheme;
    QTimer timer;

    void waitClose();
    void setRun(bool active);
    void setBuild(bool active);
    void setDownload(bool active);
    void setStage(int stage);

    Language language;
    Compiler * compiler;
    QStringList compilersteps;

    void runCompilerStep();
    void runCompiler(QString name);
    void cleanupCompiler();

public:
    explicit BuildManager(QWidget *parent = 0);
    ~BuildManager();
    void setFont(const QFont & font);
    void setTextColor(QColor color);

    typedef struct ConfigurationType
    {
        QString compiler;
        QStringList includes;
        QString file;
        QString port;

        bool load;
        bool write;

        PropellerManager * manager;
    } Configuration;
    
    Configuration config;

    void setConfiguration(BuildManager::Configuration config);
    void build();
    bool load(const QByteArray & binary = QByteArray());

signals:
    void highlightLine(const QString & file,
            int line,
            int col,
            const QString & text);
    void statusChanged(const QString & text);
    void finished();
    void buildError();

private slots:
    void loadSuccess();
    void loadFailure();
    void compilerFinished(bool success);

protected:
    void keyPressEvent(QKeyEvent * event);

public slots:
    void updateColors();
    void showStatus();
    void hideStatus();

    void print(const QString & text);
    void setText(const QString & text);
};
