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
    int isFileOpen(const QString & fileName);
    int isFileEmpty(int index);

public slots:
    int  newFile();
    void open();
    int  openFile(const QString & fileName);
    void fileChanged();

    void save();
    void save(int index);
    void saveAs();
    void saveAs(int index);
    void saveFile(const QString & fileName, int index);
    void saveAll();

    void closeAll();
    void closeFile();
    void closeFile(int index);
    int  saveAndClose();

    void nextTab();
    void previousTab();
    void changeTab(int index);

    void cut();
    void copy();
    void paste();
    void undo();
    void redo();
    void selectAll();

    void setUndo(bool available);
    void setRedo(bool available);
    void setCopy(bool available);

signals:
    void fileUpdated(int index);
    void undoAvailable(bool available);
    void redoAvailable(bool available);
    void saveAvailable(bool available);
    void copyAvailable(bool available);
    void closeAvailable(bool available);
    void sendMessage(const QString & message);

};
