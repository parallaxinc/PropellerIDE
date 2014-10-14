#include "terminal.h"
#include "console.h"
#include "Sleeper.h"
#include "graphwidget.h"

// It's Spinning Windows world.
#define SPIN_RETURN     0xA
#define SPIN_NEWLINE    0xD

Console::Console(QWidget *parent) : QPlainTextEdit(parent)
{
    setFont(QFont("courier"));
    isEnabled = true;
    pcmd = Console::PCMD_NONE;
    pcmdx = 0;
    pcmdy = 0;
    maxcol = 32;
    wrapMode = 0;
    tabsize = 8;
    hexmode = false;
    hexbytes = 0;
    maxhex = 16;
    for(int n = 0; n < maxhex; n++)
        hexbyte[n] = 0;
    sbuff = NULL;

#if defined(Q_OS_WIN32)
    newline = '\n';
    creturn = '\r';
#else
    newline = '\r';
    creturn = '\n';
#endif
}

void Console::setPortEnable(bool value)
{
    utfparse = false;
    utfbytes = 0;
    utf8 = 0;
    hexbytes = 0;
    for(int n = 0; n < maxhex; n++)
        hexbyte[n] = 0;
    isEnabled = value;
}

bool Console::enabled()
{
    return isEnabled;
}

void Console::clear()
{
    utfparse = false;
    utfbytes = 0;
    utf8 = 0;
    hexbytes = 0;
    for(int n = 0; n < maxhex; n++)
        hexbyte[n] = 0;
    setPlainText("");
}

void Console::eventKey(QKeyEvent* event)
{
    int key = event->key();

    switch(key)
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if(enableEnterIsNL)
            key = SPIN_NEWLINE;
        else
            key = SPIN_RETURN;
    case Qt::Key_Alt:
    case Qt::Key_Control:
    case Qt::Key_Shift:
        return;
    default:
        if(QApplication::keyboardModifiers() & Qt::CTRL) {
            key &= ~0xe0;
        }
        else {
            if(key & Qt::Key_Escape)
                return;

            QString s = event->text();
            if(s.length() == 1) {
                QChar c = event->text().at(0);
                key = (int)c.toLatin1();
            }
            else {
                sendPortMessage(s);
                return;
            }
        }
        break;
    }
}

void Console::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "keyPressEvent";

    Terminal *term = (Terminal*) this->parentWidget();
    if(event->matches((QKeySequence::Copy))) {
        term->copyFromFile();
    }
    else
    if(event->matches((QKeySequence::Paste))) {
        QClipboard *clip = QApplication::clipboard();
        QString s = clip->text();
        s = s.replace("\n","\r");
        this->sendPortMessage(s);
    }
    else {
        if(this->enableEchoOn) {
            //this->insertPlainText(event->text());
        }
        int key = event->key();

        switch(key)
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            if(enableEnterIsNL)
                key = SPIN_NEWLINE;
            else
                key = SPIN_RETURN;
            break;
        case Qt::Key_Alt:
        case Qt::Key_Control:
        case Qt::Key_Shift:
            return;
        default:
            if(QApplication::keyboardModifiers() & Qt::CTRL) {
                key &= ~0xe0;
            }
            else {
                if(key & Qt::Key_Escape)
                    return;

                QString s = event->text();
                if(s.length() == 1) {
                    QChar c = event->text().at(0);
                    key = (int)c.toLatin1();
                }
                else {
                    sendPortMessage(s);
                    return;
                }
            }
            break;
        }
        QByteArray barry;
        barry.append((char)key);
        emit sendSerial(barry);
    }
}

void Console::sendPortMessage(QString s)
{
    QByteArray barry;
    foreach(QChar c, s.toUtf8()) {
        Sleeper::ms(1);
        barry.append(c);
        emit sendSerial(barry);
        barry.clear();
        this->thread()->yieldCurrentThread();
    }
}

