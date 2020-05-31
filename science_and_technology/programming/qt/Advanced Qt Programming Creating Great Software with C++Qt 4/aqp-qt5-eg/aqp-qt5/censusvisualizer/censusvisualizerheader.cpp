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

#include "censusvisualizer.hpp"
#include "censusvisualizerheader.hpp"
#include <QAbstractItemModel>
#include <QPainter>
#include <QPaintEvent>


QSize CensusVisualizerHeader::minimumSizeHint() const
{
    CensusVisualizer *visualizer = qobject_cast<CensusVisualizer*>(
                                                parent());
    Q_ASSERT(visualizer);
    return QSize(visualizer->widthOfYearColumn() +
                 visualizer->maleFemaleHeaderTextWidth() +
                 visualizer->widthOfTotalColumn(),
                 QFontMetrics(font()).height() + ExtraHeight);
}


void CensusVisualizerHeader::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);
    paintHeader(&painter, height());
    painter.setPen(QPen(palette().button().color().darker(), 0.5));
    painter.drawRect(0, 0, width(), height());
}


void CensusVisualizerHeader::paintHeader(QPainter *painter,
                                         const int RowHeight)
{
    const int Padding = 2;

    CensusVisualizer *visualizer = qobject_cast<CensusVisualizer*>(
                                                parent());
    Q_ASSERT(visualizer);
    paintHeaderItem(painter,
            QRect(0, 0, visualizer->widthOfYearColumn() + Padding,
                  RowHeight),
            visualizer->model()->headerData(Year, Qt::Horizontal)
                                            .toString(),
            visualizer->selectedColumn() == Year);

    paintHeaderItem(painter,
            QRect(visualizer->widthOfYearColumn() + Padding, 0,
                  visualizer->widthOfMaleFemaleColumn(), RowHeight),
            visualizer->maleFemaleHeaderText(),
            visualizer->selectedColumn() == Males ||
            visualizer->selectedColumn() == Females);
    paintHeaderItem(painter,
            QRect(visualizer->widthOfYearColumn() + Padding +
                  visualizer->widthOfMaleFemaleColumn(),
                  0, visualizer->widthOfTotalColumn(), RowHeight),
            visualizer->model()->headerData(Total, Qt::Horizontal)
                                            .toString(),
            visualizer->selectedColumn() == Total);
}


void CensusVisualizerHeader::paintHeaderItem(QPainter *painter,
        const QRect &rect, const QString &text, bool selected)
{
    CensusVisualizer *visualizer = qobject_cast<CensusVisualizer*>(
                                                parent());
    Q_ASSERT(visualizer);
    int x = rect.center().x();
    QLinearGradient gradient(x, rect.top(), x, rect.bottom());
    QColor color = selected ? palette().highlight().color()
                            : palette().button().color();
    gradient.setColorAt(0, color.darker(125));
    gradient.setColorAt(0.5, color.lighter(125));
    gradient.setColorAt(1, color.darker(125));
    painter->fillRect(rect, gradient);
    visualizer->paintItemBorder(painter, palette(), rect);
    painter->setPen(selected ? palette().highlightedText().color()
                             : palette().buttonText().color());
    painter->drawText(rect, text, QTextOption(Qt::AlignCenter));
}
