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
#include <QApplication>
#include <QStringList>
#include <QTextStream>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv, false);
    QTextStream out(stdout);
    out << "Application's executable directory is "
        << AQP::applicationPathOf() << "\n";
    out << "Application's images directory is "
        << AQP::applicationPathOf("images") << "\n";
    out << AQP::filenameFilter("HTML", QStringList() << "text/html")
        << "\n";
    out << AQP::filenameFilter("Audio", QStringList() << "audio/mpeg"
        << "audio/ogg") << "\n";
    out << AQP::filenameFilter("Video", QStringList() << "video/3gpp"
        << "video/dv" << "video/mpeg") << "\n";
}

