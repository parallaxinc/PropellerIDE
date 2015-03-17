#pragma once

#include <QList>
#include <QVariant>

class TreeItem
{
public:
    TreeItem(const QList<QVariant> &data, TreeItem *parent = 0);
    TreeItem(const QList<QVariant> &data, TreeItem *parent, QString &file);
    ~TreeItem();

    TreeItem *appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeItem *parent();
    QString file() const;

    QList<TreeItem*> childItems;
    QList<QVariant> itemData;

private:
    TreeItem *parentItem;
    QString filePath;
};
