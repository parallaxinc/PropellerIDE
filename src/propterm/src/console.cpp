#include "console.h"

#include <QScrollBar>
#include <QTextBlock>

#include <QDebug>

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent)
{
    setMaximumBlockCount(100);

    enable(true);

    paused = false;
    pstMode = true;
    lastChar = 0;
    lastChar2 = 0;
    lastCursor = textCursor();
}

int Console::cursorX(QTextCursor cursor)
{
    int position = cursor.position();
    cursor.movePosition(QTextCursor::StartOfBlock);
    int anchor = cursor.position();
    return position-anchor;
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
    if (cursorX(cursor) == lineW(cursor))
    {
        insertPlainText(" ");
        cursor.movePosition(QTextCursor::EndOfLine);
    }
    else
    {
        cursor.movePosition(QTextCursor::Right);
    }
    return cursor;

}

QTextCursor Console::moveDown(QTextCursor cursor)
{
    int x = cursorX(cursor);
    if (!cursor.movePosition(QTextCursor::Down))
    {
        cursor.movePosition(QTextCursor::End);
        insertPlainText("\n");
        while (cursorX(cursor) < x)
        {
            insertPlainText(" ");
            cursor.movePosition(QTextCursor::End);
        }
    }
    return cursor;
}

QTextCursor Console::moveUp(QTextCursor cursor)
{
    int x = cursorX(cursor);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::Up);
    cursor = positionX(cursor, x);
    return cursor;
}

QTextCursor Console::positionX(QTextCursor cursor, int x)
{
    cursor.movePosition(QTextCursor::StartOfBlock);
    for (int i = 0; i < x; i++)
    {
        cursor = moveRight(cursor);
    }
    return cursor;
}


QTextCursor Console::positionY(QTextCursor cursor, int y)
{
    int x = cursorX(cursor);
    cursor.movePosition(QTextCursor::Start);
    for (int i = 0; i < y; i++)
        cursor = moveDown(cursor);

    for (int i = 0; i < x; i++)
        cursor = moveRight(cursor);
    return cursor;
}


void Console::putData(QByteArray data)
{
    paused = true;
    setTextCursor(lastCursor);
    foreach(char c, data)
    {
        if (pstMode)
        {
            QTextCursor cursor = textCursor();

            // backspace, tab, new line, and line feed
            // do not require special implementations
            // beep speaker is no longer a relevant operation
            if (lastChar == 14 || lastChar == 2)
            {
                cursor = positionX(cursor, c);
            }
            else if (lastChar == 15 || lastChar2 == 2)
            {
                cursor = positionY(cursor, c);
            }
            else
            {
                switch (c) {

                case 16:    // clear screen
                            clear();
                            break;

                case 11:    // clear to end of line
                            cursor.movePosition(QTextCursor::EndOfLine,
                                    QTextCursor::KeepAnchor);
                            cursor.removeSelectedText();
                            break;

                case 12:    // clear lines below
                            cursor.movePosition(QTextCursor::End,
                                    QTextCursor::KeepAnchor);
                            cursor.removeSelectedText();
                            break;

                case  1:    // home cursor
                            cursor.movePosition(QTextCursor::Start);
                            break;

                case 10:    // line feed
                case 13:    // new line
                            insertPlainText("\n");
                            break;

                case  2:    // position x, y
                case 14:    // position x cursor
                case 15:    // position y cursor
                            break;
                case  3:    
                            cursor.movePosition(QTextCursor::Left);
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
                case  8:
                            cursor.deletePreviousChar();
                            break;
                default:
                    insertPlainText(QString(c));
                }
            }
            setTextCursor(cursor);
        }
        else
        {
            insertPlainText(QString(c));
        }

        if (textCursor().columnNumber() % columns == 0 
                && textCursor().positionInBlock() > 0
                && c != 10 
                && c != 13)
        {
            insertPlainText("\n");
        }

        lastChar2 = lastChar;
        lastChar = c;
        lastCursor = textCursor();
    }

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
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
}

void Console::setEcho(bool enable)
{
    echoMode = enable;
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
            case Qt::Key_Backspace:
            case Qt::Key_Left:
            case Qt::Key_Right:
            case Qt::Key_Up:
            case Qt::Key_Down:
                if (!echoMode)
                    emit getData(e->text().toLocal8Bit());
                break;
            default:
                emit getData(e->text().toLocal8Bit());
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

void Console::updateColumns()
{
    columns = width() / QFontMetrics(font()).width(' ') + 1;
}

void Console::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    updateColumns();
}

void Console::setFont(const QFont & font)
{
    QPlainTextEdit::setFont(font);
    updateColumns();
}
