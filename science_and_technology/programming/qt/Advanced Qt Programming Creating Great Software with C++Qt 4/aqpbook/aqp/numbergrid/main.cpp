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
#include <QTranslator>
#include <ctime>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(app.translate("main", "Number Grid"));
    app.setWindowIcon(QIcon(":/icon.png"));
#ifdef Q_WS_MAC
    app.setCursorFlashTime(0);
#endif
    QTranslator translator;
    translator.load(AQP::applicationPathOf() + "/numbergrid_en.qm");
    app.installTranslator(&translator);

    qsrand(static_cast<uint>(time(0)));
    MainWindow window;
    window.resize(800, 600);
    window.show();
    return app.exec();
}
