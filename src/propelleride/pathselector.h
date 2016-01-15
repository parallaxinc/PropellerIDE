#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QFileDialog>
#include <QSettings>

#include "ui_language.h"

class PathSelector : public QWidget
{
    Q_OBJECT

    Ui::Language ui;

public:
    explicit PathSelector(QWidget *parent = 0);
    ~PathSelector();
};
