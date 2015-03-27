#include "editor.h"

#include <QToolTip>
#include <QRect>
#include <QColor>
#include <QPainter>
#include <QApplication>

#include "mainwindow.h"
#define MAINWINDOW MainWindow

#include <QLinearGradient>
#include <QPlainTextEdit>

Editor::Editor(QWidget *parent) : QPlainTextEdit(parent)
{
    mainwindow = parent;
    propDialog = static_cast<MAINWINDOW*>(mainwindow)->propDialog;

    ctrlPressed = false;
    isSpin = false;
    expectAutoComplete = false;
    canUndo = false;
    canRedo = false;
    canCopy = false;

    lineNumberArea = new LineNumberArea(this);
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    updateLineNumberAreaWidth(0);

    highlighter = 0;
    setHighlights();
    setMouseTracking(true);
    //    setCenterOnScroll(true);
    setWordWrapMode(QTextOption::NoWrap);

    currentTheme = &Singleton<ColorScheme>::Instance();
    updateColors();
    updateFonts();
    saveContent();

    connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(updateBackgroundColors()));
    connect(propDialog,SIGNAL(updateColors()),this,SLOT(updateColors()));
    connect(propDialog,SIGNAL(updateFonts()),this,SLOT(updateFonts()));
    connect(propDialog->getTabSpaceLedit(),SIGNAL(textChanged(QString)), this, SLOT(tabSpacesChanged()));

    connect(this,SIGNAL(undoAvailable(bool)), this, SLOT(setUndo(bool)));
    connect(this,SIGNAL(redoAvailable(bool)), this, SLOT(setRedo(bool)));
    connect(this,SIGNAL(copyAvailable(bool)), this, SLOT(setCopy(bool)));

    // this must be a pointer otherwise we can't control the position.
    cbAuto = new QComboBox(this);
    cbAuto->hide();
}

Editor::~Editor()
{
    cbAuto->clear();
    delete cbAuto;
    delete highlighter;
    delete lineNumberArea;
}

void Editor::setHighlights()
{
    if(highlighter) {
        delete highlighter;
        highlighter = 0;
    }
    highlighter = new Highlighter(this->document());
    isSpin = true;
}

void Editor::saveContent()
{
    oldcontents = toPlainText();
}

int Editor::contentChanged()
{
    return oldcontents.compare(toPlainText());
}


void Editor::setLineNumber(int num)
{
    QTextCursor cur = textCursor();
    cur.setPosition(0);
    cur.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,num-1);
    setTextCursor(cur);
}

#define SPIN_AUTOCON

void Editor::keyPressEvent (QKeyEvent *e)
{
    if((QApplication::keyboardModifiers() & Qt::CTRL)) {
        if(e->key() == Qt::Key_S) {
            emit saveEditorFile();
            return;
        }
    }

    /* source browser */
    if((QApplication::keyboardModifiers() & Qt::CTRL) && ctrlPressed == false) {
        ctrlPressed = true;
        QTextCursor tcur = this->textCursor();
        QTextCursor cur = this->cursorForPosition(mousepos);
        tcur.select(QTextCursor::WordUnderCursor);
        cur.select(QTextCursor::WordUnderCursor);
        QString text = cur.selectedText();
        if(tcur.selectedText().compare(text)) {
            QPlainTextEdit::keyPressEvent(e);
            return;
        }
        QPlainTextEdit::keyPressEvent(e);
        return;
    }

    /* if F1 do help.
     * if tab or shift tab, do block shift
     * if "." do auto complete
     */
    int key = e->key();

    if((key == Qt::Key_Enter) || (key == Qt::Key_Return)) {
        if(QToolTip::isVisible() && propDialog->getSpinSuggestEnable()) {
            useSpinSuggestion(key);
        }
        else {
            if(autoIndent() == 0)
                QPlainTextEdit::keyPressEvent(e);
        }
    }
    else if(key == Qt::Key_BraceRight) {
        if(braceMatchColumn() == 0)
            QPlainTextEdit::keyPressEvent(e);;
    }
    /* if F1 get word under mouse and pass to findSymbolHelp. no word is ok too. */
    else if(key == Qt::Key_F1) {
        contextHelp();
        QPlainTextEdit::keyPressEvent(e);
    }
#ifdef SPIN_AUTOCON
    /* #-auto complete */
    else if(key == Qt::Key_NumberSign) {
        if(isSpin) {
            if(spinAutoCompleteCON() == 0)
                QPlainTextEdit::keyPressEvent(e);
        }
        else {
            QPlainTextEdit::keyPressEvent(e);
        }
    }
#endif
    /* dot-auto complete */
    else if(key == Qt::Key_Period && propDialog->getAutoCompleteEnable()) {
        if(isSpin) {
            if(spinAutoComplete() == 0)
                QPlainTextEdit::keyPressEvent(e);
        }
        else {
            QPlainTextEdit::keyPressEvent(e);
        }
    }

    /* if TAB key do block move */
    else if(key == Qt::Key_Tab || key == Qt::Key_Backtab) {
        if(QToolTip::isVisible() && propDialog->getSpinSuggestEnable()) {
            useSpinSuggestion(key);
        }
        else {
            tabBlockShift();
        }
    }
    /* if key up/down*/
    else if(key == Qt::Key_Up || key == Qt::Key_Down) {
        /* if tooltip, higlight the selected entry */
        if(QToolTip::isVisible() && propDialog->getSpinSuggestEnable()) {
            selectSpinSuggestion(key);
        }
        else {
            QPlainTextEdit::keyPressEvent(e);
        }
    }
    else {

        QPlainTextEdit::keyPressEvent(e);

        // If key is escape, don't show info.
        if(key == Qt::Key_Escape) {
            QToolTip::hideText();
            return;
        }
        else if(key == Qt::Key_Backspace) {
            QToolTip::hideText();
            return;
        }
        else if(key == Qt::Key_Space) {
            QToolTip::hideText();
            return;
        }

        if(propDialog->getSpinSuggestEnable()) {
            spinSuggest();
        }
    }
}

