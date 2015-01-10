#pragma once

#include <QTabWidget>

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
    void saveFile();
    void newFile();

    void closeFile();
    void closeTab(int index = 0);

    void nextTab();
    void previousTab();
    void changeTab(int index);

};
