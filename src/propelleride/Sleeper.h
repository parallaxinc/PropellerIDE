#pragma once

#include <QThread>

class Sleeper : QThread
{
public:
    static void ms(int ms)
    {
        msleep(ms);
    }
    static void sec(int s)
    {
        sleep(s);
    }
};
