#include "status.h"
#include <QDebug>

Status::Status(QWidget *parent)
    : QFrame(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | 
            Qt::WindowStaysOnTopHint | Qt::Dialog);
    setFrameShadow(QFrame::Raised);

    ui.plainTextEdit->hide();

    ui.activeText->setText(" ");
    setStage(0);
    stage = 0;

    connect(ui.label, SIGNAL(clicked()), this, SLOT(toggleDetails()));
}

void Status::setBuild(bool active)
{
    ui.iconBuild->setEnabled(active);
    ui.textBuild->setEnabled(active);
}

void Status::setDownload(bool active)
{
    ui.arrow1->setEnabled(active);
    ui.iconDownload->setEnabled(active);
    ui.textDownload->setEnabled(active);
}

void Status::setRun(bool active)
{
    ui.arrow2->setEnabled(active);
    ui.iconRun->setEnabled(active);
    ui.textRun->setEnabled(active);

}

void Status::setStage(int newstage)
{
    if (newstage > 0)
        setBuild(true);
    else
        setBuild(false);

    if (newstage > 1)
        setDownload(true);
    else
        setDownload(false);

    if (newstage > 2)
        setRun(true);
    else
        setRun(false);
}


void Status::showDetails()
{
    ui.label->setText("Details -");
    ui.plainTextEdit->show();
    adjustSize();
}

void Status::hideDetails()
{
    ui.label->setText("Details +");
    ui.plainTextEdit->hide();
    adjustSize();
}

void Status::toggleDetails()
{
    if (ui.plainTextEdit->isVisible())
        hideDetails();
    else
        showDetails();
}

void Status::setText(const QString & text)
{
    ui.activeText->setText(text);
}

void Status::keyPressEvent(QKeyEvent * event)
{
    if(event->key() == Qt::Key_Escape)
    {
        event->ignore();
    }
}

QPlainTextEdit * Status::getOutput()
{
    return ui.plainTextEdit;
}
