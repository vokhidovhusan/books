#ifndef GETMD5STHREAD_HPP
#define GETMD5STHREAD_HPP
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
#include <QThread>
#include <QStringList>


class GetMD5sThread : public QThread
{
    Q_OBJECT

public:
    explicit GetMD5sThread(volatile bool *stopped,
            const QString &root, const QStringList &directories,
            FilesForMD5 *filesForMD5)
        : m_stopped(stopped), m_root(root),
          m_directories(directories), m_filesForMD5(filesForMD5) {}

signals:
    void readOneFile();

private:
    void run();

    volatile bool *m_stopped;
    const QString m_root;
    const QStringList m_directories;
    FilesForMD5 *m_filesForMD5;
};


#endif //GETMD5STHREAD_HPP
