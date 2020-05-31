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
#include "mainwindow.hpp"
#include <phonon/audiooutput.h>
#include <phonon/backendcapabilities.h>
#include <phonon/mediaobject.h>
#include <phonon/seekslider.h>
#include <phonon/videowidget.h>
#include <phonon/volumeslider.h>
#include <QAction>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QEvent>
#include <QEventLoop>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QIcon>
#include <QLCDNumber>
#include <QShortcut>
#include <QTime>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>


namespace {
const int OneSecond = AQP::MSecPerSecond;
const QString ZeroTime(" 0:00");
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    playIcon = QIcon(":/play.png");
    pauseIcon = QIcon(":/pause.png");

    mediaObject = new Phonon::MediaObject(this);
    mediaObject->setTickInterval(OneSecond);
    videoWidget = new Phonon::VideoWidget(this);
    Phonon::createPath(mediaObject, videoWidget);
    audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory,
                                          this);
    Phonon::createPath(mediaObject, audioOutput);

    createActions();
    createToolBar();
    createWidgets();
    createLayout();
    createConnections();

    setWindowTitle(QApplication::applicationName());
}


void MainWindow::createActions()
{
    chooseVideoAction = new QAction(QIcon(":/video_folder.png"),
#ifdef Q_WS_MAC
            tr("Open Videos"), this);
#else
            tr("Open a Video to Play (Ctrl+O)"), this);
    chooseVideoAction->setShortcut(tr("Ctrl+O"));
#endif
    playOrPauseAction = new QAction(playIcon,
#ifdef Q_WS_MAC
            tr("Play/Pause"), this);
#else
            tr("Play/Pause (Spacebar)"), this);
    (void) new QShortcut(QKeySequence("Space"),
                         this, SLOT(playOrPause()));
#endif
    playOrPauseAction->setDisabled(true);
    stopAction = new QAction(QIcon(":/stop.png"),
#ifdef Q_WS_MAC
            tr("Stop"), this);
#else
            tr("Stop (Ctrl+S)"), this);
    stopAction->setShortcut(tr("Ctrl+S"));
#endif
    stopAction->setDisabled(true);
    fullScreenAction = new QAction(QIcon(":/fullscreen.png"),
#ifdef Q_WS_MAC
            tr("Full-screen"), this);
#else
            tr("Show Full-screen (Ctrl+F). "
               "Press Esc or click the mouse to cancel"), this);
    fullScreenAction->setShortcut(tr("Ctrl+F"));
#endif
    videoWidget->installEventFilter(this);
    (void) new QShortcut(QKeySequence("Escape"),
                         videoWidget, SLOT(exitFullScreen()));
#if QT_VERSION >= 0x040600
    QKeySequence quitKeySequence(QKeySequence::Quit);
#else
    QKeySequence quitKeySequence(tr("Ctrl+Q"));
#endif
    quitAction = new QAction(QIcon(":/exit.png"),
            tr("Quit (%1)").arg(quitKeySequence.toString(
                        QKeySequence::NativeText)), this);
    quitAction->setShortcut(quitKeySequence);
}


void MainWindow::createToolBar()
{
    toolbar = new QToolBar;
#ifdef Q_WS_MAC
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
#endif
    toolbar->addAction(chooseVideoAction);
    toolbar->addAction(playOrPauseAction);
    toolbar->addAction(stopAction);
    toolbar->addAction(fullScreenAction);
}


void MainWindow::createWidgets()
{
    seekSlider = new Phonon::SeekSlider(this);
    seekSlider->setMediaObject(mediaObject);
    seekSlider->setToolTip(tr("Playback Position"));
    volumeSlider = new Phonon::VolumeSlider(this);
    volumeSlider->setToolTip(tr("Volume Control"));
    volumeSlider->setAudioOutput(audioOutput);
    volumeSlider->setSizePolicy(QSizePolicy::Maximum,
                                QSizePolicy::Maximum);
    timeLcd = new QLCDNumber;
    timeLcd->setSegmentStyle(QLCDNumber::Flat);
    timeLcd->setToolTip(tr("Playback Time"));
    timeLcd->display(ZeroTime);
    quitButton = new QToolButton;
    quitButton->setDefaultAction(quitAction);
}


void MainWindow::createLayout()
{
    QHBoxLayout *timeLayout = new QHBoxLayout;
    timeLayout->addWidget(seekSlider);
    timeLayout->addWidget(timeLcd);

    QHBoxLayout *playbackLayout = new QHBoxLayout;
    playbackLayout->addWidget(toolbar);
    playbackLayout->addStretch();
    playbackLayout->addWidget(volumeSlider);
    playbackLayout->addWidget(quitButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoWidget, 1);
    layout->addLayout(timeLayout);
    layout->addLayout(playbackLayout);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
}