void Console::resizeEvent(QResizeEvent *e)
{
    QFontMetrics fm(this->font());
    maxcol = this->width()/8;
    if(fm.width("X") > 0) {
        maxcol = width()/fm.width("X")-3;
    }

    //qDebug() << maxcol << width() << fm.width("X");
    QPlainTextEdit::resizeEvent(e);
}

void Console::setEnableClearScreen(bool value)
{
     enableClearScreen = value;
}
void Console::setEnableHomeCursor(bool value)
{
     enableHomeCursor = value;
}
void Console::setEnablePosXYCursor(bool value)
{
     enablePosXYCursor = value;
}
void Console::setEnableMoveCursorLeft(bool value)
{
     enableMoveCursorLeft = value;
}
void Console::setEnableMoveCursorRight(bool value)
{
     enableMoveCursorRight = value;
}
void Console::setEnableMoveCursorUp(bool value)
{
     enableMoveCursorUp = value;
}
void Console::setEnableMoveCursorDown(bool value)
{
     enableMoveCursorDown = value;
}
void Console::setEnableBeepSpeaker(bool value)
{
     enableBeepSpeaker = value;
}
void Console::setEnableBackspace(bool value)
{
     enableBackspace = value;
}
void Console::setEnableTab(bool value)
{
     enableTab = value;
}
void Console::setEnableCReturn(bool value)
{
     enableCReturn = value;
}
void Console::setEnableClearToEOL(bool value)
{
     enableClearToEOL = value;
}
void Console::setEnableClearLinesBelow(bool value)
{
     enableClearLinesBelow = value;
}
void Console::setEnableNewLine(bool value)
{
     enableNewLine = value;
}
void Console::setEnablePosCursorX(bool value)
{
     enablePosCursorX = value;
}
void Console::setEnablePosCursorY(bool value)
{
     enablePosCursorY = value;
}
void Console::setEnableClearScreen16(bool value)
{
     enableClearScreen16 = value;
}
void Console::setEnableEchoOn(bool value)
{
     enableEchoOn = value;
}
void Console::setEnableEnterIsNL(bool value)
{
     enableEnterIsNL = value;
}


void Console::setEnableSwapNLCR(bool value)
{
     enableSwapNLCR = value;
     if(enableSwapNLCR) {
         newline = 10;
         creturn = 13;
     }
     else {
         newline = 13;
         creturn = 10;
     }
}

bool Console::getHexMode()
{
    return hexmode;
}

int Console::getEnter()
{
    if(enableEnterIsNL)
        return newline;
    return creturn;
}

void Console::setWrapMode(int mode)
{
    wrapMode = mode;
    if(mode == 0) {
        this->setWordWrapMode(QTextOption::WordWrap);
    }
    else {
        this->setWordWrapMode(QTextOption::WrapAnywhere);
    }
}

void Console::setTabSize(int size)
{
    tabsize = size;
}

void Console::setHexMode(bool enable)
{
    hexmode = enable;
}

void Console::setHexDump(bool enable)
{
    hexdump = enable;
}

void Console::setMaxRows(int rows)
{
     maxrow = rows;
}

void Console::setTabWidget(QTabWidget *tabs)
{
#ifdef ENABLE_TERM_TABS
    tabWidget = tabs;
#endif
}

bool Console::graphMode()
{
#ifdef ENABLE_TERM_TABS
    if(tabWidget) {
        if(tabWidget->currentIndex() > -1) {
            return tabWidget->tabText(tabWidget->currentIndex()).compare(TabConsole) != 0;
        }
    }
#endif
    return false;
}

#ifdef EVENT_DRIVEN
enum { BUFFERSIZE = 2048 };
#else
#ifdef Q_OS_WIN32
enum { BUFFERSIZE = 32 };
#else
enum { BUFFERSIZE = 64 };
#endif
#endif

