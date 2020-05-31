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

#include "alt_key.hpp"
#include "aqp.hpp"
#include "global.hpp"
#include "smileyitem.hpp"
#include <QAction>
#include <QGraphicsScene>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>


namespace {
const int SmileySize = 60;
const int SmileyHalfSize = SmileySize / 2;
const int SmileyMargin = 10;
const int SmileyEyeWidth = 16;
const int SmileyEyeHeight = 12;
}


SmileyItem::SmileyItem(const QPoint &position, QGraphicsScene *scene)
#if QT_VERSION >= 0x040600
    : QGraphicsObject(),
#else
    : QObject(), QGraphicsItem(),
#endif
      m_face(Happy), m_pen(Qt::NoPen), m_brush(Qt::yellow),
      m_showHat(false), m_angle(0.0), m_shearHorizontal(0.0),
      m_shearVertical(0.0)
{
    createPaths();
    setFlags(QGraphicsItem::ItemIsSelectable|
#if QT_VERSION >= 0x040600
             QGraphicsItem::ItemSendsGeometryChanges|
#endif
             QGraphicsItem::ItemIsMovable);
    setPos(position);
    scene->clearSelection();
    scene->addItem(this);
    setSelected(true);
}


void SmileyItem::createPaths()
{
    m_facePath.addEllipse(-SmileyHalfSize, -SmileyHalfSize,
                          SmileySize, SmileySize);

    const int LeftX = -(SmileyHalfSize + (SmileyMargin / 2));
    const int RightX = SmileyHalfSize - (SmileyMargin / 2);
    const int Y = -SmileyHalfSize + (SmileyMargin / 2);
    QPolygonF polygon;
    polygon << QPointF(LeftX * 1.4, Y + SmileyMargin)
            << QPointF(RightX * 1.3, Y)
            << QPointF(RightX * 0.8, Y - SmileyMargin)
            << QPointF(RightX * 0.5, Y - (SmileyMargin * 2))
            << QPointF(RightX * 0.2, Y - (SmileyMargin * 1.75))
            << QPointF(LeftX * 0.3, Y - (SmileyMargin * 2))
            << QPointF(LeftX * 0.6, Y - (SmileyMargin * 0.5))
            << QPointF(LeftX * 1.4, Y + SmileyMargin);
    m_hatPath.addPolygon(polygon);
}


void SmileyItem::setPen(const QPen &pen)
{
    if (isSelected() && m_pen != pen) {
        m_pen = pen;
        update();
        emit dirty();
    }
}


void SmileyItem::setBrush(const QBrush &brush)
{
    if (isSelected() && m_brush != brush) {
        m_brush = brush;
        update();
        emit dirty();
    }
}


void SmileyItem::setFace(Face face)
{
    if (m_face != face) {
        m_face = face;
        update();
        emit dirty();
    }
}


void SmileyItem::setShowHat(bool on)
{
    if (m_showHat != on) {
        prepareGeometryChange();
        m_showHat = on;
        emit dirty();
    }
}


void SmileyItem::edit()
{
    QMenu menu;
    QAction *showHatAction = createMenuAction(&menu, QIcon(),
            tr("Show Hat"), m_showHat);
    connect(showHatAction, SIGNAL(triggered(bool)),
            this, SLOT(setShowHat(bool)));
    menu.addSeparator();
    QActionGroup *group = new QActionGroup(this);
    createMenuAction(&menu, QIcon(":/smileysmile.png"),
            tr("Happy"), m_face == Happy, group, Happy);
    createMenuAction(&menu, QIcon(":/smileyneutral.png"),
            tr("Neutral"), m_face == Neutral, group, Neutral);
    createMenuAction(&menu, QIcon(":/smileysad.png"),
            tr("Sad"), m_face == Sad, group, Sad);
    AQP::accelerateMenu(&menu);
    QAction *chosen = menu.exec(QCursor::pos());
    if (chosen && chosen != showHatAction)
        setFace(static_cast<Face>(chosen->data().toInt()));
}


QAction *SmileyItem::createMenuAction(QMenu *menu, const QIcon &icon,
        const QString &text, bool checked, QActionGroup *group,
        const QVariant &data)
{
    QAction *action = menu->addAction(icon, text);
    action->setCheckable(true);
    action->setChecked(checked);
    if (group)
        group->addAction(action);
    action->setData(data);
    return action;
}


void SmileyItem::setAngle(double angle)
{
    if (isSelected() && !qFuzzyCompare(m_angle, angle)) {
        m_angle = angle;
        updateTransform();
    }
}


void SmileyItem::setShear(double shearHorizontal, double shearVertical)
{
    if (isSelected() &&
        (!qFuzzyCompare(m_shearHorizontal, shearHorizontal) ||
         !qFuzzyCompare(m_shearVertical, shearVertical))) {
        m_shearHorizontal = shearHorizontal;
        m_shearVertical = shearVertical;
        updateTransform();
    }
}


