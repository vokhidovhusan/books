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

#include "aqp.hpp"
#include "global.hpp"
#include "treemodel.hpp"
#include <QFile>
#include <QMimeData>


namespace {
const int ColumnCount = 3;
const int MaxCompression = 9;
enum Column {Name, Today, Total};
const QString MimeType = "application/vnd.qtrac.xml.task.z";
}


Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractItemModel::flags(index);
    if (index.isValid()) {
        theFlags |= Qt::ItemIsSelectable|Qt::ItemIsEnabled;
        if (index.column() == Name)
            theFlags |= Qt::ItemIsUserCheckable|Qt::ItemIsEditable|
                        Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled;
    }
    return theFlags;
}


QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!rootItem || !index.isValid() || index.column() < 0 ||
        index.column() >= ColumnCount)
        return QVariant();
    if (TaskItem *item = itemForIndex(index)) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            switch (index.column()) {
                case Name: return item->name();
                case Today: return item->todaysTime();
                case Total: return item->totalTime();
                default: Q_ASSERT(false);
            }
        }
        if (role == Qt::CheckStateRole && index.column() == Name)
            return static_cast<int>(item->isDone() ? Qt::Checked
                                                   : Qt::Unchecked);
        if (role == Qt::TextAlignmentRole) {
            if (index.column() == Name)
                return static_cast<int>(Qt::AlignVCenter|
                                        Qt::AlignLeft);
            return static_cast<int>(Qt::AlignVCenter|Qt::AlignRight);
        }
        if (role == Qt::DecorationRole && index.column() == Today &&
            timedItem && item == timedItem && !m_icon.isNull())
            return m_icon;
    }
    return QVariant();
}


QVariant TreeModel::headerData(int section,
        Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == Name)
            return tr("Task/Subtask/...");
        else if (section == Today)
            return tr("Time (Today)");
        else if (section == Total)
            return tr("Time (Total)");
    }
    return QVariant();
}


int TreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return 0;
    TaskItem *parentItem = itemForIndex(parent);
    return parentItem ? parentItem->childCount() : 0;
}


int TreeModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() && parent.column() != 0 ? 0 : ColumnCount;
}


QModelIndex TreeModel::index(int row, int column,
                             const QModelIndex &parent) const
{
    if (!rootItem || row < 0 || column < 0 || column >= ColumnCount
        || (parent.isValid() && parent.column() != 0))
        return QModelIndex();
    TaskItem *parentItem = itemForIndex(parent);
    Q_ASSERT(parentItem);
    if (TaskItem *item = parentItem->childAt(row))
        return createIndex(row, column, item);
    return QModelIndex();
}


TaskItem *TreeModel::itemForIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        if (TaskItem *item = static_cast<TaskItem*>(
                index.internalPointer()))
            return item;
    }
    return rootItem;
}


QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();
    if (TaskItem *childItem = itemForIndex(index)) {
        if (TaskItem *parentItem = childItem->parent()) {
            if (parentItem == rootItem)
                return QModelIndex();
            if (TaskItem *grandParentItem = parentItem->parent()) {
                int row = grandParentItem->rowOfChild(parentItem);
                return createIndex(row, 0, parentItem);
            }
        }
    }
    return QModelIndex();
}


bool TreeModel::setData(const QModelIndex &index,
                        const QVariant &value, int role)
{
    if (!index.isValid() || index.column() != Name)
        return false;
    if (TaskItem *item = itemForIndex(index)) {
        if (role == Qt::EditRole)
            item->setName(value.toString());
        else if (role == Qt::CheckStateRole)
            item->setDone(value.toBool());
        else
            return false;
        emit dataChanged(index, index);
        return true;
    }
    return false;
}


bool TreeModel::insertRows(int row, int count,
                           const QModelIndex &parent)
{
    if (!rootItem)
        rootItem = new TaskItem;
    TaskItem *parentItem = parent.isValid() ? itemForIndex(parent)
                                            : rootItem;
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        TaskItem *item = new TaskItem(tr("New Task"), false);
        parentItem->insertChild(row, item);
    }
    endInsertRows();
    return true;
}


