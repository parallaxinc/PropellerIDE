#pragma once

class PColor : public QColor
{
private:
    QString mName;
    QString mTrName;
    QColor mValue;

public:
    PColor (QString trname, QString name, QColor value) {
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
    QColor getValue() {
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
