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

#include "newgridform.hpp"
#include <QApplication>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QFormLayout>
#include <QSpinBox>


NewGridForm::NewGridForm(QWidget *parent)
    : QDialog(parent)
{
    createWidgets();
    createLayout();
    createConnections();

    randomInitialValuesCheckBox->setChecked(true);
    setWindowTitle(tr("%1 - New Grid")
                      .arg(QApplication::applicationName()));
}


void NewGridForm::createWidgets()
{
    rowsSpinBox = new QSpinBox;
    rowsSpinBox->setRange(1, 1000);
    rowsSpinBox->setValue(50);
    rowsSpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    columnsSpinBox = new QSpinBox;
    columnsSpinBox->setRange(1, 1000);
    columnsSpinBox->setValue(50);
    columnsSpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    randomInitialValuesCheckBox = new QCheckBox(
            tr("Random &Initial Values"));
    initialValueSpinBox = new QDoubleSpinBox;
    initialValueSpinBox->setRange(-1e6, 1e6);
    initialValueSpinBox->setValue(0.0);
    initialValueSpinBox->setAlignment(Qt::AlignVCenter|
                                      Qt::AlignRight);
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|
                                     QDialogButtonBox::Cancel);
}


void NewGridForm::createLayout()
{
    layout = new QFormLayout;
    layout->addRow(tr("&Rows:"), rowsSpinBox);
    layout->addRow(tr("&Columns:"), columnsSpinBox);
    layout->addRow(randomInitialValuesCheckBox);
    layout->addRow(tr("Fixed Initial &Value:"), initialValueSpinBox);
    layout->addRow(buttonBox);
    setLayout(layout);
}


void NewGridForm::createConnections()
{
    connect(randomInitialValuesCheckBox, SIGNAL(toggled(bool)),
            initialValueSpinBox, SLOT(setDisabled(bool)));
    connect(randomInitialValuesCheckBox, SIGNAL(toggled(bool)),
            layout->labelForField(initialValueSpinBox),
            SLOT(setDisabled(bool)));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


void NewGridForm::accept()
{
    gridSpecification.rows = rowsSpinBox->value();
    gridSpecification.columns = columnsSpinBox->value();;
    gridSpecification.randomInitialValue =
            randomInitialValuesCheckBox->isChecked();
    gridSpecification.initialValue = initialValueSpinBox->value();
    QDialog::accept();
}
