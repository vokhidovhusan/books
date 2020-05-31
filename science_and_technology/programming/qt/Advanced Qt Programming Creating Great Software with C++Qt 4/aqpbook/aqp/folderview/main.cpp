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

#include "datetimedelegate.hpp"
#include <QAction>
#include <QApplication>
#include <QMainWindow>
#include <QDir>
#include <QFileSystemModel>
#include <QMenu>
#include <QMenuBar>
#include <QHeaderView>
#include <QStatusBar>
#include <QSplitter>
#include <QTreeView>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(app.translate("main", "Folder View"));
#ifdef Q_WS_MAC
    app.setCursorFlashTime(0);
#endif

    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath(QDir::homePath());
    QModelIndex index = model->index(QDir::homePath());

    QTreeView *viewWithoutDelegate = new QTreeView;
//    QHeaderView *header = viewWithoutDelegate->header();
//    header->setResizeMode(QHeaderView::ResizeToContents);
    viewWithoutDelegate->setModel(model);
    viewWithoutDelegate->setColumnHidden(1, true); // for screenshots
    viewWithoutDelegate->setColumnHidden(2, true); // for screenshots
    viewWithoutDelegate->scrollTo(index);
    viewWithoutDelegate->expand(index);
    viewWithoutDelegate->setCurrentIndex(index);

    QTreeView *view = new QTreeView;
    view->setItemDelegateForColumn(3, new DateTimeDelegate);
//    header = view->header();
//    header->setResizeMode(QHeaderView::ResizeToContents);
    view->setModel(model);
    view->setColumnHidden(1, true);
    view->setColumnHidden(2, true);
    view->scrollTo(index);
    view->expand(index);
    view->setCurrentIndex(index);

    QMainWindow *window = new QMainWindow;
    QMenu *fileMenu = window->menuBar()->addMenu(
            app.translate("main", "&File"));
    QAction *quitAction = fileMenu->addAction(
            app.translate("main", "&Quit"));
    QSplitter *splitter = new QSplitter;
    splitter->addWidget(viewWithoutDelegate);
    splitter->addWidget(view);
    window->statusBar()->clearMessage(); // create statusbar + useful size grip
    window->setCentralWidget(splitter);
    window->resize(800, 480);
    window->setWindowTitle(app.applicationName());
    window->show();
    window->connect(quitAction, SIGNAL(triggered()),
                    window, SLOT(close()));
    viewWithoutDelegate->setColumnWidth(0, window->width() / 4);
    view->setColumnWidth(0, window->width() / 4);
    int result = app.exec();
    delete window;
    delete model;
    return result;
}

