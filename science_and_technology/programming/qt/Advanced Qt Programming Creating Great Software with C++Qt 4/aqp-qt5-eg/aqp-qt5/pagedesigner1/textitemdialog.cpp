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
#include "swatch.hpp"
#include "textedit.hpp"
#include "textitem.hpp"
#include "textitemdialog.hpp"
#include <QApplication>
#include <QDialogButtonBox>
#include <QGraphicsScene>
#include <QPoint>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>


TextItemDialog::TextItemDialog(TextItem *item_,
        const QPoint &position_, QGraphicsScene *scene_,
        QWidget *parent)
    : QDialog(parent), item(item_), position(position_), scene(scene_)
{
    createWidgets();
    createLayout();
    createConnections();

    AQP::accelerateWidget(this);
    setWindowTitle(tr("%1 - %2 Text Item")
            .arg(QApplication::applicationName())
            .arg(item ? tr("Edit") : tr("Add")));
    updateUi();
}


void TextItemDialog::createWidgets()
{
    textEdit = new TextEdit;
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|
                                     QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    buttonBox->button(QDialogButtonBox::Ok)->setIcon(
            style()->standardIcon(QStyle::SP_DialogOkButton));
    buttonBox->button(QDialogButtonBox::Cancel)->setIcon(
            style()->standardIcon(QStyle::SP_DialogCancelButton));

    if (item)
        textEdit->setHtml(item->toHtml());
}


void TextItemDialog::createLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(textEdit);
    layout->addWidget(buttonBox);
    setLayout(layout);
}


void TextItemDialog::createConnections()
{
    connect(textEdit, SIGNAL(textChanged()), this, SLOT(updateUi()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


void TextItemDialog::updateUi()
{
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
            !textEdit->toPlainText().isEmpty());
}


void TextItemDialog::accept()
{
    if (item && !textEdit->document()->isModified()) {
        QDialog::reject();
        return;
    }
    if (!item)
        item = new TextItem(position, scene);
    item->setHtml(textEdit->toHtml());
    item->update();
    QDialog::accept();
}
