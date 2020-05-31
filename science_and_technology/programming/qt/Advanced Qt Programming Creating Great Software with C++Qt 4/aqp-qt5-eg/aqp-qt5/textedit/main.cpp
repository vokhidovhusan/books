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

#include "textedit.hpp"
#include <QApplication>
#include <QtWidgets> // added for Qt5


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(app.translate("main", "Text Edit"));
#ifdef Q_WS_MAC
    app.setCursorFlashTime(0);
#endif

    TextEdit textEdit;
    textEdit.setHtml(
"<p style='font-size: 15pt; color: magenta'>"
"<b>Now</b>, what I want is, <i>Facts</i>. "
"Teach these boys and girls nothing but <i>Facts</i>. "
"<i>Facts</i> alone are wanted in life. "
"Plant nothing else, and root out everything else. "
"You can only form the minds of reasoning animals upon <i>Facts</i>: "
"nothing else will ever be of any service to them. "
"This is the principle on which I bring up my own children, "
"and this is the principle on which I bring up these children. "
"Stick to <i>Facts</i>, sir!</p>");
    textEdit.show();
    textEdit.setWindowTitle(app.applicationName());
    return app.exec();
}
