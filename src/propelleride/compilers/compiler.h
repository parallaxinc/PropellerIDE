#pragma once

#include <QObject>

class Compiler : public QObject
{
    Q_OBJECT

public:
    Compiler(QObject * parent = 0)
        : QObject(parent) {}

    virtual ~Compiler() {};

    virtual void build(QString filename) = 0;

signals:
    void finished(bool success);
    void highlightLine(const QString & file,
            int line,
            int col,
            const QString & text);
};

