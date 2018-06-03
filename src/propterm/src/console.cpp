#include "console.h"

#include <QScrollBar>
#include <QTextBlock>

#include <QDebug>

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent), minColumns(80), maxColumns(256), minRows(25), maxRows(100)
{
    setMaximumBlockCount(100);

    enable(true);

    addLinefeed = false;
    paused = false;
    pstMode = true;
    sendCursor = false;
    pasting = false;
    lastChar = 0;
    lastChar2 = 0;
    lastCursor = textCursor();
    columns = minColumns;
    rows = minRows;
    clear();
    myParent = parent;
}

int Console::cursorX(QTextCursor cursor)
{
    return cursor.positionInBlock();
}

int Console::cursorY(QTextCursor cursor)
{
    return cursor.block().firstLineNumber();
}

int Console::lineW(QTextCursor cursor)
{
    cursor.movePosition(QTextCursor::EndOfLine);
    return cursorX(cursor);
}

QTextCursor Console::moveRight(QTextCursor cursor)
{
    if (cursorX(cursor) < columns-1)
        cursor.movePosition(QTextCursor::Right);
    else
        cursor.movePosition(QTextCursor::NextBlock);
    return cursor;

}

QTextCursor Console::moveLeft(QTextCursor cursor)
{
    if (cursorX(cursor) > 0)
        cursor.movePosition(QTextCursor::Left);
    else
        if (cursor.movePosition(QTextCursor::PreviousBlock))
            cursor = positionX(cursor, columns-1);
    return cursor;
}

QTextCursor Console::moveDown(QTextCursor cursor)
{
    int x = cursorX(cursor);
    if (cursor.movePosition(QTextCursor::NextBlock))
        if (x > 0)
            cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor, x);
    return cursor;
}

QTextCursor Console::moveUp(QTextCursor cursor)
{
    int x = cursorX(cursor);
    if (cursor.movePosition(QTextCursor::PreviousBlock))
        if (x > 0)
            cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor, x);
    return cursor;
}

QTextCursor Console::positionX(QTextCursor cursor, int x)
{
    cursor.movePosition(QTextCursor::StartOfBlock);
    if (x > (columns-1))
        x = columns-1;
    if (x)
        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, x);
    return cursor;
}


QTextCursor Console::positionY(QTextCursor cursor, int y)
{
    int x = cursorX(cursor);
    cursor.movePosition(QTextCursor::Start);
    if (y)
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, y);
    if (x)
        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, x);
    return cursor;
}

void Console::clear()
{
    QPlainTextEdit::clear();
    QTextCursor cursor = textCursor();
    insertBlankLine(rows);
    if (pstMode)
        cursor.movePosition(QTextCursor::Start);
    else
    {
        cursor.movePosition(QTextCursor::End);
        cursor.movePosition(QTextCursor::StartOfBlock);
    }
    lastCursor = cursor;
    setTextCursor(cursor);
}

void Console::insertBlankLine(int count)
{
    while (count--)
    {
        for (int j=0; j<columns; j++)
            insertPlainText(" ");
        if (count) // add return if more blank lines to come
            insertPlainText("\n");
    }
}

