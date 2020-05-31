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

#include "cell.hpp"
#include <QHash>
#include <QList>
#include <QMainWindow>
#include <QStateMachine>


class QCheckBox;
class QFinalState;
class QGraphicsEllipseItem;
class QGraphicsScene;
class QGraphicsView;
class QLabel;
class QLCDNumber;
class QPushButton;
class QSpinBox;
class QState;


class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running WRITE setRunning)

public:
    explicit MainWindow(QWidget *parent=0);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void start();
    void pause();
    void doOneIteration();
    void showIds(bool show);

private:
    void createWidgets();
    void createProxyWidgets();
    void createLayout();
    void createCentralWidget();
    void createStates();
    void createTransitions();
    void createConnections();

    bool running() const { return m_running; }
    void setRunning(bool running) { m_running = running; }

    QGraphicsView *view;
    QGraphicsScene *scene;
    QGraphicsEllipseItem *dishItem;
    QPushButton *startButton;
    QPushButton *pauseOrResumeButton;
    QPushButton *stopButton;
    QPushButton *quitButton;
    QLabel *initialCountLabel;
    QSpinBox *initialCountSpinBox;
    QLabel *currentCountLabel;
    QLCDNumber *currentCountLCD;
    QLabel *iterationsLabel;
    QLCDNumber *iterationsLCD;
    QCheckBox *showIdsCheckBox;
    QHash<QString, QGraphicsProxyWidget*> proxyForName;

    QStateMachine stateMachine;
/* // The weird bug seems to affect all platforms with Qt 4.6.3
#if !defined(Q_WS_WIN) && !defined(Q_WS_MAC)
    QState *initialState;
    QState *normalState;
    QState *stoppedState;
    QState *runningState;
    QState *pausedState;
    QFinalState *finalState;
#endif
*/

    QList<Cell*> cells;
    int iterations;
    bool m_running;
};

#endif // MAINWINDOW_HPP

