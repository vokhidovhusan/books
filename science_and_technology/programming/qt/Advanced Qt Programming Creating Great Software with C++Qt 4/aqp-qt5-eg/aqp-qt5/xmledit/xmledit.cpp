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

#include "xmledit.hpp"
#include "xmlhighlighter.hpp"
#include <QCompleter>
#include <QScrollBar>
#include <QShortcut>
#include <QStringListModel>


inline
bool caseInsensitiveLessThan(const QString &a, const QString &b)
{
    return a.compare(b, Qt::CaseInsensitive) < 0;
}


XmlEdit::XmlEdit(QWidget *parent)
    : QPlainTextEdit(parent), completedAndSelected(false)
{
    createWidgets();
    createConnections();
    highlightCurrentLine();
}


void XmlEdit::createWidgets()
{
    (void) new XmlHighlighter(document());

    model = new QStringListModel(this);
    completer = new QCompleter(this);
    completer->setWidget(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setModel(model);
    completer->setModelSorting(
            QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(true);
}


void XmlEdit::createConnections()
{
    connect(this, SIGNAL(cursorPositionChanged()),
            this, SLOT(highlightCurrentLine()));
    connect(completer, SIGNAL(activated(const QString&)),
            this, SLOT(insertCompletion(const QString&)));
    (void) new QShortcut(QKeySequence(tr("Ctrl+M", "Complete")),
                         this, SLOT(performCompletion()));
}


void XmlEdit::keyPressEvent(QKeyEvent *event)
{
    if (completedAndSelected && handledCompletedAndSelected(event))
        return;
    completedAndSelected = false;

    if (completer->popup()->isVisible()) {
        switch (event->key()) {
            case Qt::Key_Up:      // Fallthrough
            case Qt::Key_Down:    // Fallthrough
            case Qt::Key_Enter:   // Fallthrough
            case Qt::Key_Return:  // Fallthrough
            case Qt::Key_Escape: event->ignore(); return;
            default: completer->popup()->hide(); break;
        }
    }

    QPlainTextEdit::keyPressEvent(event);
}


bool XmlEdit::handledCompletedAndSelected(QKeyEvent *event)
{
    completedAndSelected = false;
    QTextCursor cursor = textCursor();
    switch (event->key()) {
        case Qt::Key_Enter:  // Fallthrough
        case Qt::Key_Return: cursor.clearSelection(); break;
        case Qt::Key_Escape: cursor.removeSelectedText(); break;
        default: return false;
    }
    setTextCursor(cursor);
    event->accept();
    return true;
}


void XmlEdit::performCompletion()
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    const QString completionPrefix = cursor.selectedText();
    if (!completionPrefix.isEmpty() &&
        completionPrefix.at(completionPrefix.length() - 1)
                            .isLetter())
        performCompletion(completionPrefix);
}


void XmlEdit::performCompletion(const QString &completionPrefix)
{
    populateModel(completionPrefix);

    if (completionPrefix != completer->completionPrefix()) {
        completer->setCompletionPrefix(completionPrefix);
        completer->popup()->setCurrentIndex(
                completer->completionModel()->index(0, 0));
    }

    if (completer->completionCount() == 1)
        insertCompletion(completer->currentCompletion(), true);
    else {
        QRect rect = cursorRect();
        rect.setWidth(completer->popup()->sizeHintForColumn(0) +
                completer->popup()->verticalScrollBar()->
                sizeHint().width());
        completer->complete(rect);
    }
}


void XmlEdit::populateModel(const QString &completionPrefix)
{
    QStringList strings = toPlainText().split(QRegExp("\\W+"));
    strings.removeAll(completionPrefix);
    strings.removeDuplicates();
    qSort(strings.begin(), strings.end(), caseInsensitiveLessThan);
    model->setStringList(strings);
}


void XmlEdit::insertCompletion(const QString &completion,
                               bool singleWord)
{
    QTextCursor cursor = textCursor();
    int numberOfCharsToComplete = completion.length() -
            completer->completionPrefix().length();
    int insertionPosition = cursor.position();
    cursor.insertText(completion.right(numberOfCharsToComplete));
    if (singleWord) {
        cursor.setPosition(insertionPosition);
        cursor.movePosition(QTextCursor::EndOfWord,
                            QTextCursor::KeepAnchor);
        completedAndSelected = true;
    }
    setTextCursor(cursor);
}


void XmlEdit::mousePressEvent(QMouseEvent *event)
{
    if (completedAndSelected) {
        completedAndSelected = false;
        QTextCursor cursor = textCursor();
        cursor.removeSelectedText();
        setTextCursor(cursor);
    }
    QPlainTextEdit::mousePressEvent(event);
}


void XmlEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    QBrush highlightColor = palette().alternateBase();
    selection.format.setBackground(highlightColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection,
                                 true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
    setExtraSelections(extraSelections);
}
