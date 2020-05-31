#ifndef SMILEYITEM_HPP
#define SMILEYITEM_HPP
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
#if QT_VERSION >= 0x040600
#include <QGraphicsObject>
#else
#include <QGraphicsItem>
#endif
#include <QPainterPath>
#include <QPen>


class QAction;
class QActionGroup;
class QGraphicsScene;


#if QT_VERSION >= 0x040600
class SmileyItem : public QGraphicsObject
#else
class SmileyItem : public QObject, public QGraphicsItem
#endif
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
    enum Face {Happy, Sad, Neutral};
    enum {Type = SmileyItemType};

    explicit SmileyItem(const QPoint &position,
                        QGraphicsScene *scene);
    int type() const { return Type; }

    double angle() const { return m_angle; }
    double shearHorizontal() const { return m_shearHorizontal; }
    double shearVertical() const { return m_shearVertical; }

    QPen pen() const { return m_pen; }
    QBrush brush() const { return m_brush; }
    Face face() const { return m_face; }
    bool isShowingHat() const { return m_showHat; }

    void paint(QPainter *painter,
            const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    QPainterPath shape() const;

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
    void setFace(Face face);
    void setShowHat(bool on);
    void edit();

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) { edit(); }
    void contextMenuEvent(QGraphicsSceneContextMenuEvent*) { edit(); }

private:
    void updateTransform();
    void paintFace(QPainter *painter);
    void paintEyes(QPainter *painter, int leftX, int rightX);
    void paintMouth(QPainter *painter, int leftX, int rightX);
    void paintHat(QPainter *painter);
    void paintSelectionOutline(QPainter *painter);
    void createPaths();
    QAction *createMenuAction(QMenu *menu, const QIcon &icon,
            const QString &text, bool checked,
            QActionGroup *group=0, const QVariant &data=QVariant());

    Face m_face;
    QPen m_pen;
    QBrush m_brush;
    bool m_showHat;
    QPainterPath m_facePath;
    QPainterPath m_hatPath;
    double m_angle;
    double m_shearHorizontal;
    double m_shearVertical;
};


QDataStream &operator<<(QDataStream &out,
                        const SmileyItem &smileyItem);
QDataStream &operator>>(QDataStream &in, SmileyItem &smileyItem);


#endif // SMILEYITEM_HPP
