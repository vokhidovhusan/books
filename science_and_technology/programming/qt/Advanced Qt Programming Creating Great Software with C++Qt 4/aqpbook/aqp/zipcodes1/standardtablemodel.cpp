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

#include "aqp.hpp"
#include "global.hpp"
#include "standardtablemodel.hpp"
#include <QDataStream>
#include <QFile>


namespace {
const qint32 MagicNumber = 0x5A697043;
const qint16 FormatNumber = 100;
}


StandardTableModel::StandardTableModel(QObject *parent)
    : QStandardItemModel(parent)
{
    initialize();
}


void StandardTableModel::initialize()
{
    setHorizontalHeaderLabels(QStringList() << tr("Zipcode")
            << tr("Post Office") << tr("County") << tr("State"));
}


void StandardTableModel::clear()
{
    QStandardItemModel::clear();
    initialize();
}


void StandardTableModel::load(const QString &filename)
{
    if (!filename.isEmpty())
        m_filename = filename;
    if (m_filename.isEmpty())
        throw AQP::Error(tr("no filename specified"));
    QFile file(m_filename);
    if (!file.open(QIODevice::ReadOnly))
        throw AQP::Error(file.errorString());

    QDataStream in(&file);
    qint32 magicNumber;
    in >> magicNumber;
    if (magicNumber != MagicNumber)
        throw AQP::Error(tr("unrecognized file type"));
    qint16 formatVersionNumber;
    in >> formatVersionNumber;
    if (formatVersionNumber > FormatNumber)
        throw AQP::Error(tr("file format version is too new"));
    in.setVersion(QDataStream::Qt_4_5);
    clear();

    quint16 zipcode;
    QString postOffice;
    QString county;
    QString state;
    QMap<quint16, QList<QStandardItem*> > itemsForZipcode;

    while (!in.atEnd()) {
        in >> zipcode >> postOffice >> county >> state;
        QList<QStandardItem*> items;
        QStandardItem *item = new QStandardItem;
        item->setData(zipcode, Qt::EditRole);
        items << item;
        foreach (const QString &text, QStringList() << postOffice
                                      << county << state)
            items << new QStandardItem(text);
        itemsForZipcode[zipcode] = items;
    }

    QMapIterator<quint16, QList<QStandardItem*> > i(itemsForZipcode);
    while (i.hasNext())
        appendRow(i.next().value());
}


void StandardTableModel::save(const QString &filename)
{
    if (!filename.isEmpty())
        m_filename = filename;
    if (m_filename.isEmpty())
        throw AQP::Error(tr("no filename specified"));
    QFile file(m_filename);
    if (!file.open(QIODevice::WriteOnly))
        throw AQP::Error(file.errorString());

    QDataStream out(&file);
    out << MagicNumber << FormatNumber;
    out.setVersion(QDataStream::Qt_4_5);
    for (int row = 0; row < rowCount(); ++row) {
        out << static_cast<quint16>(
                    item(row, Zipcode)->data(Qt::EditRole).toUInt())
            << item(row, PostOffice)->text()
            << item(row, County)->text() << item(row, State)->text();
    }
}
