#ifndef ZIPCODEITEM_HPP
#define ZIPCODEITEM_HPP
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
#include <QString>


struct ZipcodeItem
{
    explicit ZipcodeItem(int zipcode_=InvalidZipcode,
            const QString &postOffice_=QString(),
            const QString &county_=QString(),
            const QString &state_=QString())
        : zipcode(zipcode_), postOffice(postOffice_), county(county_),
          state(state_) {}

    bool operator<(const ZipcodeItem &other) const
        { return zipcode != other.zipcode ? zipcode < other.zipcode
                                    : postOffice < other.postOffice; }

    int zipcode;
    QString postOffice;
    QString county;
    QString state;
};


#endif // ZIPCODEITEM_HPP
