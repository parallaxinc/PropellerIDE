#include "FileManager.h"

FileManager::FileManager(QWidget *parent) :
    QTabWidget(parent)
{
    setTabsClosable(true);
    setMovable(true);

    createBackgroundImage();
}

void FileManager::newFile()
{
}


void FileManager::save()
{
    int index = currentIndex();
    save(index);
}

void FileManager::save(int index)
{
    QString fileName = tabToolTip(index);

    if (fileName.isEmpty())
        saveAs();
    else
        saveFile(fileName, index);
}


void FileManager::saveAs()
{
    int n = currentIndex();

    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save File As..."), 
            QDir(tabToolTip(n)).path(), 
            "Spin Files (*.spin)");

    if (fileName.isEmpty())
        return;

    saveFile(fileName, n);
}



void FileManager::saveFile(const QString & fileName, int index)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Recent Files"),
                    tr("Cannot write file %1:\n%2.")
                    .arg(fileName)
                    .arg(file.errorString()));
        return;
    }

    QTextStream os(&file);
    os.setCodec("UTF-8");

    QString data = getEditor(index)->toPlainText();
    os << data;

    file.close();

    // gui stuff - doesn't belong here
    QString tab = tabText(index);

    if(tab.endsWith('*'))
    {
        tab = tab.mid(0, tab.length()-1);
        tab = tab.trimmed();
        setTabText(index,tab);
    }

    setTabToolTip(index,QFileInfo(fileName).canonicalFilePath());
    setTabText(index,QFileInfo(fileName).fileName());
    getEditor(index)->saveContent();

    //setProject();
}


void FileManager::closeFile()
{
    closeFile(currentIndex());
}

void FileManager::closeFile(int index)
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
    getEditor(num)->saveContent();
}

