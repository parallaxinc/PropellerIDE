#pragma once

#include <QTabWidget>
#include <QMessageBox>
#include <QStatusBar>

#include "editor.h"
#include "logging.h"
#include "language.h"

class FileManager : public QTabWidget
{
    Q_OBJECT

    Language language;

    void createBackgroundImage();
    QString reformatText(QString text);
    QString getExtensionString(bool supportedoption = false,
                               bool alloption = false);

public:
    explicit FileManager(QWidget *parent = 0);

    Editor * getEditor(int num);
    int isFileOpen(const QString & fileName);
    int isFileEmpty(int index);

    bool createHome();
    QString getDirectory();
    void setDirectory(QString dir);

public slots:
    int  newFile();
    void newFromFile();
    int  newFromFile(const QString & fileName);
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
    bool closeFile(int index);
    bool saveAndClose(int index);

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
