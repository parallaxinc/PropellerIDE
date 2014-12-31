#include "mainwindow.h"

#include <QMessageBox> 

#ifndef QT5
#define QStringLiteral QLatin1String
#endif

QFrame *MainWindow::newFindFrame(QSplitter *split)
{
    findFrame = new QFrame(split);
    QFrame *theFrame = new QFrame(findFrame);
    theFrame->setGeometry(QRect(2, 2, 900, 55));

    /* this outer layout contains find & replace layouts */
    verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(8);
    verticalLayout->setContentsMargins(11, 11, 11, 11);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    verticalLayout->setContentsMargins(4, 4, 4, 0);

    /* layout for find feature buttons/fields */
    findLayout = new QHBoxLayout();
    findLayout->setSpacing(8);
    findLayout->setContentsMargins(11, 11, 11, 11);
    findLayout->setObjectName(QStringLiteral("findLayout"));
    findLayout->setContentsMargins(4, 4, 4, 0);

    /* "Find:" */
    findLabel = new QLabel();
    findLabel->setObjectName(QStringLiteral("findLabel"));
    findLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    findLayout->addWidget(findLabel);

    /* find-text field */
    findEdit = new QLineEdit();
    findEdit->setObjectName(QStringLiteral("findEdit"));
    QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(findEdit->sizePolicy().hasHeightForWidth());
    findEdit->setSizePolicy(sizePolicy1);
    findEdit->setMinimumSize(QSize(105, 0));
    findEdit->setBaseSize(QSize(75, 0));
    findLayout->addWidget(findEdit);

    /* find-previous button */
    findPreviousBtn = new QToolButton();
    findPreviousBtn->setObjectName(QStringLiteral("findPreviousBtn"));
    findPreviousBtn->setStyleSheet("QToolButton {\
        background: url(:/icons/find-previous.png) center no-repeat;\
            width: 24;\
            height: 24;\
            border-width: 0;\
            border: none;\
        }\
        QToolButton:pressed {\
        background: url(:/icons/find-previous-pressed.png) center no-repeat;\
        border-width: 0;\
        border: none;\
    }\
    ");
    findLayout->addWidget(findPreviousBtn);
    connect(findPreviousBtn,SIGNAL(clicked()),this,SLOT(findPrevClicked()));

    /* find-next button */
    findNextBtn = new QToolButton();
    findNextBtn->setObjectName(QStringLiteral("findNextBtn"));
    findNextBtn->setStyleSheet("QToolButton {\
        background: url(:/icons/find-next.png) center no-repeat;\
            width: 24;\
            height: 24;\
            border-width: 0;\
            border: none;\
        }\
        QToolButton:pressed {\
        background: url(:/icons/find-next-pressed.png) center no-repeat;\
        border-width: 0;\
        border: none;\
    }\
    ");
    findLayout->addWidget(findNextBtn);
    connect(findNextBtn,SIGNAL(clicked()),this,SLOT(findNextClicked()));

    /* whole-word search button */
    /* dgately 03/17/2014 - need pressed icon mods */
    wholeWordFilterBtn = new QToolButton();
    wholeWordFilterBtn->setObjectName(QStringLiteral("wholeWordFilterBtn"));
    wholeWordFilterBtn->setIcon(QIcon(":/icons/find-toggle-wholeword.png"));
    wholeWordFilterBtn->setCheckable(true);
    wholeWordFilterBtn->setStyleSheet("background-color: white");
    findLayout->addWidget(wholeWordFilterBtn);

    /* case-sensitive search button */
    /* dgately 03/17/2014 - need pressed icon mods */
    caseSensitiveFilterBtn = new QToolButton();
    caseSensitiveFilterBtn->setObjectName(QStringLiteral("caseSensitiveFilterBtn"));
    caseSensitiveFilterBtn->setIcon(QIcon(":/icons/find-toggle-case.png"));
    caseSensitiveFilterBtn->setCheckable(true);
    caseSensitiveFilterBtn->setStyleSheet("background-color: white");
    findLayout->addWidget(caseSensitiveFilterBtn);

    /* done-searching button */
    doneFindBtn = new QToolButton();
    doneFindBtn->setObjectName(QStringLiteral("doneFindBtn"));
    findLayout->addWidget(doneFindBtn);
    connect(doneFindBtn,SIGNAL(clicked()),this,SLOT(hideFindFrame()));
    /* Find menu command will also hide the Find frame if currently visible */

    /* add find feature to the layout */
    verticalLayout->addLayout(findLayout);

    /* layout for replace feature buttons/fields */
    replaceLayout = new QHBoxLayout();
    replaceLayout->setSpacing(8);
    replaceLayout->setContentsMargins(11, 11, 11, 11);
    replaceLayout->setObjectName(QStringLiteral("replaceLayout"));
    replaceLayout->setContentsMargins(4, 0, 4, 4);

    /* "Replace:" */
    replaceLabel = new QLabel();
    replaceLabel->setObjectName(QStringLiteral("replaceLabel"));
    replaceLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    replaceLayout->addWidget(replaceLabel);

    /* replace-text field */
    replaceEdit = new QLineEdit("");
    replaceEdit->setObjectName(QStringLiteral("replaceEdit"));
    sizePolicy1.setHeightForWidth(replaceEdit->sizePolicy().hasHeightForWidth());
    replaceEdit->setSizePolicy(sizePolicy1);
    replaceEdit->setMinimumSize(QSize(105, 0));
    replaceEdit->setSizeIncrement(QSize(105, 0));
    replaceEdit->setBaseSize(QSize(105, 0));
    replaceLayout->addWidget(replaceEdit);

    /* replace-previous button */
    replacePreviousBtn = new QToolButton();
    replacePreviousBtn->setObjectName(QStringLiteral("replacePreviousBtn"));
    replaceLayout->addWidget(replacePreviousBtn);
    connect(replacePreviousBtn,SIGNAL(clicked()),this,SLOT(replacePrevClicked()));

    /* replace-next button */
    replaceNextBtn = new QToolButton();
    replaceNextBtn->setObjectName(QStringLiteral("replaceNextBtn"));
    replaceLayout->addWidget(replaceNextBtn);
    connect(replaceNextBtn,SIGNAL(clicked()),this,SLOT(replaceNextClicked()));

    /* replace-all button */
    replaceAllBtn = new QToolButton();
    replaceAllBtn->setObjectName(QStringLiteral("replaceAllBtn"));
    replaceLayout->addWidget(replaceAllBtn);
    connect(replaceAllBtn,SIGNAL(clicked()),this,SLOT(replaceAllClicked()));

    /* add replace feature to the layout */
    verticalLayout->addLayout(replaceLayout);

    /* combine layouts */
    theFrame->setLayout(verticalLayout);
    findFrame->setLayout(verticalLayout);
    findFrame->setFrameShape(QFrame::Box);
    findFrame->setFrameShadow(QFrame::Raised);
    findFrame->setVisible(false);

    /* label translations */
    /* note: added 5 spaces in front of "Find" to align with "Replace" label */
    findLabel->setText(QApplication::translate("MainWindow", "     Find:", 0));
    replaceLabel->setText(QApplication::translate("MainWindow", "Replace:", 0));

    /* button translations */
    doneFindBtn->setText(QApplication::translate("MainWindow", "Done", 0));
    replacePreviousBtn->setText(QApplication::translate("MainWindow", "Replace Previous", 0));
    replaceNextBtn->setText(QApplication::translate("MainWindow", "Replace Next", 0));
    replaceAllBtn->setText(QApplication::translate("MainWindow", "Replace All", 0));

    /* tool-tips translations */
    findEdit->setToolTip(tr("Enter search text here"));
    findPreviousBtn->setToolTip(tr("Backward search"));
    findNextBtn->setToolTip(tr("Forward search"));
    wholeWordFilterBtn->setToolTip(tr("Whole word search"));
    caseSensitiveFilterBtn->setToolTip(tr("Case sensitive search"));
    doneFindBtn->setToolTip(tr("Exit Search"));
    replaceEdit->setToolTip(tr("Enter replacement text here"));
    replacePreviousBtn->setToolTip(tr("Search backward and replace text"));
    replaceNextBtn->setToolTip(tr("Search forward and replace text"));
    replaceAllBtn->setToolTip(tr("Search and replace all occurrences of text"));

    connect(findEdit,SIGNAL(textChanged(QString)),this,SLOT(findChanged(QString)));

    return findFrame;
}

