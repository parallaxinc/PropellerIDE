#pragma once

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include "treemodel.h"

class SpinModel : public TreeModel
{
    Q_OBJECT

public:
    SpinModel(const QString name, QObject *parent = 0);
    virtual ~SpinModel();

    void includes(QString filePath, QString incPath, QString text);

    void addObjects(QString filePath, QString incPath, QString text, int level);
    void addFileReferences(QString filePath, QString incPath, QString text, int level);

private:
    bool isSectionHead(QString s, QString nocmp);
    QString removeComments(QString text);
};
