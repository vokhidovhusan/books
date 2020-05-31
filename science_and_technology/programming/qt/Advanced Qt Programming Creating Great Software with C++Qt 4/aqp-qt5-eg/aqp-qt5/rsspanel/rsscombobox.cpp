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

#include "rsscombobox.hpp"
#include "rssitem.hpp"
#include "rssreader.hpp"
#include <QDesktopServices>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QWebSettings>


RssComboBox::RssComboBox(QWidget *parent)
    : QComboBox(parent), intervalMSec(5 * 60000)
{
    addItem(tr("Feed gathering..."));
    networkAccess = new QNetworkAccessManager(this);

    connect(networkAccess, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(readFeed(QNetworkReply*)));
    connect(this, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateDescription(int)));

    QTimer::singleShot(0, this, SLOT(requestFeed()));
}


void RssComboBox::requestFeed()
{
    if (!m_url.isEmpty())
        networkAccess->get(QNetworkRequest(m_url));
}


void RssComboBox::readFeed(QNetworkReply *reply)
{
    clear();
    RssItem errorItem;
    QVariant rssVariantItem;
    RssReader reader;
    if (!reader.read(reply)) {
        errorItem.description = tr("failed to read RSS feed:\n");
        if (!reply->errorString().isEmpty())
            errorItem.description.append(reply->errorString());
        else
            errorItem.description.append(reader.errorString());
    }
    else {
        QVectorIterator<RssItem> i(reader.items());
        while (i.hasNext()) {
            const RssItem &item = i.next();
            rssVariantItem.setValue(item);
            addItem(QWebSettings::iconForUrl(item.url), item.title,
                    rssVariantItem);
        }
    }
    if (!count()) {
        errorItem.title = tr("No feed items found, "
                             "will try again later");
        rssVariantItem.setValue(errorItem);
        addItem(errorItem.title, rssVariantItem);
    }
    QTimer::singleShot(intervalMSec, this, SLOT(requestFeed()));
}


void RssComboBox::updateDescription(int index)
{
    RssItem item = itemData(index).value<RssItem>();
    if (item.url.isEmpty() && item.description.isEmpty())
        emit updatedDescription(itemText(index));
    else
        emit updatedDescription(tr("<b>%1</b><br>%2")
                .arg(itemText(index)).arg(item.description));
}


void RssComboBox::openUrl()
{
    RssItem item = itemData(currentIndex()).value<RssItem>();
    if (!item.url.isEmpty())
        QDesktopServices::openUrl(QUrl(item.url));
}
