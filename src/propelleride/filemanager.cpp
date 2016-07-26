#include "filemanager.h"

#include <QDebug>
#include <QFileDialog>
#include <QRegularExpression>
#include <QStandardPaths>

FileManager::FileManager(QWidget *parent) :
    QTabWidget(parent)
{
    setTabsClosable(true);
    setMovable(true);

    createBackgroundImage();

    connect(this,   SIGNAL(tabCloseRequested(int)), this,   SLOT(closeFile(int)));
    connect(this,   SIGNAL(currentChanged(int)),    this,   SLOT(changeTab(int)));

    createHome();
}

bool FileManager::createHome()
{
    QSettings settings;
    settings.beginGroup("Paths");

    QDir projects(QDir::homePath() + "/" + tr("PropellerIDE Projects"));

    if (!projects.exists())
    {
        if (!QDir().mkdir(projects.path()))
        {
            qWarning() << "failed to create projects folder:" << projects.path();
            return false;
        }
    }

    settings.setValue("homeDirectory", projects.path());

    if (settings.value("lastDirectory").toString().isEmpty())
        settings.setValue("lastDirectory", projects.path());

    settings.endGroup();

    return true;
}

QString FileManager::getDirectory()
{
    QSettings settings;
    settings.beginGroup("Paths");

    return settings.value("lastDirectory", 
                settings.value("homeDirectory", QDir::homePath()).toString()).toString();
}

void FileManager::setDirectory(QString dir)
{
    QSettings settings;
    settings.beginGroup("Paths");
    settings.setValue("lastDirectory", dir);
}

int FileManager::newFile()
{
    // removes the background image (need to move this elsewhere)
    setStyleSheet("");

    EditorView * editor = new EditorView(QWidget::window());
    editor->setAttribute(Qt::WA_DeleteOnClose);
    editor->installEventFilter(QWidget::window());
    editor->saveContent();

    int index = addTab(editor,tr("Untitled"));

    setTabToolTip(index,"");

    connect(editor, SIGNAL(textChanged()),          this,   SLOT(fileChanged()));
    connect(editor, SIGNAL(undoAvailable(bool)),    this,   SLOT(setUndo(bool)));
    connect(editor, SIGNAL(redoAvailable(bool)),    this,   SLOT(setRedo(bool)));
    connect(editor, SIGNAL(copyAvailable(bool)),    this,   SLOT(setCopy(bool)));

    connect(this,   SIGNAL(accepted()),                 editor,   SLOT(loadPreferences()));
    connect(this,   SIGNAL(updateColors()),             editor,   SLOT(updateColors()));
    connect(this,   SIGNAL(updateFonts(const QFont &)), editor,   SLOT(updateFonts()));

    emit closeAvailable(true);
    setCurrentIndex(index);

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
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                tr("Open File"),
                getDirectory(),
                getExtensionString());

    for (int i = 0; i < fileNames.size(); i++)
        if (!fileNames.at(i).isEmpty())
        {
            setDirectory(QDir(fileNames.at(i)).path());
            openFile(fileNames.at(i));
        }
}


int FileManager::isFileOpen(const QString & fileName)
{
    // check if file already open before opening another instance
    for (int i = 0; i < count(); i++)
    {
        if (fileName == tabToolTip(i))
        {
            setCurrentIndex(i);
            emit sendMessage(tr("File already open: %1").arg(fileName));
            return 1;
        }
    }
    return 0;
}

int FileManager::isFileEmpty(int index)
{
    if (count() && (tabToolTip(index).isEmpty()
                    && getView(index)->toPlainText().isEmpty()
                    && !getView(index)->contentChanged()) )
        return 1;
    else
        return 0;
}

QString FileManager::reformatText(QString text)
{
    QRegularExpression newlines("\r\n|\r|\n",QRegularExpression::DotMatchesEverythingOption);
    text.replace(newlines,"\n");
    return text;
}

QString FileManager::getExtensionString()
{
    return getExtensionList().join(";;");
}

QString FileManager::getExtensionPattern(QStringList extensions)
{
    QStringList newext;

    foreach (QString s, extensions)
        newext.append("*."+s);

    newext.sort();

    return newext.join(" ");
}

QStringList FileManager::getExtensionList()
{
    QStringList extensionlist; 

    foreach (QString l, language.languages())
    {
        language.loadKey(l);
        extensionlist.append(tr("%1 files (%2)")
            .arg(language.name())
            .arg(getExtensionPattern(language.extensions())));
    }

    extensionlist.sort();

    extensionlist.prepend(tr("Supported filetypes (%1)")
            .arg(getExtensionPattern(language.allExtensions())));

    extensionlist.append(tr("All files (*)"));

    return extensionlist;
}


int FileManager::openFile(const QString & fileName)
{
    qDebug() << "opening" << fileName;

    if (fileName.isEmpty())
        return -1;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return -1;
    }

    if (isFileOpen(fileName))
        return -1;

    int index;
    if (isFileEmpty(currentIndex()))
        index = currentIndex();
    else
        index = newFile();

    QTextStream in(&file);
    in.setAutoDetectUnicode(true);
    in.setCodec("UTF-8");
    getView(index)->setPlainText(reformatText(in.readAll()));

    QFileInfo fi(fileName);

    getView(index)->setExtension(fi.suffix());

    setTabToolTip(index,fi.canonicalFilePath());
    setTabText(index,fi.fileName());
    getView(index)->saveContent();
    fileChanged();

    emit fileUpdated(index);
    emit sendMessage(tr("File opened successfully: %1").arg(fileName));

    return index;
}


