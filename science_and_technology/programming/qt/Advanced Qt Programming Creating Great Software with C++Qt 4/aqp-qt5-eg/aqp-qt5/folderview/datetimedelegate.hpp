#ifndef DATETIMEDELEGATE_HPP
#define DATETIMEDELEGATE_HPP
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

#include <QStyledItemDelegate>


class QModelIndex;
class QPainter;
class QStyleOptionViewItem;


class DateTimeDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit DateTimeDelegate(QObject *parent=0)
        : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

private:
    QRectF clockRect(const QRectF &rect, const qreal &diameter) const;
    void drawClockFace(QPainter *painter, const QRectF &rect,
                       const QDateTime &lastModified) const;
    void drawClockHand(QPainter *painter, const QPointF &center,
            const qreal &length, const qreal &degrees) const;
    void drawDate(QPainter *painter,
            const QStyleOptionViewItem &option, const qreal &size,
            const QDateTime &lastModified) const;
};

#endif // DATETIMEDELEGATE_HPP
