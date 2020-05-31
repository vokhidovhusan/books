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
#include "censusvisualizerview.hpp"
#include <QAbstractItemModel>
#include <QLocale>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollArea>
#include <QScrollBar>


CensusVisualizerView::CensusVisualizerView(QWidget *parent)
    : QWidget(parent)
{
    visualizer = qobject_cast<CensusVisualizer*>(parent);
    Q_ASSERT(visualizer);
    setFocusPolicy(Qt::WheelFocus);
    setMinimumSize(minimumSizeHint());
}


QSize CensusVisualizerView::minimumSizeHint() const
{
    return QSize(visualizer->widthOfYearColumn() +
                 visualizer->maleFemaleHeaderTextWidth() +
                 visualizer->widthOfTotalColumn(),
                 QFontMetrics(font()).height() + ExtraHeight);
}


QSize CensusVisualizerView::sizeHint() const
{
    int rows = visualizer->model()
               ? visualizer->model()->rowCount() : 1;
    return QSize(visualizer->widthOfYearColumn() +
            qMax(100, visualizer->maleFemaleHeaderTextWidth()) +
            visualizer->widthOfTotalColumn(),
            visualizer->yOffsetForRow(rows));
}


bool CensusVisualizerView::eventFilter(QObject *target, QEvent *event)
{
    if (QScrollArea *scrollArea = visualizer->scrollArea()) {
        if (target == scrollArea && event->type() == QEvent::Resize) {
            if (QResizeEvent *resizeEvent =
                    static_cast<QResizeEvent*>(event)) {
                QSize size = resizeEvent->size();
                size.setHeight(sizeHint().height());
                int width = size.width() - (ExtraWidth +
                        scrollArea->verticalScrollBar()->sizeHint()
                        .width());
                size.setWidth(width);
                resize(size);
            }
        }
    }
    return QWidget::eventFilter(target, event);
}


void CensusVisualizerView::mousePressEvent(QMouseEvent *event)
{
    int row = static_cast<int>(event->y() /
              (QFontMetricsF(font()).height() + ExtraHeight));
    int column;
    if (event->x() < visualizer->widthOfYearColumn())
        column = Year;
    else if (event->x() < (visualizer->widthOfYearColumn() +
                           visualizer->widthOfMaleFemaleColumn() / 2))
        column = Males;
    else if (event->x() < (visualizer->widthOfYearColumn() +
                           visualizer->widthOfMaleFemaleColumn()))
        column = Females;
    else
        column = Total;
    visualizer->setSelectedRow(row);
    visualizer->setSelectedColumn(column);
    emit clicked(visualizer->model()->index(row, column));
}


void CensusVisualizerView::keyPressEvent(QKeyEvent *event)
{
    if (visualizer->model()) {
        int row = Invalid;
        int column = Invalid;
        if (event->key() == Qt::Key_Left) {
            column = visualizer->selectedColumn();
            if (column == Males || column == Total)
                --column;
            else if (column == Females)
                column = Year;
        }
        else if (event->key() == Qt::Key_Right) {
            column = visualizer->selectedColumn();
            if (column == Year || column == Females)
                ++column;
            else if (column == Males)
                column = Total;
        }
        else if (event->key() == Qt::Key_Up)
            row = qMax(0, visualizer->selectedRow() - 1);
        else if (event->key() == Qt::Key_Down)
            row = qMin(visualizer->selectedRow() + 1,
                       visualizer->model()->rowCount() - 1);
        row = row == Invalid ? visualizer->selectedRow() : row;
        column = column == Invalid ? visualizer->selectedColumn()
                                   : column;
        if (row != visualizer->selectedRow() ||
            column != visualizer->selectedColumn()) {
            QModelIndex index = visualizer->model()->index(row,
                                                           column);
            visualizer->setCurrentIndex(index);
            emit clicked(index);
            return;
        }
    }
    QWidget::keyPressEvent(event);
}


