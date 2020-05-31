#ifndef CELL_HPP
#define CELL_HPP
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

#include <QBrush>
#include <QGraphicsItem>


class QPainter;
class QStyleOptionGraphicsItem;


class Cell : public QGraphicsItem
{
public:
    enum {Type = UserType + 1};
    enum CellState {Live, Die};

    explicit Cell(int id, QGraphicsItem *parent=0);

    QRectF boundingRect() const { return m_path.boundingRect(); }
    QPainterPath shape() const { return m_path; }
    void paint(QPainter *painter,
            const QStyleOptionGraphicsItem *option, QWidget *widget);
    int type() const { return Type; }

    CellState shrinkOrGrow();
    int id() const { return m_id; }

    static bool showIds() { return s_showIds; }
    static void setShowIds(bool show) { s_showIds = show; }

private:
    static bool s_showIds;

    QBrush m_brush;
    QPainterPath m_path;
    const int m_id;
    qreal m_size;
};

#endif // CELL_HPP