void Editor::spinSuggest()
{
    QTextCursor cur = textCursor();
    int oldpos = cur.position();
    cur.movePosition(QTextCursor::StartOfWord,QTextCursor::QTextCursor::KeepAnchor);
    QString text = cur.selectedText();
    if(text.length() < 1) {
        return;
    }
    cur.select(QTextCursor::WordUnderCursor);
    QString word = cur.selectedText();
    if(text.compare(word)) {
        return;
    }

    QPoint popPoint = mapToGlobal(keyPopPoint(cur));
    int newpos = cur.position();
    cur.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, oldpos-newpos);

    QStringList toolTextList;
    if(text.length() > 2) {
        int added = 0;
        QStringList list = spinParser.spinSymbols(fileName,"");
        foreach(QString s, list) {
            s = s.mid(s.indexOf("\t")+1); // skip type field
            if(s.contains(text,Qt::CaseInsensitive)) {
                QRegExp rx("([ \t]+)");
                if(s.contains(rx)) // replace multiple space/tab with space
                    s = s.replace(rx, " ");
                int pos = s.indexOf(text,0,Qt::CaseInsensitive);
                while(pos > 0 && s[pos-1] != ' ' && s[pos-1] != '\t')
                    pos--; // find beginning of word
                s = s.mid(pos);
                s = s.trimmed();
                // We can't really depend on the pub/pri tags for deciding what to do
                // with var names. Treat everything as a simple symbol.
                s = s.mid(0, s.indexOf(QRegExp("[ \[,()<>:=+-*/!@#$%^&|\\t\\r\\n]")));
                s = s.trimmed();
                if(s.compare(word) != 0) {
                    toolTextList.append(s);
                    added++;
                }
            }
        }
        if(!added) {
            QToolTip::hideText();
        }

        if(toolTextList.count() > 0) {
            QString tooltext("");
            toolTextList.sort(Qt::CaseInsensitive);
            for(int n = 0; n < toolTextList.count(); n++) {
                if(n) {
                    if(QString(toolTextList[n-1]).compare(toolTextList[n],Qt::CaseInsensitive))
                        tooltext += "<br/>"+toolTextList[n];
                } else {
                    tooltext = "<b>"+toolTextList[n]+"</b>";
                }
            }
            QToolTip::showText(popPoint, tooltext, this, this->rect());
        }
    }
}

void Editor::selectSpinSuggestion(int key)
{
    QTextCursor cur = textCursor();
    QString s = QToolTip::text();
    QStringList list = s.split(QRegExp("<br/>"));
    int len = list.count();
    int bnum = 0;
    for(int n = 0; n < len && len > 1; n++) {
        s = list[n];
        if(s.contains("<b>")) {
            s = s.replace(QRegExp("<b>|</b>"),"");
            list[n] = s;
            if(key == Qt::Key_Up) {
                if(!n) n = len;
                bnum = (n-1) % len;
            }
            else  if(key == Qt::Key_Down) {
                bnum = (n+1) % len;
            }
        }
    }
    QString tooltext;
    for(int n = 0; n < len && len > 1; n++) {
        if(n) {
            tooltext += "<br/>";
        }
        if(n == bnum) {
            tooltext += "<b>"+list[n]+"</b>";
        }
        else {
            tooltext += list[n];
        }
    }
    cur.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
    QPoint pop = mapToGlobal(keyPopPoint(cur));
    QToolTip::showText(pop,tooltext,this,rect());
}