void CensusVisualizerView::paintEvent(QPaintEvent *event)
{
    if (!visualizer->model())
        return;
    QFontMetricsF fm(font());
    const int RowHeight = fm.height() + ExtraHeight;
    const int MinY = qMax(0, event->rect().y() - RowHeight);
    const int MaxY = MinY + event->rect().height() + RowHeight;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);

    int row = MinY / RowHeight;
    int y = row * RowHeight;
    for (; row < visualizer->model()->rowCount(); ++row) {
        paintRow(&painter, row, y, RowHeight);
        y += RowHeight;
        if (y > MaxY)
            break;
    }
}


void CensusVisualizerView::paintRow(QPainter *painter, int row,
                                    int y, const int RowHeight)
{
    paintYear(painter, row,
            QRect(0, y, visualizer->widthOfYearColumn(), RowHeight));
    paintMaleFemale(painter, row,
            QRect(visualizer->widthOfYearColumn(), y,
                  visualizer->widthOfMaleFemaleColumn(), RowHeight));
    paintTotal(painter, row,
            QRect(visualizer->widthOfYearColumn() +
                  visualizer->widthOfMaleFemaleColumn(), y,
                  visualizer->widthOfTotalColumn(), RowHeight));
}


void CensusVisualizerView::paintYear(QPainter *painter, int row,
                                     const QRect &rect)
{
    paintItemBackground(painter, rect,
                        row == visualizer->selectedRow() &&
                        visualizer->selectedColumn() == Year);
    painter->drawText(rect,
            visualizer->model()->data(
                    visualizer->model()->index(row, Year)).toString(),
            QTextOption(Qt::AlignCenter));
}


void CensusVisualizerView::paintItemBackground(QPainter *painter,
        const QRect &rect, bool selected)
{
    painter->fillRect(rect, selected ? palette().highlight()
                                     : palette().base());
    visualizer->paintItemBorder(painter, palette(), rect);
    painter->setPen(selected ? palette().highlightedText().color()
                             : palette().windowText().color());
}


// QLocale is needed because the data is stored as _strings_ with
// locale-specific separators, e.g., 8.392.419 or 8,392,419.
void CensusVisualizerView::paintMaleFemale(QPainter *painter,
        int row, const QRect &rect)
{
    QRect rectangle(rect);
    QLocale locale;
    int males = locale.toInt(visualizer->model()->data(
            visualizer->model()->index(row, Males)).toString());
    int females = locale.toInt(visualizer->model()->data(
            visualizer->model()->index(row, Females)).toString());
    qreal total = males + females;
    int offset = qRound(
            ((1 - (total / visualizer->maximumPopulation())) / 2) *
             rectangle.width());

    painter->fillRect(rectangle,
            (row == visualizer->selectedRow() &&
             (visualizer->selectedColumn() == Females ||
              visualizer->selectedColumn() == Males))
            ? palette().highlight() : palette().base());

    visualizer->paintItemBorder(painter, palette(), rectangle);
    rectangle.setLeft(rectangle.left() + offset);
    rectangle.setRight(rectangle.right() - offset);
    int rectY = rectangle.center().y();
    painter->fillRect(rectangle.adjusted(0, 1, 0, -1),
            maleFemaleGradient(rectangle.left(), rectY,
                    rectangle.right(), rectY, males / total));
}


QLinearGradient CensusVisualizerView::maleFemaleGradient(
        qreal x1, qreal y1, qreal x2, qreal y2, qreal crossOver)
{
    QLinearGradient gradient(x1, y1, x2, y2);
    QColor maleColor = Qt::green;
    QColor femaleColor = Qt::red;
    gradient.setColorAt(0, maleColor.darker());
    gradient.setColorAt(crossOver - 0.001, maleColor.lighter());
    gradient.setColorAt(crossOver + 0.001, femaleColor.lighter());
    gradient.setColorAt(1, femaleColor.darker());
    return gradient;
}


void CensusVisualizerView::paintTotal(QPainter *painter, int row,
                                      const QRect &rect)
{
    paintItemBackground(painter, rect,
                        row == visualizer->selectedRow() &&
                        visualizer->selectedColumn() == Total);
 // allow right margin
    painter->drawText(rect.adjusted(0, 0, -5, 0),
            visualizer->model()->data(
                visualizer->model()->index(row, Total)).toString(),
            QTextOption(Qt::AlignVCenter|Qt::AlignRight));
}
