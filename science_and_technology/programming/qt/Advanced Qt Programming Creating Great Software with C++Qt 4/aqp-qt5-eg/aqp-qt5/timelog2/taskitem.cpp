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


TaskItem::TaskItem(const QString &name, bool done, TaskItem *parent)
        : m_name(name), m_done(done), m_parent(parent)
{
    if (m_parent)
        m_parent->addChild(this);
}


QString TaskItem::todaysTime() const
{
    int minutes = minutesForTask(true);
    return QString("%1:%2").arg(minutes / 60)
                           .arg(minutes % 60, 2, 10, QChar('0'));
}


QString TaskItem::totalTime() const
{
    int minutes = minutesForTask(false);
    return QString("%1:%2").arg(minutes / 60)
                           .arg(minutes % 60, 2, 10, QChar('0'));
}


int TaskItem::minutesForTask(bool onlyForToday) const
{
    int minutes = 0;
    QListIterator<QPair<QDateTime, QDateTime> > i(m_dateTimes);
    while (i.hasNext()) {
        const QPair<QDateTime, QDateTime> &dateTime = i.next();
        if (onlyForToday &&
            dateTime.first.date() != QDate::currentDate())
            continue;
        minutes += (dateTime.first.secsTo(dateTime.second) / 60);
    }
    foreach (TaskItem *child, m_children)
        minutes += child->minutesForTask(onlyForToday);
    return minutes;
}


void TaskItem::incrementLastEndTime(int msec)
{
    QDateTime &endTime = m_dateTimes.last().second;
    endTime.setTime(endTime.time().addMSecs(msec));
}


TaskItem* TaskItem::takeChild(int row)
{
    TaskItem *item = m_children.takeAt(row);
    Q_ASSERT(item);
    item->m_parent = 0;
    return item;
}
