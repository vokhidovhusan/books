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
#include "standarditem.hpp"
#include "standardtreemodel.hpp"
#include <QFile>
#include <QStack>


StandardTreeModel::StandardTreeModel(QObject *parent)
    : QStandardItemModel(parent)
{
    initialize();
}


void StandardTreeModel::initialize()
{
    setHorizontalHeaderLabels(QStringList() << tr("Task/Subtask/...")
            << tr("Time (Today)") << tr("Time (Total)"));
    for (int column = 1; column < columnCount(); ++column)
        horizontalHeaderItem(column)->setTextAlignment(
                Qt::AlignVCenter|Qt::AlignRight);
}


void StandardTreeModel::clear()
{
    QStandardItemModel::clear();
    initialize();
}


void StandardTreeModel::load(const QString &filename)
{
    if (!filename.isEmpty())
        m_filename = filename;
    if (m_filename.isEmpty())
        throw AQP::Error(tr("no filename specified"));
    QFile file(m_filename);
    if (!file.open(QIODevice::ReadOnly))
        throw AQP::Error(file.errorString());

    clear();

    QStack<QStandardItem*> stack;
    stack.push(invisibleRootItem());
    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == TaskTag) {
                const QString name = reader.attributes()
                        .value(NameAttribute).toString();
                bool done = reader.attributes().value(DoneAttribute)
                            == "1";
                StandardItem *nameItem = createNewTask(stack.top(),
                                                       name, done);
                stack.push(nameItem);
            }
            else if (reader.name() == WhenTag) {
                const QDateTime start = QDateTime::fromString(
                        reader.attributes().value(StartAttribute)
                            .toString(), Qt::ISODate);
                const QDateTime end = QDateTime::fromString(
                        reader.attributes().value(EndAttribute)
                            .toString(), Qt::ISODate);
                StandardItem *nameItem = static_cast<StandardItem*>(
                        stack.top());
                nameItem->addDateTime(start, end);
            }
        }
        else if (reader.isEndElement()) {
            if (reader.name() == TaskTag)
                stack.pop();
        }
    }
    if (reader.hasError())
        throw AQP::Error(reader.errorString());
    if (stack.count() != 1 || stack.top() != invisibleRootItem())
        throw AQP::Error(tr("loading error: possibly corrupt file"));

    calculateTotalsFor(invisibleRootItem());
}


StandardItem *StandardTreeModel::createNewTask(QStandardItem *root,
        const QString &name, bool done)
{
    StandardItem *nameItem = new StandardItem(name, done);
    root->appendRow(QList<QStandardItem*>() << nameItem
            << nameItem->todayItem() << nameItem->totalItem());
    return nameItem;
}


QStandardItem *StandardTreeModel::insertNewTask(Insert insert,
        const QString &name, const QModelIndex &index)
{
    QStandardItem *parent;
    if (insert == AtTopLevel)
        parent = invisibleRootItem();
    else {
        if (index.isValid()) {
            parent = itemFromIndex(index);
            if (!parent)
                return 0;
            if (insert == AsSibling)
                parent = parent->parent() ? parent->parent()
                                          : invisibleRootItem();
        }
        else
            return 0;
    }
    return createNewTask(parent, name, false);
}


QStringList StandardTreeModel::pathForIndex(const QModelIndex &index)
    const
{
    QStringList path;
    if (index.isValid()) {
        QStandardItem *item = itemFromIndex(index);
        while (item) {
            path.prepend(item->text());
            item = item->parent();
        }
    }
    return path;
}


QStandardItem *StandardTreeModel::itemForPath(const QStringList &path)
    const
{
    return itemForPath(invisibleRootItem(), path);
}


QStandardItem *StandardTreeModel::itemForPath(QStandardItem *root,
        const QStringList &path) const
{
    Q_ASSERT(root);
    if (path.isEmpty())
        return 0;
    for (int row = 0; row < root->rowCount(); ++row) {
        QStandardItem *item = root->child(row, 0);
        if (item->text() == path.at(0)) {
            if (path.count() == 1)
                return item;
            if ((item = itemForPath(item, path.mid(1))))
                return item;
        }
    }
    return 0;
}


void StandardTreeModel::calculateTotalsFor(QStandardItem *root)
{
    if (root != invisibleRootItem()) {
        StandardItem *item = static_cast<StandardItem*>(root);
        item->todayItem()->setText(item->todaysTime());
        item->totalItem()->setText(item->totalTime());
    }
    for (int row = 0; row < root->rowCount(); ++row)
        calculateTotalsFor(root->child(row, 0));
}


void StandardTreeModel::save(const QString &filename)
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
    writeTaskAndChildren(&writer, invisibleRootItem());
    writer.writeEndElement(); // TIMELOG
    writer.writeEndDocument();
}


void StandardTreeModel::writeTaskAndChildren(QXmlStreamWriter *writer,
                                             QStandardItem *root)
{
    if (root != invisibleRootItem()) {
        StandardItem *item = static_cast<StandardItem*>(root);
        writer->writeStartElement(TaskTag);
        writer->writeAttribute(NameAttribute, item->text());
        writer->writeAttribute(DoneAttribute,
                item->checkState() == Qt::Checked ? "1" : "0");
        QListIterator<
                QPair<QDateTime, QDateTime> > i(item->dateTimes());
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
    for (int row = 0; row < root->rowCount(); ++row)
        writeTaskAndChildren(writer, root->child(row, 0));
    if (root != invisibleRootItem())
        writer->writeEndElement(); // TASK
}
