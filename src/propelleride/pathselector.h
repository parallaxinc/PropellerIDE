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

#include "language.h"

#include "ui_language.h"

class PathSelector : public QWidget
{
    Q_OBJECT

    Ui::Language ui;

    QStringList defaultincludes;

    Language language;
    QStringList includes;

private slots:
    void setIncludes(const QStringList & paths);

    void addPath();
    void deletePath();

public:
    explicit PathSelector(QString languagekey,
                          QWidget * parent = 0);
    ~PathSelector();

    void setDefaultCompiler(QString path);
    void setDefaultIncludes(QStringList paths);

public slots:
    void restore();
    void accept();
    void reject();
    void save();
    void load();

};
