#pragma once

#include <QTabWidget>

#include "editor.h"

class FileManager : public QTabWidget
{
    Q_OBJECT
private:


public:

    explicit FileManager(QWidget *parent = 0);

    void saveFile();
    void newFile();
    void closeFile();

    Editor * getEditor(int num);
    void setEditor(int num, QString shortName, QString fileName, QString text);

    void nextTab();
    void previousTab();
    void changeTab(int index);

};
