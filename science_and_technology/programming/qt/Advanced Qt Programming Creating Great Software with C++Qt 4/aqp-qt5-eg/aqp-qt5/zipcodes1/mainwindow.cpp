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
#include "global.hpp"
#include "itemdelegate.hpp"
#include "mainwindow.hpp"
#include "proxymodel.hpp"
#ifdef CUSTOM_MODEL
#include "tablemodel.hpp"
#else
#include "standardtablemodel.hpp"
#endif
#ifdef MODEL_TEST
#include <modeltest.h>
#endif
#include "uniqueproxymodel.hpp"
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QTableView>
#include <QVBoxLayout>


const int StatusTimeout = AQP::MSecPerSecond * 10;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), loading(false)
{
#ifdef CUSTOM_MODEL
    model = new TableModel(this);
#else
    model = new StandardTableModel(this);
#endif
#ifdef MODEL_TEST
    (void) new ModelTest(model, this);
#endif
    proxyModel = new ProxyModel(this);
    proxyModel->setSourceModel(model);

    createWidgets();
    createComboBoxModels();
    createLayout();
    createConnections();

    AQP::accelerateWidget(this);
#ifdef CUSTOM_MODEL
    setWindowTitle(tr("%1 (Custom Model)[*]")
            .arg(QApplication::applicationName()));
#else
    setWindowTitle(tr("%1 (QStandardItemModel)[*]")
            .arg(QApplication::applicationName()));
#endif
    statusBar()->showMessage(tr("Ready"), StatusTimeout);
}


void MainWindow::createWidgets()
{
    buttonBox = new QDialogButtonBox;
    loadButton = buttonBox->addButton(tr("Load..."),
            QDialogButtonBox::AcceptRole);
    saveButton = buttonBox->addButton(tr("&Save"),
            QDialogButtonBox::AcceptRole);
    saveButton->setEnabled(false);
    addButton = buttonBox->addButton(tr("Add"),
            QDialogButtonBox::ActionRole);
    addButton->setEnabled(false);
    deleteButton = buttonBox->addButton(tr("Delete..."),
            QDialogButtonBox::ActionRole);
    deleteButton->setEnabled(false);
    quitButton = buttonBox->addButton(tr("Quit"),
            QDialogButtonBox::ApplyRole);

    filterSelectGroupBox = new QGroupBox(tr("Filter or Select"));
    dontFilterOrSelectRadioButton = new QRadioButton(
            tr("Don't Filter or Select"));
    filterRadioButton = new QRadioButton(tr("Filter"));
    filterRadioButton->setChecked(true);
    selectByCriteriaRadioButton = new QRadioButton(
            tr("Select by Criteria"));
    minimumZipLabel = new QLabel(tr("Min. Zip:"));
    minimumZipSpinBox = new QSpinBox;
    minimumZipLabel->setBuddy(minimumZipSpinBox);
    minimumZipSpinBox->setRange(MinZipcode, MaxZipcode);
    minimumZipSpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    maximumZipLabel = new QLabel(tr("Max. Zip:"));
    maximumZipSpinBox = new QSpinBox;
    maximumZipLabel->setBuddy(maximumZipSpinBox);
    maximumZipSpinBox->setRange(MinZipcode, MaxZipcode);
    maximumZipSpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    maximumZipSpinBox->setValue(MaxZipcode);
    countyGroupBox = new QGroupBox(tr("County"));
    countyGroupBox->setCheckable(true);
    countyGroupBox->setChecked(false);
    countyComboBox = new QComboBox;
    stateGroupBox = new QGroupBox(tr("State"));
    stateGroupBox->setCheckable(true);
    stateGroupBox->setChecked(false);
    stateComboBox = new QComboBox;

    tableView = new QTableView;
    tableView->setModel(proxyModel);
    tableView->setItemDelegate(new ItemDelegate(this));
    tableView->verticalHeader()->setDefaultAlignment(
            Qt::AlignVCenter|Qt::AlignRight);
}


