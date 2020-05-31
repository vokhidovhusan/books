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

#include <QMainWindow>
#include <QPixmap>


class JingleAction;
class QLabel;
class QMovie;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum ReloadMode {DontReload, Reload};

    explicit MainWindow(QWidget *parent=0);

private slots:
    void fileOpen();
    void fileSave();
    void startOrStop(ReloadMode reloadMode=Reload);
    void takeSnapshot();
    void muteJingles(bool mute);
    void updateUi();

private:
    enum MovieState {NoMovie, Stopped, Playing};

    void createActions();
    void createMenusAndToolBar();
    void createWidgets();
    void createLayout();
    void createConnections();

    JingleAction *fileOpenAction;
    JingleAction *fileSaveAction;
    JingleAction *fileMuteJinglesAction;
    JingleAction *startOrStopAction;
    JingleAction *takeSnapshotAction;
    MovieState movieState;
    QAction *fileQuitAction;
    QLabel *movieLabel;
    QMovie *movie;
    QLabel *snapshotLabel;
    QPixmap snapshot;
    QString jinglePath;
    QString imagePath;
};

#endif // MAINWINDOW_HPP
