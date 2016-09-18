#include "editorview.h"

#include <QApplication>
#include <QColor>
#include <QLinearGradient>
#include <QPainter>
#include <QRect>

#include <ProjectParser>

#include "mainwindow.h"
#include "logging.h"

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(EditorView *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    EditorView * codeEditor;
};


EditorView::EditorView(QWidget *parent) : QPlainTextEdit(parent)
{
    highlighter = 0;
    setExtension("spin");

    canUndo = false;
    canRedo = false;
    canCopy = false;
    tabOn = false;

    currentTheme = &Singleton<ColorScheme>::Instance();
    loadPreferences();

    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth()));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    updateLineNumberAreaWidth();

    setMouseTracking(true);
    setCenterOnScroll(true);
    setWordWrapMode(QTextOption::NoWrap);

    updateColors();
    updateFonts();
    saveContent();

    connect(this,   SIGNAL(undoAvailable(bool)), this, SLOT(setUndo(bool)));
    connect(this,   SIGNAL(redoAvailable(bool)), this, SLOT(setRedo(bool)));
    connect(this,   SIGNAL(copyAvailable(bool)), this, SLOT(setCopy(bool)));

    // this must be a pointer otherwise we can't control the position.
    cbAuto = new QComboBox(this);
    cbAuto->setMaxVisibleItems(10);
    cbAuto->hide();
}

EditorView::~EditorView()
{
    cbAuto->clear();
    delete cbAuto;
    if (highlighter)
        delete highlighter;
    delete lineNumberArea;
}

void EditorView::loadPreferences()
{
    QSettings settings;

    settings.beginGroup("Features");
    tabStop       = settings.value("tabStop", 4).toInt();
    autoComplete  = settings.value("autoComplete", true).toBool();
    smartIndent   = settings.value("smartIndent", true).toBool();
    indentGuides  = settings.value("indentGuides", true).toBool();
    highlightLine = settings.value("highlightLine", true).toBool();
    settings.endGroup();

    setTabStopWidth(tabStop * QFontMetrics(currentTheme->getFont()).width(' '));
}

void EditorView::setExtension(QString ext)
{
    language.loadExtension(ext);
    parser = language.parser();
    blocks = language.listBlocks();
    re_blocks = language.buildTokenizer(blocks);

    if (highlighter)
        delete highlighter;

    highlighter = new Highlighter(ext, document());
}

void EditorView::saveContent()
{
    oldcontents = toPlainText();
}

int EditorView::contentChanged()
{
    return oldcontents.compare(toPlainText());
}

