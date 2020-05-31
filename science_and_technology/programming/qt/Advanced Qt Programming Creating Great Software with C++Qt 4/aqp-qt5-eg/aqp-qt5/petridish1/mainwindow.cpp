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
#include "mainwindow.hpp"
#include <QApplication>
#include <QCheckBox>
#include <QGraphicsEllipseItem>
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QLCDNumber>
#include <QList>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>

// Can't use a QTimer because the interval between each iteration varies
// depending on how many items there are. So we use single shot timers
// instead

namespace {
const int DishSize = 350;
const int Margin = 20;
const int IterationDelay = 50;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), simulationState(Stopped), iterations(0)
{
    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    createWidgets();
    createProxyWidgets();
    createLayout();
    createCentralWidget();
    createConnections();

    startButton->setFocus();
    setWindowTitle(QApplication::applicationName());
}


void MainWindow::createWidgets()
{
    startButton = new QPushButton(tr("St&art"));
    pauseOrResumeButton = new QPushButton(tr("Pa&use"));
    pauseOrResumeButton->setEnabled(false);
    stopButton = new QPushButton(tr("Stop"));
    quitButton = new QPushButton(tr("Quit"));

    QString styleSheet("background-color: bisque;");
    initialCountLabel = new QLabel(tr("Initial count:"));
    initialCountLabel->setStyleSheet(styleSheet);
    initialCountSpinBox = new QSpinBox;
    initialCountLabel->setBuddy(initialCountSpinBox);
    initialCountSpinBox->setRange(1, 100);
    initialCountSpinBox->setValue(60);
    initialCountSpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    currentCountLabel = new QLabel(tr("Current count"));
    currentCountLabel->setStyleSheet(styleSheet);
    currentCountLCD = new QLCDNumber;
    currentCountLCD->setSegmentStyle(QLCDNumber::Flat);
    currentCountLCD->setStyleSheet(styleSheet);
    iterationsLabel = new QLabel(tr("Iterations"));
    iterationsLabel->setStyleSheet(styleSheet);
    iterationsLCD = new QLCDNumber;
    iterationsLCD->setSegmentStyle(QLCDNumber::Flat);
    iterationsLCD->setStyleSheet(styleSheet);
    showIdsCheckBox = new QCheckBox(tr("Show I&Ds"));
    showIdsCheckBox->setStyleSheet(styleSheet);
    showIdsCheckBox->setChecked(true);

    AQP::accelerateWidgets(QList<QWidget*>() << startButton
            << stopButton << quitButton << initialCountLabel
            << showIdsCheckBox);
}


void MainWindow::createProxyWidgets()
{
    proxyForName["startButton"] = scene->addWidget(startButton);
    proxyForName["pauseOrResumeButton"] = scene->addWidget(
            pauseOrResumeButton);
    proxyForName["stopButton"] = scene->addWidget(stopButton);
    proxyForName["quitButton"] = scene->addWidget(quitButton);
    proxyForName["initialCountLabel"] = scene->addWidget(
            initialCountLabel);
    proxyForName["initialCountSpinBox"] = scene->addWidget(
            initialCountSpinBox);
    proxyForName["currentCountLabel"] = scene->addWidget(
            currentCountLabel);
    proxyForName["currentCountLCD"] = scene->addWidget(
            currentCountLCD);
    proxyForName["iterationsLabel"] = scene->addWidget(
            iterationsLabel);
    proxyForName["iterationsLCD"] = scene->addWidget(iterationsLCD);
    proxyForName["showIdsCheckBox"] = scene->addWidget(
            showIdsCheckBox);;
}


void MainWindow::createLayout()
{
    QGraphicsLinearLayout *leftLayout = new QGraphicsLinearLayout(
            Qt::Vertical);
    leftLayout->addItem(proxyForName["startButton"]);
    leftLayout->addItem(proxyForName["pauseOrResumeButton"]);
    leftLayout->addItem(proxyForName["stopButton"]);
    leftLayout->addItem(proxyForName["quitButton"]);

    QGraphicsLinearLayout *rightLayout = new QGraphicsLinearLayout(
            Qt::Vertical);
    foreach (const QString &name, QStringList()
            << "initialCountLabel" << "initialCountSpinBox"
            << "currentCountLabel" << "currentCountLCD"
            << "iterationsLabel" << "iterationsLCD"
            << "showIdsCheckBox")
        rightLayout->addItem(proxyForName[name]);

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
    layout->addItem(leftLayout);
    layout->setItemSpacing(0, DishSize + Margin);
    layout->addItem(rightLayout);

    QGraphicsWidget *widget = new QGraphicsWidget;
    widget->setLayout(layout);
    scene->addItem(widget);

    int width = qRound(layout->preferredWidth());
    int height = DishSize + (2 * Margin);
    setMinimumSize(width, height);
    scene->setSceneRect(0, 0, width, height);
}