void Editor::useSpinSuggestion(int key)
{
    QTextCursor cur = textCursor();
    QString s = QToolTip::text();
    QStringList list = s.split(QRegExp("<br/>"));

    if(list.count() < 2) { // don't insert multi-line
        cur.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
        s = s.replace(QRegExp("<b>|</b>"),"");
        cur.insertText(s);
        setTextCursor(cur);
    }
    else {
        int n;
        int len = list.count();
        for(n = 0; n < len && len > 1; n++) {
            s = list[n];
            if(s.contains("<b>")) {
                s = s.replace(QRegExp("<b>|</b>"),"");
                break;
            }
        }
        if(n <= len) {
            cur.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
            cur.insertText(s);
            setTextCursor(cur);
        }
    }
    // if the key is enter and end of line, insert a block
    if(key == Qt::Key_Enter || key == Qt::Key_Return) {
        if(cur.atBlockEnd()) {
            cur.insertBlock();
            setTextCursor(cur);
        }
    }
    // if the key is a tab, insert a space
    if(key == Qt::Key_Tab) {
        int spaces = propDialog->getTabSpaces();
        int n = cur.columnNumber();
        int pos = cur.position();
        QChar ch = this->toPlainText().at(pos);
        if(ch == Qt::Key_Space || ch == Qt::Key_Tab) {
            n++;
        }
        for(; n % spaces; n++) {
            cur.insertText(" ");
        }
        //cur.insertText(" "); // insert a single space
        setTextCursor(cur);
    }
    QToolTip::hideText();
}

QPoint Editor::keyPopPoint(QTextCursor cursor)
{
    int ht = fontMetrics().height();
    int wd = fontMetrics().width(QLatin1Char('9'));
    int col = cursor.columnNumber();
    int row = cursor.blockNumber()+2; // show just below line

    QTextBlock block = firstVisibleBlock();
    int top = block.firstLineNumber();

    QPoint pt = QPoint(lineNumberAreaWidth()+col*wd,(row-top)*ht);
    //qDebug() << "keyPopPoint" << pt.x() << pt.y();
    return pt;
}

void Editor::keyReleaseEvent (QKeyEvent *e)
{
    QTextCursor cur = this->textCursor();
    if(ctrlPressed) {
        ctrlPressed = false;
        cur.setPosition(cur.anchor());
        //qDebug() << "keyReleaseEvent ctrlReleased";
    }
    QPlainTextEdit::keyReleaseEvent(e);
    if(expectAutoComplete) {
#if defined(Q_OS_MAC)
        if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
            this->undo();
#endif
        expectAutoComplete = false;
    }
}

void Editor::clearCtrlPressed()
{
    ctrlPressed = false;
}

void Editor::mouseMoveEvent (QMouseEvent *e)
{
    mousepos = e->pos();
    //qDebug() << "mouseMoveEvent " << mousepos.x() << "," << mousepos.y();
    QPlainTextEdit::mouseMoveEvent(e);
}

void Editor::mousePressEvent (QMouseEvent *e)
{
    if(ctrlPressed) {
        qDebug() << "mousePressEvent ctrlPressed (not implemented yet)";
        //static_cast<MAINWINDOW*>(mainwindow)->findDeclaration(e->pos());
        ctrlPressed = false;
    }
    QPlainTextEdit::mousePressEvent(e);
}

void Editor::mouseDoubleClickEvent (QMouseEvent *e)
{
    QPlainTextEdit::mouseDoubleClickEvent(e);
    static_cast<MAINWINDOW*>(mainwindow)->findMultilineComment(e->pos());
    mousepos = e->pos();
}

int Editor::autoIndent()
{
    QTextCursor cur = this->textCursor();
    if(cur.selectedText().length() > 0) {
        return 0;
    }

    int col = cur.columnNumber();

    cur.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor,col);
    QString text = cur.selectedText();
    cur.clearSelection();

    QRegExp sections("\\b(con|dat|pub|pri|obj|var)\\b");
    sections.setCaseSensitivity(Qt::CaseInsensitive);

    // handle indent for spin
    int stop = -1;
    int indent = -1;
    int slcm = text.indexOf("'");

    if(slcm > -1) {
        stop = slcm;
    }

    cur.beginEditBlock();

    cur.insertBlock();

    for(int n = 0; n <= stop || isspace(text[n].toLatin1()); n++) {
        if(n == slcm) {
            if(text.indexOf("''") > -1)
                cur.insertText("''");
            else
                cur.insertText("'");
        }
        else {
            cur.insertText(" ");
        }
    }

    if(indent > 0) {
        for(int n = 0; n < indent; n++) {
            cur.insertText(" ");
        }
    }

    this->setTextCursor(cur);
    cur.endEditBlock();

    return 1;
}