void MainWindow::createConnections()
{
    connect(mediaObject, SIGNAL(tick(qint64)),
            this, SLOT(tick(qint64)));
    connect(mediaObject,
            SIGNAL(stateChanged(Phonon::State, Phonon::State)),
            this, SLOT(stateChanged(Phonon::State)));
    connect(mediaObject, SIGNAL(finished()), this, SLOT(stop()));
    connect(fullScreenAction, SIGNAL(triggered()),
            videoWidget, SLOT(enterFullScreen()));
    connect(stopAction, SIGNAL(triggered()), this, SLOT(stop()));
    connect(playOrPauseAction, SIGNAL(triggered()),
            this, SLOT(playOrPause()));
    connect(chooseVideoAction, SIGNAL(triggered()),
            this, SLOT(chooseVideo()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
}


void MainWindow::playOrPause()
{
    switch (mediaObject->state()) {
        case Phonon::PlayingState:
            mediaObject->pause();
            playOrPauseAction->setIcon(playIcon);
            break;
        case Phonon::PausedState:  // Fallthrough
        case Phonon::StoppedState:
            mediaObject->play();
            playOrPauseAction->setIcon(pauseIcon);
            break;
        default:
            break;
    }
}


void MainWindow::stop()
{
    mediaObject->stop();
    mediaObject->clearQueue();
    playOrPauseAction->setIcon(playIcon);
    timeLcd->display(ZeroTime);
    setWindowTitle(QApplication::applicationName());
}


void MainWindow::stateChanged(Phonon::State newState)
{
    switch (newState) {
        case Phonon::ErrorState:
            AQP::warning(this, tr("Error"),
                         mediaObject->errorString());
            playOrPauseAction->setEnabled(false);
            stopAction->setEnabled(false);
            break;
        case Phonon::PlayingState:
            videoWidget->setAspectRatio(
                    Phonon::VideoWidget::AspectRatioWidget);
            videoWidget->setAspectRatio(
                    Phonon::VideoWidget::AspectRatioAuto);

            playOrPauseAction->setEnabled(true);
            playOrPauseAction->setIcon(pauseIcon);
            stopAction->setEnabled(true);
            break;
        case Phonon::StoppedState:
            playOrPauseAction->setEnabled(true);
            playOrPauseAction->setIcon(playIcon);
            stopAction->setEnabled(false);
            timeLcd->display(ZeroTime);
            break;
        case Phonon::PausedState:
            playOrPauseAction->setEnabled(true);
            playOrPauseAction->setIcon(playIcon);
            stopAction->setEnabled(true);
            break;
        default:
            playOrPauseAction->setEnabled(false);
            break;
    }
}


void MainWindow::tick(qint64 msec)
{
    int minutes;
    int seconds;
    AQP::hoursMinutesSecondsForMSec(msec, 0, &minutes, &seconds);
    timeLcd->display(QString("%1:%2").arg(minutes, 2, 10, QChar(' '))
            .arg(seconds, 2, 10, QChar('0')));
}


void MainWindow::chooseVideo()
{
    QString filename = QFileDialog::getOpenFileName(this,
            tr("Choose Video"), QDesktopServices::storageLocation(
                QDesktopServices::MoviesLocation), getFileFormats());
    if (filename.isEmpty())
        return;

    stop();
    playOrPauseAction->setEnabled(false);
    stopAction->setEnabled(false);
    mediaObject->setCurrentSource(filename);
    if (!mediaObject->hasVideo()) {
        QEventLoop eventLoop;
        QTimer timer;
        timer.setSingleShot(true);
        timer.setInterval(3 * OneSecond);
        connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
        connect(mediaObject, SIGNAL(hasVideoChanged(bool)),
                &eventLoop, SLOT(quit()));
        timer.start();
        eventLoop.exec();
    }

    if (mediaObject->hasVideo()) {
        QString title(mediaObject->metaData(Phonon::TitleMetaData)
                      .join("/").trimmed());
        if (title.isEmpty())
            title = QFileInfo(filename).baseName();
        setWindowTitle(tr("%1 - %2")
                .arg(QApplication::applicationName()).arg(title));
        mediaObject->play();
    }
    else {
        setWindowTitle(QApplication::applicationName());
        AQP::warning(this, tr("Error"),
                tr("Cannot play video from %1").arg(filename));
    }
}


QString MainWindow::getFileFormats()
{
    QStringList mimeTypes;
    foreach (const QString &mimeType,
             Phonon::BackendCapabilities::availableMimeTypes())
        if (mimeType.startsWith("video/"))
            mimeTypes << mimeType;
    return AQP::filenameFilter(tr("Video"), mimeTypes);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    mediaObject->stop();
    mediaObject->clearQueue();
    event->accept();
}


bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (target == videoWidget &&
        event->type() == QEvent::MouseButtonPress &&
        videoWidget->isFullScreen())
        videoWidget->exitFullScreen();
    return QMainWindow::eventFilter(target, event);
}