void Console::updateReady(QextSerialPort* port)
{
    char buf[BUFFERSIZE];
    if(isEnabled == false)
        return;

    if(port->bytesAvailable() < 1)
        return;

    int length = port->readLine(buf,BUFFERSIZE);
    //int length = ba.length();

    if(length < 1)
        return;

    if(hexmode != false) {
        for(int n = 0; n < length; n++)
            dumphex((int)buf[n]);
    }
    else {
#ifdef EVENT_DRIVEN
        while (length > 0) {
            for(int n = 0; n < length; n++)
                update(buf[n]);
            length = port->readLine(buf,BUFFERSIZE);
        }
#else
        for(int n = 0; n < length; n++)
            update(buf[n]);
#endif
    }
}

void Console::graph(QString s)
{
    emit queueGraph(s);
}

void Console::dumphex(int ch)
{
    unsigned char c = ch;
    QTextCursor cur = this->textCursor();
    // always start at the end just in case someone clicked the window
    moveCursor(QTextCursor::End);

    if(hexdump != true) {
        if(wrapMode > 0) {
            if(cur.block().length() > wrapMode)
                cur.insertBlock();
        }
        else if(cur.block().length() > maxcol-2) {
            cur.insertBlock();
        }

        cur.insertText(QString(" %1").arg(c,2,16,QChar('0')));
    }
    else {
        int byte = hexbytes % maxhex;
        if(byte == 0) {
            cur.insertText(QString("  "));
            for(int n = 0; n < maxhex; n++) {
                if(isprint(hexbyte[n]))
                    cur.insertText(QString(hexbyte[n]));
                else
                    cur.insertText(QString("."));
            }
            cur.insertBlock();
        }
        hexbyte[byte] = ch;
        hexbytes++;

        cur.insertText(QString(" %1").arg(c,2,16,QChar('0')));
    }
}

void Console::update(char ch)
{
    QString text = "";
    QTextCursor cur = this->textCursor();

    this->setMaximumBlockCount(maxrow);
    this->setWordWrapMode(QTextOption::WrapAnywhere);

    if(wrapMode > 0) {
        if(cur.block().length() > wrapMode)
            cur.insertBlock();
    }
    else if(cur.block().length() > maxcol) {
        cur.insertBlock();
    }

    // always start at the end just in case someone clicked the window
    //moveCursor(QTextCursor::End);
    // now that we have cursor positioning we can't always start at the end.

    //qDebug(QString(" %1 %2").arg(ch, 2, 16, QChar('0')).arg(QChar(ch)).toAscii());
    //insertPlainText(QString(" %1 ").arg(ch, 2, 16, QChar('0')));
    //insertPlainText(QChar(ch));

    //int _x = cur.columnNumber();
    //int _y = cur.blockNumber();

    //qDebug() << QString("%1 %2 %3 %4 %5").arg(pcmd,2,10,QChar('0')).arg(pcmdx,2,10,QChar('0')).arg(pcmdy,2,10,QChar('0')).arg(ch,2,10,QChar('0')).arg(ch,1,QChar('0'));
    //qDebug() << QString("P %1,%2 X,Y %3,%4 %5").arg(pcmdx,2,10,QChar('0')).arg(pcmdy,2,10,QChar('0')).arg(cur.columnNumber(),2,10,QChar('0')).arg(cur.blockNumber(),2,10,QChar('0')).arg(ch,2,10,QChar('0'));
    switch(pcmd)
    {
        case PCMD_CURPOS_X: {
                pcmdx = ch;
                qDebug() << "CURPOS_X " << pcmdx;
#if 0
                cur.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
                cur.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,pcmdy);
                setTextCursor(cur);
#endif
#if 1
                int j = cur.block().length();
                for(; j <= pcmdx; j++) {
                    cur.movePosition(QTextCursor::EndOfLine,QTextCursor::MoveAnchor);
                    cur.insertText(" ");
                }
#endif
                cur.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
                if(pcmdx > 0) cur.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,pcmdx);
                setTextCursor(cur);
                pcmd = PCMD_NONE;
                qDebug() << "CURPOS_X Column" << cur.columnNumber();
            }
            break;

        case PCMD_CURPOS_Y: {
                pcmdy = ch;
                qDebug() << "CURPOS_Y " << pcmdy;
                int j = this->blockCount();
                for(; j <= pcmdy; j++) {
                    cur.movePosition(QTextCursor::End);
                    cur.insertBlock(cur.blockFormat(),cur.charFormat());
                }
                cur.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
                if(pcmdy > 0) cur.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,pcmdy);
                setTextCursor(cur);
