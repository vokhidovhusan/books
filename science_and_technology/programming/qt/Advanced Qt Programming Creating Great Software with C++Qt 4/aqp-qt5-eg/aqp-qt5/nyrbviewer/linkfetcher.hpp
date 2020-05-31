#ifndef LINKFETCHER_HPP
#define LINKFETCHER_HPP
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

#include <QMap>
#include <QObject>
#include <QString>
#include <QWebPage>


class LinkFetcher : public QObject
{
    Q_OBJECT

public:
    explicit LinkFetcher(const QString &site_,
            const QString &scriptOrScriptName_, QObject *parent=0);

    void load(const QString &url);
    QMap<QString, QString> namesForUrls() const
        { return m_namesForUrls; }
    void clear() { m_namesForUrls.clear(); }

signals:
    void finished(bool);

public slots:
    void addUrlAndName(const QString &url, const QString &name);

    void debug(const QString &value)
        { qDebug("%s", qPrintable(value)); }

private slots:
    void injectJavaScriptIntoWindowObject();
    void fetchLinks(bool ok);

private:
    QWebPage page;
    QMap<QString, QString> m_namesForUrls;
    const QString site;
    const QString scriptOrScriptName;
};

#endif // LINKFETCHER_HPP
