#ifndef XBASICMODEL_H
#define XBASICMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include "treemodel.h"

class XBasicModel : public TreeModel
{
    Q_OBJECT

public:
    XBasicModel(const QString name, QObject *parent = 0);
    virtual ~XBasicModel();

    void includes(QString filePath, QString incPath, QString text);
    void addFileReferences(QString filePath, QString incPath, QString text, int level);
};
//! [0]

#endif
