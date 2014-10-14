#ifndef PROGRESSFORM_H
#define PROGRESSFORM_H

#include <QWidget>

namespace Ui {
    class ProgressForm;
}

class ProgressForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressForm(QWidget *parent = 0);
    ~ProgressForm();

private:
    Ui::ProgressForm *ui;
};

#endif // PROGRESSFORM_H
