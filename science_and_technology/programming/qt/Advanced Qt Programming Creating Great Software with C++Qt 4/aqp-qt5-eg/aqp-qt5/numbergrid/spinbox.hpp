#ifndef SPINBOX_HPP
#define SPINBOX_HPP
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

#include <QDoubleSpinBox>
#include <limits>


class SpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    explicit SpinBox(QWidget *parent=0)
        : QDoubleSpinBox(parent)
    {
        setRange(-std::numeric_limits<double>::max(),
                 std::numeric_limits<double>::max());
        setDecimals(3);
        setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    }
};

#endif // SPINBOX_HPP

