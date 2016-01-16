#include "editor.h"

#include <QRect>
#include <QColor>
#include <QPainter>
#include <QApplication>

#include <QLinearGradient>
#include <QPlainTextEdit>

#include <ProjectParser>

#include "mainwindow.h"

Editor::Editor(QWidget *parent) : QPlainTextEdit(parent)
{
    propDialog = &((MainWindow *) parent)->preferences;

    ctrlPressed = false;
    expectAutoComplete = false;
    canUndo = false;
    canRedo = false;
    canCopy = false;
    tabOn = false;

    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth()));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    updateLineNumberAreaWidth();

    highlighter = new Highlighter(this->document());
    setMouseTracking(true);
    setCenterOnScroll(true);
    setWordWrapMode(QTextOption::NoWrap);

    currentTheme = &Singleton<ColorScheme>::Instance();
    updateColors();
    updateFonts();
    saveContent();

    connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(updateBackgroundColors()));
    connect(propDialog,SIGNAL(updateColors()),this,SLOT(updateColors()));
    connect(propDialog,SIGNAL(updateFonts(const QFont &)),this,SLOT(updateFonts()));
    connect(propDialog,SIGNAL(tabStopChanged()), this, SLOT(tabStopChanged()));

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

void Editor::saveContent()
{
    oldcontents = toPlainText();
}

int Editor::contentChanged()
{
    return oldcontents.compare(toPlainText());
}

void Editor::keyPressEvent (QKeyEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier)
    {
        QPlainTextEdit::keyPressEvent(e);
    }
    else
    {
        switch (e->key())
        {
            case Qt::Key_Enter:
            case Qt::Key_Return:
                if(autoIndent() == 0)
                {
                    QPlainTextEdit::keyPressEvent(e);
                }
                break;
            case Qt::Key_Period:
                if (propDialog->getAutoCompleteEnable())
                {
                    if(!spinAutoComplete())
                        QPlainTextEdit::keyPressEvent(e);
                }
                else
                {
                    QPlainTextEdit::keyPressEvent(e);
                }
                break;
            case Qt::Key_Tab:
                tabOn = true;
                if (textCursor().hasSelection())
                    tabBlockShift();
                else
                    indent();
                break;
            case Qt::Key_Backtab:
                tabOn = true;
                tabBlockShift();
                break;
            case Qt::Key_Backspace:
                if (tabOn)
                    dedent();
                else
                    QPlainTextEdit::keyPressEvent(e);
                break;
            case Qt::Key_Space:
                tabOn = false;
                QPlainTextEdit::keyPressEvent(e);
                break;
            default:
                QPlainTextEdit::keyPressEvent(e);
        }
    }
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

    int indent = -1;
    int slcm = text.indexOf("'");

    if(slcm > -1) {
        indent = slcm;
    }

    cur.beginEditBlock();

    cur.insertBlock();

    for(int n = 0; n <= indent || isspace(text[n].toLatin1()); n++) {
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

    this->setTextCursor(cur);
    cur.endEditBlock();

    return 1;
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
    QString text = selectAutoComplete();
    qDebug() << "keyPressEvent object dot pressed" << text;

    QSettings settings;
    settings.beginGroup("Paths");
    lang.parser.setLibraryPaths(QStringList() << settings.value("Library").toString());
    settings.endGroup();

    cbAuto->clear();
    cbAuto->addItem(".");

    QList<ProjectParser::Match> matches;
    if(text.length() > 0)
    {
        matches = lang.parser.matchRule("_includes_",toPlainText());

        QStringList filenames;
        foreach(ProjectParser::Match m, matches)
        {
            if (m.exact.contains(text))
                filenames << lang.parser.findFileName(m.pretty);
        }

        if (!(filenames.count() > 0))
            return 0;

        lang.parser.setFile(filenames[0]);

        matches = lang.parser.matchRuleFromFile("public",filenames[0]);
    }
    else
    {
        matches = lang.parser.matchRule("public",toPlainText());

        if (!(matches.count() > 0))
            return 0;
    }

    QStringList functions;

    int width = 0;
    foreach(ProjectParser::Match m, matches)
    {
        int w = addAutoItem("f", m.pretty.simplified());
        if(w > width) width = w;
    }

    connect(cbAuto, SIGNAL(activated(int)), this, SLOT(cbAutoSelected0insert(int)));
    spinAutoShow(width);
    return 1;
}

