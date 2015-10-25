#pragma once

#include <QPlainTextEdit>
#include <QTextCursor>

class Console : public QPlainTextEdit
{
    Q_OBJECT

    bool paused;
    char lastChar;
    char lastChar2;
    QTextCursor lastCursor;

signals:
    void getData(const QByteArray &data);

public:
    explicit Console(QWidget *parent = 0);

    void putData(const QByteArray &data);

public slots:
    void setPstMode(bool enable);
    void enable(bool set);

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);

private:
    bool pstMode;

    int cursorX(QTextCursor cursor);
    int cursorY(QTextCursor cursor);
    int lineW(QTextCursor cursor);
    QTextCursor moveRight(QTextCursor cursor);
    QTextCursor moveDown(QTextCursor cursor);
    QTextCursor moveUp(QTextCursor cursor);

    QTextCursor positionX(QTextCursor cursor, int x);
    QTextCursor positionY(QTextCursor cursor, int y);
};
