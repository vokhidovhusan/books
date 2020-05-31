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
#include "xmledit.hpp"
#include <QApplication>
#include <QFile>
#include <QShortcut>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(app.translate("main", "XmlEdit Widget"));
#ifdef Q_WS_MAC
    app.setCursorFlashTime(0);
#endif

    QString filename;
    if (argc == 2)
        filename = argv[1];
    else {
        QTextStream err(stderr);
        err << app.translate("main", "usage: xmledit <xmlfile>\n");
        filename = AQP::applicationPathOf() + "/jen.xbel";
        if (!QFile::exists(filename))
            return 2;
    }
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream err(stderr);
        err << app.translate("main", "failed to read %1: %2\n")
               .arg(file.fileName()).arg(file.errorString());
        return 1;

    }
    XmlEdit xmlEdit;
    QTextStream in(&file);
    in.setCodec("utf-8");
    xmlEdit.setPlainText(in.readAll());
    file.close();
    xmlEdit.setWindowTitle(app.applicationName());
    xmlEdit.resize(800, 600);
    xmlEdit.show();
#if QT_VERSION >= 0x040600
    (void) new QShortcut(QKeySequence::Quit,
                         &xmlEdit, SLOT(close()));
#else
    (void) new QShortcut(QKeySequence("Ctrl+Q"),
                         &xmlEdit, SLOT(close()));
#endif
    return app.exec();
}
