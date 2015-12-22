#pragma once

#include <QKeyEvent>
#include <Qt>

#include "ui_buildstatus.h"

#include "colorscheme.h"

class BuildStatus : public QFrame
{
    Q_OBJECT

public:
    BuildStatus(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent * event);
    QPlainTextEdit * getOutput();

public slots:
    void toggleDetails();
    void showDetails();
    void hideDetails();
    void updateColors();

    void setStage(int stage);
    void setText(const QString & text);

private:
    Ui::statusDialog ui;
    void setRun(bool active);
    void setBuild(bool active);
    void setDownload(bool active);

    ColorScheme * currentTheme;
};
