#include "treeitem.h"

#include <QStringList>

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
{
    parentItem = parent;
    itemData = data;
    filePath = "";
}

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent, QString &file)
{
    parentItem = parent;
    itemData = data;
    filePath = file;
}

TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
}

TreeItem *TreeItem::appendChild(TreeItem *item)
{
    childItems.append(item);
    return item;
}

TreeItem *TreeItem::child(int row)
{
    return childItems.value(row);
}

int TreeItem::childCount() const
{
    return childItems.count();
}

int TreeItem::columnCount() const
{
    return itemData.count();
}

QVariant TreeItem::data(int column) const
{
    return itemData.value(column);
}

TreeItem *TreeItem::parent()
{
    return parentItem;
}

int TreeItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

QString TreeItem::file() const
{
    return filePath;
}
