/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

/*
    treemodel.cpp

    Provides a simple tree model to show how to create and use hierarchical
    models.
*/

#include <QApplication>
#include <QLabel>
#include <QObject>
#include <QAbstractItemModel>
#include <QVariant>
#include <QList>
#include <QString>
#include <QStringList>
#include <QModelIndex>

#include "treeitem.h"
#include "treemodel.h"
#include "preferences.h"

//! [0]
TreeModel::TreeModel(const QString &shortFileName, QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << shortFileName;
    rootItem = new TreeItem(rootData);
    treeName = shortFileName;
    //setupModelData(data.split(QString("\n")), rootItem);
}
//! [0]

//! [1]
TreeModel::~TreeModel()
{
    if(rootItem)
        delete rootItem;
}
//! [1]

QString TreeModel::getTreeName()
{
    return treeName;
}

//! [2]
int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}
//! [2]

//! [3]
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}
//! [3]

//! [4]
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
//! [4]

//! [5]
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}
//! [5]

//! [6]
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
//! [6]

//! [7]
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
//! [7]

//! [8]
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
//! [8]

QString TreeModel::file(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();
    /*
    if (role != Qt::DisplayRole)
        return QString();
    */
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
            // Read the column data from the rest of the line.
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

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

            // Append a new item to the current parent's list of children.
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

/*
 * this is overloaded in a child class
 */
void TreeModel::includes(QString filePath, QString incPath, QString text, int level)
{
    /* suppress stupid compiler warnings */
    if(filePath.length() && incPath.length() && text.length() && level)
        return;
}

/*
 * this is overloaded in a child class
 */
void TreeModel::addFileReferences(QString filePath, QString incPath, QString text, int level)
{
    /* suppress stupid compiler warnings */
    if(filePath.length() && incPath.length() && text.length() && level)
        return;
}

/**
 * @brief TreeModel::getSymbolFile
 * Get information collected in the method hash table.
 *
 * @param symbol the file::method name, such as "PUB start" from reference model entry
 * Example: C:/Propeller/Parallax Serial Terminal.spin::PUB Char(bytchr)
 * Yields:  C:/Propeller/Parallax Serial Terminal.spin::115
 * @param [out] line Storage for the line number provided by caller
 * @return Empty string if symbol not found, otherwise filename.
 */
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
    //qDebug() << "addSymbolInfo" << k << v;
    symbolHash.insert(k, v);
}

int TreeModel::hashCount()
{
    return symbolHash.count();
}

QString TreeModel::shortFileName(QString fileName)
{
    QString rets;
    if(fileName.indexOf("/") > -1)
        rets = fileName.mid(fileName.lastIndexOf("/")+1);
    else
        rets = fileName;
    return rets;
}
