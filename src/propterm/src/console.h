#pragma once

#include <QPlainTextEdit>
#include <QTextCursor>
#include <QMimeData>

class Console : public QPlainTextEdit
{
    Q_OBJECT

    int columns;
    int rows;
    const int minColumns;
    const int maxColumns;
    const int minRows;
    const int maxRows;
    bool paused;
    bool pasting;
    bool addLinefeed;
    bool sendCursor;
    char lastChar;
    char lastChar2;
    QTextCursor lastCursor;

signals:
    void getData(const QByteArray &data);

public:
    explicit Console(QWidget *parent = 0);

    void putData(QByteArray data);
    void setFont(const QFont & font);
    void insertFromMimeData(const QMimeData *source);
    void dropEvent(QDropEvent *e);

public slots:
    void setPstMode(bool enable);
    void setEcho(bool enable);
    void setAddLinefeed(bool enable);
    void setSendCursor(bool enable);
    void enable(bool set);
    void clear();

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

private:
    bool pstMode;
    bool echoMode;
    QWidget *myParent;

    int cursorX(QTextCursor cursor);
    int cursorY(QTextCursor cursor);
    int lineW(QTextCursor cursor);
    QTextCursor moveRight(QTextCursor cursor);
    QTextCursor moveLeft(QTextCursor cursor);
    QTextCursor moveDown(QTextCursor cursor);
    QTextCursor moveUp(QTextCursor cursor);

    QTextCursor positionX(QTextCursor cursor, int x);
    QTextCursor positionY(QTextCursor cursor, int y);

    void updateSize(int newColumns, int newRows);
    void updateColumns();
    void insertBlankLine(int count);
};