void Console::putData(QByteArray data)
{
    int tab, xpos, ypos;
    paused = true;
    setTextCursor(lastCursor);
    foreach(char c, data)
    {
        if (pstMode)
        {
            QTextCursor cursor = textCursor();

            // backspace, tab, new line, and line feed
            // do require special implementations
            // beep speaker is no longer a relevant operation
            if (lastChar == 14 || lastChar == 2)
            {
                cursor = positionX(cursor, c);
            }
            else if (lastChar == 15 || lastChar2 == 2)
            {
                cursor = positionY(cursor, c);
                c = 0;
            }
            else
            {
                switch (c) {

                case 16:    // clear screen
                            clear();
                            cursor = textCursor();
                            break;

                case 11:    // clear to end of line
                            xpos = textCursor().positionInBlock();
                            cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                            cursor.removeSelectedText();
                            for (int j=xpos; j<columns; j++)
                                insertPlainText(" ");
                            cursor = positionX(cursor, xpos);
                            break;

                case 12:    // clear lines below
                            ypos = cursor.blockNumber();
                            xpos = textCursor().positionInBlock();
                            cursor.movePosition(QTextCursor::End,
                                    QTextCursor::KeepAnchor);
                            cursor.removeSelectedText();
                            for (int j=xpos; j<columns; j++)
                                insertPlainText(" ");
                            if (cursor.blockNumber() < (rows-1))
                            {
                                insertPlainText("\n");
                                insertBlankLine(rows-cursor.blockNumber());
                            }
                            cursor = positionX(cursor, xpos);
                            cursor = positionY(cursor, ypos);
                            break;

                case  1:    // home cursor
                            cursor.movePosition(QTextCursor::Start);
                            break;

                case  9:    // tab
                            tab = 8 - (textCursor().positionInBlock() & 7);
                            if (textCursor().positionInBlock()+tab < columns)
                                cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, tab);
                            break;

                case 10:    // line feed
                            // qDebug() << "Linefeed";
                case 13:    // new line, handled below
                            break;

                case  2:    // position x, y
                case 14:    // position x cursor
                case 15:    // position y cursor
                            break;
                case  3:    
                            cursor = moveLeft(cursor);
                            break;
                case  4:    
                            cursor = moveRight(cursor);
                            break;
                case  5:
                            cursor = moveUp(cursor);
                            break;
                case  6:
                            cursor = moveDown(cursor);
                            break;
                case  127: // treat DEL as BS too
                case  8:
                            if (cursorX(cursor) == 0)
                                break;
                            cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
                            cursor.removeSelectedText();
                            insertPlainText(" ");
                            if (cursorX(cursor) > 0)
                                cursor.movePosition(QTextCursor::Left);
                            break;
                default:
                    if (c > 31) 
                    {   // show if printable ASCII 
                        moveCursor(QTextCursor::Right, QTextCursor::KeepAnchor);
                        insertPlainText(QString(c));
                    }
                }
            }
            setTextCursor(cursor);
        }
        else
        {
            if ((c == 8 || c == 127) && textCursor().positionInBlock() > 0)
            {   // be nice and also at least provide BS/DEL support in non-PST (minimal) mode
                moveCursor(QTextCursor::Left, QTextCursor::KeepAnchor);
                textCursor().removeSelectedText();
                insertPlainText(" ");
                if (textCursor().positionInBlock()> 0)
                    moveCursor(QTextCursor::Left);
            }
            else if (c > 31)
            {
                moveCursor(QTextCursor::Right, QTextCursor::KeepAnchor);
                insertPlainText(QString(c));
            }
        }

        xpos = textCursor().positionInBlock();
        if (xpos == columns || c == 13)  // wrap to next line
        {
            QTextCursor cursor = textCursor();
            if (!cursor.movePosition(QTextCursor::NextBlock))
            {
                cursor.movePosition(QTextCursor::Start);
                cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
                cursor.movePosition(QTextCursor::End);
                setTextCursor(cursor);
                insertPlainText("\n");
                cursor.movePosition(QTextCursor::NextBlock);
                cursor.movePosition(QTextCursor::StartOfBlock);
                setTextCursor(cursor);
                insertBlankLine(1);
                cursor.movePosition(QTextCursor::StartOfBlock);
            }
            setTextCursor(cursor);
        }

        lastChar2 = lastChar;
        lastChar = c;
        lastCursor = textCursor();
    }

    QScrollBar *bar = verticalScrollBar();
    if (pstMode)
        bar->setValue(bar->minimum()); // keep top visible
    else
        bar->setValue(bar->maximum()); // keep bottom visible
    paused = false;
}

