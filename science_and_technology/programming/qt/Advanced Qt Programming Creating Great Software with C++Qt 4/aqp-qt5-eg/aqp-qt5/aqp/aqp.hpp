#ifndef AQP_HPP
#define AQP_HPP
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

#include <QApplication>
#include <QByteArray>
#include <QList>
#include <QMessageBox>
#include <QSet>
#if QT_VERSION >= 0x040600
#include <QScopedPointer>
#else
#include <QSharedPointer>
#endif
#include <QString>
#include <QVector>
#ifdef Q_CC_MSVC
#include <math.h>
#else
#include <cmath>
#endif
#include <exception>
// For weird Microsoft compilers, e.g. MSVC9
#ifndef M_PI
#define M_PI 3.14159265358
#endif

class QStringList;

namespace AQP {

class Error : public std::exception
{
public:
    explicit Error(const QString &message) throw()
        : message(message.toUtf8()) {}
    ~Error() throw() {}

    const char *what() const throw() { return message; }

private:
    const char *message;
};

void information(QWidget *parent, const QString &title,
        const QString &text, const QString &detailedText=QString());
void warning(QWidget *parent, const QString &title,
        const QString &text, const QString &detailedText=QString());
bool question(QWidget *parent, const QString &title,
        const QString &text, const QString &detailedText=QString(),
        const QString &yesText=QObject::tr("&Yes"),
        const QString &noText=QObject::tr("&No"));
bool okToDelete(QWidget *parent, const QString &title,
        const QString &text, const QString &detailedText=QString());

template<typename T>
bool okToClearData(bool (T::*saveData)(), T *parent,
        const QString &title, const QString &text,
        const QString &detailedText=QString())
{
    Q_ASSERT(saveData && parent);
#if QT_VERSION >= 0x040600
    QScopedPointer<QMessageBox> messageBox(new QMessageBox(parent));
#else
    QSharedPointer<QMessageBox> messageBox(new QMessageBox(parent));
#endif
    messageBox->setWindowModality(Qt::WindowModal);
    messageBox->setIcon(QMessageBox::Question);
    messageBox->setWindowTitle(QString("%1 - %2")
            .arg(QApplication::applicationName()).arg(title));
    messageBox->setText(text);
    if (!detailedText.isEmpty())
        messageBox->setInformativeText(detailedText);
    messageBox->addButton(QMessageBox::Save);
    messageBox->addButton(QMessageBox::Discard);
    messageBox->addButton(QMessageBox::Cancel);
    messageBox->setDefaultButton(QMessageBox::Save);
    messageBox->exec();
    if (messageBox->clickedButton() ==
        messageBox->button(QMessageBox::Cancel))
        return false;
    if (messageBox->clickedButton() ==
        messageBox->button(QMessageBox::Save))
        return (parent->*saveData)();
    return true;
}

const int MSecPerSecond = 1000;

void hoursMinutesSecondsForMSec(const int msec, int *hours,
                                int *minutes, int *seconds);

inline qreal radiansFromDegrees(qreal degrees)
    { return degrees * M_PI / 180.0; }
inline qreal degreesFromRadians(qreal radians)
    { return radians * 180.0 / M_PI; }

QString applicationPathOf(const QString &path=QString());
QString filenameFilter(const QString &name,
                       const QList<QByteArray> formats);
QString filenameFilter(const QString &name,
                       const QStringList &mimeTypes);
QSet<QString> suffixesForMimeTypes(const QStringList &mimeTypes);

QVector<int> chunkSizes(const int size, const int chunkCount);

} // namespace AQP

#endif // AQP_HPP
