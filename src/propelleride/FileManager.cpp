#include "FileManager.h"

FileManager::FileManager(QWidget *parent) :
    QTabWidget(parent)
{
    setTabsClosable(true);
    setMovable(true);

    createBackgroundImage();
}

bool FileManager::saveFile(QString fileName, int index)
{
    QFile file(fileName);
    if (file.open(QFile::WriteOnly)) {
        QTextStream os(&file);
        QString data = getEditor(index)->toPlainText();

        os.setCodec("UTF-8");
        os << data;

        file.close();
        QString tab = tabText(index);
        if(tab.endsWith('*')) {
            tab = tab.mid(0, tab.length()-1);
            tab = tab.trimmed();
            setTabText(index,tab);
        }
        return true;
    }
    else {
        QMessageBox::information(this, tr("Can't Save File"), tr("Can't save file ")+fileName, QMessageBox::Ok);
    }
    return false;
}

void FileManager::newFile()
{
}

void FileManager::closeFile()
{
    closeTab(currentIndex());
}

void FileManager::closeTab(int index)
{
    if (count() > 0)
    {
        getEditor(index)->close();
        removeTab(index);
    }

    if (count() == 0)
    {
        createBackgroundImage();
    }
}

void FileManager::createBackgroundImage()
{
    setStyleSheet("background-image: url(:/icons/propellerhat.png);"
                  "background-repeat: no-repeat;"
                  "background-position: center;");
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

