#ifndef BRUSHWIDGET_HPP
#define BRUSHWIDGET_HPP
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

#include <QBrush>
#include <QWidget>


class QComboBox;


class BrushWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BrushWidget(QWidget *parent=0);

    QBrush brush() const { return m_brush; }

public slots:
    void setBrush(const QBrush &brush);

signals:
    void brushChanged(const QBrush &brush);

private slots:
    void updateColor(int index);
    void updateStyle(int index);

private:
    void createWidgets();
    void createLayout();
    void createConnections();
    void updateSwatches();

    QComboBox *colorComboBox;
    QComboBox *styleComboBox;

    QBrush m_brush;
};

#endif // BRUSHWIDGET_HPP
