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

#include "standarditem.hpp"


StandardItem::StandardItem(const QString &text, bool done)
    : QStandardItem(text)
{
    setCheckable(true);
    setCheckState(done ? Qt::Checked : Qt::Unchecked);
    setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled|
             Qt::ItemIsEditable|Qt::ItemIsUserCheckable);
    m_today = new QStandardItem;
    m_today->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    m_today->setTextAlignment(Qt::AlignVCenter|Qt::AlignRight);
    m_total = new QStandardItem;
    m_total->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    m_total->setTextAlignment(Qt::AlignVCenter|Qt::AlignRight);
}


void StandardItem::incrementLastEndTime(int msec)
{
    Q_ASSERT(!m_dateTimes.isEmpty());
    QDateTime &endTime = m_dateTimes.last().second;
    endTime.setTime(endTime.time().addMSecs(msec));
}


QString StandardItem::todaysTime() const
{
    int minutes = minutesForTask(true);
    return QString("%1:%2").arg(minutes / 60)
                           .arg(minutes % 60, 2, 10, QChar('0'));
}


QString StandardItem::totalTime() const
{
    int minutes = minutesForTask(false);
    return QString("%1:%2").arg(minutes / 60)
                           .arg(minutes % 60, 2, 10, QChar('0'));
}


int StandardItem::minutesForTask(bool onlyForToday) const
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
    for (int row = 0; row < rowCount(); ++row) {
        StandardItem *item = static_cast<StandardItem*>(child(row,
                                                              0));
        Q_ASSERT(item);
        minutes += item->minutesForTask(onlyForToday);
    }
    return minutes;
}
