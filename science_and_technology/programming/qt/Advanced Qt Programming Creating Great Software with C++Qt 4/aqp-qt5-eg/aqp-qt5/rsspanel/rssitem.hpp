#ifndef RSSITEM_HPP
#define RSSITEM_HPP
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

#include <QMetaType>
#include <QString>


struct RssItem
{
    explicit RssItem(const QString &title_=QString(),
            const QString &url_=QString(),
            const QString &description_=QString())
        : title(title_), url(url_), description(description_) {}

    QString title;
    QString url;
    QString description;
};
Q_DECLARE_METATYPE(RssItem)

#endif // RSSITEM_HPP