bool Editor::isCommentOpen(int line)
{
    // find out if there is a brace mismatch
    QString s = this->toPlainText();
    QStringList sl = s.split("\n",QString::SkipEmptyParts);
    int length = sl.length();

    int como = 0;
    int comc = 0;

    for(int n = 0; n < length; n++) {
        QString s = sl.at(n);
        if(s.contains("/*")) {
            como++;
        }
        if(s.contains("*/")) {
            comc++;
        }
        if(line == n && como > comc)
            return true;
    }
    return !(como == comc);
}

bool Editor::isSpinCommentOpen(int line)
{
    // find out if there is a brace mismatch
    QString s = this->toPlainText();
    QStringList sl = s.split("\n",QString::SkipEmptyParts);
    int length = sl.length();

    int como = 0;
    int comc = 0;

    for(int n = 0; n < length; n++) {
        QString s = sl.at(n);
        if(s.contains("{")) {
            como++;
        }
        if(s.contains("}")) {
            comc++;
        }
        if(line == n && como > comc)
            return true;
    }
    return !(como == comc);
}

int Editor::braceMatchColumn()
{
    int position = this->textCursor().columnNumber();
    int indent = propDialog->getTabSpaces();

    if(position <= indent)
        return 0;

    QTextCursor cur = this->textCursor();

    /* if this line has a brace, ignore */
    cur.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
    QString text = cur.selectedText();

    cur.clearSelection();
    if(text.contains("{"))
        return 0;

    // find out if there is a brace mismatch
    QString s = this->toPlainText();
    QStringList sl = this->toPlainText().split(QRegExp("/\\*.*\\*/"));
    s = sl.join("\n");
    sl = s.split("\n",QString::SkipEmptyParts);
    int length = sl.length();

    int braceo = 0;
    int bracec = 0;

    for(int n = 0; n < length; n++) {
        QString s = sl.at(n);
        if(s.contains("{")) {
            braceo++;
        }
        if(s.contains("}")) {
            bracec++;
        }
    }

    if(braceo == bracec) {
        return 0;
    }

    // if brace mismatch, set closing brace
    cur.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor,1);
    cur.movePosition(QTextCursor::Down,QTextCursor::KeepAnchor,1);

    text = cur.selectedText();
    cur.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
    if(text.length() == 0)
        return 0;

    //cur.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor,text.length());
    setTextCursor(cur);
    cur.clearSelection();

    /* start a single undo/redo operation */
    cur.beginEditBlock();

    for(int n = 0; n < position-indent; n++) {
        cur.insertText(" ");
    }
    cur.insertText("}");

    this->setTextCursor(cur);
    /* end a single undo/redo operation */
    cur.endEditBlock();

    return 1;
}

QString Editor::spinPrune(QString s)
{
    QRegExp re("\\b(byte|long|word|org)\\b");
    re.setCaseSensitivity(Qt::CaseInsensitive);
    s = s.mid(s.indexOf("\t")+1);
    if(s.lastIndexOf(")") > 0)
        s = s.mid(0,s.lastIndexOf(")")+1);
    if(s.lastIndexOf(":") > 0)
        s = s.mid(0,s.lastIndexOf(":"));
    if(s.lastIndexOf("|") > 0)
        s = s.mid(0,s.lastIndexOf("|"));
    if(s.lastIndexOf("[") > 0)
        s = s.mid(0,s.lastIndexOf("["));
    if(s.lastIndexOf("=") > 0)
        s = s.mid(0,s.lastIndexOf("="));
    if(s.indexOf(re) > 0)
        s = s.mid(0,s.indexOf(re));

    return s.trimmed();
}

QString Editor::deletePrefix(QString s)
{
    QRegExp re("\\b(byte|long|word)\\b");
    re.setCaseSensitivity(Qt::CaseInsensitive);

    QRegExp sections("\\b(con|dat|pub|pri|obj|var)\\b");
    sections.setCaseSensitivity(Qt::CaseInsensitive);
    if(s.indexOf(sections) == 0) {
        s = s.mid(s.indexOf(sections)+4);
    }
    else if(s.indexOf(re) == 0) {
        s = s.mid(s.indexOf(re)+5);
    }
    return s;
}

