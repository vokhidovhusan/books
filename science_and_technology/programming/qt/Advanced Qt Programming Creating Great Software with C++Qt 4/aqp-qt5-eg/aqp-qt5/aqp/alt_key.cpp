/*
    Copyright (c) 2008-10 Qtrac Ltd. All rights reserved.
    This program or module is free software: you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 2 of
    the License, or version 3 of the License, or (at your option) any
    later version. This program is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.
*/

#include "alt_key.hpp"
#include "kuhn_munkres.hpp"
#include <QAbstractButton>
#include <QAction>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QSet>
#include <QToolButton>
#include <limits>


namespace {

typedef QSet<QChar> CharSet;


struct StringItem
{
    explicit StringItem(const QString &string_=QString(),
                        const CharSet alphabet=CharSet())
        : string(string_), ustring(string_.toUpper())
    {
        foreach (const QChar &c, ustring)
            if (alphabet.contains(c))
                unique.insert(c);
    }

    void calculateWeights(int sizeOfAlphabet)
    {
        const double factor = unique.count() /
                static_cast<double>(sizeOfAlphabet);
        QList<QChar> characters = QList<QChar>::fromSet(unique);
        qSort(characters);
        double weight;
        foreach (const QChar &c, characters) {
            if (ustring[0] == c)    // First character's weight
                weight = factor;
            else if (ustring.contains(QString(" %1").arg(c))) {
                int pos = string.indexOf(QString(" %1").arg(c));
                if (pos > -1)       // Capital start of word's weight
                    weight = factor * 10;
                else {              // Start of word's weight
                    weight = factor * 100;
                    pos = ustring.indexOf(QString(" %1").arg(c));
                }
                weight += pos;
            }
            else                    // Any character's weight
                weight = (factor * 1000) + ustring.indexOf(c);
            weightOf[c] = weight;
        }
    }

    QString string;
    QString ustring;
    CharSet unique;
    QHash<QChar, double> weightOf;
};


QStringList accelerated_(const QStringList &strings,
                         const CharSet &alphabet)
{
    const double Invalid = std::numeric_limits<double>::max();

    QList<StringItem> items;
    foreach (const QString &string, strings)
        items.append(StringItem(string, alphabet));

    CharSet alphabetSet;
    foreach (const StringItem &item, items)
        alphabetSet |= item.unique;
    QList<QChar> characters = QList<QChar>::fromSet(alphabetSet);
    qSort(characters);
    QString alphabetString;
    foreach (const QChar &c, characters)
        alphabetString.append(c);
    const int rows = items.count();
    const int columns = alphabetString.count();

    KuhnMunkres::Grid grid;
    for (int row = 0; row < rows; ++row) {
        StringItem &item = items[row];
        item.calculateWeights(alphabetString.count());
        KuhnMunkres::Row rowData;
        foreach (const QChar &c, alphabetString)
            rowData.push_back(item.weightOf.value(c, Invalid));
        grid.push_back(rowData);
    }

    KuhnMunkres km;
    KuhnMunkres::Indexes indexes = km.calculate(grid);

    QStringList results;
    foreach (const KuhnMunkres::Index &index, indexes) {
        int row = index.first;
        int column = index.second;
        if (row >= rows || column >= columns)
            continue;
        StringItem &item = items[row];
        QString string = item.string;
        if (grid.at(row).at(column) < Invalid) {
            const QChar &c = alphabetString.at(column);
            if (c == item.ustring.at(0))
                string = QString("&%1").arg(item.string);
            else {
                int i = item.ustring.indexOf(QString(" %1").arg(c));
                if (i > -1)
                    ++i;
                else
                    i = item.ustring.indexOf(c);
                string = item.string.left(i) + "&" + item.string.mid(i);
            }
        }
        results.append(string);
    }
    return results;
}

} // anonymous namespace

