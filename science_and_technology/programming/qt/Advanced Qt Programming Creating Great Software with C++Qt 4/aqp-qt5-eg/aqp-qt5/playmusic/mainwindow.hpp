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

#include "treewidgetitem.hpp"
#include <phonon/phononnamespace.h>
#include <QHash>
#include <QIcon>
#include <QMainWindow>
#include <QSet>


namespace Phonon
{
    class AudioOutput;
    class MediaObject;
    class SeekSlider;
    class VolumeSlider;
}

class QAction;
class QEvent;
class QLCDNumber;
class QToolBar;
class QToolButton;
class QTreeWidget;
class QTreeWidgetItem;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=0);

protected:
    bool eventFilter(QObject *target, QEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void playOrPause();
    void currentSourceChanged();
    void stop();
    void tick(qint64 msec);
    void stateChanged(Phonon::State newState);
    void aboutToFinish();
    void setMusicDirectory();
    void currentItemChanged(QTreeWidgetItem *item);
    void playTrack(QTreeWidgetItem *item);

private:
    void createActions();
    void createToolBar();
    void createWidgets();
    void createLayout();
    void createConnections();
    QSet<QString> getSuffixes();
    void addTrack(const QString &filename,
            QHash<QString, TreeWidgetItem*> *itemForArtist,
            QHash<QString, TreeWidgetItem*> *itemForArtistAlbum);
    bool waitForMediaObjectToLoad(Phonon::MediaObject *mediaObject,
                                  int timeoutMSec);
    QString minutesSecondsAsStringForMSec(qint64 msec);

    Phonon::SeekSlider *seekSlider;
    Phonon::VolumeSlider *volumeSlider;
    Phonon::MediaObject *mediaObject;
    Phonon::AudioOutput *audioOutput;

    QAction *setMusicDirectoryAction;
    QAction *playOrPauseAction;
    QAction *stopAction;
    QAction *quitAction;
    QLCDNumber *timeLcd;
    QToolBar *toolbar;
    QToolButton *quitButton;
    QTreeWidget *treeWidget;
    QTreeWidgetItem *nextItem;
    QIcon playIcon;
    QIcon pauseIcon;
};

#endif // MAINWINDOW_HPP