int Editor::addAutoItem(QString type, QString s)
{
    int width = 0;

    QIcon icon;
    bool split = false;

    QString ch = type.at(0);

    if(ch.compare("c") == 0) {
        icon.addFile(":/icons/block-con.png");
        split = true;
    } else if(ch.compare("e") == 0) {
        icon.addFile(":/icons/block-con.png");
        split = true;
    } else if(ch.compare("o") == 0) {
        icon.addFile(":/icons/blocks.png");
    } else if(ch.compare("p") == 0) {
        icon.addFile(":/icons/block-pri.png");
    } else if(ch.compare("f") == 0) {
        icon.addFile(":/icons/block-pub.png");
    } else if(ch.compare("v") == 0) {
        icon.addFile(":/icons/block-var.png");
        split = true;
    } else if(ch.compare("x") == 0) {
        icon.addFile(":/icons/block-dat.png");
        split = true;
    }

    QStringList lst;

    if(deletePrefix(s).length() != 0) {
        s = spinPrune(deletePrefix(s));
    }
    if(split)
        lst = s.split(",", QString::SkipEmptyParts);
    else
        lst.append(s);

    foreach (QString name, lst) {
        int len = cbAuto->count();
        int skip = 0;
        for(int n = 0; n < len; n++) {
            // linear search ok for short list ... maybe optimize with hash later.
            if(cbAuto->itemText(n).compare(name) == 0)
                skip++;
        }
        if(!skip) {
            name = name.trimmed();
            if(name.length() > width)
                width = name.length();
            cbAuto->addItem(icon,name);
        }
    }
    return width;
}

void Editor::spinAutoShow(int width)
{
    QPoint pt = keyPopPoint(textCursor());
    int fw = cbAuto->fontMetrics().width(QLatin1Char('9'));
    int fh = cbAuto->fontMetrics().height();
    cbAuto->setGeometry(pt.x(), pt.y(), (width+10)*fw, fh*10);

    expectAutoComplete = true;
    cbAuto->move(pt.x(), pt.y());
    cbAuto->setFrame(false);
    cbAuto->setEditable(true);
    cbAuto->setMaxVisibleItems(10);
    cbAuto->setAutoCompletion(true);
    cbAuto->showPopup();
}

bool Editor::isNotAutoComplete()
{
    QTextCursor cur = this->textCursor();
    cur.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    if(cur.selectedText().contains("'")) {
        return true;
    }

    while(cur.atStart() == false) {
        if(cur.selectedText().contains("{")) {
            if(cur.selectedText().contains("}") != true)
                return true;
            else
                return false;
        }
        cur.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
    }

    QTextCursor qcur = this->textCursor();
    int column = qcur.columnNumber();
    qcur.select(QTextCursor::LineUnderCursor);
    QString text = qcur.selectedText();
    if(text.contains("\"")) {
        int instring = 0;
        for(int n = 0; n < text.length(); n++) {
            if(text.at(n) == '\"')
                instring ^= 1;
            if(instring && (n == column))
                return true;
        }
    }
    return false;
}

QString Editor::selectAutoComplete()
{
    QTextCursor cur = this->textCursor();
    int col;
    char ch;
    QString text = cur.selectedText();
    cur.removeSelectedText();
    while((col = cur.columnNumber()) > 0) {
        cur.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor,1);
        text = cur.selectedText();
        ch = text.at(0).toLatin1();
        if(isspace(ch))
            break;
    }
    return cur.selectedText().trimmed();
}


int Editor::spinAutoComplete()
{
    if(isNotAutoComplete())
        return 0;

    QString text = selectAutoComplete();
    // if(text.compare("#") == 0) // would like to autocomplete on previous object

    /*
     * we have an object name. get object info
     */
    if(text.length() > 0) {
        qDebug() << "keyPressEvent object dot pressed" << text;
        QStringList list = spinParser.spinSymbols(fileName,text);
        if(list.count() == 0)
            return 0;
        cbAuto->clear();
        // we depend on index item 0 to be the auto-start key
        cbAuto->addItem(".");
        if(list.count() > 0) {
            int width = 0;
            //list.sort(); // let the parser do this
            for(int j = 0; j < list.count(); j++) {
                QString s = list[j];
                QString type = s;

#ifdef SPIN_AUTOCON
                if(type.at(0) == 'c') // don't show con for object. - # shows con
                    continue;
                if(type.at(0) == 'e') // don't show enum for object. - # shows enums
                    continue;
#endif
                if(type.at(0) == 'o') // don't show obj for object.
                    continue;
                if(type.at(0) == 'p') // don't show pri for object.
                    continue;
                if(type.at(0) == 'v') // don't show var for object.
                    continue;
                if(type.at(0) == 'x') // don't show dat for object.
                    continue;

                s = spinPrune(s);
                int w = addAutoItem(type, s);
                if(w > width) width = w;

            }
            connect(cbAuto, SIGNAL(activated(int)), this, SLOT(cbAutoSelected0insert(int)));
            spinAutoShow(width);
        }
        return 1;
    }
    /*
     * no object name. get local info
     */
    else {
        //qDebug() << "keyPressEvent local dot pressed";
        QStringList list = spinParser.spinSymbols(fileName,"");
        if(list.count() == 0)
            return 0;
        cbAuto->clear();
        cbAuto->addItem(".");
        if(list.count() > 0) {
            int width = 0;
            // always put objects on top
            for(int j = 0; j < list.count(); j++) {
                QString s = list[j];
                QString type = s;
                if(type.at(0) == 'o') {
                    list.removeAt(j);
                    list.insert(0,s);
                }
            }
            // add all elements
            for(int j = 0; j < list.count(); j++) {
                QString s = list[j];
                QString type = s;
#ifdef SPIN_AUTOCON
                if(type.at(0) == 'c')
                    continue;
                if(type.at(0) == 'e')
                    continue;
#endif
                s = spinPrune(s);
                int w = addAutoItem(type, s);
                if(w > width) width = w;
            }
            connect(cbAuto, SIGNAL(activated(int)), this, SLOT(cbAutoSelected(int)));
            spinAutoShow(width);
        }
        return 1;
    }
    return 0;
}

