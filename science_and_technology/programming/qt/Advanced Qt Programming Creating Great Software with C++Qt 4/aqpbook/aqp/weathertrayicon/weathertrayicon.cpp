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

#include "alt_key.hpp"
#include "weathertrayicon.hpp"
#include <QApplication>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QStringList>
#ifndef Q_WS_X11
#include <QTextDocumentFragment>
#endif
#include <QTimer>


#if QT_VERSION < 0x040700
inline uint qHash(const QUrl &url) { return qHash(url.toString()); }
#endif


WeatherTrayIcon::WeatherTrayIcon()
    : QSystemTrayIcon(), retryDelaySec(1)
{
    setIcon(QIcon(":/rss.png"));
    createContextMenu();

    networkXmlAccess = new QNetworkAccessManager(this);
    networkIconAccess = new QNetworkAccessManager(this);
    connect(networkXmlAccess, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(readXml(QNetworkReply*)));
    connect(networkIconAccess, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(readIcon(QNetworkReply*)));

    QTimer::singleShot(0, this, SLOT(requestXml()));
}


void WeatherTrayIcon::createContextMenu()
{
    QStringList airports;
    airports << "Austin-Bergstrom International Airport (KAUS)"
             << "Chicago/Ohare (KORD)"
             << "Dallas / Fort Worth International Airport (KDFW)"
             << "Detroit City Airport (KDET)"
             << "Houston Intercontinental Airport (KIAH)"
             << "Indianapolis International Airport (KIND)"
             << "Jacksonville International Airport (KJAX)"
             << "Los Angeles Intl Airport (KLAX)"
             << "New York/John F. Kennedy Intl Airport (KJFK)"
             << "Philadelphia International Airport (KPHL)"
             << "Phoenix/Sky Harbor (KPHX)"
             << "Port Columbus International Airport (KCMH)"
             << "San Antonio International Airport (KSAT)"
             << "San Diego/Brown Fld (KSDM)"
             << "San Francisco Intl Airport (KSFO)"
             << "San Jose International Airport (KSJC)";
    QSettings settings;
    airport = settings.value("airport", QVariant(airports.at(0)))
                             .toString();

    QActionGroup *group = new QActionGroup(this);
    foreach (const QString &anAirport, airports) {
        QAction *action = menu.addAction(anAirport);
        group->addAction(action);
        action->setCheckable(true);
        action->setChecked(anAirport == airport);
        action->setData(anAirport);
    }
    connect(group, SIGNAL(triggered(QAction*)),
            this, SLOT(setAirport(QAction*)));
    menu.addSeparator();
    menu.addAction(QIcon(":/exit.png"), tr("E&xit"), qApp,
                   SLOT(quit()));

    AQP::accelerateMenu(&menu);
    setContextMenu(&menu);
}


void WeatherTrayIcon::requestXml()
{
#ifdef NO_REGEX
    QString airportId = airport.right(6);
    if (airportId.startsWith("(") && airportId.endsWith(")")) {
        QString url = QString("http://www.weather.gov/xml/"
                "current_obs/%1.xml").arg(airportId.mid(1, 4));
        networkXmlAccess->get(QNetworkRequest(QUrl(url)));
    }
#else
    QRegExp airportIdRx("^.*[(]([A-Z]{4})[)]$");
    airportIdRx.setPatternSyntax(QRegExp::RegExp2);
    if (airportIdRx.exactMatch(airport)) {
        QString url = QString("http://www.weather.gov/xml/"
                "current_obs/%1.xml").arg(airportIdRx.cap(1));
        networkXmlAccess->get(QNetworkRequest(QUrl(url)));
    }
#endif
    setToolTip(tr("Failed to connect to network.\n"
            "A proxy can be set on the command line:\n"
            "weathertrayicon --help"));
}


void WeatherTrayIcon::readXml(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        setToolTip(tr("Failed to retrieve weather data:\n%1")
                   .arg(reply->errorString()));
        QTimer::singleShot(retryDelaySec * 1000,
                           this, SLOT(requestXml()));
        retryDelaySec <<= 1;
        if (retryDelaySec > 60 * 60)
            retryDelaySec = 1;
        return;
    }
    retryDelaySec = 1;
    QDomDocument document;
    if (document.setContent(reply))
        populateToolTip(&document);
    QTimer::singleShot(60 * 60 * 1000, this, SLOT(requestXml()));
}


void WeatherTrayIcon::populateToolTip(QDomDocument *document)
{
    QString toolTipText = tr("<font color=darkblue>%1</font><br>")
                             .arg(airport);
    QString weather = textForTag("weather", document);
    if (!weather.isEmpty())
        toolTipText += toolTipField("Weather", "green", weather);
    QString color("green");
    QString fahrenheit = textForTag("temp_f", document);
    if (!fahrenheit.isEmpty()) {
        bool ok;
        int f = fahrenheit.toInt(&ok);
        if (ok) {
            if (f <= 32)
                color = "blue";
            else if (f >= 85)
                color = "red";
        }
    }
    QString temperature = textForTag("temperature_string", document);
    if (!temperature.isEmpty())
        toolTipText += toolTipField("Temp.", color, temperature);
    QString wind = textForTag("wind_string", document);
    if (!wind.isEmpty())
        toolTipText += toolTipField("Wind", "green", wind, false);
    QString iconUrl = textForTag("icon_url_base", document);
    if (!iconUrl.isEmpty()) {
        QString name = textForTag("icon_url_name", document);
        if (!name.isEmpty()) {
            iconUrl += name;
            QUrl url(iconUrl);
            QIcon *icon = iconCache.object(url);
            if (icon && !icon->isNull())
                setIcon(*icon);
            else
                networkIconAccess->get(QNetworkRequest(url));
        }
    }
#ifndef Q_WS_X11
    toolTipText = QTextDocumentFragment::fromHtml(toolTipText)
                  .toPlainText();
#endif
    setToolTip(toolTipText);
}


QString WeatherTrayIcon::textForTag(const QString &tag,
                                    QDomDocument *document)
{
    QDomNodeList nodes = document->elementsByTagName(tag);
    if (!nodes.isEmpty()) {
        const QDomNode &node = nodes.item(0);
        if (!node.isNull() && node.hasChildNodes())
            return node.firstChild().nodeValue();
    }
    return QString();
}


QString WeatherTrayIcon::toolTipField(const QString &name,
        const QString &htmlColor, const QString &value, bool appendBr)
{
    return QString("<i>%1:</i>&nbsp;<font color=\"%2\">%3</font>%4")
                   .arg(name).arg(htmlColor).arg(value)
                   .arg(appendBr ? "<br>" : "");
}


void WeatherTrayIcon::readIcon(QNetworkReply *reply)
{
    QUrl redirect = reply->attribute(
            QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (redirect.isValid())
        networkIconAccess->get(QNetworkRequest(redirect));
    else {
        QByteArray ba(reply->readAll());
        QPixmap pixmap;
        if (pixmap.loadFromData(ba)) {
            QIcon *icon = new QIcon(pixmap);
            setIcon(*icon);
            iconCache.insert(reply->request().url(), icon);
        }
    }
}


void WeatherTrayIcon::setAirport(QAction *action)
{
    airport = action->data().toString();
    QSettings settings;
    settings.setValue("airport", airport);
    requestXml();
}
