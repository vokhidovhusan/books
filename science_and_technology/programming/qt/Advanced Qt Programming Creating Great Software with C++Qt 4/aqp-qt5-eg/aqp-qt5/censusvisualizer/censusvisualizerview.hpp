#ifndef CENSUSVISUALIZERVIEW_HPP
#define CENSUSVISUALIZERVIEW_HPP
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

#include <QLinearGradient>
#include <QWidget>


class QModelIndex;
class QPainter;
class QPaintEvent;
class CensusVisualizer;


class CensusVisualizerView : public QWidget
{
    Q_OBJECT

public:
    explicit CensusVisualizerView(QWidget *parent);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

signals:
    void clicked(const QModelIndex&);

protected:
    bool eventFilter(QObject *target, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void paintRow(QPainter *painter, int row, int y,
                  const int RowHeight);
    void paintItemBackground(QPainter *painter, const QRect &rect,
                             bool selected);
    void paintYear(QPainter *painter, int row, const QRect &rect);
    void paintMaleFemale(QPainter *painter, int row,
                         const QRect &rect);
    void paintTotal(QPainter *painter, int row, const QRect &rect);
    QLinearGradient maleFemaleGradient(qreal x1, qreal y1,
            qreal x2, qreal y2, qreal crossOver);

    CensusVisualizer *visualizer;
};

#endif // CENSUSVISUALIZERVIEW_HPP
