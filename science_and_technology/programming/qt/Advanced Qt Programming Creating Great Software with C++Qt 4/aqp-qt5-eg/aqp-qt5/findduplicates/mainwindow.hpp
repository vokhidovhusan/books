#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
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

#include "global.hpp"
#include <QList>
#include <QMainWindow>
#include <QPointer>


class QCloseEvent;
class QLabel;
class QLineEdit;
class QPushButton;
class QStandardItemModel;
class QTreeView;
class GetMD5sThread;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=0);

private slots:
    void find();
    void completed();
    void finished();
    void cancel();
    void updateUi();
    void readOneFile();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void createWidgets();
    void createLayout();
    void createConnections();
    void prepareToProcess();
    void processDirectories(const QStringList &directories);
    void processResults();
    void addOneResult(const QStringList &files, qint64 *maximumSize);
    void updateView(qint64 maximumSize);
    void stopThreads();

    QLabel *rootDirectoryLabel;
    QLineEdit *rootDirectoryEdit;
    QPushButton *findButton;
    QPushButton *cancelButton;
    QPushButton *quitButton;
    QStandardItemModel *model;
    QTreeView *view;

    volatile bool stopped;
    QList<QPointer<GetMD5sThread> > threads;
    FilesForMD5 filesForMD5;
};

#endif // MAINWINDOW_HPP
