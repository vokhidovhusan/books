#ifndef RSSCOMBOBOX_HPP
#define RSSCOMBOBOX_HPP
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

#include <QComboBox>
#include <QUrl>


class QNetworkAccessManager;
class QNetworkReply;


class RssComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit RssComboBox(QWidget *parent=0);

    QUrl url() const { return m_url; }
    int intervalMinutes() const { return intervalMSec / 60000; }

signals:
    void updatedDescription(const QString &text);

public slots:
    void openUrl();
    void setUrl(const QUrl &url) { m_url = url; }
    void setIntervalMinutes(int minutes)
        { intervalMSec =
                qBound(2 * 60000, minutes * 60000, 120 * 60000); }

private slots:
    void requestFeed();
    void readFeed(QNetworkReply *reply);
    void updateDescription(int index);

private:
    QNetworkAccessManager *networkAccess;
    int intervalMSec;
    QUrl m_url;
};

#endif // RSSCOMBOBOX_HPP
