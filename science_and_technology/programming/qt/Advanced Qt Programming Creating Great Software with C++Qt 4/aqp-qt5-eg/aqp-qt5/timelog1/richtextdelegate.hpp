#ifndef RICHTEXTDELEGATE_HPP
#define RICHTEXTDELEGATE_HPP
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

#include <QCheckBox>
#include <QLabel>
#include <QStyledItemDelegate>
#include <QTextDocument>


class QModelIndex;
class QPainter;
class QStyleOptionViewItem;


class RichTextDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit RichTextDelegate(QObject *parent=0);
    ~RichTextDelegate() { delete checkbox; delete label; }

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

private slots:
    void closeAndCommitEditor();

private:
    void paintWidget(QPainter *painter, const QRect &rect,
                     const QString &cacheKey, QWidget *widget) const;

    QCheckBox *checkbox;
    QLabel *label;
    mutable QTextDocument document;
};

#endif // RICHTEXTDELEGATE_HPP
