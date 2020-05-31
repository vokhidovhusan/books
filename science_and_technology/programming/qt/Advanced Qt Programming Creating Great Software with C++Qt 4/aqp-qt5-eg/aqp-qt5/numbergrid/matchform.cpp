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

#include "matchform.hpp"
#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include <limits>


QString comparisonName(ComparisonType comparisonType)
{
    switch (comparisonType) {
        case LessThan: return QObject::tr("<");
        case LessThanOrEqual: return QObject::tr("<=");
        case GreaterThanOrEqual: return QObject::tr(">=");
        case GreaterThan: return QObject::tr(">");
        case ApproximatelyEqual: return QObject::tr("~=");
    }
    Q_ASSERT(false);
    return QString();
}


MatchForm::MatchForm(ActionType actionType_, QWidget *parent)
    : QDialog(parent), actionType(actionType_)
{
    createWidgets();
    createLayout();
    createConnections();

    setWindowTitle((actionType == Count
            ? tr("%1 - Count") : tr("%1 - Select"))
            .arg(QApplication::applicationName()));
}


void MatchForm::createWidgets()
{
    label = new QLabel(actionType == Count ? tr("Count C&ells:")
                                           : tr("Select C&ells:"));
    comparisonComboBox = new QComboBox;
    label->setBuddy(comparisonComboBox);
    comparisonComboBox->addItem(tr("<"),
            static_cast<int>(LessThan));
    comparisonComboBox->addItem(tr("<="),
            static_cast<int>(LessThanOrEqual));
    comparisonComboBox->addItem(tr(">="),
            static_cast<int>(GreaterThanOrEqual));
    comparisonComboBox->addItem(tr(">"),
            static_cast<int>(GreaterThan));
    comparisonComboBox->addItem(tr("~="),
            static_cast<int>(ApproximatelyEqual));
    valueSpinBox = new QDoubleSpinBox;
    valueSpinBox->setRange(-(std::numeric_limits<double>::max() - 1),
                           std::numeric_limits<double>::max());
    valueSpinBox->setValue(0.0);
    valueSpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    if (actionType != Select) {
        applyToAllRadioButton = new QRadioButton(tr("Apply to &All"));
        applyToAllRadioButton->setChecked(true);
        applyToSelectionRadioButton = new QRadioButton(
                tr("Apply to &Selected"));
    }
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|
                                     QDialogButtonBox::Cancel);
}


void MatchForm::createLayout()
{
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(label);
    topLayout->addWidget(comparisonComboBox);
    topLayout->addWidget(valueSpinBox);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(topLayout);
    if (actionType != Select) {
        QHBoxLayout *middleLayout = new QHBoxLayout;
        middleLayout->addWidget(applyToAllRadioButton);
        middleLayout->addWidget(applyToSelectionRadioButton);
        middleLayout->addStretch();
        layout->addLayout(middleLayout);
    }
    layout->addWidget(buttonBox);
    setLayout(layout);
}


void MatchForm::createConnections()
{
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


void MatchForm::accept()
{
    matchCriteria.comparisonType = static_cast<ComparisonType>(
            comparisonComboBox->itemData(
                comparisonComboBox->currentIndex()).toInt());
    matchCriteria.value = valueSpinBox->value();
    matchCriteria.applyToAll = actionType == Select ? false
            : applyToAllRadioButton->isChecked();
    QDialog::accept();
}