bool TreeModel::removeRows(int row, int count,
                           const QModelIndex &parent)
{
    if (!rootItem)
        return false;
    TaskItem *item = parent.isValid() ? itemForIndex(parent)
                                      : rootItem;
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i)
        delete item->takeChild(row);
    endRemoveRows();
    return true;
}


QStringList TreeModel::mimeTypes() const
{
    return QStringList() << MimeType;
}


QMimeData *TreeModel::mimeData(const QModelIndexList &indexes) const
{
    Q_ASSERT(indexes.count());
    if (indexes.count() != 1)
        return 0;
    if (TaskItem *item = itemForIndex(indexes.at(0))) {
        QMimeData *mimeData = new QMimeData;
        QByteArray xmlData;
        QXmlStreamWriter writer(&xmlData);
        writeTaskAndChildren(&writer, item);
        mimeData->setData(MimeType, qCompress(xmlData,
                                              MaxCompression));
        return mimeData;
    }
    return 0;
}


bool TreeModel::dropMimeData(const QMimeData *mimeData,
        Qt::DropAction action, int row, int column,
        const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;
    if (action != Qt::MoveAction || column > 0 ||
        !mimeData || !mimeData->hasFormat(MimeType))
        return false;
    if (TaskItem *item = itemForIndex(parent)) {
        emit stopTiming();
        QByteArray xmlData = qUncompress(mimeData->data(MimeType));
        QXmlStreamReader reader(xmlData);
        if (row == -1)
            row = parent.isValid() ? parent.row()
                                   : rootItem->childCount();
        beginInsertRows(parent, row, row);
        readTasks(&reader, item);
        endInsertRows();
        return true;
    }
    return false;
}


bool TreeModel::isChecked(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;
    return data(index, Qt::CheckStateRole).toInt() == Qt::Checked;
}


QModelIndex TreeModel::moveUp(const QModelIndex &index)
{
    if (!index.isValid() || index.row() <= 0)
        return index;
    TaskItem *item = itemForIndex(index);
    Q_ASSERT(item);
    TaskItem *parent = item->parent();
    Q_ASSERT(parent);
    return moveItem(parent, index.row(), index.row() - 1);
}


QModelIndex TreeModel::moveItem(TaskItem *parent, int oldRow,
                                int newRow)
{
    Q_ASSERT(0 <= oldRow && oldRow < parent->childCount() &&
             0 <= newRow && newRow < parent->childCount());
    parent->swapChildren(oldRow, newRow);
    QModelIndex oldIndex = createIndex(oldRow, 0,
                                       parent->childAt(oldRow));
    QModelIndex newIndex = createIndex(newRow, 0,
                                       parent->childAt(newRow));
    emit dataChanged(oldIndex, newIndex);
    return newIndex;
}


QModelIndex TreeModel::moveDown(const QModelIndex &index)
{
    if (!index.isValid())
        return index;
    TaskItem *item = itemForIndex(index);
    Q_ASSERT(item);
    TaskItem *parent = item->parent();
    int newRow = index.row() + 1;
    if (!parent || parent->childCount() <= newRow)
        return index;
    return moveItem(parent, index.row(), newRow);
}


QModelIndex TreeModel::cut(const QModelIndex &index)
{
    if (!index.isValid())
        return index;
    delete cutItem;
    cutItem = itemForIndex(index);
    Q_ASSERT(cutItem);
    TaskItem *parent = cutItem->parent();
    Q_ASSERT(parent);
    int row = parent->rowOfChild(cutItem);
    Q_ASSERT(row == index.row());
    beginRemoveRows(index.parent(), row, row);
    TaskItem *child = parent->takeChild(row);
    endRemoveRows();
    Q_ASSERT(child == cutItem);
    child = 0; // Silence compiler unused variable warning

    if (row > 0) {
        --row;
        return createIndex(row, 0, parent->childAt(row));
    }
    if (parent != rootItem) {
        TaskItem *grandParent = parent->parent();
        Q_ASSERT(grandParent);
        return createIndex(grandParent->rowOfChild(parent), 0, parent);
    }
    return QModelIndex();
}


