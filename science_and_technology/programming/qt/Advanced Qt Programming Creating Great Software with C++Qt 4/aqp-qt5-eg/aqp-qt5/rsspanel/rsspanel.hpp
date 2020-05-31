#ifndef RSSPANEL_HPP
#define RSSPANEL_HPP
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
#include <QWidget>


class QTextBrowser;


class RssPanel : public QWidget
{
    Q_OBJECT

public:
    explicit RssPanel(QWidget *parent=0);

    QUrl url() const { return rssComboBox->url(); }
    int intervalMinutes() const
        { return rssComboBox->intervalMinutes(); }

public slots:
    void setUrl(const QUrl &url) { rssComboBox->setUrl(url); }
    void setIntervalMinutes(int minutes)
        { rssComboBox->setIntervalMinutes(minutes); }

private:
    RssComboBox *rssComboBox;
    QTextBrowser *textBrowser;
};

#endif // RSSPANEL_HPP
