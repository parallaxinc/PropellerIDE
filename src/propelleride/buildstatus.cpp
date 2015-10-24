#include "buildstatus.h"

#include <QDebug>

BuildStatus::BuildStatus(QWidget *parent)
    : QFrame(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::Dialog);
    setFrameShadow(QFrame::Raised);

    ui.plainTextEdit->hide();
    adjustSize();

    ui.activeText->setText(" ");
    setStage(0);

    currentTheme = &Singleton<ColorScheme>::Instance();
    updateColors();

    connect(ui.label, SIGNAL(clicked()), this, SLOT(toggleDetails()));
}

void BuildStatus::setBuild(bool active)
{
    ui.iconBuild->setEnabled(active);
}

void BuildStatus::setDownload(bool active)
{
    ui.arrow1->setEnabled(active);
    ui.iconDownload->setEnabled(active);
}

void BuildStatus::setRun(bool active)
{
    ui.arrow2->setEnabled(active);
    ui.iconRun->setEnabled(active);
}

void BuildStatus::setStage(int stage)
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


void BuildStatus::showDetails()
{
    ui.label->setText("Details -");
    ui.plainTextEdit->show();
    adjustSize();
}

void BuildStatus::hideDetails()
{
    ui.label->setText("Details +");
    ui.plainTextEdit->hide();
    adjustSize();
}

void BuildStatus::toggleDetails()
{
    if (ui.plainTextEdit->isVisible())
        hideDetails();
    else
        showDetails();
}

void BuildStatus::setText(const QString & text)
{
    ui.activeText->setText(text);
}

void BuildStatus::keyPressEvent(QKeyEvent * event)
{
    if(event->key() == Qt::Key_Escape)
    {
        event->ignore();
    }
}

QPlainTextEdit * BuildStatus::getOutput()
{
    return ui.plainTextEdit;
}

void BuildStatus::updateColors()
{
    ui.plainTextEdit->setFont(currentTheme->getFont());
}
