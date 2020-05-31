#ifndef TREEMODEL_HPP
#define TREEMODEL_HPP
/*
    Copyright (c) 2009-10 Qtrac Ltd. All rights reserved.

    This program or module is free software: you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version. It is provided
    for educational purposes and is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
    the GNU General Public License for more details.
*/

#include "taskitem.hpp"
#include <QAbstractItemModel>
#include <QIcon>
// Mac needs these included this way---and needs them in the header
#include <QtCore/QXmlStreamReader>
#include <QtCore/QXmlStreamWriter>


class QMimeData;


class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(QObject *parent=0)
        : QAbstractItemModel(parent), timedItem(0), rootItem(0),
          cutItem(0) {}
    ~TreeModel() { delete rootItem; delete cutItem; }

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index,
                  int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role=Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent=QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    bool setHeaderData(int, Qt::Orientation, const QVariant&,
                       int=Qt::EditRole) { return false; }
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role=Qt::EditRole);
    bool insertRows(int row, int count,
                    const QModelIndex &parent=QModelIndex());
    bool removeRows(int row, int count,
                    const QModelIndex &parent=QModelIndex());

    Qt::DropActions supportedDragActions() const
        { return Qt::MoveAction; }
    Qt::DropActions supportedDropActions() const
        { return Qt::MoveAction; }
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *mimeData,
            Qt::DropAction action, int row, int column,
            const QModelIndex &parent);

    bool isChecked(const QModelIndex &index) const;
    bool hasCutItem() const { return cutItem; }

    QModelIndex moveUp(const QModelIndex &index);
    QModelIndex moveDown(const QModelIndex &index);
    QModelIndex cut(const QModelIndex &index);
    QModelIndex paste(const QModelIndex &index);
    QModelIndex promote(const QModelIndex &index);
    QModelIndex demote(const QModelIndex &index);

    void setTimedItem(const QModelIndex &index);
    void clearTimedItem();
    bool isTimedItem(const QModelIndex &index);
    void addDateTimeToTimedItem(const QDateTime &start,
                                const QDateTime &end);
    void setIconForTimedItem(const QIcon &icon=QIcon());
    void incrementEndTimeForTimedItem(int msec);

    void clear();
    QString filename() const { return m_filename; }
    void setFilename(const QString &filename)
        { m_filename = filename; }
    void load(const QString &filename=QString());
    void save(const QString &filename=QString());
    QStringList pathForIndex(const QModelIndex &index) const;
    QModelIndex indexForPath(const QStringList &path) const;

signals:
    void stopTiming();

private:
    TaskItem *itemForIndex(const QModelIndex &index) const;
    void readTasks(QXmlStreamReader *reader, TaskItem *task);
    void writeTaskAndChildren(QXmlStreamWriter *writer,
                              TaskItem *task) const;
    void announceItemChanged(TaskItem *item);
    QModelIndex indexForPath(const QModelIndex &parent,
                             const QStringList &path) const;
    QModelIndex moveItem(TaskItem *parent, int oldRow, int newRow);

    QString m_filename;
    QIcon m_icon;
    TaskItem *timedItem;
    TaskItem *rootItem;
    TaskItem *cutItem;
};
#endif // TREEMODEL_HPP
