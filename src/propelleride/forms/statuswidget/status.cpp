#include "status.h"
#include <QDebug>
#include <QGraphicsDropShadowEffect>

StatusDialog::StatusDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    ui.plainTextEdit->hide();
    ui.plainTextEdit->setPlainText("Propeller Spin/PASM Compiler 'OpenSpin' (c)2012-2014 Parallax Inc. DBA Parallax Semiconductor.\n"
"Version 1.00.71 Compiled on Nov 30 2014 07:40:47\n"
"Compiling...\n"
"../../../library/Graphics.spin\n"
"Done.\n"
"Program size is 3256 bytes\n");

    ui.activeText->setText(" ");
    setStage(0);
    stage = 0;

    connect(ui.label, SIGNAL(clicked()), this, SLOT(toggleDetails()));
}

void StatusDialog::setBuild(bool active)
{
    ui.iconBuild->setEnabled(active);
    ui.textBuild->setEnabled(active);
}

void StatusDialog::setDownload(bool active)
{
    ui.arrow1->setEnabled(active);
    ui.iconDownload->setEnabled(active);
    ui.textDownload->setEnabled(active);
}

void StatusDialog::setRun(bool active)
{
    ui.arrow2->setEnabled(active);
    ui.iconRun->setEnabled(active);
    ui.textRun->setEnabled(active);

}

void StatusDialog::setStage(int newstage)
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

void StatusDialog::toggleDetails()
{
    if (ui.plainTextEdit->isVisible())
    {
        ui.label->setText("Details +");
        ui.plainTextEdit->hide();
    }
    else
    {
        ui.label->setText("Details -");
        ui.plainTextEdit->show();
    }
}


void StatusDialog::selectStage()
{
    setStage(stage);
    switch (stage)
    {
    case 0: ui.activeText->setText(" ");
            break;
    case 1: ui.activeText->setText("Building file.spin...");
            break;
    case 2: ui.activeText->setText("Downloading to TTYUSB0...");
            break;
    case 3: ui.activeText->setText("Download complete!");
            break;
    default: stage = 0;
    }

    stage++;
    if (stage > 3)
        stage = 0;

    adjustSize();
}

void StatusDialog::keyPressEvent(QKeyEvent * event)
{
    if(event->key() == Qt::Key_Escape)
    {
        event->ignore();
    }
    else if (event->key() == Qt::Key_Space)
    {
        selectStage();
    }
}
