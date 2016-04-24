#pragma once

#include <QApplication>

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int &argc, char** argv);
    ~Application();

protected:
    bool event(QEvent *);

private:
    void loadFile(const QString & filename);

signals:
    void fileOpened(const QString & filename);

};
