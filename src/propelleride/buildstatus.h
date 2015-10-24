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
    void setText(const QString & text);
    void setStage(int stage);

public slots:
    void toggleDetails();
    void showDetails();
    void hideDetails();
    void updateColors();

private:
    Ui::statusDialog ui;
    void setRun(bool active);
    void setBuild(bool active);
    void setDownload(bool active);

    ColorScheme * currentTheme;
};
