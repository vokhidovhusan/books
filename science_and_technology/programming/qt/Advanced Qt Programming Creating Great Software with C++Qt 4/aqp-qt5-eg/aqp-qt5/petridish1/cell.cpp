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
#include "cell.hpp"
#include <QStyleOptionGraphicsItem>
#include <QPainter>


inline qreal randomReal() { return ((qrand() % 100) / 100.0); }


bool Cell::s_showIds = true;


Cell::Cell(int id, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_id(id)
{
    m_brush = QBrush(QColor(qrand() % 200, qrand() % 200,
                     qrand() % 200, qMax(127, qrand() % 256)));
    m_size = 5.5 + (qrand() % 10);
    shrinkOrGrow();
}


void Cell::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option, QWidget*)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_brush);
    painter->drawPath(m_path);
    if (s_showIds) {
        QPointF center = m_path.boundingRect().center();
        QString id = QString::number(m_id);
        center.setX(center.x() - (option->fontMetrics.width(id) / 2));
        center.setY(center.y() + (option->fontMetrics.height() / 4));
        painter->setPen(QPen());
        painter->drawText(center, id);
    }
}


Cell::CellState Cell::shrinkOrGrow()
{
    int dishSize = qRound(parentItem()->boundingRect().width());
    int neighbours = 0;
    QListIterator<QGraphicsItem*> i(parentItem()->childItems());
    while (i.hasNext()) {
        QGraphicsItem *item = i.next();
        if (item != this && collidesWithItem(item))
            ++neighbours;
    }
    if (!neighbours || m_size > dishSize / 3)
        m_size *= randomReal(); // shrink - lonely or too big
    else if (neighbours < 4) // grow - happy
        m_size *= ((5 - neighbours) * randomReal());
    else // shrink - too crowded
        m_size *= ((1.0 / neighbours) + randomReal());

    QPainterPath path;
#ifdef MSVC_COMPILER
    qreal x = m_size * cos(AQP::radiansFromDegrees(1));
    qreal y = m_size * sin(AQP::radiansFromDegrees(1));
#else
    qreal x = m_size * std::cos(AQP::radiansFromDegrees(1));
    qreal y = m_size * std::sin(AQP::radiansFromDegrees(1));
#endif
    path.moveTo(x, y);
    for (int angle = 1; angle < 360; ++angle) {
        qreal factor = m_size + ((m_size / 3) * (randomReal() - 0.5));
#ifdef MSVC_COMPILER
        x = factor * cos(AQP::radiansFromDegrees(angle));
        y = factor * sin(AQP::radiansFromDegrees(angle));
#else
        x = factor * std::cos(AQP::radiansFromDegrees(angle));
        y = factor * std::sin(AQP::radiansFromDegrees(angle));
#endif
        path.lineTo(x, y);
    }
    path.closeSubpath();
    m_path = path;

    prepareGeometryChange();
    if (m_size < 5.0 && (qrand() % 20 == 0))
        return Die; // small ones randomly die
    return Live;
}
