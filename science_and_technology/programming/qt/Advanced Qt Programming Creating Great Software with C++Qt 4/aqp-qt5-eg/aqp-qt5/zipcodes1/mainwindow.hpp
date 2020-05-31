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

#include <QMainWindow>


class QComboBox;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QTableView;
#ifdef CUSTOM_MODEL
class TableModel;
#else
class StandardTableModel;
#endif
class ProxyModel;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=0);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void load();
    void load(const QString &filename);
    bool save();
    void addZipcode();
    void deleteZipcode();
    void setDirty() { setWindowModified(true); }
    void updateUi();
    void radioButtonClicked();
    void selectionChanged();

private:
    void createWidgets();
    void createComboBoxModels();
    void createComboBoxModel(QComboBox *comboBox, int column);
    void createLayout();
    void createConnections();
    bool okToClearData();
    void enableButtons(bool enable=true);
    bool matchingColumn(const QString &value, int row, int column);
    void restoreFilters();
    void reportFilterEffect();
    void performSelection();

    QDialogButtonBox *buttonBox;
    QPushButton *loadButton;
    QPushButton *saveButton;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QPushButton *quitButton;
    QGroupBox *filterSelectGroupBox;
    QRadioButton *dontFilterOrSelectRadioButton;
    QRadioButton *filterRadioButton;
    QRadioButton *selectByCriteriaRadioButton;
    QLabel *minimumZipLabel;
    QSpinBox *minimumZipSpinBox;
    QLabel *maximumZipLabel;
    QSpinBox *maximumZipSpinBox;
    QGroupBox *countyGroupBox;
    QComboBox *countyComboBox;
    QGroupBox *stateGroupBox;
    QComboBox *stateComboBox;
    QTableView *tableView;

#ifdef CUSTOM_MODEL
    TableModel *model;
#else
    StandardTableModel *model;
#endif
    ProxyModel *proxyModel;
    bool loading;
};

#endif // MAINWINDOW_HPP
