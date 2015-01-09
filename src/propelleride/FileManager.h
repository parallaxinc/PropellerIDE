#pragma once

#include <QTabWidget>

class FileManager : public QTabWidget
{
    Q_OBJECT
private:


public:

    explicit FileManager(QWidget *parent = 0);

    void save();

};
