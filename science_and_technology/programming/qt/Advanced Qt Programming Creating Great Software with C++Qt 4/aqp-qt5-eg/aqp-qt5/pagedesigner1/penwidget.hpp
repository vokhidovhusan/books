#ifndef PENWIDGET_HPP
#define PENWIDGET_HPP
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

#include <QPen>
#include <QWidget>


class QComboBox;
class QSpinBox;


class PenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PenWidget(QWidget *parent=0);

    QPen pen() const { return m_pen; }

signals:
    void penChanged(const QPen &pen);

public slots:
    void setPen(const QPen &pen);

private slots:
    void updateColor(int index);
    void updateWidth(int value);
    void updateLineStyle(int index);
    void updateCapStyle(int index);
    void updateJoinStyle(int index);

private:
    void createWidgets();
    void createLayout();
    void createConnections();
    void updateSwatches();

    QComboBox *colorComboBox;
    QSpinBox *widthSpinBox;
    QComboBox *lineStyleComboBox;
    QComboBox *capStyleComboBox;
    QComboBox *joinStyleComboBox;

    QPen m_pen;
};

#endif // PENWIDGET_HPP