int  Editor::spinAutoCompleteCON()
{
#ifdef SPIN_AUTOCON
    QString text = selectAutoComplete();

    if(text.length() > 0) {
        connect(cbAuto, SIGNAL(activated(int)), this, SLOT(cbAutoSelected0insert(int)));
        qDebug() << "keyPressEvent # pressed" << text;
        QStringList list = spinParser.spinConstants(fileName,text);
        if(list.count() == 0)
            return 0;
        cbAuto->clear();
        // we depend on index item 0 to be the auto-start key
        cbAuto->addItem(QString("#"));
        if(list.count() > 0) {
            int width = 0;
            list.sort();
            for(int j = 0; j < list.count(); j++) {
                QString s = list[j];
                QString type = s;
                s = spinPrune(s);
                int w = addAutoItem(type, s);
                if(w > width) width = w;
            }
            spinAutoShow(width);
        }
        return 1;
    }
    /*
     * no object name. get local info
     */
    else {
        connect(cbAuto, SIGNAL(activated(int)), this, SLOT(cbAutoSelected(int)));
        qDebug() << "keyPressEvent local # pressed";
        QStringList list = spinParser.spinConstants(fileName,"");
        if(list.count() == 0)
            return 0;
        cbAuto->clear();
        cbAuto->addItem(QString("#"));
        if(list.count() > 0) {
            int width = 0;
            list.sort();
            for(int j = 0; j < list.count(); j++) {
                QString s = list[j];
                QString type = s;
                s = spinPrune(s);
                int w = addAutoItem(type, s);
                if(w > width) width = w;
            }
            spinAutoShow(width);
        }
        return 1;
    }
#endif
    return 0;
}

void Editor::cbAutoSelected0insert(int index)
{
    disconnect(cbAuto,SIGNAL(activated(int)),this,SLOT(cbAutoSelected0insert(int)));

    QString s = cbAuto->itemText(index);
    QTextCursor cur = this->textCursor();

    s = deletePrefix(s);

    // we depend on index item 0 to be the auto-start key
    if(index != 0)
        cur.insertText(cbAuto->itemText(0)+s.trimmed());
    else
        cur.insertText(cbAuto->itemText(0));

    if(s.indexOf("(") > 0) {
        int left = s.length()-s.indexOf("(")-1;
        cur.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, left);
        cur.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, left-1);
        s = cur.selectedText();
        qDebug() << s;
    }

    this->setTextCursor(cur);
    cbAuto->hide();
}

void Editor::cbAutoSelected(int index)
{
    disconnect(cbAuto,SIGNAL(activated(int)),this,SLOT(cbAutoSelected(int)));

    QString s = cbAuto->itemText(index);
    QTextCursor cur = this->textCursor();
    s = deletePrefix(s);
    cur.insertText(s.trimmed());

    if(s.indexOf("(") > 0) {
        int left = s.length()-s.indexOf("(")-1;
        cur.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, left);
        cur.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, left-1);
        s = cur.selectedText();
        qDebug() << s;
    }

    this->setTextCursor(cur);
    cbAuto->hide();
}

int Editor::contextHelp()
{
    QTextCursor cur = this->cursorForPosition(mousepos);
    cur.select(QTextCursor::WordUnderCursor);
    QString text = cur.selectedText();
    qDebug() << "keyPressEvent F1 (not implemented yet)" << text;
    return 1;
}

