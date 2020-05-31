#ifndef THREADSAFEERRORINFO_HPP
#define THREADSAFEERRORINFO_HPP
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

#include <QMutex>
#include <QMutexLocker>
#include <QSet>


class ThreadSafeErrorInfo
{
public:
    explicit ThreadSafeErrorInfo() : m_count(0) {}

    int count() const
    {
        QMutexLocker locker(&mutex);
        return m_count;
    }


    QStringList errors() const
    {
        QMutexLocker locker(&mutex);
        return QStringList::fromSet(m_errors);
    }

    bool isEmpty() const
    {
        QMutexLocker locker(&mutex);
        return m_count == 0;
    }


    void add(const QString &error)
    {
        QMutexLocker locker(&mutex);
        ++m_count;
        m_errors << error;
    }

    void clear()
    {
        QMutexLocker locker(&mutex);
        m_count = 0;
        m_errors.clear();
    }

private:
    mutable QMutex mutex;
    int m_count;
    QSet<QString> m_errors;
};

#endif // THREADSAFEERRORINFO_HPP
