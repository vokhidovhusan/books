#ifndef JINGLEACTION_HPP
#define JINGLEACTION_HPP
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

#include <QAction>


class JingleAction : public QAction
{
    Q_OBJECT

public:
    explicit JingleAction(QObject *parent) : QAction(parent) {}
    explicit JingleAction(const QString &text, QObject *parent)
        : QAction(text, parent) {}
    explicit JingleAction(const QIcon &icon, const QString &text,
                          QObject *parent)
        : QAction(icon, text, parent) {}
    explicit JingleAction(const QString &jingleFile,
                          const QString &text, QObject *parent);
    explicit JingleAction(const QString &jingleFile,
            const QIcon &icon, const QString &text, QObject *parent);

    QString jingleFile() const { return m_jingleFile; }
    void setJingleFile(const QString &jingleFile);

    static void setMute(bool mute) { s_mute = mute; }

private slots:
    void play();

private:
    static bool s_mute;

    QString m_jingleFile;
};

#endif // JINGLEACTION_HPP