int Editor::tabBlockShift()
{
    /* make tabs based on user preference - set by mainwindow */
    int tabSpaces = propDialog->getTabSpaces();
    QString tab(tabSpaces, ' ');

    QTextCursor cur = this->textCursor();

    /* do we have shift ? */
    bool shiftTab = QApplication::keyboardModifiers() & Qt::SHIFT;

    /* block is selected */
    if (cur.hasSelection() && cur.selectedText().contains(QChar::ParagraphSeparator)) {
        /* determine current selection */
        int curbeg = cur.selectionStart();
        int curend = cur.selectionEnd();

        /* create workable selection */
        cur.setPosition(curbeg, QTextCursor::MoveAnchor);
        cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        cur.setPosition(curend, QTextCursor::KeepAnchor);

        /* don't inflate last line selection if cursor is at start */
        if (!cur.selectedText().endsWith(QChar::ParagraphSeparator))
            cur.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

        /* get a list of lines in the selected block. keep empty lines */
        QStringList mylist = cur.selectedText().split(QChar::ParagraphSeparator);

        /* initialise curend limiter */
        int climit = cur.selectionEnd() - mylist.last().length();

        /* indent list */
        QString text;

        for(int n = 1; n <= mylist.length(); n++) {
            QString s = mylist[n-1];
            int size = s.length();

            /* ignore empty last line */
            if (size == 0 && n == mylist.length()) break;

            if (!shiftTab) s.insert(0, tab);                        // increase line indent
            else if (s.startsWith(tab)) s.remove(0, tabSpaces);     // decrease line indent
            else s.replace(QRegExp("^ *"), "");                     // remove leading spaces

            size -= s.length();                                     // size is now delta
            // inc/dec indent -ve/+ve
            /* rebuild block */
            text += s;
            if (n < mylist.length()) {
                text   += QChar::ParagraphSeparator;
                climit -= size;                                     // update limiter
            }

            /* adjust selection */
            if (n == 1) {
                curbeg -= size;                                     // only first line
                curbeg  = std::max(curbeg, cur.selectionStart());   // avoid underflow
            }
            curend = std::max(curend - size, climit);               // all but an empty last line
        }
        /* insert new block */
        if (cur.selectedText().length() != text.length())           // avoid empty undo actions
            cur.insertText(text);

        /* update selection */
        cur.setPosition(curbeg, QTextCursor::MoveAnchor);
        cur.setPosition(curend, QTextCursor::KeepAnchor);

    } else if (!shiftTab) {
        int column = cur.columnNumber() + (cur.selectionStart() - cur.position());
        cur.insertText(QString(tabSpaces - column % tabSpaces, ' '));
    } else {
        /* determine current selection */
        int curbeg = cur.selectionStart();
        int curend = cur.selectionEnd();

        /* create workable selection */
        cur.setPosition(curbeg, QTextCursor::MoveAnchor);
        cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        cur.movePosition(QTextCursor::EndOfLine,   QTextCursor::KeepAnchor);

        /* indent line */
        QString line = cur.selectedText();
        int size = line.length();

        if (line.startsWith(tab)) line.remove(0, tabSpaces);        // decrease line indent
        else line.replace(QRegExp("^ *"), "");                      // remove leading spaces

        /* adjust selection */
        curbeg = std::max(curbeg - size + line.length(), cur.selectionStart());
        curend = std::max(curend - size + line.length(), cur.selectionStart());

        /* insert new line */
        if (cur.selectedText().length() != line.length())           // avoid empty undo actions
            cur.insertText(line);

        /* update selection */
        cur.setPosition(curbeg, QTextCursor::MoveAnchor);
        cur.setPosition(curend, QTextCursor::KeepAnchor);
    }
    this->setTextCursor(cur);

    return 1;
}

int Editor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = fontMetrics().width(' ')*(digits+3);

    return space;
}

void Editor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth()-6, -4, -3, 0);
}

void Editor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);

    QPalette p = lineNumberArea->palette();
    lineNumberArea->setPalette(p);
}

void Editor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left()-2, cr.top()-3, lineNumberAreaWidth(), cr.height()+3));
}

void Editor::updateColors()
{
    qDebug() << Q_FUNC_INFO;
    colors = currentTheme->getColorList();
    colorsAlt = colors;

    QMap<ColorScheme::Color, ColorScheme::color>::iterator i;
    for (i = colorsAlt.begin(); i != colorsAlt.end(); ++i)
    {
        // little fun formula to create two editor tones when color updated.
        float colordiff = 1.0 - i.value().color.valueF();
        i.value().color = i.value().color.lighter(105+((int)10.0*colordiff ));
    }

    setHighlights();

    QPalette p = this->palette();
    p.setColor(QPalette::Text, colors[ColorScheme::SyntaxText].color);
    p.setColor(QPalette::Base, colors[ColorScheme::ConBG].color);
    this->setPalette(p);


    updateBackgroundColors();

}

void Editor::updateFonts()
{
    this->setFont(currentTheme->getFont());
}

