#ifndef STANDARDTREEMODEL_HPP
#define STANDARDTREEMODEL_HPP
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

#include <QStandardItemModel>
// Mac needs these included this way---and needs them in the header
#include <QtCore/QXmlStreamReader>
#include <QtCore/QXmlStreamWriter>


class QStandardItem;
class StandardItem;


class StandardTreeModel : public QStandardItemModel
{
    Q_OBJECT

public:
    enum Insert {AtTopLevel, AsSibling, AsChild};

    explicit StandardTreeModel(QObject *parent=0);

    QString filename() const { return m_filename; }
    void setFilename(const QString &filename)
        { m_filename = filename; }
    void clear();
    void load(const QString &filename=QString());
    void save(const QString &filename=QString());
    QStandardItem *insertNewTask(Insert insert, const QString &name,
                                 const QModelIndex &index);
    QStringList pathForIndex(const QModelIndex &index) const;
    QStandardItem *itemForPath(const QStringList &path) const;

private:
    void initialize();
    void calculateTotalsFor(QStandardItem *root);
    QStandardItem *itemForPath(QStandardItem *root,
                               const QStringList &path) const;
    StandardItem *createNewTask(QStandardItem *root,
                                const QString &name, bool checked);
    void writeTaskAndChildren(QXmlStreamWriter *writer,
                              QStandardItem *item);

    QString m_filename;
};


#endif // STANDARDTREEMODEL_HPP