void Console::enable(bool set)
{
    if (set)
    {
        setStyleSheet("Console {"
                      "  background-image: url(:/icons/propterm/term-bg-blue.png);"
                      "  background-repeat: no-repeat;"
                      "  background-position: center;"
                      "  background-color: #000030;"
                      "}");
    }
    else
    {
        setStyleSheet("Console {"
                      "  background-image: url(:/icons/propterm/term-bg-grey.png);"
                      "  background-repeat: no-repeat;"
                      "  background-position: center;"
                      "  background-color: #000000;"
                      "}");
    }
}

void Console::setPstMode(bool enable)
{
    pstMode = enable;
    clear();
    QScrollBar *bar = verticalScrollBar();
    if (pstMode)
        bar->setValue(bar->minimum()); // keep top visible
    else
        bar->setValue(bar->maximum()); // keep bottom visible
}

void Console::setEcho(bool enable)
{
    echoMode = enable;
}

void Console::setAddLinefeed(bool enable)
{
    addLinefeed = enable;
}

void Console::setSendCursor(bool enable)
{
    sendCursor = enable;
}

void Console::insertFromMimeData(const QMimeData *source)
{
    QString lf(10);
    QString crlf(13);
    crlf += lf;
    if (source->hasText())
    {
        QString data(source->data("text/plain"));
        if (data.contains(10) && !data.contains(13)) // has LF only
        {
            if (addLinefeed)
                data = data.replace(lf,crlf); // prepend CR to LF
            else
                data = data.replace(10,13);  // replace LF with CR
        }
        emit getData(data.toLocal8Bit());
    }
}

void Console::dropEvent(QDropEvent *e)
{
    insertFromMimeData(e->mimeData());
}

void Console::keyPressEvent(QKeyEvent *e)
{
    paused = true;

    if (e->modifiers() & Qt::ControlModifier)
    {
        switch (e->key())
        {
            case Qt::Key_C:
            case Qt::Key_Copy:
                copy();
                break;
            case Qt::Key_V:
            case Qt::Key_Paste:
                paste();
                break;
        }
    }
    else
    {
        switch (e->key())
        {
            case Qt::Key_Left:  if (sendCursor) emit getData(QByteArray(1,3)); break;
            case Qt::Key_Right: if (sendCursor) emit getData(QByteArray(1,4)); break;
            case Qt::Key_Up:    if (sendCursor) emit getData(QByteArray(1,5)); break;
            case Qt::Key_Down:  if (sendCursor) emit getData(QByteArray(1,6)); break;
            case Qt::Key_Backspace:
                    emit getData(QByteArray(1,8));
                break;
            default:
                emit getData(e->text().toLocal8Bit());
                QByteArray lf(1,10);
                if ((e->text().toLocal8Bit()[0] == '\r') && addLinefeed)
                    emit getData(lf); // add lf to cr
        }
    }

    paused = false;
}

void Console::mousePressEvent(QMouseEvent *e)
{
    if (!paused)
    {
        QPlainTextEdit::mousePressEvent(e);
    }
}

