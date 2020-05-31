#ifndef ZIPCODESPINBOX_HPP
#define ZIPCODESPINBOX_HPP
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

#include "global.hpp"
#include <QSpinBox>


class ZipcodeSpinBox : public QSpinBox
{
    Q_OBJECT

public:
    explicit ZipcodeSpinBox(QWidget *parent)
        : QSpinBox(parent)
    {
        setRange(MinZipcode, MaxZipcode);
        setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    }

protected:
    QString textFromValue(int value) const
        { return QString("%1").arg(value, 5, 10, QChar('0')); }
};

#endif // ZIPCODESPINBOX_HPP