#if 0
                cur.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
                cur.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,pcmdx);
                setTextCursor(cur);
#endif
                pcmd = PCMD_NONE;
                qDebug() << "CURPOS_Y Column" << cur.columnNumber();
            }
            break;

        case PCMD_CURPOS_XY: {
#if 1
            if(pcmdlen == 2) {
                pcmdx = ch;
            }
            else if(pcmdlen == 1) {
                pcmdy = ch;
                int j = this->blockCount();
                for(; j <= pcmdy; j++) {
                    cur.movePosition(QTextCursor::End);
                    cur.insertBlock();
                }
                cur.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
                if(pcmdy > 0)
                    cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, pcmdy);

                j = cur.block().length();
                for(; j <= pcmdx; j++) {
                    cur.movePosition(QTextCursor::EndOfLine,QTextCursor::MoveAnchor);
                    cur.insertText(" ");
                }
                cur.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
                if(pcmdx > 0)
                    cur.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,pcmdx);
                setTextCursor(cur);
            }
            pcmdlen--;
            if(pcmdlen < 1) {
                pcmd = PCMD_NONE;
            }
#endif
        }
        break;

        default: {


            if (ch & 0x80) {    //UTF-8 parsing and handling
                if (utfparse == true) {

                    utf8 <<= 6;
                    utf8 |= (ch & 0x3F);

                    utfbytes--;

                    if (utfbytes == 0) {
                        utfparse = false;
                        cur.insertText(QChar(utf8));
                    }
                } else {
                    utfparse = true;
                    utf8 = 0;

                    while (ch & 0x80) {
                        ch <<= 1;
                        utfbytes++;
                    }

                    ch >>= utfbytes;

                    utf8 = (int)ch;

                    utfbytes--;
                }
                return;
            }

            switch(ch)
            {
            case EN_ClearScreen: {
                    if(this->enableClearScreen) {
                        setPlainText("");
                    }
                }
                break;
            case EN_ClearScreen2: {
                    if(this->enableClearScreen16) {
                        setPlainText("");
                    }
                }
                break;

            case EN_HomeCursor: {
                    if(this->enableHomeCursor) {
                        cur.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
                        setTextCursor(cur);
                    }
                }
                break;

            case EN_PosCursorX: {
                    if(this->enablePosCursorX) {
                        pcmd = PCMD_CURPOS_X;
                        pcmdlen  = 1;
                    }
                }
                break;

            case EN_PosCursorY: {
                    if(this->enablePosCursorY) {
                        pcmd = PCMD_CURPOS_Y;
                        pcmdlen  = 1;
                    }
                }
                break;

            case EN_PosXYCursor: {
                    if(this->enablePosXYCursor) {
                        pcmd = PCMD_CURPOS_XY;
                        pcmdlen  = 2;
                    }
                }
                break;

            case EN_MoveCursorLeft: {
                    if(this->enableMoveCursorLeft) {
                        if(cur.columnNumber() > 0) {
                            cur.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor);
                            setTextCursor(cur);
                        }
                    }
                }
                break;

            case EN_MoveCursorRight: {
                    if(this->enableMoveCursorRight) {
                        if(cur.columnNumber() >= cur.block().length()-1)
                            cur.insertText(" ");
                        cur.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor);
                        setTextCursor(cur);
                    }
                }
                break;

            case EN_MoveCursorUp: {
                    if(this->enableMoveCursorUp) {
                        if(cur.blockNumber() > 0) {
                            int col = cur.columnNumber();
                            qDebug() << "MU Column" << col;
                            cur.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor);
                            int end = cur.block().length();
                            for(int n = end; n <= col; n++) {
                                cur.insertText(" ");
                            }
                            cur.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
                            cur.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,col);
                            setTextCursor(cur);
                        }
                    }
                }
                break;

            case EN_MoveCursorDown: {
                    if(this->enableMoveCursorDown) {
                        int col = cur.columnNumber();
                        int row = cur.blockNumber();
                        int cnt = this->blockCount();
                        int end = cur.block().length();
                        qDebug() << "MD Column" << col;
                        if(row+1 < cnt) {
                            cur.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor);
                            end = cur.block().length();
                            for(int n = end; n <= col; n++) {
                                cur.insertText(" ");
                                cur.movePosition(QTextCursor::EndOfLine,QTextCursor::MoveAnchor);
                            }
                            end = cur.block().length();
                            cur.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
                            cur.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor, col);
                        } else {
                            cur.insertBlock();
                            for(int n = 1; n < col; n++) {
                                cur.insertText(" ");
                            }
                        }
                        setTextCursor(cur);
                    }
                }
                break;

            case EN_BeepSpeaker: {
                    if(this->enableBeepSpeaker) {
                        QApplication::beep();
                    }
                }
                break;

            case EN_Backspace: {
                    if(this->enableBackspace) {
                        text = toPlainText();
                        setPlainText(text.mid(0,text.length()-1));
                        moveCursor(QTextCursor::End);
                    }
                }
                break;

            case EN_Tab: {
                    if(this->enableTab) {
                        int column = cur.columnNumber() % tabsize;
                        while(column++ < tabsize) {
                            moveCursor(QTextCursor::Right,QTextCursor::MoveAnchor);
                            cur.insertText(" ");
                        }
                        setTextCursor(cur);
                    }
                }
                break;

            /* fall through case because newline and creturn can change roles
             * the switch is just to speed things up
             */
            case EN_NewLine: // fall through
            case EN_CReturn: {
                    if(ch == SPIN_NEWLINE) {
                        if(enableNewLine) {
                            int row = cur.blockNumber();
                            int max = this->blockCount()-1;
                            if(row < max) {
                                // insert a newline
                                cur.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
                                text = cur.selectedText();
                                cur.removeSelectedText();
                                cur.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor);
                                cur.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
                                cur.insertText(text);
                            }
                            else {
                                cur.insertBlock();
                            }
#if 0
                            if(this->enableAddCRtoNL == false) {
                                while(col-- > -1) {
                                    cur.insertText(" ");
                                    cur.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor, 1);
                                }
                            }
#endif
                            setTextCursor(cur);
                        }
                    }
                    else if(ch == SPIN_RETURN) {
                        if(enableCReturn) {
                            text = toPlainText();
                            cur.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
                            setTextCursor(cur);
                        }
                    }
                }
                break;

            case EN_ClearToEOL: {
                    if(this->enableClearToEOL) {
                        int end = cur.block().length();
                        int col = cur.columnNumber();
                        if(end > col) {
                            cur.clearSelection();
                            cur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,end-col-1);
                            text = cur.selectedText();
                            col = cur.columnNumber();
                            if(text.length()) {
                                cur.removeSelectedText();
                                col = cur.columnNumber();
                                text = cur.selectedText();
                                col = cur.columnNumber();
                                setTextCursor(cur);
                                col = cur.columnNumber();
                            }
                        }
                    }
                }
                break;

            case EN_ClearLinesBelow: {
                    if(this->enableClearLinesBelow) {
                        cur.movePosition(QTextCursor::Down,QTextCursor::KeepAnchor);
                        cur.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
                        //qDebug() << "Selected " << cur.selectedText();
                        cur.removeSelectedText();
                    }
                }
                break;

            default: {
                    if(cur.block().length()-1 > cur.columnNumber())
                        cur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor);
                    cur.insertText(QString(ch));
                    setTextCursor(cur);
                }
                break;
            }
            break;

        } // end pcmd default
    } // end pcmd switch
    //qDebug() << "End Row   " << cur.blockNumber();
    //qDebug() << "End Column" << cur.columnNumber();
    return;
}