void EditorView::keyPressEvent(QKeyEvent * e)
{
    QTextCursor cursor = textCursor();

    if (e->modifiers() & Qt::ControlModifier)
    {
        switch (e->key())
        {
            case Qt::Key_Home:
                cursor.movePosition(QTextCursor::Start);
                setTextCursor(cursor);
                break;
            case Qt::Key_End:
                cursor.movePosition(QTextCursor::End);
                setTextCursor(cursor);
                break;

            default:
                QPlainTextEdit::keyPressEvent(e);
        }
    }
    else
    {
        switch (e->key())
        {
            case Qt::Key_Enter:
            case Qt::Key_Return:
                if (smartIndent)
                {
                    if(autoIndent() == 0)
                    {
                        QPlainTextEdit::keyPressEvent(e);
                    }
                }
                else
                    QPlainTextEdit::keyPressEvent(e);

                break;
            case Qt::Key_Period:
                if(!handleAutoComplete('.'))
                    QPlainTextEdit::keyPressEvent(e);
                break;
            case Qt::Key_NumberSign:
                if(!handleAutoComplete('#'))
                    QPlainTextEdit::keyPressEvent(e);
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

QPoint EditorView::keyPopPoint(QTextCursor cursor)
{
    int ht = fontMetrics().height();
    int wd = fontMetrics().width(' ');
    int col = cursor.columnNumber();
    int row = cursor.blockNumber() + 1; // show just below line

    QTextBlock block = firstVisibleBlock();
    int top = block.firstLineNumber();

    QPoint pt = QPoint(lineNumberAreaWidth()+col*wd,(row-top)*ht);
    return pt;
}

int EditorView::autoIndent()
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

int EditorView::addAutoCompleteItem(QString type, QString s)
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

QString EditorView::selectAutoComplete()
{
    QTextCursor cur = this->textCursor();
    QString text = cur.selectedText();
    cur.removeSelectedText();

    while(cur.columnNumber() > 0)
    {
        cur.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor,1);

        if(cur.selectedText().at(0).isSpace())
        {
            break;
        }
    }
    return cur.selectedText().trimmed();
}


int EditorView::handleAutoComplete(QChar c)
{
    if (!autoComplete) return 0;

    QString text = selectAutoComplete();
    qDebug() << "autocomplete" << text;

    cbAuto->clear();
    cbAuto->addItem(c);

    QList<ProjectParser::Match> matches;
    if(text.length() > 0)
    {
        matches = parser->matchRule("_includes_",toPlainText());

        QStringList filenames;
        foreach(ProjectParser::Match m, matches)
        {
            if (m.exact.contains(text))
                filenames << parser->findFileName(m.pretty);
        }

        if (!(filenames.count() > 0))
            return 0;

        if (c == '.')
            matches = parser->matchRuleFromFile("public",filenames[0]);
        else if (c == '#')
            matches = parser->matchRuleFromFile("constants",filenames[0]);
    }
    else
    {
        if (c == '.')
            matches = parser->matchRule("public",toPlainText());
        else if (c == '#')
            matches = parser->matchRule("constants",toPlainText());

        if (!(matches.count() > 0))
            return 0;
    }

    QStringList functions;

    int width = 0;
    foreach(ProjectParser::Match m, matches)
    {
        int w = addAutoCompleteItem("f", m.pretty.simplified());
        if(w > width) width = w;
    }

    connect(cbAuto, SIGNAL(activated(int)), this, SLOT(finishAutoComplete(int)));

    QPoint pt = keyPopPoint(textCursor());
    int fw = cbAuto->fontMetrics().width(' ');
    int fh = cbAuto->fontMetrics().height();
    cbAuto->setGeometry(pt.x(), pt.y(), width*fw, fh);
    cbAuto->showPopup();

    return 1;
}

void EditorView::finishAutoComplete(int index)
{
    disconnect(cbAuto,SIGNAL(activated(int)),this,SLOT(finishAutoComplete(int)));

    QString text = selectAutoComplete();

    QString s = cbAuto->itemText(index);
    QTextCursor cur = this->textCursor();

    // we depend on index item 0 to be the auto-start key
    if(index != 0)
    {
        if (text.length() > 0)
            cur.insertText(cbAuto->itemText(0)+s.trimmed());
        else
            cur.insertText(s.trimmed());
    }
    else
    {
        cur.insertText(cbAuto->itemText(0));
    }

    if(s.indexOf("(") > 0)
    {
        int left = s.length()-s.indexOf("(")-1;
        cur.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, left);
        cur.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, left-1);
        s = cur.selectedText();
    }

    this->setTextCursor(cur);
    cbAuto->hide();
}

void EditorView::dedent()
{
    QTextCursor cursor = textCursor();
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
            if (n == 0)
                cursor.deleteChar();
            break;
        }
    }

    cursor.endEditBlock();
}

void EditorView::indent()
{
    QTextCursor cursor = textCursor();
    int column = cursor.columnNumber() + (cursor.selectionStart() - cursor.position());
    int spaces = tabStop - column % tabStop;

    cursor.beginEditBlock();

    cursor.insertText(QString(spaces, ' '));

    cursor.endEditBlock();
}

void EditorView::tabBlockShift()
{
    QTextCursor cur = textCursor();
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

int EditorView::lineNumberAreaWidth()
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

void EditorView::updateLineNumberAreaWidth()
{
    setViewportMargins(lineNumberAreaWidth()-6, -4, -3, 0);
}

void EditorView::updateLineNumberArea(const QRect &rect, int dy)
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

void EditorView::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left()-2, cr.top()-3, lineNumberAreaWidth(), cr.height()+3));
}

void EditorView::updateColors()
{
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
}

void EditorView::updateFonts()
{
    this->setFont(currentTheme->getFont());
}

