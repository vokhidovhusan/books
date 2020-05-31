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
#include <QTranslator>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(app.translate("main", "Zipcodes"));
#ifdef Q_WS_MAC
    app.setCursorFlashTime(0);
#endif

    QTranslator translator;
    translator.load(AQP::applicationPathOf() + "/zipcodes_en.qm");
    app.installTranslator(&translator);

    MainWindow window;
    window.resize(800, 640);
    window.show();
    return app.exec();
}
