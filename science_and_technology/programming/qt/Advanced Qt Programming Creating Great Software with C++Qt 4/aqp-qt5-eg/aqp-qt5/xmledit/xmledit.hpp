#ifndef XMLEDIT_HPP
#define XMLEDIT_HPP
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

#include <QtWidgets> // added for Qt5
#include <QPlainTextEdit>


class QCompleter;
class QStringListModel;


class XmlEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit XmlEdit(QWidget *parent=0);

protected:
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void insertCompletion(const QString &completion,
                          bool singleWord=false);
    void highlightCurrentLine();
    void performCompletion();

private:
    void createWidgets();
    void createConnections();
    QString textUnderCursor() const;
    void performCompletion(const QString &completionPrefix);
    bool handledCompletedAndSelected(QKeyEvent *event);
    void populateModel(const QString &completionPrefix);

private:
    bool completedAndSelected;
    QCompleter *completer;
    QStringListModel *model;
};

#endif // XMLEDIT_HPP
