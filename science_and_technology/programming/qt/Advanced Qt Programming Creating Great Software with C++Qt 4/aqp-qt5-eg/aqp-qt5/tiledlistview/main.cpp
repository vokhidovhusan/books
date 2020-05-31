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

#include "tiledlistview.hpp"
#include <QAction>
#include <QApplication>
#include <QMainWindow>
#include <QListView>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QtWidgets> // added for Qt5


void populateModel(QStandardItemModel *model)
{
    foreach (const QString &name, QStringList()
            << "Barack Obama" << "George W. Bush" << "Bill Clinton"
            << "George H. W. Bush" << "Ronald Reagan"
            << "Jimmy Carter" << "Gerald Ford" << "Richard Nixon"
            << "Lyndon B. Johnson" << "John F. Kennedy"
            << "Dwight D. Eisenhower" << "Harry S. Truman"
            << "Franklin D. Roosevelt" << "Herbert Hoover"
            << "Calvin Coolidge" << "Warren G. Harding"
            << "Woodrow Wilson" << "William Howard Taft"
            << "Theodore Roosevelt" << "William McKinley"
            << "Grover Cleveland" << "Benjamin Harrison"
            << "Grover Cleveland" << "Chester A. Arthur"
            << "James A. Garfield" << "Rutherford B. Hayes"
            << "Ulysses S. Grant" << "Andrew Johnson"
            << "Abraham Lincoln" << "James Buchanan"
            << "Franklin Pierce" << "Millard Fillmore"
            << "Zachary Taylor" << "James K. Polk" << "John Tyler"
            << "William Henry Harrison" << "Martin Van Buren"
            << "Andrew Jackson" << "John Quincy Adams"
            << "James Monroe" << "James Madison"
            << "Thomas Jefferson" << "John Adams"
            << "George Washington")
        model->appendRow(new QStandardItem(name));
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(app.translate("main", "Tiled List View"));
#ifdef Q_WS_MAC
    app.setCursorFlashTime(0);
#endif

    QStandardItemModel *model = new QStandardItemModel;
    populateModel(model);
    TiledListView *tiledListView = new TiledListView;
    tiledListView->setModel(model);
    QListView *listView = new QListView;
    listView->setModel(model);

    QMainWindow *window = new QMainWindow;
    QMenu *fileMenu = window->menuBar()->addMenu(
            app.translate("main", "&File"));
    QAction *quitAction = fileMenu->addAction(
            app.translate("main", "&Quit"));
    QSplitter *splitter = new QSplitter;
    splitter->addWidget(listView);
    splitter->addWidget(tiledListView);
    window->statusBar()->clearMessage(); // create statusbar + useful size grip
    window->setCentralWidget(splitter);
    window->resize(600, 350);
    window->setWindowTitle(app.applicationName());
    window->show();
    window->connect(listView->selectionModel(),
            SIGNAL(currentChanged(const QModelIndex&,
                                  const QModelIndex&)),
            tiledListView, SLOT(setCurrentIndex(const QModelIndex&)));
    window->connect(tiledListView->selectionModel(),
            SIGNAL(currentChanged(const QModelIndex&,
                                  const QModelIndex&)),
            listView, SLOT(setCurrentIndex(const QModelIndex&)));
    window->connect(quitAction, SIGNAL(triggered()),
                    window, SLOT(close()));
    int result = app.exec();
    delete window;
    delete model;
    return result;
}
