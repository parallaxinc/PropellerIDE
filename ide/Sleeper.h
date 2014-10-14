#ifndef SLEEPER_H
#define SLEEPER_H

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


#endif // SLEEPER_H