void MainWindow::showFindFrame()
{
    Editor *editor = getEditor(editorTabs->currentIndex());
    if(editor->textCursor().selectedText().length() > 0) {
        setFindText(editor->textCursor().selectedText());
    }
    else {
        clearFindText();
    }

    findEdit->setFocus();
    findFrame->setVisible(true);
}

void MainWindow::hideFindFrame()
{
    findFrame->setVisible(false);
}

QTextDocument::FindFlag MainWindow::getFlags(int prev)
{
    int flags = prev;
    if(this->caseSensitiveFilterBtn->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if(this->wholeWordFilterBtn->isChecked())
        flags |= QTextDocument::FindWholeWords;
    return (QTextDocument::FindFlag) flags;
}

/*
 * Find text for user as typed in find line edit box.
 */

void MainWindow::findChanged(QString text)
{
    Editor *editor = getEditor(editorTabs->currentIndex());

    if(editor == NULL)
        return;
    QTextCursor cur = editor->textCursor();
    cur.beginEditBlock();
    cur.setPosition(findPosition,QTextCursor::MoveAnchor);
    editor->setTextCursor(cur);
    cur.endEditBlock();

    editor->find(text,getFlags());
}


void MainWindow::findClicked()
{
    QString text = findEdit->text();

    if (text.isEmpty()) {
        return;
    } else {
        findText = text;
        hide();
    }
}

void MainWindow::findNextClicked()
{

    Editor *editor = getEditor(editorTabs->currentIndex());
    if(editor == NULL)
        return;

    int count = 0;
    QString text = findEdit->text();
    QString edtext = editor->toPlainText();

    editor->setCenterOnScroll(true);

    {
        if(editor->find(text,getFlags()) == true) {
            count++;
        }
        else {
            if(showBeginMessage(tr("Find"))) {
                if(editor->find(text,getFlags()) == true) {
                    count++;
                }
            }
        }
    }

    if(count > 0) {
        findPosition = editor->textCursor().position();
    }

}
void MainWindow::findPrevClicked()
{
    Editor *editor = getEditor(editorTabs->currentIndex());
    if(editor == NULL)
        return;

    int count = 0;
    QString text = findEdit->text();
    QString edtext = editor->toPlainText();

    editor->setCenterOnScroll(true);

    {
        if(edtext.contains(text,Qt::CaseInsensitive)) {
            if(editor->find(text,getFlags(QTextDocument::FindBackward)) == true) {
                count++;
            }
            else {
                if(showEndMessage(tr("Find"))) {
                    if(editor->find(text,getFlags(QTextDocument::FindBackward)) == true) {
                        count++;
                    }
                }
            }
        }
    }

    if(count > 0) {
        QTextCursor cur = editor->textCursor();
        findPosition = editor->textCursor().position();
    }
}

QString MainWindow::getFindText()
{
    return findText;
}
void MainWindow::clearFindText()
{
    findEdit->clear();
}
void MainWindow::setFindText(QString text)
{
    return findEdit->setText(text);
}

void MainWindow::replaceClicked()
{
    QString text = replaceEdit->text();

    if (text.isEmpty()) {
        return;
    } else {
        replaceText = text;
        hide();
    }
}

void MainWindow::replaceNextClicked()
{
    Editor *editor = getEditor(editorTabs->currentIndex());
    if(editor == NULL)
        return;

    QString s = editor->textCursor().selectedText();
    if(s.length()) {
        QTextCursor cur = editor->textCursor();
        cur.beginEditBlock();
        cur.removeSelectedText();
        cur.insertText(replaceEdit->text());
        cur.endEditBlock();
        findNextClicked();
    }
}

void MainWindow::replacePrevClicked()
{
    Editor *editor = getEditor(editorTabs->currentIndex());
    if(editor == NULL)
        return;

    QString s = editor->textCursor().selectedText();
    if(s.length()) {
        QTextCursor cur = editor->textCursor();
        cur.beginEditBlock();
        cur.removeSelectedText();
        cur.insertText(replaceEdit->text());
        cur.movePosition(QTextCursor::PreviousWord, QTextCursor::MoveAnchor);
        editor->setTextCursor(cur);
        cur.endEditBlock();

        findPrevClicked();
    }
}

void MainWindow::replaceAllClicked()
{
    int count = 0;
    QString text = findEdit->text();
    Editor *editor = getEditor(editorTabs->currentIndex());
    if(editor == NULL)
        return;

    editor->textCursor().beginEditBlock();
    QString edtext = editor->toPlainText();
    editor->setCenterOnScroll(true);

    QString s = editor->textCursor().selectedText();
    if(s.length()>0) {
        editor->textCursor().removeSelectedText();
        editor->textCursor().insertText(replaceEdit->text());
        count++;
    }
    while(edtext.contains(text,Qt::CaseInsensitive)) {
        editor->find(text,getFlags());
        s = editor->textCursor().selectedText();
        if(s.length()>0) {
            editor->textCursor().removeSelectedText();
            editor->textCursor().insertText(replaceEdit->text());
            count++;
        }
        else {
            if(showBeginMessage(tr("Replace")) == false)
                break;
        }
        edtext = editor->toPlainText();
    }
    editor->textCursor().endEditBlock();

    QMessageBox::information(this, tr("Replace Done"),
            tr("Replaced %1 instances of \"%2\".").arg(count).arg(text));
}

QString MainWindow::getReplaceText()
{
    return replaceText;
}
void MainWindow::clearReplaceText()
{
    replaceEdit->clear();
}


bool MainWindow::showFindMessage(QString text)
{
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();

    if(ret == QMessageBox::Cancel) {
        return false;
    }
    Editor *editor = getEditor(editorTabs->currentIndex());
    if(editor == NULL)
        return false;
    QTextCursor cur = editor->textCursor();
    cur.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    editor->setTextCursor(cur);
    return true;
}

bool MainWindow::showBeginMessage(QString type)
{
    return showFindMessage(type+tr(" from beginning?"));
}

bool MainWindow::showEndMessage(QString type)
{
    return showFindMessage(type+tr(" from end?"));
}
