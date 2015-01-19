#include "FileManager.h"

FileManager::FileManager(QWidget *parent) :
    QTabWidget(parent)
{
    setTabsClosable(true);
    setMovable(true);

    createBackgroundImage();
}

int FileManager::newFile()
{
    // removes the background image (need to move this elsewhere)
    setStyleSheet("");

    Editor *editor = new Editor(QWidget::window());
    editor->setAttribute(Qt::WA_DeleteOnClose);
    editor->installEventFilter(QWidget::window());
    editor->saveContent();

    int index = addTab(editor,tr("Untitled"));

    setCurrentIndex(index);
    setTabToolTip(index,"");

//  newProjectTrees();

    connect(editor,SIGNAL(textChanged()),this,SLOT(fileChanged()));

    emit fileUpdated(index);

    return index;
}

void FileManager::open()
{
    QString dir = QDir(tabToolTip(currentIndex())).path();
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                tr("Open File"), dir, "Spin Files (*.spin);;All Files (*)");

    for (int i = 0; i < fileNames.size(); i++)
        if (!fileNames.at(i).isEmpty())
            openFile(fileNames.at(i));
}


void FileManager::openFile(const QString & fileName)
{
    qDebug() << "FileManager::openFile(" << fileName << ")";

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Recent Files"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    // check if file already open before opening another instance
    for (int i = 0; i < count(); i++)
        if (fileName == tabToolTip(i))
            return;

    int index = newFile();

    QTextStream in(&file);
    in.setAutoDetectUnicode(true);
    in.setCodec("UTF-8");
    QApplication::setOverrideCursor(Qt::WaitCursor);
    getEditor(index)->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setTabToolTip(index,QFileInfo(fileName).canonicalFilePath());
    setTabText(index,QFileInfo(fileName).fileName());
    getEditor(index)->saveContent();

    //setProject();
}


void FileManager::save()
{
    save(currentIndex());
}

void FileManager::save(int index)
{
    QString fileName = tabToolTip(index);

    if (fileName.isEmpty())
        saveAs(index);
    else
        saveFile(fileName, index);
}

void FileManager::saveAs()
{
    saveAs(currentIndex());
}


void FileManager::saveAs(int index)
{
    QString fileName = tabToolTip(index);

    if (fileName.isEmpty())
        fileName = tr("Untitled.spin");

    fileName = QFileDialog::getSaveFileName(this,
            tr("Save File As..."), 
            QDir(fileName).path(), 
            tr("Spin Files (*.spin)"));

    if (fileName.isEmpty())
        return;

    saveFile(fileName, index);
}


void FileManager::saveAll()
{
    for (int i = 0; i < count(); i++)
        save(i);
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
    QApplication::setOverrideCursor(Qt::WaitCursor);
    os.setCodec("UTF-8");
    os << getEditor(index)->toPlainText();
    QApplication::restoreOverrideCursor();

    setTabToolTip(index,QFileInfo(fileName).canonicalFilePath());
    setTabText(index,QFileInfo(fileName).fileName());
    getEditor(index)->saveContent();

    //setProject();
}


void FileManager::closeFile()
{
    int index = currentIndex();

    if (count() > 0)
    {
        if (getEditor(index)->contentChanged())
            saveAndClose();
        else
            closeFile(index);
    }
}

void FileManager::closeAll()
{
    setCurrentIndex(0);
    while (count() > 0)
    {
        if (getEditor(0)->contentChanged())
        {
            if (saveAndClose())
                return;
        }
        else
        {
            closeFile(0);
        }
    }
}

int FileManager::saveAndClose()
{
    QMessageBox dialog;
    dialog.setText(tr("Your code has been modified."));
    dialog.setInformativeText(tr("Do you want to save your changes?"));
    dialog.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    dialog.setDefaultButton(QMessageBox::Cancel);

    switch (dialog.exec())
    {
        case QMessageBox::Save:
            save();
            closeFile(currentIndex());
            break;
        case QMessageBox::Discard:
            closeFile(currentIndex());
            break;
        case QMessageBox::Cancel:
            return 1;
        default:
            break;
    }
    return 0;
}

void FileManager::closeFile(int index)
{
    if (count() > 0 && index >= 0 && index < count())
    {
        getEditor(index)->disconnect();
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

void FileManager::fileChanged()
{
    int index = currentIndex();
    QString file = tabToolTip(index);
    QString name = QFileInfo(file).fileName();

    if (file.isEmpty())
        name = tr("Untitled");

    if (getEditor(index)->contentChanged())
        name += '*';

    setTabText(index, name);
}

