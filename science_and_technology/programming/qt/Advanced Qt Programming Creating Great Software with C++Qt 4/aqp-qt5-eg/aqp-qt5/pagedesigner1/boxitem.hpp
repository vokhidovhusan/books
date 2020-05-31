#ifndef BOXITEM_HPP
#define BOXITEM_HPP
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

#include "itemtypes.hpp"
#include <QBrush>
#include <QGraphicsRectItem>
#include <QPen>


class QDataStream;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QKeyEvent;


class BoxItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
#ifdef ANIMATE_ALIGNMENT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
#endif
    Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
    Q_PROPERTY(QPen pen READ pen WRITE setPen)
    Q_PROPERTY(double angle READ angle WRITE setAngle)
    Q_PROPERTY(double shearHorizontal READ shearHorizontal
               WRITE setShearHorizontal)
    Q_PROPERTY(double shearVertical READ shearVertical
               WRITE setShearVertical)

public:
    enum {Type = BoxItemType};

    explicit BoxItem(const QRect &rect, QGraphicsScene *scene);
    int type() const { return Type; }

    double angle() const { return m_angle; }
    double shearHorizontal() const { return m_shearHorizontal; }
    double shearVertical() const { return m_shearVertical; }

signals:
    void dirty();

public slots:
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAngle(double angle);
    void setShearHorizontal(double shearHorizontal)
        { setShear(shearHorizontal, m_shearVertical); }
    void setShearVertical(double shearVertical)
        { setShear(m_shearHorizontal, shearVertical); }
    void setShear(double shearHorizontal, double shearVertical);

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    void updateTransform();

    bool m_resizing;
    double m_angle;
    double m_shearHorizontal;
    double m_shearVertical;
};


QDataStream &operator<<(QDataStream &out, const BoxItem &boxItem);
QDataStream &operator>>(QDataStream &in, BoxItem &boxItem);


#endif // BOXITEM_HPP
