#pragma once

#include <QPlainTextEdit>
#include <QTextCursor>

class Console : public QPlainTextEdit
{
    Q_OBJECT

    int columns;
    bool paused;
    char lastChar;
    char lastChar2;
    QTextCursor lastCursor;

signals:
    void getData(const QByteArray &data);

public:
    explicit Console(QWidget *parent = 0);

    void putData(QByteArray data);
    void setFont(const QFont & font);

public slots:
    void setPstMode(bool enable);
    void setEcho(bool enable);
    void enable(bool set);

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

private:
    bool pstMode;
    bool echoMode;

    int cursorX(QTextCursor cursor);
    int cursorY(QTextCursor cursor);
    int lineW(QTextCursor cursor);
    QTextCursor moveRight(QTextCursor cursor);
    QTextCursor moveDown(QTextCursor cursor);
    QTextCursor moveUp(QTextCursor cursor);

    QTextCursor positionX(QTextCursor cursor, int x);
    QTextCursor positionY(QTextCursor cursor, int y);

    void updateColumns();
};
