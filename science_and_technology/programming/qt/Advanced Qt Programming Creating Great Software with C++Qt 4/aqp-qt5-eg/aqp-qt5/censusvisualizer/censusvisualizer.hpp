#ifndef CENSUSVISUALIZER_HPP
#define CENSUSVISUALIZER_HPP
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


class QAbstractItemModel;
class QModelIndex;
class QScrollArea;
class CensusVisualizerHeader;
class CensusVisualizerView;


const int ExtraHeight = 5;
const int ExtraWidth = 5;
const int Invalid = -1;
enum {Year, Males, Females, Total};


class CensusVisualizer : public QWidget
{
    Q_OBJECT

public:
    explicit CensusVisualizer(QWidget *parent=0);

    QAbstractItemModel *model() const { return m_model; }
    void setModel(QAbstractItemModel *model);
    QScrollArea *scrollArea() const { return m_scrollArea; }
    int maximumPopulation() const { return m_maximumPopulation; }
    int widthOfYearColumn() const { return m_widthOfYearColumn; }
    int widthOfMaleFemaleColumn() const;
    int widthOfTotalColumn() const { return m_widthOfTotalColumn; }
    int selectedRow() const { return m_selectedRow; }
    void setSelectedRow(int row);
    int selectedColumn() const { return m_selectedColumn; }
    void setSelectedColumn(int column);

    void paintItemBorder(QPainter *painter, const QPalette &palette,
                         const QRect &rect);
    QString maleFemaleHeaderText() const;
    int maleFemaleHeaderTextWidth() const;
    int xOffsetForMiddleOfColumn(int column) const;
    int yOffsetForRow(int row) const;

public slots:
    void setCurrentIndex(const QModelIndex &index);

signals:
    void clicked(const QModelIndex&);

private:
    QAbstractItemModel *m_model;
    QScrollArea *m_scrollArea;

    CensusVisualizerHeader *header;
    CensusVisualizerView *view;

    int m_widthOfYearColumn;
    int m_widthOfTotalColumn;
    int m_selectedRow;
    int m_selectedColumn;
    int m_maximumPopulation;
};

#endif // CENSUSVISUALIZER_HPP
