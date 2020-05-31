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

#include "aqp.hpp"
#include "browserwindow.hpp"
#include "webpage.hpp"
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWebSettings>
#include <ctime>
#include <QtWebKitWidgets> //added for Qt5


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(app.translate("main", "Matrix Quiz"));
#ifdef Q_WS_MAC
    app.setCursorFlashTime(0);
#endif

    qsrand(static_cast<uint>(time(0)));

    QWebSettings *webSettings = QWebSettings::globalSettings();
    webSettings->setAttribute(QWebSettings::AutoLoadImages, true);
    webSettings->setAttribute(QWebSettings::JavascriptEnabled, true);
    webSettings->setAttribute(QWebSettings::PluginsEnabled, true);

    QString url = QUrl::fromLocalFile(AQP::applicationPathOf() +
                                      "/matrixquiz.html").toString();
    BrowserWindow *browser = new BrowserWindow(url, new WebPage);
    browser->showToolBar(false);
    browser->enableActions(false);
    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    QPushButton *quitButton = buttonBox->addButton(
            app.translate("main", "&Quit"),
            QDialogButtonBox::AcceptRole);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(browser, 1);
    layout->addWidget(buttonBox);
    QDialog dialog;
    dialog.setLayout(layout);
    QObject::connect(quitButton, SIGNAL(clicked()),
                     &dialog, SLOT(accept()));
    dialog.setWindowTitle(app.applicationName());
    dialog.show();
    return app.exec();
}