void Editor::cbAutoSelected0insert(int index)
{
    disconnect(cbAuto,SIGNAL(activated(int)),this,SLOT(cbAutoSelected0insert(int)));

    QString s = cbAuto->itemText(index);
    QTextCursor cur = this->textCursor();

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

void Editor::dedent()
{
    QTextCursor cursor = textCursor();
    int tabStop = propDialog->getTabSpaces();
    int column = cursor.columnNumber() - (cursor.selectionStart() - cursor.position());
    int spaces = tabStop - column % tabStop;

    cursor.beginEditBlock();

    for (int n = 0; n < spaces; n++)
    {
        if (!cursor.movePosition(QTextCursor::PreviousCharacter, 
                                 QTextCursor::KeepAnchor))
            break;

        QString c = cursor.selectedText();

        if (c == " ")
        {
            cursor.deleteChar();
        }
        else
        {
            if (n > 0)
                break;
            else
                cursor.deleteChar();
        }
    }

    cursor.endEditBlock();
}

void Editor::indent()
{
    QTextCursor cursor = textCursor();
    int tabStop = propDialog->getTabSpaces();
    int column = cursor.columnNumber() + (cursor.selectionStart() - cursor.position());
    int spaces = tabStop - column % tabStop;

    cursor.beginEditBlock();

    cursor.insertText(QString(spaces, ' '));

    cursor.endEditBlock();
}

void Editor::tabBlockShift()
{
    QTextCursor cur = textCursor();
    int tabStop = propDialog->getTabSpaces();
    QString tab(tabStop, ' ');
    bool shiftTab = QApplication::keyboardModifiers() & Qt::SHIFT;

    int curbeg = cur.selectionStart();
    int curend = cur.selectionEnd();

    cur.setPosition(curbeg, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cur.setPosition(curend, QTextCursor::KeepAnchor);

    if (!cur.selectedText().endsWith(QChar::ParagraphSeparator))
    {
        cur.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    }

    QStringList selectionblocks = cur.selectedText().split(QChar::ParagraphSeparator);

    int climit = cur.selectionEnd() - selectionblocks.last().length();

    QString text;

    for (int n = 1; n <= selectionblocks.length(); n++)
    {
        QString s = selectionblocks[n-1];
        int size = s.length();

        /* ignore empty last line */
        if (size == 0 && n == selectionblocks.length())
            break;

        if (!shiftTab)
        {
            s.insert(0, tab);                        // increase line indent
        }
        else if (s.startsWith(tab)) 
        {
            s.remove(0, tabStop);     // decrease line indent
        }
        else
        {
            s.replace(QRegExp("^ *"), "");                     // remove leading spaces
        }

        size -= s.length();                                     // size is now delta

        // inc/dec indent -ve/+ve
        /* rebuild block */
        text += s;
        if (n < selectionblocks.length())
        {
            text   += QChar::ParagraphSeparator;
            climit -= size;                                     // update limiter
        }

        /* adjust selection */
        if (n == 1)
        {
            curbeg -= size;                                     // only first line
            curbeg  = std::max(curbeg, cur.selectionStart());   // avoid underflow
        }
        curend = std::max(curend - size, climit);               // all but an empty last line
    }

    if (cur.selectedText().length() != text.length())           // avoid empty undo actions
        cur.insertText(text);

    cur.setPosition(curbeg, QTextCursor::MoveAnchor);
    cur.setPosition(curend, QTextCursor::KeepAnchor);

    setTextCursor(cur);
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

void Editor::updateLineNumberAreaWidth()
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
        updateLineNumberAreaWidth();

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

    QMap<ColorScheme::Color, ColorScheme::colorcontainer>::iterator i;
    for (i = colorsAlt.begin(); i != colorsAlt.end(); ++i)
    {
        // little fun formula to create two editor tones when color updated.
        float colordiff = 1.0 - i.value().color.valueF();
        i.value().color = i.value().color.lighter(105+((int)10.0*colordiff ));
    }

    highlighter->rehighlight();

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

            if (block == textCursor().block())
            {
                if (bgcolor.lightness() < 128)
                    painter.setPen(pen.lighter(140));
                else
                    painter.setPen(pen.darker(140));
            }
            else
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


void Editor::tabStopChanged()
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

