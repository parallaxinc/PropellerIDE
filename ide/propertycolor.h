#ifndef PROPERTYCOLORS_H
#define PROPERTYCOLORS_H

#include "qtversion.h"

class PColor : public QColor
{
private:
    QString mName;
    QString mTrName;
    Qt::GlobalColor mValue;

public:
    PColor (QString trname, QString name, Qt::GlobalColor value) {
        mTrName = trname;
        mName = name;
        mValue = value;
    }
    QString getName() {
        return mName;
    }
    QString getTrName() {
        return mTrName;
    }
    Qt::GlobalColor getValue() {
        return mValue;
    }

    enum Colors {
        Black, DarkGray,
        Gray, LightGray,
        Blue, DarkBlue,
        Cyan, DarkCyan,
        Green, DarkGreen,
        Magenta, DarkMagenta,
        Red, DarkRed,
        Yellow, DarkYellow,
        White, LastColor };

};

#endif // PROPERTYCOLORS_H