QModelIndex TreeModel::paste(const QModelIndex &index)
{
    if (!index.isValid() || !cutItem)
        return index;
    TaskItem *sibling = itemForIndex(index);
    Q_ASSERT(sibling);
    TaskItem *parent = sibling->parent();
    Q_ASSERT(parent);
    int row = parent->rowOfChild(sibling) + 1;
    beginInsertRows(index.parent(), row, row);
    parent->insertChild(row, cutItem);
    TaskItem *child = cutItem;
    cutItem = 0;
    endInsertRows();
    return createIndex(row, 0, child);
}


QModelIndex TreeModel::promote(const QModelIndex &index)
{
    if (!index.isValid())
        return index;
    TaskItem *item = itemForIndex(index);
    Q_ASSERT(item);
    TaskItem *parent = item->parent();
    Q_ASSERT(parent);
    if (parent == rootItem)
        return index; // Already a top-level item

    int row = parent->rowOfChild(item);
    TaskItem *child = parent->takeChild(row);
    Q_ASSERT(child == item);
    TaskItem *grandParent = parent->parent();
    Q_ASSERT(grandParent);
    row = grandParent->rowOfChild(parent) + 1;
    grandParent->insertChild(row, child);
    QModelIndex newIndex = createIndex(row, 0, child);
    emit dataChanged(newIndex, newIndex);
    return newIndex;
}


QModelIndex TreeModel::demote(const QModelIndex &index)
{
    if (!index.isValid())
        return index;
    TaskItem *item = itemForIndex(index);
    Q_ASSERT(item);
    TaskItem *parent = item->parent();
    Q_ASSERT(parent);
    int row = parent->rowOfChild(item);
    if (row == 0)
        return index; // No preceding sibling to move this under
    TaskItem *child = parent->takeChild(row);
    Q_ASSERT(child == item);
    TaskItem *sibling = parent->childAt(row - 1);
    Q_ASSERT(sibling);
    sibling->addChild(child);
    QModelIndex newIndex = createIndex(sibling->childCount() - 1, 0,
                                       child);
    emit dataChanged(newIndex, newIndex);
    return newIndex;
}


void TreeModel::setTimedItem(const QModelIndex &index)
{
    clearTimedItem();
    timedItem = itemForIndex(index);
    Q_ASSERT(timedItem);
    announceItemChanged(timedItem);
}


void TreeModel::clearTimedItem()
{
    if (timedItem) {
        TaskItem *item = timedItem;
        timedItem = 0;
        announceItemChanged(item);
    }
}


bool TreeModel::isTimedItem(const QModelIndex &index)
{
    return timedItem && itemForIndex(index) == timedItem;
}


void TreeModel::announceItemChanged(TaskItem *item)
{
    if (item == rootItem)
        return;
    TaskItem *parent = item->parent();
    Q_ASSERT(parent);
    int row = parent->rowOfChild(item);
    QModelIndex startIndex = createIndex(row, static_cast<int>(Name),
                                         item);
    QModelIndex endIndex = createIndex(row, static_cast<int>(Total),
                                       item);
    emit dataChanged(startIndex, endIndex);
    // Update the parent & parent's parent etc too
    announceItemChanged(parent);
}


void TreeModel::addDateTimeToTimedItem(const QDateTime &start,
                                       const QDateTime &end)
{
    if (timedItem) {
        timedItem->addDateTime(start, end);
        announceItemChanged(timedItem);
    }
}


void TreeModel::setIconForTimedItem(const QIcon &icon)
{
    m_icon = icon;
    if (timedItem)
        announceItemChanged(timedItem);
}


void TreeModel::incrementEndTimeForTimedItem(int msec)
{
    if (timedItem) {
        timedItem->incrementLastEndTime(msec);
        announceItemChanged(timedItem);
    }
}


void TreeModel::clear()
{
    delete rootItem;
    rootItem = 0;
    delete cutItem;
    cutItem = 0;
    timedItem = 0;
    reset();
}


