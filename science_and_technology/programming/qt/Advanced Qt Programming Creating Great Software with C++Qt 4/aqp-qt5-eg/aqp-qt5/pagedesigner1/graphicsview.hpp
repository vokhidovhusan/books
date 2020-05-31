#ifndef GRAPHICSVIEW_HPP
#define GRAPHICSVIEW_HPP
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

#include <QGraphicsView>
#include <QPainter>
#include <QWheelEvent>
#include <cmath>


class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GraphicsView(QWidget *parent=0) : QGraphicsView(parent)
    {
        setDragMode(RubberBandDrag);
        setRenderHints(QPainter::Antialiasing|
                       QPainter::TextAntialiasing);
    }

public slots:
    void zoomIn() { scaleBy(1.1); }
    void zoomOut() { scaleBy(1.0 / 1.1); }

protected:
    void wheelEvent(QWheelEvent *event)
        { scaleBy(std::pow(4.0 / 3.0, (-event->delta() / 240.0))); }

private:
    void scaleBy(double factor) { scale(factor, factor); }
};

#endif // GRAPHICSVIEW_HPP
