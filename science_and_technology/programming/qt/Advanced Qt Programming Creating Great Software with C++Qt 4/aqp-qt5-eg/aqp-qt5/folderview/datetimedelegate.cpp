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
#include "datetimedelegate.hpp"
#include <QDateTime>
#include <QFileSystemModel>
#include <QModelIndex>
#include <QPainter>
#include <QPoint>


void DateTimeDelegate::paint(QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    const QFileSystemModel *model =
            qobject_cast<const QFileSystemModel*>(index.model());
    Q_ASSERT(model);
    const QDateTime &lastModified = model->lastModified(index);
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing|
                            QPainter::TextAntialiasing);

    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());
    const qreal diameter = qMin(option.rect.width(),
                                option.rect.height());
    const QRectF rect = clockRect(option.rect, diameter);
    drawClockFace(painter, rect, lastModified);
    drawClockHand(painter, rect.center(), diameter / 3.5,
                  ((lastModified.time().hour() +
                   (lastModified.time().minute() / 60.0))) * 30);
    drawClockHand(painter, rect.center(), diameter / 2.5,
                  lastModified.time().minute() * 6);
    drawDate(painter, option, diameter, lastModified);
    painter->restore();
}


QRectF DateTimeDelegate::clockRect(const QRectF &rect,
                                   const qreal &diameter) const
{
    QRectF rectangle(rect);
    rectangle.setWidth(diameter);
    rectangle.setHeight(diameter);
    return rectangle.adjusted(1.5, 1.5, -1.5, -1.5);
}


void DateTimeDelegate::drawClockFace(QPainter *painter,
        const QRectF &rect, const QDateTime &lastModified) const
{
    const int Light = 120;
    const int Dark = 220;

    int shade = lastModified.date() == QDate::currentDate()
                ? Light : Dark;
    QColor background(shade, shade,
                      lastModified.time().hour() < 12 ? 255 : 175);
    painter->setPen(background);
    painter->setBrush(background);
    painter->drawEllipse(rect);
    shade = shade == Light ? Dark : Light;
    painter->setPen(QColor(shade, shade,
                    lastModified.time().hour() < 12 ? 175 : 255));
}


void DateTimeDelegate::drawClockHand(QPainter *painter,
        const QPointF &center, const qreal &length,
        const qreal &degrees) const
{
    const qreal angle = AQP::radiansFromDegrees(
            (qRound(degrees) % 360) - 90);
#ifdef MSVC_COMPILER
    const qreal x = length * cos(angle);
    const qreal y = length * sin(angle);
#else
    const qreal x = length * std::cos(angle);
    const qreal y = length * std::sin(angle);
#endif
    painter->drawLine(center, center + QPointF(x, y));
}


void DateTimeDelegate::drawDate(QPainter *painter,
        const QStyleOptionViewItem &option, const qreal &diameter,
        const QDateTime &lastModified) const
{
    painter->setPen(option.state & QStyle::State_Selected
                    ? option.palette.highlightedText().color()
                    : option.palette.windowText().color());
#ifndef DEBUG
    QString text = lastModified.date().toString(Qt::ISODate);
#else
    QString text = lastModified.toString(Qt::ISODate);
#endif
    painter->drawText(option.rect.adjusted(
            qRound(diameter * 1.2), 0, 0, 0), text,
            QTextOption(Qt::AlignVCenter|Qt::AlignLeft));
}
