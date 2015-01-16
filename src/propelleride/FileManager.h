#pragma once

#include <QTabWidget>
#include <QMessageBox>

#include "editor.h"

class FileManager : public QTabWidget
{
    Q_OBJECT
private:
    void createBackgroundImage();


public:

    explicit FileManager(QWidget *parent = 0);

    Editor * getEditor(int num);
    void setEditor(int num, QString shortName, QString fileName, QString text);

public slots:
    void newFile();

    void save();
    void save(int index);
    void saveAs();
    void saveFile(const QString & fileName, int index);

    void closeFile();
    void closeFile(int index);

    void nextTab();
    void previousTab();
    void changeTab(int index);

};