void SmileyItem::updateTransform()
{
    QTransform transform;
    transform.shear(m_shearHorizontal, m_shearVertical);
    transform.rotate(m_angle);
    setTransform(transform);
}


QVariant SmileyItem::itemChange(GraphicsItemChange change,
                                const QVariant &value)
{
    if (isDirtyChange(change))
        emit dirty();
    return QGraphicsItem::itemChange(change, value);
}


void SmileyItem::paint(QPainter *painter,
        const QStyleOptionGraphicsItem*, QWidget*)
{
    paintFace(painter);
    if (m_showHat)
        paintHat(painter);
    if (isSelected())
        paintSelectionOutline(painter);
}


void SmileyItem::paintFace(QPainter *painter)
{
    painter->setPen(m_pen);
    painter->setBrush(m_brush);
    painter->drawPath(m_facePath);
    int leftX = -SmileyHalfSize + SmileyMargin;
    int rightX = SmileyHalfSize - SmileyEyeWidth - SmileyMargin;
    paintEyes(painter, leftX, rightX);
    paintMouth(painter, leftX, rightX);
}


void SmileyItem::paintEyes(QPainter *painter, int leftX, int rightX)
{
    int y = -SmileyHalfSize + qRound(SmileyMargin * 1.5);
    painter->setBrush(m_brush.color().darker());
    painter->drawEllipse(leftX, y, SmileyEyeWidth, SmileyEyeHeight);
    painter->drawEllipse(rightX, y, SmileyEyeWidth, SmileyEyeHeight);
}


void SmileyItem::paintMouth(QPainter *painter, int leftX, int rightX)
{
    int y = SmileyHalfSize - qRound(SmileyMargin * 1.1);
    int offset = 0;
    if (m_face == Neutral)
        offset = SmileyMargin;
    else {
        offset = SmileyMargin / 2;
        if (m_face == Happy)
            y -= SmileyMargin;
        else if (m_face == Sad)
            y -= SmileyMargin / 2;
    }
    QPointF leftPoint(leftX + offset, y);
    QPointF rightPoint(rightX + SmileyEyeWidth - offset, y);
    QRectF mouthRect(leftPoint, rightPoint);
    mouthRect.setHeight(m_face == Neutral ? SmileyMargin / 2
                                          : SmileyMargin);
    if (m_face == Neutral)
        painter->drawRoundedRect(mouthRect, 5, 5);
    else if (m_face == Happy)
        painter->drawChord(mouthRect, 170 * 16, 200 * 16);
    else
        painter->drawChord(mouthRect, 30 * 16, 120 * 16);
}


void SmileyItem::paintHat(QPainter *painter)
{
    QPen pen(m_pen);
    if (pen.style() != Qt::NoPen)
        pen.setColor(pen.color().lighter());
    painter->setPen(pen);
    QBrush brush(m_brush);
    if (brush.style() != Qt::NoBrush)
        brush.setColor(brush.color().lighter());
    painter->setBrush(brush);
    painter->drawPath(m_hatPath);
}


void SmileyItem::paintSelectionOutline(QPainter *painter)
{
    QPen pen(Qt::DashLine);
    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(m_showHat ? m_facePath.united(m_hatPath)
                                : m_facePath);
}


QPainterPath SmileyItem::shape() const
{
    QPainterPath path;
    path.addPath(m_facePath);
    if (m_showHat)
        path.addPath(m_hatPath);
    return path;
}


QRectF SmileyItem::boundingRect() const
{
    QRectF rect(-SmileyHalfSize, -SmileyHalfSize, SmileySize,
                SmileySize);
    if (m_showHat)
        rect = rect.united(m_hatPath.boundingRect());
    return rect;
}


QDataStream &operator<<(QDataStream &out,
                        const SmileyItem &smileyItem)
{
    out << smileyItem.pos() << smileyItem.angle()
        << smileyItem.shearHorizontal() << smileyItem.shearVertical()
        << smileyItem.zValue()
        << static_cast<qint16>(smileyItem.face())
        << smileyItem.pen() << smileyItem.brush()
        << smileyItem.isShowingHat();
    return out;
}


QDataStream &operator>>(QDataStream &in, SmileyItem &smileyItem)
{
    QPointF position;
    double angle;
    double shearHorizontal;
    double shearVertical;
    double z;
    qint16 face;
    QPen pen;
    QBrush brush;
    bool showHat;
    in >> position >> angle >> shearHorizontal >> shearVertical >> z
       >> face >> pen >> brush >> showHat;
    smileyItem.setPos(position);
    smileyItem.setAngle(angle);
    smileyItem.setShear(shearHorizontal, shearVertical);
    smileyItem.setZValue(z);
    smileyItem.setFace(static_cast<SmileyItem::Face>(face));
    smileyItem.setPen(pen);
    smileyItem.setBrush(brush);
    smileyItem.setShowHat(showHat);
    return in;
}