void EditorView::paintEvent(QPaintEvent * e)
{
    QPainter p(viewport());

    int top = 0, bottom = 0, prevColor = 0;
    bool bAlt = false;
    QColor newBlockColor = colors[ColorScheme::ConBG].color;

    QList<QTextEdit::ExtraSelection> selections;

    QTextBlock block = document()->firstBlock();
    while (block.isValid())
    {
        ColorScheme::Color newColor = ColorScheme::Invalid;
        QString text = block.text();

        if (text.contains(QRegularExpression("^CON\\b", QRegularExpression::CaseInsensitiveOption)))
        {
            newColor = ColorScheme::ConBG;
        }
        else if (text.contains(QRegularExpression("^VAR\\b", QRegularExpression::CaseInsensitiveOption)))
        {
            newColor = ColorScheme::VarBG;
        }
        else if (text.contains(QRegularExpression("^OBJ\\b", QRegularExpression::CaseInsensitiveOption)))
        {
            newColor = ColorScheme::ObjBG;
        }
        else if (text.contains(QRegularExpression("^PUB\\b", QRegularExpression::CaseInsensitiveOption)))
        {
            newColor = ColorScheme::PubBG;
        }
        else if (text.contains(QRegularExpression("^PRI\\b", QRegularExpression::CaseInsensitiveOption)))
        {
            newColor = ColorScheme::PriBG;
        }
        else if (text.contains(QRegularExpression("^DAT\\b", QRegularExpression::CaseInsensitiveOption)))
        {
            newColor = ColorScheme::DatBG;
        }

        if (newColor != ColorScheme::Invalid)
        {

            bottom = (int) blockBoundingGeometry(block).translated(contentOffset()).top();

            if (block.isVisible() && bottom > top)
            {
                p.fillRect(0, top, viewport()->width(), bottom - top, newBlockColor);
            }

            top = bottom;

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
            prevColor = newColor;
        }

        if (highlightLine)
        {
            if (block.blockNumber() == textCursor().blockNumber())
            {

                QTextEdit::ExtraSelection line;
                line.cursor = textCursor();
                line.cursor.clearSelection();
                line.cursor.movePosition(QTextCursor::StartOfLine);
                line.cursor.movePosition(QTextCursor::NextBlock,QTextCursor::KeepAnchor);
                line.format.setBackground(contrastColor(newBlockColor));
                line.format.setProperty(QTextFormat::FullWidthSelection, true);
                selections.append(line);
            }
        }

        if (block == document()->lastBlock())
        {
            bottom = (int) blockBoundingGeometry(block).translated(contentOffset()).bottom();

            if (block.isVisible() && bottom > top)
            {
                p.fillRect(0, top, viewport()->width(), bottom - top, newBlockColor);
            }
        }

        block = block.next();
    }
    setExtraSelections(selections);

    if (indentGuides)
    {
        int wd = fontMetrics().width(' ');
        int linew = font().pointSize()/6;

        block = firstVisibleBlock();
        QPen pen(contrastColor(colors[ColorScheme::ConBG].color, 30));
        pen.setWidth(linew);
        p.setPen(pen);

        while (block.isValid() && block.isVisible())
        {
                int y1 = (int) blockBoundingGeometry(block).translated(contentOffset()).top()+linew;
                int y2 = (int) blockBoundingGeometry(block).translated(contentOffset()).bottom()-linew;

                QString text = block.text();
                for (int i = 0; i < text.size(); i++)
                {
                    if (text[i] != ' ')
                        break;
                    
                    if (i % tabStop == 0)
                    {
                        int x = wd*i;
                        p.drawLine(x, y1, x, y2);
                    }
                }

            block = block.next();
        }
    }

    QPlainTextEdit::paintEvent(e);
}

QColor EditorView::contrastColor(QColor color, int amount)
{
    if (color.lightness() < 128)
        return QColor(255,255,255, amount);
    else
        return QColor(0,0,0, amount);
}

void EditorView::lineNumberAreaPaintEvent(QPaintEvent * event)
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

void EditorView::setUndo(bool available)
{
    canUndo = available;
}

bool EditorView::getUndo()
{
    return canUndo;
}

void EditorView::setRedo(bool available)
{
    canRedo = available;
}
bool EditorView::getRedo()
{
    return canRedo;
}

void EditorView::setCopy(bool available)
{
    canCopy= available;
}
bool EditorView::getCopy()
{
    return canCopy;
}

