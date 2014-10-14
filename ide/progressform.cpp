#include "progressform.h"
#include "ui_progressform.h"

ProgressForm::ProgressForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgressForm)
{
    ui->setupUi(this);
}

ProgressForm::~ProgressForm()
{
    delete ui;
}
