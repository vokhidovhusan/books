#ifndef MATCHFORM_HPP
#define MATCHFORM_HPP
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


class QComboBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QLabel;
class QRadioButton;


enum ComparisonType {LessThan, LessThanOrEqual, GreaterThan,
                     GreaterThanOrEqual, ApproximatelyEqual};
QString comparisonName(ComparisonType comparisonType);


struct MatchCriteria
{
    ComparisonType comparisonType;
    double value;
    bool applyToAll;
};


class MatchForm : public QDialog
{
    Q_OBJECT

public:
    enum ActionType {Count, Select};

    explicit MatchForm(ActionType actionType_, QWidget *parent=0);

    MatchCriteria result() const { return matchCriteria; }

public slots:
    void accept();

private:
    void createWidgets();
    void createLayout();
    void createConnections();

    QLabel *label;
    QComboBox *comparisonComboBox;
    QDoubleSpinBox *valueSpinBox;
    QRadioButton *applyToAllRadioButton;
    QRadioButton *applyToSelectionRadioButton;
    QDialogButtonBox *buttonBox;

    ActionType actionType;
    MatchCriteria matchCriteria;
};

#endif // MATCHFORM_HPP
