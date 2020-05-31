#ifndef CROSSFADER_HPP
#define CROSSFADER_HPP
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

#include <QImage>
#include <QThread>


class CrossFader : public QThread
{
    Q_OBJECT

public:
    explicit CrossFader(const QString &filename, const QImage &first,
               const double &firstWeight, const QImage &last,
               const double &lastWeight, QObject *parent=0);

public slots:
    void stop() { m_stopped = true; }

signals:
    void progress(int);
    void saving(const QString&);
    void saved(bool, const QString&);

private:
    void run();

    const QString m_filename;
    QImage m_first;
    QImage m_last;
    const double m_firstWeight;
    const double m_lastWeight;
    volatile bool m_stopped;
};

#endif // CROSSFADER_HPP
