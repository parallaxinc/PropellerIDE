#pragma once

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QHash>

#include "treeitem.h"

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(const QString &name, QObject *parent = 0);
    virtual ~TreeModel();

    void addRootItem(QString text, QString file = "");
    QString     getTreeName();
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QString file(const QModelIndex &index) const;

    bool isDuplicate(TreeItem *item, QString str);

    virtual void includes(QString filePath, QString incPath, QString text);
    virtual void addFileReferences(QString filePath, QString incPath, QString text, int level);

    void    clearSymbolInfo();
    void    addSymbolInfo(QString symbol, QString file, int line);
    QString getSymbolInfo(QString symbol, int *line);

    int hashCount();

    TreeItem *rootItem;

private:
    void setupModelData(const QStringList &lines, TreeItem *parent);

    QString treeName;
    QHash<QString,QString> itemHash;

protected:
    QHash<QString, QString> symbolHash;
};
