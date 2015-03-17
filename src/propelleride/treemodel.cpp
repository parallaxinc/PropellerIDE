#include "treemodel.h"

#include <QLabel>
#include <QAbstractItemModel>
#include <QVariant>
#include <QList>
#include <QString>
#include <QStringList>
#include <QModelIndex>

#include "treeitem.h"
#include "Preferences.h"

TreeModel::TreeModel(const QString &shortFileName, QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << shortFileName;
    rootItem = new TreeItem(rootData);
    treeName = shortFileName;
}

TreeModel::~TreeModel()
{
    if(rootItem)
        delete rootItem;
}

QString TreeModel::getTreeName()
{
    return treeName;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

QString TreeModel::file(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->file();
}

void TreeModel::setupModelData(const QStringList &lines, TreeItem *parent)
{
    QList<TreeItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].mid(position, 1) != " ")
                break;
            position++;
        }

        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) {
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            if (position > indentations.last()) {
                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            parents.last()->appendChild(new TreeItem(columnData, parents.last()));
        }

        number++;
    }
}

bool TreeModel::isDuplicate(TreeItem *item, QString str)
{
    QList<TreeItem*> items = item->childItems;
    int count = items.count();
    for(int n = 0; n < count; n++)
    {
        QVariant qv = items.at(n)->data(0);
        if(qv.canConvert(QVariant::String)) {
            if(qv.toString() == str)
                return true;
        }
    }
    return false;
}


void TreeModel::addRootItem(QString text, QString file)
{
    QList<QVariant> clist;
    clist << text.toLatin1();
    if(!isDuplicate(rootItem, text))
        rootItem->appendChild(new TreeItem(clist, rootItem, file));
}

void TreeModel::includes(QString filePath, QString incPath, QString text)
{
    if(filePath.length() && incPath.length() && text.length())
        return;
}

void TreeModel::addFileReferences(QString filePath, QString incPath, QString text, int level)
{
    if(filePath.length() && incPath.length() && text.length() && level)
        return;
}

QString TreeModel::getSymbolInfo(QString symbol, int *line)
{
    QString file = symbolHash.value(symbol);
    if(file.length()) {
        QString s;
        QStringList sp = file.split("::",QString::SkipEmptyParts);
        bool ok;
        if(sp.count() > 0) {
            file = sp.at(0);
            s = sp.at(1);
            *line = s.toInt(&ok, 10);
            if(!ok) {
                *line = -1;
            }
        }
    }
    return file;
}

void TreeModel::clearSymbolInfo()
{
    symbolHash.clear();
}

void TreeModel::addSymbolInfo(QString symbol, QString file, int line)
{
    QString k = file+"::"+symbol;
    QString v = file+QString("::%1").arg(line);
    symbolHash.insert(k, v);
}

int TreeModel::hashCount()
{
    return symbolHash.count();
}
