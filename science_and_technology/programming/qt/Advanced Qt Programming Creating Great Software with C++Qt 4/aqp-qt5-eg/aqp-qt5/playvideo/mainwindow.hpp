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

#include <phonon/phononnamespace.h>
#include <QIcon>
#include <QMainWindow>


namespace Phonon
{
    class AudioOutput;
    class MediaObject;
    class SeekSlider;
    class VideoWidget;
    class VolumeSlider;
}

class QAction;
class QLCDNumber;
class QToolBar;
class QToolButton;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=0);

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *target, QEvent *event);

private slots:
    void playOrPause();
    void stop();
    void tick(qint64 msec);
    void stateChanged(Phonon::State newState);
    void chooseVideo();

private:
    void createActions();
    void createToolBar();
    void createWidgets();
    void createLayout();
    void createConnections();
    QString getFileFormats();

    Phonon::SeekSlider *seekSlider;
    Phonon::MediaObject *mediaObject;
    Phonon::AudioOutput *audioOutput;
    Phonon::VolumeSlider *volumeSlider;
    Phonon::VideoWidget *videoWidget;

    QAction *chooseVideoAction;
    QAction *playOrPauseAction;
    QAction *stopAction;
    QAction *fullScreenAction;
    QAction *quitAction;
    QToolBar *toolbar;
    QToolButton *quitButton;
    QLCDNumber *timeLcd;
    QIcon playIcon;
    QIcon pauseIcon;
};

#endif // MAINWINDOW_HPP
