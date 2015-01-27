#include "ui_status.h"
#include <QKeyEvent>
#include <Qt>

class Status : public QDialog
{
    Q_OBJECT

public:
    Status(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent * event);
    QPlainTextEdit * getOutput();

private slots:
    void toggleDetails();

private:
    Ui::statusDialog ui;
    int stage;
    void setRun(bool active);
    void setBuild(bool active);
    void setDownload(bool active);
    void setStage(int newstage);
    void selectStage();
};