void MainWindow::createCentralWidget()
{
    dishItem = new QGraphicsEllipseItem;
    dishItem->setFlags(QGraphicsItem::ItemClipsChildrenToShape);
    dishItem->setPen(QPen(QColor("brown"), 2.5));
    dishItem->setBrush(Qt::white);
    dishItem->setRect(pauseOrResumeButton->width() + Margin,
            Margin, DishSize, DishSize);

    scene->addItem(dishItem);

    view = new QGraphicsView(scene);
    view->setRenderHints(QPainter::Antialiasing|
                         QPainter::TextAntialiasing);
    view->setBackgroundBrush(QColor("bisque"));
    setCentralWidget(view);
}


void MainWindow::createConnections()
{
    connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
    connect(pauseOrResumeButton, SIGNAL(clicked()),
            this, SLOT(pauseOrResume()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(showIdsCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(showIds(bool)));
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    simulationState = Stopped;
    qDeleteAll(cells);
    event->accept();
}


void MainWindow::showIds(bool show)
{
    Cell::setShowIds(show);
    scene->invalidate();
}


void MainWindow::start()
{
    if (simulationState != Stopped)
        stop();
    initialCountSpinBox->setEnabled(false);
    iterationsLCD->display(iterations = 0);
    qDeleteAll(cells);
    cells.clear();
    QRectF rect = dishItem->sceneBoundingRect();
    int xOffset = qRound(rect.x() + (rect.width() / 2));
    int yOffset = qRound(rect.y() + (rect.height() / 2));
    for (int i = 0; i < initialCountSpinBox->value(); ++i) {
        int angle = qrand() % 360;
        qreal factor = qrand() % qRound(rect.width() / 2);
#ifdef MSVC_COMPILER
        int x = qRound(factor * cos(AQP::radiansFromDegrees(angle)));
        int y = qRound(factor * sin(AQP::radiansFromDegrees(angle)));
#else
        int x = qRound(factor *
                std::cos(AQP::radiansFromDegrees(angle)));
        int y = qRound(factor *
                std::sin(AQP::radiansFromDegrees(angle)));
#endif
        Cell *cell = new Cell(i, dishItem);
        cell->setPos(x + xOffset, y + yOffset);
        cells << cell;
    }
    scene->invalidate();
    startButton->setEnabled(false);
    pauseOrResumeButton->setEnabled(true);
    pauseOrResumeButton->setFocus();
    update();
    simulationState = Running;
    QTimer::singleShot(IterationDelay, this, SLOT(doOneIteration()));
}


void MainWindow::doOneIteration()
{
    if (simulationState != Running)
        return;
    int count = 0;
    QMutableListIterator<Cell*> i(cells);
    while (i.hasNext()) {
        Cell *cell = i.next();
        if (!cell)
            continue;
        if (cell->shrinkOrGrow() == Cell::Die) {
            i.remove();
            delete cell;
        }
        else
            ++count;
    }
    scene->invalidate();
    iterationsLCD->display(++iterations);
    currentCountLCD->display(count);
    update();
    if (count <= 1)
        stop();
    if (simulationState != Running)
        return;
    QTimer::singleShot(IterationDelay, this, SLOT(doOneIteration()));
}


void MainWindow::pauseOrResume()
{
    if (pauseOrResumeButton->text() == tr("Pa&use")) {
        pauseOrResumeButton->setText(tr("Res&ume"));
        simulationState = Paused;
        setWindowOpacity(0.95);
    }
    else {
        pauseOrResumeButton->setText(tr("Pa&use"));
        simulationState = Running;
        setWindowOpacity(1.0);
        QTimer::singleShot(IterationDelay,
                           this, SLOT(doOneIteration()));
    }
    update();
}


void MainWindow::stop()
{
    simulationState = Stopped;
    initialCountSpinBox->setEnabled(true);
    pauseOrResumeButton->setEnabled(false);
    startButton->setEnabled(true);
    startButton->setFocus();
    update();
}
