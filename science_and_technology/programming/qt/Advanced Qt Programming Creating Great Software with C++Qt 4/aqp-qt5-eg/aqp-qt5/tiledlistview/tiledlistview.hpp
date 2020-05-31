#ifndef TILEDLISTVIEW_HPP
#define TILEDLISTVIEW_HPP
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

#include <QAbstractItemView>
#include <QHash>
#include <QRectF>


class TiledListView : public QAbstractItemView
{
    Q_OBJECT

public:
    explicit TiledListView(QWidget *parent=0);

    void setModel(QAbstractItemModel *model);
    QRect visualRect(const QModelIndex &index) const;
    void scrollTo(const QModelIndex &index,
                  QAbstractItemView::ScrollHint);
    QModelIndex indexAt(const QPoint &point_) const;

protected slots:
    void dataChanged(const QModelIndex &topLeft,
                     const QModelIndex &bottomRight);
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start,
                              int end);
    void updateGeometries();

protected:
    QModelIndex moveCursor(
            QAbstractItemView::CursorAction cursorAction,
            Qt::KeyboardModifiers modifiers);
    bool isIndexHidden(const QModelIndex&) const { return false; }
    int horizontalOffset() const;
    int verticalOffset() const;
    void scrollContentsBy(int dx, int dy);
    void setSelection(const QRect &rect,
            QFlags<QItemSelectionModel::SelectionFlag> flags);
    QRegion visualRegionForSelection(
            const QItemSelection &selection) const;

    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void mousePressEvent(QMouseEvent *event);

private:
    void calculateRectsIfNecessary() const;
    QRectF viewportRectForRow(int row) const;
    void paintOutline(QPainter *painter, const QRectF &rectangle);

    mutable int idealWidth;
    mutable int idealHeight;
    mutable QHash<int, QRectF> rectForRow;
    mutable bool hashIsDirty;
};

#endif // TILEDLISTVIEW_HPP
