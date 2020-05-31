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


class QCheckBox;
class QGraphicsEllipseItem;
class QGraphicsScene;
class QGraphicsView;
class QLabel;
class QLCDNumber;
class QPushButton;
class QSpinBox;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=0);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void start();
    void pauseOrResume();
    void stop();
    void doOneIteration();
    void showIds(bool show);

private:
    enum SimulationState {Stopped, Running, Paused};

    void createWidgets();
    void createProxyWidgets();
    void createLayout();
    void createCentralWidget();
    void createConnections();

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

    QList<Cell*> cells;
    SimulationState simulationState;
    int iterations;
};

#endif // MAINWINDOW_HPP

