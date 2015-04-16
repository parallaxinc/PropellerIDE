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

    changeTab(index);
    setTabToolTip(index,"");

    connect(editor,SIGNAL(textChanged()),this,SLOT(fileChanged()));
    connect(editor,SIGNAL(undoAvailable(bool)),this,SLOT(setUndo(bool)));
    connect(editor,SIGNAL(redoAvailable(bool)),this,SLOT(setRedo(bool)));
    connect(editor,SIGNAL(copyAvailable(bool)),this,SLOT(setCopy(bool)));

    emit closeAvailable(true);

    return index;
}

void FileManager::setUndo(bool available)
{
    if (sender() == currentWidget())
        emit undoAvailable(available);
}

void FileManager::setRedo(bool available)
{
    if (sender() == currentWidget())
        emit redoAvailable(available);
}

void FileManager::setCopy(bool available)
{
    if (sender() == currentWidget())
        emit copyAvailable(available);
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


int FileManager::isFileOpen(const QString & fileName)
{
    // check if file already open before opening another instance
    for (int i = 0; i < count(); i++)
    {
        if (fileName == tabToolTip(i))
        {
            changeTab(i);
            emit sendMessage(tr("File already open: %1").arg(fileName));
            return 1;
        }
    }
    return 0;
}

int FileManager::isFileEmpty(int index)
{
    if (count() && (tabToolTip(index).isEmpty()
                    && getEditor(index)->toPlainText().isEmpty()
                    && !getEditor(index)->contentChanged()) )
        return 1;
    else
        return 0;
}

int FileManager::openFile(const QString & fileName)
{
    qDebug() << "FileManager::openFile(" << fileName << ")";

    if (fileName.isEmpty())
        return 1;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return 1;
    }

    if (isFileOpen(fileName))
        return 1;

    int index;
    if (isFileEmpty(currentIndex()))
        index = currentIndex();
    else
        index = newFile();

    QTextStream in(&file);
    in.setAutoDetectUnicode(true);
    in.setCodec("UTF-8");
    QApplication::setOverrideCursor(Qt::WaitCursor);
    getEditor(index)->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setTabToolTip(index,QFileInfo(fileName).canonicalFilePath());
    setTabText(index,QFileInfo(fileName).fileName());
    getEditor(index)->saveContent();
    fileChanged();

    emit fileUpdated(index);
    emit sendMessage(tr("File opened successfully: %1").arg(fileName));
    return 0;
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
    qDebug() << "FileManager::saveFile(" << fileName << ")";
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
    os.flush();
    QApplication::restoreOverrideCursor();

    setTabToolTip(index,QFileInfo(fileName).canonicalFilePath());
    setTabText(index,QFileInfo(fileName).fileName());
    getEditor(index)->saveContent();
    fileChanged();
    emit fileUpdated(index);
    emit sendMessage(tr("File saved successfully: %1").arg(fileName));
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
        emit fileUpdated(index);
    }

    if (count() == 0)
    {
        createBackgroundImage();
        emit closeAvailable(false);
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

// this function is needed to set focus after changing the index
void FileManager::changeTab(int index)
{
    setCurrentIndex(index);
    if(index < 0) return;

    Editor * editor = getEditor(currentIndex());
    editor->setFocus();
    
    emit undoAvailable(editor->getUndo());
    emit redoAvailable(editor->getRedo());
    emit copyAvailable(editor->getCopy());
    emit fileUpdated(index);
}

Editor * FileManager::getEditor(int num)
{
    return (Editor *)widget(num);
}

// this needs some rethinking a lot
void FileManager::fileChanged()
{
    int index = currentIndex();
    QString file = tabToolTip(index);
    QString name = QFileInfo(file).fileName();

    if (file.isEmpty())
        name = tr("Untitled");

    if (getEditor(index)->contentChanged())
    {
        name += '*';
        emit saveAvailable(true);
    }
    else
    {
        emit saveAvailable(false);
    }

    setTabText(index, name);
}

// does anybody have a good idea of how to
// replace this hacky structure with something
// more flexible?
// -------------------------------------------
void FileManager::cut()
{
    if (count() > 0)
        getEditor(currentIndex())->cut();
}

void FileManager::copy()
{
    if (count() > 0)
        getEditor(currentIndex())->copy();
}

void FileManager::paste()
{
    if (count() > 0)
        getEditor(currentIndex())->paste();
}

void FileManager::undo()
{
    if (count() > 0)
        getEditor(currentIndex())->undo();
}

void FileManager::redo()
{
    if (count() > 0)
        getEditor(currentIndex())->redo();
}

void FileManager::selectAll()
{
    if (count() > 0)
        getEditor(currentIndex())->selectAll();
}
// -------------------------------------------
