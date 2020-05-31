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
#include <phonon/backendcapabilities.h>
#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/seekslider.h>
#include <phonon/volumeslider.h>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDirIterator>
#include <QEvent>
#include <QEventLoop>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLCDNumber>
#include <QShortcut>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>


namespace {
const int OneSecond = AQP::MSecPerSecond;
const QString ZeroTime(" 0:00");
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), nextItem(0)
{
    playIcon = QIcon(":/play.png");
    pauseIcon = QIcon(":/pause.png");

    mediaObject = new Phonon::MediaObject(this);
    mediaObject->setTickInterval(OneSecond);
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory,
                                          this);
    Phonon::createPath(mediaObject, audioOutput);

    createActions();
    createToolBar();
    createWidgets();
    createLayout();
    createConnections();
    qApp->installEventFilter(this);

    setWindowTitle(QApplication::applicationName());
}


void MainWindow::createActions()
{
    setMusicDirectoryAction = new QAction(QIcon(":/music_folder.png"),
#ifdef Q_WS_MAC
            tr("Open Music"), this);
#else
            tr("Open a Music Directory (Ctrl+O)"), this);
    setMusicDirectoryAction->setShortcut(tr("Ctrl+O"));
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
    toolbar->addAction(setMusicDirectoryAction);
    toolbar->addAction(playOrPauseAction);
    toolbar->addAction(stopAction);
}


void MainWindow::createWidgets()
{
    seekSlider = new Phonon::SeekSlider(this);
    seekSlider->setToolTip(tr("Playback Position"));
    seekSlider->setMediaObject(mediaObject);
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

    treeWidget = new QTreeWidget;
    treeWidget->setHeaderLabel(tr("Open a music directory... (%1)")
            .arg(setMusicDirectoryAction->shortcut().toString()));
    treeWidget->headerItem()->setIcon(0, QIcon(":/music_folder.png"));
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
    layout->addWidget(treeWidget);
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
    connect(mediaObject, SIGNAL(aboutToFinish()),
            this, SLOT(aboutToFinish()));
    connect(mediaObject,
            SIGNAL(currentSourceChanged(const Phonon::MediaSource&)),
            this, SLOT(currentSourceChanged()));

    connect(setMusicDirectoryAction, SIGNAL(triggered()),
            this, SLOT(setMusicDirectory()));
    connect(playOrPauseAction, SIGNAL(triggered()),
            this, SLOT(playOrPause()));
    connect(stopAction, SIGNAL(triggered()), this, SLOT(stop()));
    connect(treeWidget,
        SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
        this, SLOT(currentItemChanged(QTreeWidgetItem*)));
    connect(treeWidget,
        SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
        this, SLOT(playTrack(QTreeWidgetItem*)));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    nextItem = 0;
    mediaObject->stop();
    mediaObject->clearQueue();
    event->accept();
}


bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress &&
        target != quitButton &&
        treeWidget->topLevelItemCount() == 0) {
        setMusicDirectory();
        return true;
    }
    return QMainWindow::eventFilter(target, event);
}


void MainWindow::currentItemChanged(QTreeWidgetItem *item)
{
    if (!playOrPauseAction->isEnabled()) {
        QString filename = item->data(0, FilenameRole).toString();
        if (!filename.isEmpty())
            playOrPauseAction->setEnabled(true);
    }
}


void MainWindow::playTrack(QTreeWidgetItem *item)
{
    Q_ASSERT(item);
    QString filename = item->data(0, FilenameRole).toString();
    if (filename.isEmpty())
        return;
    if (!QFile::exists(filename)) {
        AQP::warning(this, tr("Error"),
                tr("File %1 appears to have been moved or deleted")
                .arg(filename));
        return;
    }
    nextItem = item;
    mediaObject->clearQueue();
    mediaObject->setCurrentSource(filename);
    mediaObject->play();
}


void MainWindow::playOrPause()
{
    switch (mediaObject->state()) {
        case Phonon::PlayingState:
            mediaObject->pause();
            playOrPauseAction->setIcon(playIcon);
            break;
        case Phonon::PausedState:
            mediaObject->play();
            playOrPauseAction->setIcon(pauseIcon);
            break;
        default:
            playTrack(treeWidget->currentItem());
            break;
    }
}


void MainWindow::currentSourceChanged()
{
    if (nextItem) {
        playOrPauseAction->setIcon(pauseIcon);
        timeLcd->display(ZeroTime);
        setWindowTitle(tr("%1 - %2")
                .arg(QApplication::applicationName())
                .arg(nextItem->text(0)));
        treeWidget->setCurrentItem(nextItem);
        nextItem = 0;
    }
}


void MainWindow::stop()
{
    nextItem = 0;
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
            playOrPauseAction->setEnabled(true);
            playOrPauseAction->setIcon(pauseIcon);
            stopAction->setEnabled(true);
            break;
        case Phonon::PausedState:
            playOrPauseAction->setEnabled(true);
            playOrPauseAction->setIcon(playIcon);
            stopAction->setEnabled(true);
            break;
        case Phonon::StoppedState:
            playOrPauseAction->setEnabled(true);
            playOrPauseAction->setIcon(playIcon);
            stopAction->setEnabled(false);
            timeLcd->display(ZeroTime);
            break;
        default:
            playOrPauseAction->setEnabled(false);
            break;
    }
}


