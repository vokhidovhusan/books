#ifndef PROXYMODEL_HPP
#define PROXYMODEL_HPP
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


#include <QSortFilterProxyModel>


class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ProxyModel(QObject *parent=0);

    int minimumZipcode() const { return m_minimumZipcode; }
    int maximumZipcode() const { return m_maximumZipcode; }
    QString county() const { return m_county; }
    QString state() const { return m_state; }

public slots:
    void clearFilters();
    void setMinimumZipcode(int minimumZipcode);
    void setMaximumZipcode(int maximumZipcode);
    void setCounty(const QString &county);
    void setState(const QString &state);

protected:
    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const;

private:
    int m_minimumZipcode;
    int m_maximumZipcode;
    QString m_county;
    QString m_state;
};

#endif // PROXYMODEL_HPP
