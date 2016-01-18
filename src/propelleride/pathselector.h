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

    QString defaultcompiler;
    QStringList defaultincludes;

    QString language;
    QString compiler;
    QStringList includes;

private slots:
    void setCompiler(const QString & path);
    void setIncludes(const QStringList & paths);

    void addPath();
    void deletePath();
    void browse();

public:
    explicit PathSelector(QString language,
            QString compiler = QString(),
            QStringList includes = QStringList(),
            QWidget *parent = 0);
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
