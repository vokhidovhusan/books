#ifndef TRANSFORMWIDGET_HPP
#define TRANSFORMWIDGET_HPP
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


class QDoubleSpinBox;


class TransformWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransformWidget(QWidget *parent=0);

    void setAngle(double angle);
    void setShear(double shearHorizontal, double shearVertical);

signals:
    void angleChanged(double angle);
    void shearChanged(double shearHorizontal, double shearVertical);

private slots:
    void updateShear();

private:
    void createWidgets();
    void createLayout();
    void createConnections();

    QDoubleSpinBox *angleSpinBox;
    QDoubleSpinBox *shearHorizontalSpinBox;
    QDoubleSpinBox *shearVerticalSpinBox;
};

#endif // TRANSFORMWIDGET_HPP
