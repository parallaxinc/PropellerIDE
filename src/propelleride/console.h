#ifndef CONSOLE_H
#define CONSOLE_H

#include <QWidget>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QString>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QByteArray>


#include "qext/qextserialport.h"

class Console : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit Console(QWidget *parent = 0);
    void setPortEnable(bool value);
    bool enabled();
    void clear();

    void setEnableClearScreen(bool value);
    void setEnableHomeCursor(bool value);
    void setEnablePosXYCursor(bool value);
    void setEnableMoveCursorLeft(bool value);
    void setEnableMoveCursorRight(bool value);
    void setEnableMoveCursorUp(bool value);
    void setEnableMoveCursorDown(bool value);
    void setEnableBeepSpeaker(bool value);
    void setEnableBackspace(bool value);
    void setEnableTab(bool value);
    void setEnableCReturn(bool value);
    void setEnableClearToEOL(bool value);
    void setEnableClearLinesBelow(bool value);
    void setEnableNewLine(bool value);
    void setEnablePosCursorX(bool value);
    void setEnablePosCursorY(bool value);
    void setEnableClearScreen16(bool value);
    void setEnableEchoOn(bool value);
    void setEnableEnterIsNL(bool value);
    void setEnableSwapNLCR(bool value);

    bool getHexMode();
    int  getEnter();
    void setWrapMode(int mode);
    void setTabSize(int size);
    void setHexMode(bool enable);
    void setHexDump(bool enable);
    void setMaxRows(int rows);

    void setTabWidget(QTabWidget *tabs);

public:

    typedef enum {
        EN_ClearScreen = 0,
        EN_HomeCursor,
        EN_PosXYCursor,
        EN_MoveCursorLeft,
        EN_MoveCursorRight, // 4
        EN_MoveCursorUp,
        EN_MoveCursorDown,
        EN_BeepSpeaker,
        EN_Backspace, // 8
        EN_Tab,
        EN_CReturn,
        EN_ClearToEOL,
        EN_ClearLinesBelow, //12
        EN_NewLine,
        EN_PosCursorX,
        EN_PosCursorY,
        EN_ClearScreen2, //16
         // everything from here down is to make save/restore settings easier
        EN_AddCRtoNL,
        EN_EnterIsNL,
        EN_LAST
    } EnableEn;

private:

    void    sendPortMessage(QString s);
    void    eventKey(QKeyEvent* event);

    typedef enum {
        PCMD_NONE = 0,
        PCMD_CURPOS_XY = 2,
        PCMD_CURPOS_X = 14,
        PCMD_CURPOS_Y = 15
    } PCmdEn;

    PCmdEn  pcmd;
    int     pcmdlen;
    int     pcmdx;
    int     pcmdy;

    bool enableClearScreen;
    bool enableHomeCursor;
    bool enablePosXYCursor;
    bool enableMoveCursorLeft;
    bool enableMoveCursorRight;
    bool enableMoveCursorUp;
    bool enableMoveCursorDown;
    bool enableBeepSpeaker;
    bool enableBackspace;
    bool enableTab;
    bool enableCReturn;
    bool enableClearToEOL;
    bool enableClearLinesBelow;
    bool enableNewLine;
    bool enablePosCursorX;
    bool enablePosCursorY;
    bool enableClearScreen16;
    bool enableEchoOn;
    bool enableEnterIsNL;

    bool enableSwapNLCR;

    bool enableANSI;

    char newline;
    char creturn;
    char lastchar;

    bool isEnabled;
    bool utfparse;
    int  utfbytes;
    int  utf8;

    int  maxcol;
    int  maxrow;

    int  wrapMode;
    int  tabsize;

    bool hexmode;
    int  hexbytes;
    int  maxhex;
    int  hexbyte[17];
    bool hexdump;

    // screen buffer
    char *sbuff;

    QTabWidget *tabWidget;

protected:
    void keyPressEvent(QKeyEvent* event);
    void resizeEvent(QResizeEvent *e);

signals:
    void sendSerial(QByteArray bytes);

public slots:
    void updateReady(QextSerialPort*);
    void dumphex(int ch);
    void update(char ch);

};

#endif // CONSOLE_H