void MainWindow::createComboBoxModels()
{
    createComboBoxModel(countyComboBox, County);
    createComboBoxModel(stateComboBox, State);
}


void MainWindow::createComboBoxModel(QComboBox *comboBox, int column)
{
    delete comboBox->model();
    UniqueProxyModel *uniqueProxyModel = new UniqueProxyModel(column,
                                                              this);
    uniqueProxyModel->setSourceModel(model);
    uniqueProxyModel->sort(column, Qt::AscendingOrder);
    comboBox->setModel(uniqueProxyModel);
    comboBox->setModelColumn(column);
}


void MainWindow::createLayout()
{
    QHBoxLayout *groupBoxTopLayout = new QHBoxLayout;
    groupBoxTopLayout->addWidget(dontFilterOrSelectRadioButton);
    groupBoxTopLayout->addWidget(filterRadioButton);
    groupBoxTopLayout->addWidget(selectByCriteriaRadioButton);
    groupBoxTopLayout->addWidget(minimumZipLabel);
    groupBoxTopLayout->addWidget(minimumZipSpinBox);
    groupBoxTopLayout->addWidget(maximumZipLabel);
    groupBoxTopLayout->addWidget(maximumZipSpinBox);

    QHBoxLayout *comboBoxLayout = new QHBoxLayout;
    comboBoxLayout->addWidget(countyComboBox);
    countyGroupBox->setLayout(comboBoxLayout);

    comboBoxLayout = new QHBoxLayout;
    comboBoxLayout->addWidget(stateComboBox);
    stateGroupBox->setLayout(comboBoxLayout);

    QHBoxLayout *groupBoxBottomLayout = new QHBoxLayout;
    groupBoxBottomLayout->addWidget(countyGroupBox);
    groupBoxBottomLayout->addWidget(stateGroupBox);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout;
    groupBoxLayout->addLayout(groupBoxTopLayout);
    groupBoxLayout->addLayout(groupBoxBottomLayout);
    filterSelectGroupBox->setLayout(groupBoxLayout);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(buttonBox);
    layout->addWidget(tableView);
    layout->addWidget(filterSelectGroupBox);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
    loadButton->setFocus();
}


