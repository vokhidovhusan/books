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

#include "censusvisualizer.hpp"
#include <QAction>
#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QTableView>
#include <QSplitter>
#include <QStandardItemModel>
#include <QStatusBar>


void populateModel(QStandardItemModel *model)
{
    const int Rows = 23;
    const int Columns = 3;
    int irish_census[Rows][Columns] = {
      // Year  Males    Females
        {1841, 3222485, 3306314},
        {1851, 2494478, 2617079},
        {1861, 2169042, 2233069},
        {1871, 1992468, 2060719},
        {1881, 1912438, 1957582},
        {1891, 1728601, 1740093},
        {1901, 1610085, 1611738},
        {1911, 1589509, 1550179},
        {1926, 1506889, 1465103},
        {1936, 1520454, 1447966},
        {1946, 1494877, 1460230},
        {1951, 1506597, 1453996},
        {1956, 1462928, 1435336},
        {1961, 1416549, 1401792},
        {1966, 1449032, 1434970},
        {1971, 1495760, 1482488},
        {1979, 1693272, 1674945},
        {1981, 1729354, 1714051},
        {1986, 1769690, 1770953},
        {1991, 1753418, 1772301},
        {1996, 1800232, 1825855},
        {2002, 1946164, 1971039},
        {2006, 2121171, 2118677},
    };

    QStandardItem *item;
    for (int row = 0; row < Rows; ++row) {
        QList<QStandardItem*> items;
        for (int column = 0; column <= Columns; ++column) {
            int number = column == Columns
                    ? irish_census[row][1] + irish_census[row][2]
                    : irish_census[row][column];
            QString text = column == 0 ? QString::number(number)
                                       : QString("%L1").arg(number);
            item = new QStandardItem(text);
            item->setTextAlignment(Qt::AlignVCenter|Qt::AlignRight);
            items << item;
        }
        model->appendRow(items);
    }
    model->setHorizontalHeaderLabels(QStringList()
            << model->tr("Year") << model->tr("Males")
            << model->tr("Females") << model->tr("Total"));
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(app.translate("main",
                                         "Census Visualizer"));
#ifdef Q_WS_MAC
    app.setCursorFlashTime(0);
#endif

    QStandardItemModel *model = new QStandardItemModel;
    populateModel(model);
    CensusVisualizer *censusVisualizer = new CensusVisualizer;
    censusVisualizer->setModel(model);
    QTableView *tableView = new QTableView;
    tableView->setModel(model);
    tableView->resizeColumnsToContents();

    QMainWindow *window = new QMainWindow;
    QMenu *fileMenu = window->menuBar()->addMenu(
            app.translate("main", "&File"));
    QAction *quitAction = fileMenu->addAction(
            app.translate("main", "&Quit"));
    QSplitter *splitter = new QSplitter;
    splitter->addWidget(tableView);
    splitter->addWidget(censusVisualizer);
    window->statusBar()->clearMessage(); // create statusbar + useful size grip
    window->setCentralWidget(splitter);
    window->resize(800, 350);
    window->setWindowTitle(app.translate("main", "%1 - Ireland")
                           .arg(app.applicationName()));
    window->show();
    window->connect(quitAction, SIGNAL(triggered()),
                    window, SLOT(close()));
    window->connect(censusVisualizer,
            SIGNAL(clicked(const QModelIndex&)),
            tableView, SLOT(setCurrentIndex(const QModelIndex&)));
    // this connection is why the first item in the CensusVisualizer is
    // selected as startup
    window->connect(tableView->selectionModel(),
            SIGNAL(currentChanged(const QModelIndex&,
                                  const QModelIndex&)),
            censusVisualizer,
            SLOT(setCurrentIndex(const QModelIndex&)));
    int result = app.exec();
    delete window;
    delete model;
    return result;
}