void Console::updateSize(int newColumns, int newRows)
{
    bool rowIncrease = false;
    bool rowDecrease = false;
    bool colIncrease = false;
    bool colDecrease = false;
    QTextCursor cursor = textCursor();

    if (newRows > rows && newRows < maxRows) 
        rowIncrease = true;
    else if (newRows < rows && newRows >= minRows) 
        rowDecrease = true;

    if (newColumns > columns && newColumns < maxColumns) 
        colIncrease = true;
    else if (newColumns < columns && newColumns >= minColumns) 
        colDecrease = true;
    
    // try to retain cursor position as much as possible
    int xpos = cursorX(cursor);
    int ypos = cursorY(cursor);
    // do column width change first
    if (colIncrease)
    {
        //qDebug() << "increase columns by" << newColumns-columns;
        for (int i = 0; i<rows; i++)
        {
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, columns);
            cursor = positionY(cursor, i);
            //cursor.movePosition(QTextCursor::EndOfLine);
            setTextCursor(cursor);
            setOverwriteMode(false); // enable insertion temporarily
            for (int j = columns; j < newColumns; j++)
                insertPlainText(QString(" "));
            setOverwriteMode(true);
        } 
        columns = newColumns;
    }
    if (colDecrease)
    {
        //qDebug() << "decrease columns by" << columns-newColumns;
        if (xpos > newColumns-1)
            xpos = newColumns-1; // reduce X position
        for (int i = 0; i<rows; i++)
        {
            cursor = positionY(cursor, i);
            cursor.movePosition(QTextCursor::EndOfLine);
            cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, columns - newColumns);
            cursor.removeSelectedText();
        } 
        columns = newColumns;
    }
    // do row change using new column width
    if (rowIncrease)
    {
        //qDebug() << "increase rows by" << newRows-rows;
        if (pstMode) // increase at the bottom
        {
            cursor.movePosition(QTextCursor::End);
            cursor.insertText("\n");
            setTextCursor(cursor);
            insertBlankLine(newRows-rows);
        }
        else // increase at the top
        {
            cursor.movePosition(QTextCursor::Start);
            setTextCursor(cursor);
            insertBlankLine(newRows-rows);
            cursor.insertText("\n");
            ypos += (newRows-rows); // adjust cursor down
        }
        rows = newRows;
    }
    if (rowDecrease)
    {
        //qDebug() << "decrease rows by" << rows-newRows;
        if (ypos > newRows-1)
            ypos = newRows-1; // reduce Y position
        if (pstMode) // remove from bottom
        {
            cursor.movePosition(QTextCursor::End);
            cursor.movePosition(QTextCursor::Up, QTextCursor::KeepAnchor, rows - newRows);
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        }
        else // remove from top
        {
            cursor.movePosition(QTextCursor::Start);
            cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, rows - newRows);
        }
        cursor.removeSelectedText();
        rows = newRows;
    }
    cursor = positionX(cursor, xpos);
    cursor = positionY(cursor, ypos);
    setTextCursor(cursor);
    lastCursor = cursor;
    if (myParent)
        myParent->setWindowTitle(tr("Terminal (%1 x %2)").arg(columns).arg(rows));
}


void Console::updateColumns()
{
    int newColumns = (contentsRect().width() - 30) / QFontMetrics(font()).width(' ') - 1;
    int newRows = (contentsRect().height() - 12) / QFontMetrics(font()).lineSpacing();
    int l,t,r,b; 

    //qDebug() << "line.spacing" << QFontMetrics(font()).lineSpacing() << "fontwidth" << QFontMetrics(font()).width(' ');
    //qDebug() << "size.height" << contentsRect().height() << "size.width" << contentsRect().width();
    if (newRows < minRows)
        newRows = minRows;
    if (newRows > maxRows)
        newRows = maxRows;
    if (newColumns < minColumns)
        newColumns = minColumns;
    if (newColumns > maxColumns)
        newColumns = maxColumns;
    updateSize(newColumns, newRows);
    getContentsMargins(&l,&t,&r,&b);
    setMinimumHeight(minRows * QFontMetrics(font()).lineSpacing() + t + b + 10);// try to compensate for margins
    setMinimumWidth(minColumns * QFontMetrics(font()).width(' ') + l + r + 35); // magic #'s needs fixing
    //qDebug() << "new columns" << columns << "new rows" << rows;
}

void Console::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QScrollBar *bar = verticalScrollBar();
    updateColumns();
    if (pstMode)
        bar->setValue(bar->minimum()); // keep top visible
    else
        bar->setValue(bar->maximum()); // keep bottom visible
}

void Console::setFont(const QFont & font)
{
    QPlainTextEdit::setFont(font);
    //qDebug() << "setting new console font";
    updateColumns();
}