void Editor::paintEvent(QPaintEvent *event) {
    QPlainTextEdit::paintEvent(event);
//    const QRect rect = event->rect();
//    qDebug() << "RECT" << rect;
//
//    QPainter p(viewport());
//    p.setPen(QPen("gray"));
//    p.drawLine(30, rect.top(), 30, rect.bottom());
//
//
//    QFontMetrics font_metrics = QFontMetrics(currentTheme->getFont());
//    int fw = font_metrics.width('X');
//    int fh = font_metrics.height();
//
//    int longest_line = 20;
//    int from_top = 0;
//
//    for (int i = 0; i < longest_line; i++)
//    {
//        int the_x = (i * 4 * fw);
//        p.drawLine(the_x, from_top, the_x, from_top + fh);
//    }
}

void Editor::updateBackgroundColors()
{
    QList<QTextEdit::ExtraSelection> OurExtraSelections;
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(colors[ColorScheme::ConBG].color);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    selection.cursor.movePosition(QTextCursor::Start);

    int prevColor = 0;
    bool bAlt = false;
    QTextBlock currBlock = document()->firstBlock();

    QColor newBlockColor = colors[ColorScheme::ConBG].color;

    while (1)
    {
        ColorScheme::Color newColor = ColorScheme::Invalid;
        QString text = currBlock.text();

        if (text.contains(QRegExp("^CON\\b", Qt::CaseInsensitive)))
        {
            newColor = ColorScheme::ConBG;
        }
        else if (text.contains(QRegExp("^VAR\\b", Qt::CaseInsensitive)))
        {
            newColor = ColorScheme::VarBG;
        }
        else if (text.contains(QRegExp("^OBJ\\b", Qt::CaseInsensitive)))
        {
            newColor = ColorScheme::ObjBG;
        }
        else if (text.contains(QRegExp("^PUB\\b", Qt::CaseInsensitive)))
        {
            newColor = ColorScheme::PubBG;
        }
        else if (text.contains(QRegExp("^PRI\\b", Qt::CaseInsensitive)))
        {
            newColor = ColorScheme::PriBG;
        }
        else if (text.contains(QRegExp("^DAT\\b", Qt::CaseInsensitive)))
        {
            newColor = ColorScheme::DatBG;
        }

        if (newColor != ColorScheme::Invalid)
        {
            newBlockColor = colors[newColor].color;
            if (newColor == prevColor)
            {
                if (!bAlt)
                {
                    newBlockColor = colorsAlt[newColor].color;
                }
                bAlt = !bAlt;
            }
            else
            {
                bAlt = false;
            }
            selection.format.setBackground(newBlockColor);
            prevColor = newColor;
        }

        selection.cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        selection.cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
        OurExtraSelections.append(selection);

        if (currBlock == textCursor().block())
        {
            QTextEdit::ExtraSelection line;
            line.cursor = textCursor();
            line.cursor.clearSelection();
            line.cursor.movePosition(QTextCursor::StartOfLine);
            line.cursor.movePosition(QTextCursor::NextBlock,QTextCursor::KeepAnchor);
            if (newBlockColor.lightness() < 128)
                line.format.setBackground(newBlockColor.lighter(125));
            else
                line.format.setBackground(newBlockColor.darker(105));
            line.format.setProperty(QTextFormat::FullWidthSelection, true);
            OurExtraSelections.append(line);
        }

        if (currBlock == document()->lastBlock())
        {
            break;
        }

        currBlock = currBlock.next();
    }
    setExtraSelections(OurExtraSelections);
}


void Editor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);

    QColor bgcolor = currentTheme->getColor(ColorScheme::ConBG);
    painter.fillRect(event->rect(), bgcolor.darker(105));

    int x1 = event->rect().left();
    int x2 = x1 + 15;
    int y = event->rect().top();

    QLinearGradient gradient(x1, y, x2, y);
    gradient.setColorAt(0, bgcolor.darker(115));
    gradient.setColorAt(1, bgcolor.darker(105));
    painter.fillRect(event->rect(), gradient);

    QColor pen;
    int light = bgcolor.lightness();
    if (light < 30)
    {
        pen = currentTheme->getColor(ColorScheme::SyntaxText).darker(150);
    }
    else
    {
        if (bgcolor.lightness() < 128)
            pen = bgcolor.lighter(170);
        else
            pen = bgcolor.darker(170);
    }

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(pen);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                    Qt::AlignRight, number+" ");
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}


void Editor::tabSpacesChanged()
{
    this->setTabStopWidth(
            propDialog->getTabSpaces() *
            QFontMetrics(currentTheme->getFont()).width(' ')
            );
}

void Editor::setUndo(bool available)
{
    canUndo = available;
}
bool Editor::getUndo()
{
    return canUndo;
}


void Editor::setRedo(bool available)
{
    canRedo = available;
}
bool Editor::getRedo()
{
    return canRedo;
}


void Editor::setCopy(bool available)
{
    canCopy= available;
}
bool Editor::getCopy()
{
    return canCopy;
}

