#ifndef TEXTITEMDIALOG_HPP
#define TEXTITEMDIALOG_HPP
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
class QGraphicsScene;
class QPoint;
class TextEdit;
class TextItem;


class TextItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextItemDialog(TextItem *item_=0,
            const QPoint &position_=QPoint(),
            QGraphicsScene *scene_=0, QWidget *parent=0);

    TextItem *textItem() const { return item; }

public slots:
    void accept();

private slots:
    void updateUi();

private:
    void createWidgets();
    void createLayout();
    void createConnections();

    TextEdit *textEdit;
    QDialogButtonBox *buttonBox;

    TextItem *item;
    QPoint position;
    QGraphicsScene *scene;
    QString originalHtml;
};

#endif // TEXTITEMDIALOG_HPP
