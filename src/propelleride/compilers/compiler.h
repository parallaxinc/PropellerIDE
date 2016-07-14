#pragma once

#include <QObject>
#include <QStringList>

class Compiler : public QObject
{
    Q_OBJECT

public:
    Compiler(QObject * parent = 0)
        : QObject(parent) {}

    virtual ~Compiler() {};

    virtual QString build(QString filename,
                          QStringList libraries = QStringList()) = 0;

signals:
    void finished(bool success);
    void highlightLine(const QString & file,
            int line,
            int col,
            const QString & text);
    void print(const QString & text);
};