void FileManager::newFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                tr("New From File..."),
                getDirectory(),
                getExtensionString());

    if (!fileName.isEmpty())
        newFromFile(fileName);
}


int FileManager::newFromFile(const QString & fileName)
{
    qDebug() << "creating file from" << fileName;

    if (fileName.isEmpty())
        return 1;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return 1;
    }

    int index = newFile();

    QTextStream in(&file);
    in.setAutoDetectUnicode(true);
    in.setCodec("UTF-8");
    getView(index)->setPlainText(reformatText(in.readAll()));

    fileChanged();

    emit fileUpdated(index);

    return 0;
}


bool FileManager::save()
{
    return save(currentIndex());
}

bool FileManager::save(int index)
{
    QString fileName = tabToolTip(index);

    if (fileName.isEmpty())
        return saveAs(index);
    else
        return saveFile(fileName, index);
}

bool FileManager::saveAs()
{
    return saveAs(currentIndex());
}


bool FileManager::saveAs(int index)
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    QStringList filters = getExtensionList();
    dialog.setNameFilters(filters);

    QString filename = tabToolTip(index);
    QString lastdir = QDir(filename).path();

    if (filename.isEmpty())
    {
        filename = tr("Untitled.spin");
        lastdir = getDirectory();

        if (!filters.isEmpty())
            dialog.selectNameFilter(filters[0]);
    }
    else
    {
        foreach (QString f, filters)
        {
            QFileInfo fi(filename);
            if (f.contains(fi.suffix())) 
            {
                dialog.selectNameFilter(f);
                break;
            }
        }
    }

    dialog.setDirectory(lastdir);
    dialog.selectFile(filename);

    if (!dialog.exec() || dialog.selectedFiles().isEmpty())
        return false;

    filename = dialog.selectedFiles()[0];

    setDirectory(QFileInfo(filename).path());
    return saveFile(filename, index);
}


void FileManager::saveAll()
{
    for (int i = 0; i < count(); i++)
        save(i);
}


bool FileManager::saveFile(const QString & fileName, int index)
{
    qDebug() << "saving" << fileName;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Recent Files"),
                    tr("Cannot write file %1:\n%2.")
                    .arg(fileName)
                    .arg(file.errorString()));
        return false;
    }

    QTextStream os(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    os.setCodec("UTF-8");
    os << getView(index)->toPlainText();
    os.flush();
    QApplication::restoreOverrideCursor();

    QFileInfo fi(fileName);

    getView(index)->setExtension(fi.suffix());
    setTabToolTip(index,fi.canonicalFilePath());
    setTabText(index,fi.fileName());

    getView(index)->saveContent();
    fileChanged();
    emit fileUpdated(index);
    emit sendMessage(tr("File saved successfully: %1").arg(fileName));

    return true;
}


void FileManager::closeFile()
{
    closeFile(currentIndex());
}

void FileManager::closeAll()
{
    setCurrentIndex(0);
    while (count() > 0)
    {
        if (!closeFile(0))
            return;
    }
}

bool FileManager::saveAndClose(int index)
{
    QMessageBox dialog;
    dialog.setIcon(QMessageBox::Warning);
    dialog.setText(tr("Your code has been modified."));
    dialog.setInformativeText(tr("Do you want to save your changes?"));
    dialog.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    dialog.setDefaultButton(QMessageBox::Save);

    switch (dialog.exec())
    {
        case QMessageBox::Save:
            save(index);
            return true;
        case QMessageBox::Discard:
            return true;
        case QMessageBox::Cancel:
        default:
            return false;
    }
}

bool FileManager::closeFile(int index)
{
    if (count() <= 0)
        return false;

    if (index < 0 || index > count()-1)
    {
        qWarning() << "attempted to close index:" << index 
                   << "(count: " << count() << ")";
        return false;
    }

    if (getView(index)->contentChanged() && !saveAndClose(index))
        return false;

    getView(index)->disconnect();
    getView(index)->close();
    removeTab(index);
    
    if (count() <= 0)
    {
        createBackgroundImage();
        emit closeAvailable(false);
    }

    emit fileUpdated(-1);

    return true;
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
    setCurrentIndex(n);
}

void FileManager::previousTab()
{
    int n = currentIndex();
    n--;
    if (n < 0)
        n = count()-1;
    setCurrentIndex(n);
}

// this function is needed to set focus after changing the index
void FileManager::changeTab(int index)
{
    if(index < 0) return;

    EditorView * editor = getView(currentIndex());
    editor->setFocus();
    
    emit undoAvailable(editor->getUndo());
    emit redoAvailable(editor->getRedo());
    emit copyAvailable(editor->getCopy());
    emit fileUpdated(index);
}

EditorView * FileManager::getView(int num)
{
    return (EditorView *)widget(num);
}

// this needs some rethinking a lot
void FileManager::fileChanged()
{
    int index = currentIndex();
    QString file = tabToolTip(index);
    QFileInfo fi(file);
    QString name = fi.fileName();

    if (file.isEmpty())
        name = tr("Untitled");

    if (getView(index)->contentChanged())
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
        getView(currentIndex())->cut();
}

void FileManager::copy()
{
    if (count() > 0)
        getView(currentIndex())->copy();
}

void FileManager::paste()
{
    if (count() > 0)
        getView(currentIndex())->paste();
}

void FileManager::undo()
{
    if (count() > 0)
        getView(currentIndex())->undo();
}

void FileManager::redo()
{
    if (count() > 0)
        getView(currentIndex())->redo();
}

void FileManager::selectAll()
{
    if (count() > 0)
        getView(currentIndex())->selectAll();
}
// -------------------------------------------
