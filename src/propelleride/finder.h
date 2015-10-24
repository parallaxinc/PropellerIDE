#pragma once

#include "ui_finder.h"

#include "filemanager.h"

class Finder : public QFrame
{
    Q_OBJECT

private:
    Ui::Finder ui;
    FileManager * fileManager;
    QTextDocument::FindFlag getFlags(int prev = 0);

    int findPosition;

public:
    Finder(QWidget *parent = 0);
    void connectFileManager(FileManager * fileManager);

signals:
    void sendMessage(const QString & message);

public slots:
    void findChanged(QString text);
    void find(int prev = 0);
    void findPrevious();
    void findNext();
    void replacePrevious();
    void replaceNext();
    void replaceAll();
    void showFinder();
};
