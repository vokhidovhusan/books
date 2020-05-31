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

#include "alt_key.hpp"
#include "aqp.hpp"
#include "jingleaction.hpp"
#include "mainwindow.hpp"
#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QImageWriter>
#include <QLabel>
#include <QList>
#include <QMenu>
#include <QMenuBar>
#include <QMovie>
#include <QStatusBar>
#include <QToolBar>


namespace {
const int StatusTimeout = AQP::MSecPerSecond * 5;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), movieState(NoMovie)
{
    movie = new QMovie(this);

    createActions();
    createMenusAndToolBar();
    createWidgets();
    createLayout();
    createConnections();

    AQP::accelerateMenu(menuBar());
    updateUi();
    statusBar()->showMessage(tr("Open a Movie file to start..."),
                             StatusTimeout);
    setWindowTitle(QApplication::applicationName());
}


void MainWindow::createActions()
{
    jinglePath = AQP::applicationPathOf("jingles");
    imagePath = AQP::applicationPathOf("images");

    fileOpenAction = new JingleAction(
            jinglePath + "/fileopen.wav",
            QIcon(imagePath + "/fileopen.png"), tr("Open..."), this);
    fileOpenAction->setShortcuts(QKeySequence::Open);
    fileSaveAction = new JingleAction(
            jinglePath + "/filesave.wav",
            QIcon(imagePath + "/filesave.png"), tr("Save"), this);
    fileSaveAction->setShortcuts(QKeySequence::Save);
    fileSaveAction->setEnabled(false);
    fileQuitAction = new QAction(
            QIcon(imagePath + "/filequit.png"), tr("Quit"), this);
    fileMuteJinglesAction = new JingleAction(
            jinglePath + "/filemutejingles.wav",
            QIcon(imagePath + "/filemutejingles.png"),
            tr("Mute Jingles"), this);
    fileMuteJinglesAction->setCheckable(true);
    fileMuteJinglesAction->setChecked(false);
#if QT_VERSION >= 0x040600
    fileQuitAction->setShortcuts(QKeySequence::Quit);
#else
    fileQuitAction->setShortcut(QKeySequence("Ctrl+Q"));
#endif

    startOrStopAction = new JingleAction(this);
    takeSnapshotAction = new JingleAction(
            jinglePath + "/takesnapshot.wav",
            QIcon(imagePath + "/takesnapshot.png"),
            tr("Take Snapshot"), this);
}


void MainWindow::createMenusAndToolBar()
{
    setUnifiedTitleAndToolBarOnMac(true);

    QMenu *fileMenu = menuBar()->addMenu(tr("File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
#ifdef Q_WS_MAC
    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
#endif
    JingleAction *emptyAction = 0;
    foreach (JingleAction *jingleAction, QList<JingleAction*>()
            << fileOpenAction << fileSaveAction << emptyAction
            << fileMuteJinglesAction) {
        if (jingleAction == emptyAction) {
            fileMenu->addSeparator();
            fileToolBar->addSeparator();
            continue;
        }
        fileMenu->addAction(jingleAction);
        fileToolBar->addAction(jingleAction);
    }
    fileMenu->addSeparator();
    fileMenu->addAction(fileQuitAction);

    QMenu *editMenu = menuBar()->addMenu(tr("Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));
#ifdef Q_WS_MAC
    editToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
#endif
    foreach (JingleAction *jingleAction, QList<JingleAction*>()
            << startOrStopAction << takeSnapshotAction) {
        editMenu->addAction(jingleAction);
        editToolBar->addAction(jingleAction);
    }
}


void MainWindow::createWidgets()
{
    movieLabel = new QLabel;
    movieLabel->setMovie(movie);
    movieLabel->setAlignment(Qt::AlignCenter);
    movieLabel->setAutoFillBackground(true);
    movieLabel->setBackgroundRole(QPalette::Dark);
    snapshotLabel = new QLabel;
    snapshotLabel->setAlignment(Qt::AlignCenter);
    snapshotLabel->setAutoFillBackground(true);
    snapshotLabel->setBackgroundRole(QPalette::Dark);
}


void MainWindow::createLayout()
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(movieLabel);
    layout->addWidget(snapshotLabel);
    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
}


void MainWindow::createConnections()
{
    connect(fileOpenAction, SIGNAL(triggered()),
            this, SLOT(fileOpen()));
    connect(fileSaveAction, SIGNAL(triggered()),
            this, SLOT(fileSave()));
    connect(fileMuteJinglesAction, SIGNAL(toggled(bool)),
            this, SLOT(muteJingles(bool)));
    connect(fileQuitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(startOrStopAction, SIGNAL(triggered()),
            this, SLOT(startOrStop()));
    connect(takeSnapshotAction, SIGNAL(triggered()),
            this, SLOT(takeSnapshot()));
}


void MainWindow::updateUi()
{
    if (movieState == Playing) {
        startOrStopAction->setText(tr("&Stop"));
        startOrStopAction->setIcon(QIcon(imagePath +
                                         "/editstop.png"));
        startOrStopAction->setJingleFile(jinglePath +
                                         "/editstop.wav");
    }
    else {
        startOrStopAction->setText(tr("&Start"));
        startOrStopAction->setIcon(QIcon(imagePath +
                                         "/editstart.png"));
        startOrStopAction->setJingleFile(jinglePath +
                                         "/editstart.wav");
    }
    startOrStopAction->setEnabled(movieState != NoMovie);
    takeSnapshotAction->setEnabled(movieState != NoMovie);
}


void MainWindow::fileOpen()
{
    QString fileFormats = AQP::filenameFilter(tr("Movies"),
            QMovie::supportedFormats());
    QString path(movie && !movie->fileName().isEmpty()
            ? QFileInfo(movie->fileName()).absolutePath() : ".");
    QString filename = QFileDialog::getOpenFileName(this,
            tr("%1 - Choose a Movie File")
            .arg(QApplication::applicationName()), path, fileFormats);
    if (filename.isEmpty())
        return;

    movie->setFileName(filename);
    statusBar()->showMessage(tr("Loaded %1").arg(filename),
                             StatusTimeout);
    movieState = Stopped;
    startOrStop(DontReload);
}


void MainWindow::fileSave()
{
    if (snapshot.isNull())
        return;
    QString fileFormats = AQP::filenameFilter(tr("Images"),
            QImageWriter::supportedImageFormats());
    QString filename = QFileDialog::getSaveFileName(this,
            tr("%1 - Save Snapshot")
            .arg(QApplication::applicationName()),
            QFileInfo(movie->fileName()).absolutePath(), fileFormats);
    if (filename.isEmpty())
        return;

    if (!snapshot.save(filename))
        AQP::warning(this, tr("Error"),
                     tr("Failed to save snapshot image"));
    else
        statusBar()->showMessage(tr("Saved %1").arg(filename),
                                 StatusTimeout);
}


void MainWindow::startOrStop(ReloadMode reloadMode)
{
    if (movieState == Stopped) {
        if (reloadMode == Reload)
            movie->setFileName(movie->fileName());
        movie->start();
        movieState = Playing;
    }
    else {
        movie->stop();
        movieState = Stopped;
    }
    updateUi();
}


void MainWindow::takeSnapshot()
{
    snapshot = movie->currentPixmap();
    fileSaveAction->setEnabled(!snapshot.isNull());
    snapshotLabel->setPixmap(snapshot);
}


void MainWindow::muteJingles(bool mute)
{
    JingleAction::setMute(mute);
}