void MainWindow::createConnections()
{
#ifdef CUSTOM_MODEL
    connect(model,
        SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SLOT(setDirty()));
#else
    connect(model, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(setDirty()));
#endif
    connect(model, SIGNAL(rowsRemoved(const QModelIndex&,int,int)),
            this, SLOT(setDirty()));
    connect(model, SIGNAL(modelReset()), this, SLOT(setDirty()));

    connect(countyGroupBox, SIGNAL(toggled(bool)),
            this, SLOT(updateUi()));
    connect(countyComboBox,
            SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(updateUi()));
    connect(stateGroupBox, SIGNAL(toggled(bool)),
            this, SLOT(updateUi()));
    connect(stateComboBox,
            SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(updateUi()));
    connect(minimumZipSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(updateUi()));
    connect(maximumZipSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(updateUi()));

    foreach (QRadioButton *radioButton, QList<QRadioButton*>()
             << dontFilterOrSelectRadioButton << filterRadioButton
             << selectByCriteriaRadioButton)
        connect(radioButton, SIGNAL(clicked()),
                this, SLOT(radioButtonClicked()));

    connect(tableView, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(selectionChanged()));
    connect(tableView->selectionModel(),
            SIGNAL(currentChanged(const QModelIndex&,
                                  const QModelIndex&)),
            this, SLOT(selectionChanged()));

    connect(tableView->horizontalHeader(),
            SIGNAL(sectionClicked(int)),
            tableView, SLOT(sortByColumn(int)));

    connect(loadButton, SIGNAL(clicked()), this, SLOT(load()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(addZipcode()));
    connect(deleteButton, SIGNAL(clicked()),
            this, SLOT(deleteZipcode()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (okToClearData())
        event->accept();
    else
        event->ignore();
}


bool MainWindow::okToClearData()
{
    if (isWindowModified())
        return AQP::okToClearData(&MainWindow::save, this,
                tr("Unsaved changes"), tr("Save unsaved changes?"));
    return true;
}


void MainWindow::load()
{
    if (!okToClearData())
        return;
    QString filename(model->filename());
    QString dir(filename.isEmpty() ? QString(".")
                : QFileInfo(filename).canonicalPath());
    filename = QFileDialog::getOpenFileName(this,
            tr("%1 - Open").arg(QApplication::applicationName()),
            dir,
            tr("%1 (*.dat)").arg(QApplication::applicationName()));
    if (filename.isEmpty())
        return;

    enableButtons(false);
    QMetaObject::invokeMethod(this, "load", Qt::QueuedConnection,
                              Q_ARG(QString, filename));
}


void MainWindow::enableButtons(bool enable)
{
    foreach(QPushButton *button, QList<QPushButton*>() << loadButton
            << saveButton << addButton << deleteButton) {
        button->setEnabled(enable);
        button->repaint(); // update isn't "strong" enough on non-Mac
    }
}


void MainWindow::load(const QString &filename)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    loading = true;
    try {
        model->load(filename);
        tableView->resizeColumnsToContents();
        QHeaderView *header = tableView->horizontalHeader();
        header->setSortIndicatorShown(true);
        header->setSortIndicator(0, Qt::AscendingOrder);
        createComboBoxModels();
        setWindowModified(false);
        setWindowTitle(tr("%1 - %2[*]")
                .arg(QApplication::applicationName())
                .arg(QFileInfo(filename).fileName()));
        statusBar()->showMessage(tr("Loaded %n zipcode(s) from %1",
                "", model->rowCount()).arg(filename), StatusTimeout);
    } catch (AQP::Error &error) {
        AQP::warning(this, tr("Error"), tr("Failed to load %1: %2")
                .arg(filename).arg(QString::fromUtf8(error.what())));
    }
    loading = false;
    tableView->setFocus();
    radioButtonClicked();
    enableButtons();
    QApplication::restoreOverrideCursor();
}


bool MainWindow::save()
{
    try {
        model->save();
        setWindowModified(false);
        setWindowTitle(tr("%1 - %2[*]")
                .arg(QApplication::applicationName())
                .arg(QFileInfo(model->filename()).fileName()));
        statusBar()->showMessage(tr("Saved %1")
                .arg(model->filename()), StatusTimeout);
        return true;
    } catch (AQP::Error &error) {
        AQP::warning(this, tr("Error"), tr("Failed to save %1: %2")
                .arg(model->filename())
                .arg(QString::fromUtf8(error.what())));
        return false;
    }
}


void MainWindow::addZipcode()
{
    dontFilterOrSelectRadioButton->click();
#ifdef CUSTOM_MODEL
    if (!model->insertRow(model->rowCount()))
        return;
#else
    QList<QStandardItem*> items;
    QStandardItem *zipItem = new QStandardItem;
    zipItem->setData(MinZipcode, Qt::EditRole);
    items << zipItem;
    for (int i = 0; i < model->columnCount() - 1; ++i)
        items << new QStandardItem(tr("(Unknown)"));
    model->appendRow(items);
#endif
    tableView->scrollToBottom();
    tableView->setFocus();
    QModelIndex index = proxyModel->index(proxyModel->rowCount() - 1,
                                          Zipcode);
    tableView->setCurrentIndex(index);
    tableView->edit(index);
}


void MainWindow::deleteZipcode()
{
    QItemSelectionModel *selectionModel = tableView->selectionModel();
    if (!selectionModel->hasSelection())
        return;
    QModelIndex index = proxyModel->mapToSource(
            selectionModel->currentIndex());
    if (!index.isValid())
        return;
    int zipcode = model->data(model->index(index.row(),
                              Zipcode)).toInt();
    if (!AQP::okToDelete(this, tr("Delete Zipcode"),
            tr("Delete Zipcode %1?").arg(zipcode, 5, 10, QChar('0'))))
        return;
    bool filtered = filterRadioButton->isChecked();
    bool selected = selectByCriteriaRadioButton->isChecked();
    QString county = countyGroupBox->isChecked()
            ? countyComboBox->currentText() : QString();
    QString state = stateGroupBox->isChecked()
            ? stateComboBox->currentText() : QString();
    dontFilterOrSelectRadioButton->click();

    model->removeRow(index.row(), index.parent());

    createComboBoxModels();
    if (!county.isEmpty())
        countyComboBox->setCurrentIndex(
                countyComboBox->findText(county));
    if (!state.isEmpty())
        stateComboBox->setCurrentIndex(
                stateComboBox->findText(state));
    if (filtered)
        filterRadioButton->click();
    else if (selected)
        selectByCriteriaRadioButton->click();
}


void MainWindow::updateUi()
{
    if (loading || dontFilterOrSelectRadioButton->isChecked())
        return;
    if (filterRadioButton->isChecked())
        restoreFilters();
    else
        performSelection();
}


void MainWindow::performSelection()
{
    proxyModel->clearFilters();
    int minimumZipcode = minimumZipSpinBox->value();
    int maximumZipcode = maximumZipSpinBox->value();
    QString county = countyGroupBox->isChecked()
            ? countyComboBox->currentText() : QString();
    QString state = stateGroupBox->isChecked()
            ? stateComboBox->currentText() : QString();

    QItemSelection selection;
    int firstSelectedRow = -1;
    for (int row = 0; row < proxyModel->rowCount(); ++row) {
        QModelIndex index = proxyModel->index(row, Zipcode);
        int zipcode = proxyModel->data(index).toInt();
        if (zipcode < minimumZipcode || zipcode > maximumZipcode)
            continue;
        if (!matchingColumn(county, row, County))
            continue;
        if (!matchingColumn(state, row, State))
            continue;
        if (firstSelectedRow == -1)
            firstSelectedRow = row;
        QItemSelection rowSelection(index, index);
        selection.merge(rowSelection, QItemSelectionModel::Select);
    }
    QItemSelectionModel *selectionModel = tableView->selectionModel();
    selectionModel->clearSelection();
    selectionModel->select(selection, QItemSelectionModel::Rows|
                                      QItemSelectionModel::Select);
    if (firstSelectedRow != -1)
        tableView->scrollTo(proxyModel->index(firstSelectedRow, 0));
    statusBar()->showMessage(tr("Selected %L1 out of %Ln zipcode(s)",
            "", model->rowCount()).arg(selection.count()),
            StatusTimeout);
}


bool MainWindow::matchingColumn(const QString &value, int row,
                                int column)
{
    if (value.isEmpty())
        return true;
    QModelIndex index = proxyModel->index(row, column);
    return value == proxyModel->data(index).toString();
}


void MainWindow::restoreFilters()
{
    proxyModel->setMinimumZipcode(minimumZipSpinBox->value());
    proxyModel->setMaximumZipcode(maximumZipSpinBox->value());
    proxyModel->setCounty(countyGroupBox->isChecked()
            ? countyComboBox->currentText() : QString());
    proxyModel->setState(stateGroupBox->isChecked()
            ? stateComboBox->currentText() : QString());
    reportFilterEffect();
}


void MainWindow::reportFilterEffect()
{
    if (loading)
        return;
    statusBar()->showMessage(tr("Filtered %L1 out of %Ln zipcode(s)",
            "", model->rowCount()).arg(proxyModel->rowCount()),
            StatusTimeout);
}


void MainWindow::radioButtonClicked()
{
    if (dontFilterOrSelectRadioButton->isChecked()) {
        proxyModel->clearFilters();
        QItemSelectionModel *selectionModel =
                tableView->selectionModel();
        selectionModel->clearSelection();
    }
    else
        updateUi();
}


void MainWindow::selectionChanged()
{
    if (filterRadioButton->isChecked())
        return;
    QItemSelectionModel *selectionModel = tableView->selectionModel();
    if (selectionModel->selection().indexes().count() <= 1)
        dontFilterOrSelectRadioButton->setChecked(true);
}
