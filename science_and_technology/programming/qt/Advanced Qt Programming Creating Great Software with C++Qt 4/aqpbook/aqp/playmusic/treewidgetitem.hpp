#ifndef TREEWIDGETITEM_HPP
#define TREEWIDGETITEM_HPP
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

#include <QTreeWidgetItem>


namespace {
const int FilenameRole = Qt::UserRole;
}


class TreeWidgetItem : public QTreeWidgetItem
{
public:
    explicit TreeWidgetItem(QTreeWidgetItem *parent,
            const QStringList &strings,
            int type=QTreeWidgetItem::UserType)
        : QTreeWidgetItem(parent, strings, type) {}

private:
    bool operator<(const QTreeWidgetItem &other) const
    {
        QString left = data(0, FilenameRole).toString();
        QString right;
        if (!left.isEmpty())
            right = other.data(0, FilenameRole).toString();
        else {
            left = text(0);
            right = other.text(0);
        }
        return QString::compare(left, right, Qt::CaseInsensitive) < 0;
    }
};

#endif // TREEWIDGETITEM_HPP
