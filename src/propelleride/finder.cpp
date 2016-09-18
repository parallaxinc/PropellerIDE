#include "finder.h"

Finder::Finder(QWidget *parent)
    : QFrame(parent)
{
    findPosition = 0;
    ui.setupUi(this);
    hide();
    connect(ui.findPrevious,    SIGNAL(clicked()),            this,   SLOT(findPrevious()));
    connect(ui.findNext,        SIGNAL(clicked()),            this,   SLOT(findNext()));
    connect(ui.replacePrevious, SIGNAL(clicked()),            this,   SLOT(replacePrevious()));
    connect(ui.replaceNext,     SIGNAL(clicked()),            this,   SLOT(replaceNext()));
    connect(ui.replaceAll,      SIGNAL(clicked()),            this,   SLOT(replaceAll()));
    connect(ui.doneButton,      SIGNAL(clicked()),            this,   SLOT(hide()));
    connect(ui.findEdit,        SIGNAL(textChanged(QString)), this,   SLOT(findChanged(QString)));
}

void Finder::connectFileManager(FileManager * fileManager)
{
    this->fileManager = fileManager;
}

void Finder::showFinder()
{
    if (!fileManager)
        return; 

    if (fileManager->count() > 0)
    {
        EditorView *editor = fileManager->getView(fileManager->currentIndex());
        if(editor->textCursor().selectedText().length() > 0) {
            ui.findEdit->setText(editor->textCursor().selectedText());
        }
        else {
            ui.findEdit->clear();
        }
    
        show();
        ui.findEdit->setFocus();
        findPosition = editor->textCursor().position();
    }
}
    
QTextDocument::FindFlag Finder::getFlags(int prev)
{
    int flags = prev;

    if (ui.matchCase->isChecked())
        flags |= QTextDocument::FindCaseSensitively;

    if(ui.wholeWord->isChecked())
        flags |= QTextDocument::FindWholeWords;

    return (QTextDocument::FindFlag) flags;
}

void Finder::findChanged(QString text)
{
    if (!fileManager)
        return; 
    EditorView *editor = fileManager->getView(fileManager->currentIndex());
    if(editor == NULL)
        return;

    QTextCursor cur = editor->textCursor();
    cur.beginEditBlock();
    cur.setPosition(findPosition,QTextCursor::MoveAnchor);
    editor->setTextCursor(cur);
    cur.endEditBlock();

    // search forward, and then backward if not found
    if (!editor->find(text,getFlags()))
    {
        editor->find(text,getFlags(QTextDocument::FindBackward));
    }
}

void Finder::find(int prev)
{
    if (!fileManager)
        return; 
    EditorView *editor = fileManager->getView(fileManager->currentIndex());
    if(editor == NULL)
        return;

    int count = 0;
    QString text = ui.findEdit->text();
    QString edtext = editor->toPlainText();

    if (editor->find(text,getFlags(prev)))
    {
        count++;
    }
    else
    {
        QTextCursor cur = editor->textCursor();
        if (prev)
        {
            emit sendMessage(tr("Find reached beginning of file; starting from end"));
            cur.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        }
        else
        {
            emit sendMessage(tr("Find reached end of file; starting from beginning"));
            cur.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
        }
        editor->setTextCursor(cur);

        if (editor->find(text,getFlags(prev)))
        {
            count++;
        }
    }

    if(count > 0) {
        findPosition = editor->textCursor().position();
    }
}

void Finder::findNext()
{
    find();
}

void Finder::findPrevious()
{
    find(QTextDocument::FindBackward);
}

void Finder::replaceNext()
{
    if (!fileManager)
        return; 

    EditorView *editor = fileManager->getView(fileManager->currentIndex());
    if(editor == NULL)
        return;

    QString s = editor->textCursor().selectedText();
    if(s.length()) {
        QTextCursor cur = editor->textCursor();
        cur.beginEditBlock();
        cur.removeSelectedText();
        cur.insertText(ui.replaceEdit->text());
        cur.endEditBlock();
        findNext();
    }
}

void Finder::replacePrevious()
{
    if (!fileManager)
        return; 

    EditorView *editor = fileManager->getView(fileManager->currentIndex());
    if(editor == NULL)
        return;

    QString s = editor->textCursor().selectedText();
    if(s.length()) {
        QTextCursor cur = editor->textCursor();
        cur.beginEditBlock();
        cur.removeSelectedText();
        cur.insertText(ui.replaceEdit->text());
        cur.movePosition(QTextCursor::PreviousWord, QTextCursor::MoveAnchor);
        editor->setTextCursor(cur);
        cur.endEditBlock();

        findPrevious();
    }
}

void Finder::replaceAll()
{
    int count = 0;
    QString text = ui.findEdit->text();
    if (!fileManager)
        return; 
    EditorView *editor = fileManager->getView(fileManager->currentIndex());
    if(editor == NULL)
        return;

    QTextCursor cur = editor->textCursor();
    cur.setPosition(0,QTextCursor::MoveAnchor);
    editor->setTextCursor(cur);

    editor->textCursor().beginEditBlock();
    QString edtext = editor->toPlainText();

    QString s = editor->textCursor().selectedText();
    if(s.length()>0) {
        editor->textCursor().removeSelectedText();
        editor->textCursor().insertText(ui.replaceEdit->text());
        count++;
    }
    while(edtext.contains(text,Qt::CaseInsensitive)) {
        editor->find(text,getFlags());
        s = editor->textCursor().selectedText();
        if(s.length()>0) {
            editor->textCursor().removeSelectedText();
            editor->textCursor().insertText(ui.replaceEdit->text());
            count++;
        }
        else {
            break;
        }
        edtext = editor->toPlainText();
    }
    editor->textCursor().endEditBlock();

    QMessageBox::information(this, tr("Replace Done"),
            tr("Replaced %1 instances of \"%2\".").arg(count).arg(text));
}
