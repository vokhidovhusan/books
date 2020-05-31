#ifndef WEBPAGE_HPP
#define WEBPAGE_HPP
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

#include <QUrl>
#include <QWebPage>


class WebPage : public QWebPage
{
public:
    explicit WebPage(QObject *parent=0) : QWebPage(parent) {}

protected:
    QObject* createPlugin(const QString &classId,
            const QUrl &url=QUrl(),
            const QStringList &parameterNames=QStringList(),
            const QStringList &parameterValues=QStringList());
};

#endif // WEBPAGE_HPP