namespace AQP {

/*
   Returns a copy of the given strings but with accelerators inserted
   where possible. Accelerators are only chosen from the given alphabet
   (which defaults to 0-9A-Z).
*/
QStringList accelerated(const QStringList &strings,
                        const QString &alphabet)
{
    CharSet alphabetSet;
    foreach (const QChar &c, alphabet.toUpper())
        alphabetSet.insert(c);

    QStringList fullResults;
    QStringList newStrings;
    foreach (const QString &string, strings) {
        QString string_copy = string;
        string_copy = string_copy.replace("&&", "");
        int i = string_copy.indexOf("&");
        if (i > -1 && i + 1 < string_copy.length()) {
            alphabetSet.remove(string_copy[string_copy.indexOf("&") + 1]
                               .toUpper());
            fullResults.append(string);
        }
        else {
            fullResults.append(QString());
            newStrings.append(string);
        }
    }
    if (newStrings.isEmpty()) // They're already accelerated
        return strings;

    QStringList results = accelerated_(newStrings, alphabetSet);
    for (int i = 0; i < fullResults.count(); ++i) {
        if (fullResults.at(i).isEmpty())
            fullResults[i] = results.isEmpty()
                    ? strings.at(i) : results.takeFirst();
    }

    return fullResults;
}


/*
    Adds accelerators to every action that has text in the menu or menu
    bar, and to all the menu or menu bar's menus and sub-menus,
    recursively.
*/
void accelerateMenu(QWidget *menuOrMenuBar, const QString &alphabet)
{
    Q_ASSERT(menuOrMenuBar->inherits("QMenuBar") ||
             menuOrMenuBar->inherits("QMenu"));
    accelerateActions(menuOrMenuBar->actions());
    foreach (QMenu *menu, menuOrMenuBar->findChildren<QMenu*>())
        accelerateActions(menu->actions(), alphabet);
}

/*
    Adds accelerators to every action that has text, but skipping
    separators.
*/
void accelerateActions(QList<QAction*> actions, const QString &alphabet)
{
    QStringList texts;
    QList<QAction*> acceleratedActions;
    foreach (QAction *action, actions) {
        if (action->isSeparator())
            continue;
        const QString &text = action->text();
        if (!text.isEmpty()) {
            texts << action->text();
            acceleratedActions << action;
        }
        if (QMenu *menu = action->menu())
            accelerateMenu(menu, alphabet);
    }
    texts = accelerated(texts, alphabet);
    Q_ASSERT(isValid(texts, 0, alphabet));
    for (int i = 0; i < texts.count(); ++i)
        acceleratedActions.at(i)->setText(texts.at(i));
}


/*
    Adds accelerators to all labels that have buddies and all abstract
    button's that have a text and all group boxes that are checkable and
    have a text that are children of the given widget.
*/
void accelerateWidget(QWidget *widget, const QString &alphabet)
{
    QStringList texts;
    QList<QWidget*> widgets;
    foreach (QLabel *label, widget->findChildren<QLabel*>()) {
        if (label->buddy() && !label->text().isEmpty()) {
            texts << label->text();
            widgets << label;
        }
    }
    foreach (QAbstractButton *button,
             widget->findChildren<QAbstractButton*>()) {
        if (!qobject_cast<QToolButton*>(button) &&
            !button->text().isEmpty()) {
            texts << button->text();
            widgets << button;
        }
    }
    foreach (QGroupBox *groupBox, widget->findChildren<QGroupBox*>()) {
        if (groupBox->isCheckable() && !groupBox->title().isEmpty()) {
            texts << groupBox->title();
            widgets << groupBox;
        }
    }
    texts = accelerated(texts, alphabet);
    Q_ASSERT(isValid(texts, 0, alphabet));
    for (int i = 0; i < texts.count(); ++i) {
        if (QLabel *label = qobject_cast<QLabel*>(widgets.at(i))) {
            label->setText(texts.at(i));
            continue;
        }
        if (QGroupBox *groupBox =
                qobject_cast<QGroupBox*>(widgets.at(i))) {
            groupBox->setTitle(texts.at(i));
            continue;
        }
        QAbstractButton *button =
                qobject_cast<QAbstractButton*>(widgets.at(i));
        Q_ASSERT(button);
        button->setText(texts.at(i));
    }
}


/*
    Adds accelerators to all the widgets in the list that are labels
    that have buddies or that are abstract button's that have a text or
    that are group boxes that are checkable and have a text.
*/
void accelerateWidgets(QList<QWidget*> widgets, const QString &alphabet)
{
    QStringList texts;
    foreach (QWidget *widget, widgets) {
        QLabel *label = qobject_cast<QLabel*>(widget);
        if (label && label->buddy() && !label->text().isEmpty()) {
            texts << label->text();
            continue;
        }
        QGroupBox *groupBox = qobject_cast<QGroupBox*>(widget);
        if (groupBox && groupBox->isCheckable() &&
            !groupBox->title().isEmpty()) {
            texts << groupBox->title();
            continue;
        }
        QAbstractButton *button = qobject_cast<QAbstractButton*>(widget);
        if (button && !qobject_cast<QToolButton*>(button) &&
            !button->text().isEmpty())
            texts << button->text();
        else
            texts << QString();
    }
    texts = accelerated(texts, alphabet);
    Q_ASSERT(isValid(texts, 0, alphabet));
    for (int i = 0; i < texts.count(); ++i) {
        const QString &text = texts.at(i);
        if (text.isEmpty())
            continue;
        if (QLabel *label = qobject_cast<QLabel*>(widgets.at(i))) {
            label->setText(text);
            continue;
        }
        if (QGroupBox *groupBox =
                qobject_cast<QGroupBox*>(widgets.at(i))) {
            groupBox->setTitle(text);
            continue;
        }
        QAbstractButton *button =
                qobject_cast<QAbstractButton*>(widgets.at(i));
        Q_ASSERT(button);
        button->setText(text);
    }
}


/*
   Returns true if all the strings are valid (i.e., each one has a
   unique accelerator---or no accelerator) and ignores where. Otherwise
   returns false and sets where to the (index of string, column) for the
   first string that is invalid (e.g., has a duplicate accelerator).
*/
bool isValid(const QStringList &listOfStrings, QPair<int, int> *where,
             const QString &alphabet)
{
    CharSet seen;
    for (int i = 0; i < listOfStrings.count(); ++i) {
        QString string = listOfStrings.at(i);
        string = string.replace("&&", "");
        int j = string.indexOf("&");
        if (j > -1 && j + 1 < string.length()) {
            const QChar c = string.at(j + 1).toUpper();
            if (seen.contains(c) || !alphabet.contains(c)) {
                if (where) {
                    where->first = i;
                    where->second = j;
                }
                return false;
            }
            seen.insert(c);
        }
    }
    return true;
}


/*
   Returns a quality value in the range 0.0 (worst) to 1.0 (best).
   The best quality is when a string has an accelerator for its first
   character, second best is an accelerator for a start of word, third
   best is any accelerator at all, and worst is no accelerator.
*/
double quality(const QStringList &listOfStrings)
{
    enum {FIRST_CHARACTER = 4, START_OF_WORD = 2, IN_STRING = 1};

    int done = 0;
    int weight = 0;
    foreach (QString string, listOfStrings) {
        string = string.replace("&&", "");
        ++done;
        if (string.at(0) == QChar('&'))
            weight += FIRST_CHARACTER;
        else if (string.contains(" &"))
            weight += START_OF_WORD;
        else if (string.contains("&"))
            weight += IN_STRING;
        else
            --done;
    }
    double range = (listOfStrings.count() *
                    static_cast<int>(FIRST_CHARACTER));
    return ((weight * (done /
            static_cast<double>(listOfStrings.count()))) / range);
}


/*
   Returns the number of strings in the given list that have
   accelerators.
*/
int numberAccelerated(const QStringList &listOfStrings)
{
    int count = 0;
    foreach (QString string, listOfStrings) {
        string = string.replace("&&", "");
        int i = string.indexOf("&");
        if (i > -1 && i + 1 < string.length())
            ++count;
    }
    return count;
}


} // namespace AQP
