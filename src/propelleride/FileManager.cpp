#include "FileManager.h"

FileManager::FileManager(QWidget *parent) :
    QTabWidget(parent)
{
    setTabsClosable(true);
    setMovable(true);
    setStyleSheet("background-image: url(./propellerhat.png);"
                  "background-repeat: no-repeat;"
                  "background-position: center;");
}

void FileManager::saveFile()
{
}

void FileManager::newFile()
{
}

void FileManager::closeFile()
{
}

Editor * FileManager::getEditor(int num)
{
    return (Editor *)widget(num);
}

void FileManager::setEditor(int num, QString shortName, QString fileName, QString text)
{
    getEditor(num)->setPlainText(text);
    setTabText(num,shortName);
    setTabToolTip(num,fileName);
    setCurrentIndex(num);
}


void FileManager::nextTab()
{
    int n = currentIndex();
    n++;
    if (n > count()-1)
        n = 0;
    changeTab(n);
}

void FileManager::previousTab()
{
    int n = currentIndex();
    n--;
    if (n < 0)
        n = count()-1;
    changeTab(n);


}

void FileManager::changeTab(int index)
{
    setCurrentIndex(index);
    if(index < 0) return;

    getEditor(currentIndex())->setFocus();
}

