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

#include "rssreader.hpp"


bool RssReader::read(QIODevice *device)
{
    m_items.clear();
    setDevice(device);
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == "item")
                readItem();
            else
                readUnknownElement();
        }
    }
    return !error();
}


void RssReader::readItem()
{
    Q_ASSERT(isStartElement() && name() == "item");

    QString title;
    QString link;
    QString description;

    while (!atEnd()) {
        readNext();
        if (isEndElement() && name() == "item") {
            m_items << RssItem(title, link, description);
            break;
        }
        if (isStartElement()) {
            if (name() == "title")
                title = readElementText();
            else if (name() == "link")
                link = readElementText();
            else if (name() == "description")
                description = readElementText();
            else
                readUnknownElement();
        }
    }
}


void RssReader::readUnknownElement()
{
    Q_ASSERT(isStartElement());

    while (!atEnd()) {
        readNext();
        if (isEndElement())
            break;
    }
}
