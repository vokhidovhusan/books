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
#include <QTime>
#include <QTimeLine>
#include <QTimer>


class QAction;
class QStandardItem;
class StandardItem;
class QModelIndex;
class QTreeView;
#ifdef CUSTOM_MODEL
class TreeModel;
#else
class StandardTreeModel;
#endif


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=0);

public slots:
    void stopTiming();

protected:
    void closeEvent(QCloseEvent*);

private slots:
    void fileNew();
    void fileOpen();
    bool fileSave();
    bool fileSaveAs();
    void editAdd();
    void editDelete();
#ifdef CUSTOM_MODEL
    void editCut();
    void editPaste();
    void editMoveUp();
    void editMoveDown();
    void editPromote();
    void editDemote();
#endif
    void editStartOrStop(bool start);
    void editHideOrShowDoneTasks(bool hide);
    void setDirty(bool dirty=true) { setWindowModified(dirty); }
    void load(const QString &filename,
              const QStringList &taskPath=QStringList());
    void timeout();
    void updateIcon(int frame);
    void updateUi();

private:
    void createModelAndView();
    void createActions();
    void createMenusAndToolBar();
    void createConnections();
    bool okToClearData();
    void setCurrentIndex(const QModelIndex &index);
#ifdef CUSTOM_MODEL
    void hideOrShowDoneTask(bool hide, const QModelIndex &index);
#else
    void hideOrShowDoneTask(bool hide, QStandardItem *item);
#endif

    QAction *fileNewAction;
    QAction *fileOpenAction;
    QAction *fileSaveAction;
    QAction *fileSaveAsAction;
    QAction *fileQuitAction;
    QAction *editAddAction;
    QAction *editDeleteAction;
#ifdef CUSTOM_MODEL
    QAction *editCutAction;
    QAction *editPasteAction;
    QAction *editMoveUpAction;
    QAction *editMoveDownAction;
    QAction *editPromoteAction;
    QAction *editDemoteAction;
#endif
    QAction *editStartOrStopAction;
    QAction *editHideOrShowDoneTasksAction;

    QTreeView *treeView;
#ifdef CUSTOM_MODEL
    TreeModel *model;
#else
    StandardTreeModel *model;
    StandardItem *timedItem;
#endif
    QTimer timer;
    QTimeLine iconTimeLine;
    QTime timedTime;
    int currentIcon;
};

#endif // MAINWINDOW_HPP

