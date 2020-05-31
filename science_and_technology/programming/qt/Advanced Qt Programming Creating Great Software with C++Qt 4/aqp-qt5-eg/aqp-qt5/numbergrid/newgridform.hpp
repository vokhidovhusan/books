#ifndef NEWGRIDFORM_HPP
#define NEWGRIDFORM_HPP
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

#include <QDialog>


class QDialogButtonBox;
class QDoubleSpinBox;
class QCheckBox;
class QFormLayout;
class QSpinBox;


struct GridSpecification
{
    int rows;
    int columns;
    bool randomInitialValue;
    double initialValue;
};


class NewGridForm : public QDialog
{
    Q_OBJECT

public:
    explicit NewGridForm(QWidget *parent=0);

    GridSpecification result() const { return gridSpecification; }

public slots:
    void accept();

private:
    void createWidgets();
    void createLayout();
    void createConnections();

    QSpinBox *rowsSpinBox;
    QSpinBox *columnsSpinBox;
    QCheckBox *randomInitialValuesCheckBox;
    QDoubleSpinBox *initialValueSpinBox;
    QDialogButtonBox *buttonBox;
    QFormLayout *layout;

    GridSpecification gridSpecification;
};

#endif // NEWGRIDFORM_HPP
