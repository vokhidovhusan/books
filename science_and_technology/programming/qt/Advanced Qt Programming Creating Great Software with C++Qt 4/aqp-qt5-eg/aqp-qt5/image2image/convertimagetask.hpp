#ifndef CONVERTIMAGETASK_HPP
#define CONVERTIMAGETASK_HPP
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

#include <QRunnable>
#include <QStringList>


class ConvertImageTask : public QRunnable
{
public:
    explicit ConvertImageTask(QObject *receiver,
            volatile bool *stopped, const QStringList &sourceFiles,
            const QString &targetType)
        : m_receiver(receiver), m_stopped(stopped),
          m_sourceFiles(sourceFiles),
          m_targetType(targetType.toLower()) {}

private:
    void run();

    QObject *m_receiver;
    volatile bool *m_stopped;
    const QStringList m_sourceFiles;
    const QString m_targetType;
};


#endif // CONVERTIMAGETASK_HPP
