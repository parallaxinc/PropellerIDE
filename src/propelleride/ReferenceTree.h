#pragma once

#include <QTreeView>

#include "ColorScheme.h"

class ReferenceTree : public QTreeView
{
    Q_OBJECT

public:
    explicit ReferenceTree(
            const QString & name,
            ColorScheme::Color colorkey,
            QWidget *parent = 0);
    //void rebuildTree(QString fileName, QString text);
    ~ReferenceTree();
    
private:
    QString name;
    ColorScheme::Color color;
    ColorScheme * currentTheme;
    //void rebuildSpinTree(QString fileName, QString includes, QString objname, int level);

public slots:
    void updateColors();

};
