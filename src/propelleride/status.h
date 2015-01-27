#include "ui_status.h"
#include <QKeyEvent>
#include <Qt>

class Status : public QFrame
{
    Q_OBJECT

public:
    Status(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent * event);
    QPlainTextEdit * getOutput();
    void setText(const QString & text);
    void setStage(int newstage);

public slots:
    void toggleDetails();
    void showDetails();
    void hideDetails();

private:
    Ui::statusDialog ui;
    int stage;
    void setRun(bool active);
    void setBuild(bool active);
    void setDownload(bool active);
};
