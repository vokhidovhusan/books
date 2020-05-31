#ifndef SCRIPTFORM_HPP
#define SCRIPTFORM_HPP
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
#include <QRadioButton>


class QDialogButtonBox;
class QLabel;
class QPlainTextEdit;


class ScriptForm : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptForm(const QString &script, QWidget *parent=0);

    QString script() const { return m_script; }
    bool applyToAll() const
        { return applyToAllRadioButton->isChecked(); }

public slots:
    void accept();

private:
    void createWidgets();
    void createLayout();
    void createConnections();

    QLabel *label;
    QPlainTextEdit *textEdit;
    QRadioButton *applyToAllRadioButton;
    QRadioButton *applyToSelectionRadioButton;
    QDialogButtonBox *buttonBox;

    QString m_script;
};

#endif // SCRIPTFORM_HPP
