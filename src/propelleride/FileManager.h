#pragma once

#include <QTabWidget>
#include <QMessageBox>
#include <QStatusBar>

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
    int  newFile();
    void open();
    void openFile(const QString & fileName);
    void fileChanged();

    void save();
    void save(int index);
    void saveAs();
    void saveFile(const QString & fileName, int index);

    void closeAll();
    void closeFile();
    void closeFile(int index);
    void saveAndClose();

    void nextTab();
    void previousTab();
    void changeTab(int index);

signals:
    void fileUpdated(int index);

};
