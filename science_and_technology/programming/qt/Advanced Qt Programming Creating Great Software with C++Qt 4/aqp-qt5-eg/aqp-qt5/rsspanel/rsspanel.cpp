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

#include "rsspanel.hpp"
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>


RssPanel::RssPanel(QWidget *parent)
    : QWidget(parent)
{
    rssComboBox = new RssComboBox;
    QPushButton *openPageButton = new QPushButton("Open &Page...");
    textBrowser = new QTextBrowser;

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(rssComboBox, 1);
    topLayout->addWidget(openPageButton);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(topLayout);
    layout->addWidget(textBrowser, 1);
    setLayout(layout);

    connect(openPageButton, SIGNAL(clicked()),
            rssComboBox, SLOT(openUrl()));
    connect(rssComboBox, SIGNAL(updatedDescription(const QString&)),
            textBrowser, SLOT(setHtml(const QString&)));
}