void MainWindow::tick(qint64 msec)
{
    timeLcd->display(minutesSecondsAsStringForMSec(msec));
}


void MainWindow::aboutToFinish()
{
    QTreeWidgetItem *item = nextItem ? nextItem :
                            treeWidget->currentItem();
    if (!item)
        return;
    item = treeWidget->itemBelow(item);
    if (!item) // Current track is the last track in the tree
        return;
    QString filename = item->data(0, FilenameRole).toString();
    if (filename.isEmpty()) { // item is an Artist or an Album
        item = item->child(0);
        if (!item)
            return;
        else {
            filename = item->data(0, FilenameRole).toString();
            if (filename.isEmpty()) // item is an Album
                item = item->child(0);
            if (!item)
                return;
            filename = item->data(0, FilenameRole).toString();
            if (filename.isEmpty())
                return;
        }
    }
    nextItem = item;
    Phonon::MediaSource source(filename);
    mediaObject->enqueue(source);
}


void MainWindow::setMusicDirectory()
{
    QString path = QFileDialog::getExistingDirectory(this,
            tr("Choose a Music Directory"),
            QDesktopServices::storageLocation(
                QDesktopServices::MusicLocation));
    if (path.isEmpty())
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QSet<QString> validSuffixes = getSuffixes();
    treeWidget->clear();
    treeWidget->headerItem()->setIcon(0, QIcon());
    treeWidget->setHeaderLabels(QStringList()
            << tr("Artist/Album/Track") << tr("Time"));
    QHash<QString, TreeWidgetItem*> itemForArtist;
    QHash<QString, TreeWidgetItem*> itemForArtistAlbum;
    QDirIterator i(path, QDirIterator::Subdirectories);
    while (i.hasNext()) {
        const QString filename = i.next();
        if (!QFileInfo(filename).isFile() ||
            !validSuffixes.contains(QFileInfo(filename).suffix()))
            continue;
        addTrack(filename, &itemForArtist, &itemForArtistAlbum);
    }

    foreach (QTreeWidgetItem *item, itemForArtistAlbum)
        if (!item->childCount())
            delete item;
    foreach (QTreeWidgetItem *item, itemForArtist)
        if (!item->childCount())
            delete item;
    treeWidget->sortItems(0, Qt::AscendingOrder);
    treeWidget->resizeColumnToContents(0);
    stop();
    QApplication::restoreOverrideCursor();
}


QSet<QString> MainWindow::getSuffixes()
{
    QStringList mimeTypes;
    foreach (const QString &mimeType,
             Phonon::BackendCapabilities::availableMimeTypes())
        if (mimeType.startsWith("audio/"))
            mimeTypes << mimeType;
    return AQP::suffixesForMimeTypes(mimeTypes);
}


void MainWindow::addTrack(const QString &filename,
        QHash<QString, TreeWidgetItem*> *itemForArtist,
        QHash<QString, TreeWidgetItem*> *itemForArtistAlbum)
{
    Phonon::MediaObject localMediaObject;
    Phonon::MediaSource source(filename);
    localMediaObject.setCurrentSource(source);
    if (!waitForMediaObjectToLoad(&localMediaObject, OneSecond))
        return;
    QString artist = localMediaObject.metaData(
            Phonon::ArtistMetaData).join("/").trimmed();
    QString album = localMediaObject.metaData(
            Phonon::AlbumMetaData).join("/").trimmed();
    QString artistAlbum = artist + "\t" + album;
    QString track = localMediaObject.metaData(
            Phonon::TitleMetaData).join("/").trimmed();
    qint64 msec = localMediaObject.totalTime();

    TreeWidgetItem *artistItem = itemForArtist->value(artist);
    if (!artistItem) {
        artistItem = new TreeWidgetItem(
                treeWidget->invisibleRootItem(),
                QStringList() << artist);
        itemForArtist->insert(artist, artistItem);
    }
    TreeWidgetItem *albumItem = itemForArtistAlbum->value(
            artistAlbum);
    if (!albumItem) {
        albumItem = new TreeWidgetItem(artistItem,
                                       QStringList() << album);
        itemForArtistAlbum->insert(artistAlbum, albumItem);
    }
    TreeWidgetItem *trackItem = new TreeWidgetItem(albumItem,
            QStringList() << track
                          << minutesSecondsAsStringForMSec(msec));
    trackItem->setData(0, FilenameRole, filename);
    trackItem->setTextAlignment(1, Qt::AlignVCenter|Qt::AlignRight);
}


QString MainWindow::minutesSecondsAsStringForMSec(qint64 msec)
{
    int minutes;
    int seconds;
    AQP::hoursMinutesSecondsForMSec(msec, 0, &minutes, &seconds);
    return QString("%1:%2").arg(minutes, 2, 10, QChar(' '))
                           .arg(seconds, 2, 10, QChar('0'));
}


bool MainWindow::waitForMediaObjectToLoad(
        Phonon::MediaObject *mediaObject, int timeoutMSec)
{
    QEventLoop eventLoop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(timeoutMSec);
    connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    connect(mediaObject,
            SIGNAL(stateChanged(Phonon::State, Phonon::State)),
            &eventLoop, SLOT(quit()));
    timer.start();
    eventLoop.exec();
    return mediaObject->state() == Phonon::StoppedState;
}
