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

#include "global.hpp"
#include "proxymodel.hpp"


ProxyModel::ProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    m_minimumZipcode = m_maximumZipcode = InvalidZipcode;
}


void ProxyModel::clearFilters()
{
    m_minimumZipcode = m_maximumZipcode = InvalidZipcode;
    m_county.clear();
    m_state.clear();
    invalidateFilter();
}


bool ProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
    if (m_minimumZipcode != InvalidZipcode ||
        m_maximumZipcode != InvalidZipcode) {
        QModelIndex index = sourceModel()->index(sourceRow, Zipcode,
                                                 sourceParent);
        if (m_minimumZipcode != InvalidZipcode &&
            sourceModel()->data(index).toInt() < m_minimumZipcode)
            return false;
        if (m_maximumZipcode != InvalidZipcode &&
            sourceModel()->data(index).toInt() > m_maximumZipcode)
            return false;
    }
    if (!m_county.isEmpty()) {
        QModelIndex index = sourceModel()->index(sourceRow, County,
                                                 sourceParent);
        if (m_county != sourceModel()->data(index).toString())
            return false;
    }
    if (!m_state.isEmpty()) {
        QModelIndex index = sourceModel()->index(sourceRow, State,
                                                 sourceParent);
        if (m_state != sourceModel()->data(index).toString())
            return false;
    }
    return true;
}


void ProxyModel::setMinimumZipcode(int minimumZipcode)
{
    if (m_minimumZipcode != minimumZipcode) {
        m_minimumZipcode = minimumZipcode;
        invalidateFilter();
    }
}


void ProxyModel::setMaximumZipcode(int maximumZipcode)
{
    if (m_maximumZipcode != maximumZipcode) {
        m_maximumZipcode = maximumZipcode;
        invalidateFilter();
    }
}


void ProxyModel::setCounty(const QString &county)
{
    if (m_county != county) {
        m_county = county;
        invalidateFilter();
    }
}


void ProxyModel::setState(const QString &state)
{
    if (m_state != state) {
        m_state = state;
        invalidateFilter();
    }
}
