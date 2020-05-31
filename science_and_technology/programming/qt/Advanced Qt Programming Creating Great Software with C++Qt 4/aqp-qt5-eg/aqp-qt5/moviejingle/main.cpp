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
#include "mainwindow.hpp"
#include <QApplication>
#include <QIcon>
#include <QtWidgets> // added for Qt5
#include <QtMultimedia> // added for Qt5


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(app.translate("main", "Movie Jingle"));
    app.setWindowIcon(QIcon(AQP::applicationPathOf("images") +
                            "/icon.png"));
#ifdef Q_WS_MAC
    app.setCursorFlashTime(0);
#endif

    MainWindow window;
    window.resize(400, 300);
    window.show();
    return app.exec();
}