void TreeModel::load(const QString &filename)
{
    if (!filename.isEmpty())
        m_filename = filename;
    if (m_filename.isEmpty())
        throw AQP::Error(tr("no filename specified"));
    QFile file(m_filename);
    if (!file.open(QIODevice::ReadOnly))
        throw AQP::Error(file.errorString());

    clear();
    rootItem = new TaskItem;
    QXmlStreamReader reader(&file);
    readTasks(&reader, rootItem);
    if (reader.hasError())
        throw AQP::Error(reader.errorString());
    reset();
}


void TreeModel::readTasks(QXmlStreamReader *reader, TaskItem *task)
{
    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isStartElement()) {
            if (reader->name() == TaskTag) {
                const QString name = reader->attributes()
                        .value(NameAttribute).toString();
                bool done = reader->attributes().value(DoneAttribute)
                            == "1";
                task = new TaskItem(name, done, task);
            }
            else if (reader->name() == WhenTag) {
                const QDateTime start = QDateTime::fromString(
                        reader->attributes().value(StartAttribute)
                            .toString(), Qt::ISODate);
                const QDateTime end = QDateTime::fromString(
                        reader->attributes().value(EndAttribute)
                            .toString(), Qt::ISODate);
                Q_ASSERT(task);
                task->addDateTime(start, end);
            }
        }
        else if (reader->isEndElement()) {
            if (reader->name() == TaskTag) {
                Q_ASSERT(task);
                task = task->parent();
                Q_ASSERT(task);
            }
        }
    }
}


void TreeModel::save(const QString &filename)
{
    if (!filename.isEmpty())
        m_filename = filename;
    if (m_filename.isEmpty())
        throw AQP::Error(tr("no filename specified"));
    QFile file(m_filename);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
        throw AQP::Error(file.errorString());

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("TIMELOG");
    writer.writeAttribute("VERSION", "2.0");
    writeTaskAndChildren(&writer, rootItem);
    writer.writeEndElement(); // TIMELOG
    writer.writeEndDocument();
}


void TreeModel::writeTaskAndChildren(QXmlStreamWriter *writer,
                                     TaskItem *task) const
{
    if (task != rootItem) {
        writer->writeStartElement(TaskTag);
        writer->writeAttribute(NameAttribute, task->name());
        writer->writeAttribute(DoneAttribute, task->isDone() ? "1"
                                                             : "0");
        QListIterator<
                QPair<QDateTime, QDateTime> > i(task->dateTimes());
        while (i.hasNext()) {
            const QPair<QDateTime, QDateTime> &dateTime = i.next();
            writer->writeStartElement(WhenTag);
            writer->writeAttribute(StartAttribute,
                    dateTime.first.toString(Qt::ISODate));
            writer->writeAttribute(EndAttribute,
                    dateTime.second.toString(Qt::ISODate));
            writer->writeEndElement(); // WHEN
        }
    }
    foreach (TaskItem *child, task->children())
        writeTaskAndChildren(writer, child);
    if (task != rootItem)
        writer->writeEndElement(); // TASK
}


QStringList TreeModel::pathForIndex(const QModelIndex &index) const
{
    QStringList path;
    QModelIndex thisIndex = index;
    while (thisIndex.isValid()) {
        path.prepend(data(thisIndex).toString());
        thisIndex = thisIndex.parent();
    }
    return path;
}


QModelIndex TreeModel::indexForPath(const QStringList &path) const
{
    return indexForPath(QModelIndex(), path);
}


QModelIndex TreeModel::indexForPath(const QModelIndex &parent,
                                    const QStringList &path) const
{
    if (path.isEmpty())
        return QModelIndex();
    for (int row = 0; row < rowCount(parent); ++row) {
        QModelIndex thisIndex = index(row, 0, parent);
        if (data(thisIndex).toString() == path.at(0)) {
            if (path.count() == 1)
                return thisIndex;
            thisIndex = indexForPath(thisIndex, path.mid(1));
            if (thisIndex.isValid())
                return thisIndex;
        }
    }
    return QModelIndex();
}
