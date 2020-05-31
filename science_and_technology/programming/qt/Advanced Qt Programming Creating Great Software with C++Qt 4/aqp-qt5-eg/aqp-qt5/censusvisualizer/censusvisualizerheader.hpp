#ifndef CENSUSVISUALIZERHEADER_HPP
#define CENSUSVISUALIZERHEADER_HPP
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

#include <QWidget>


class QPainter;
class QPaintEvent;
class CensusVisualizer;


class CensusVisualizerHeader : public QWidget
{
    Q_OBJECT

public:
    explicit CensusVisualizerHeader(QWidget *parent)
        : QWidget(parent) {}

    QSize minimumSizeHint() const;
    QSize sizeHint() const { return minimumSizeHint(); }

protected:
    void paintEvent(QPaintEvent *event);

private:
    void paintHeader(QPainter *painter, const int RowHeight);
    void paintHeaderItem(QPainter *painter, const QRect &rect,
                         const QString &text, bool selected);
};

#endif // CENSUSVISUALIZERHEADER_HPP
