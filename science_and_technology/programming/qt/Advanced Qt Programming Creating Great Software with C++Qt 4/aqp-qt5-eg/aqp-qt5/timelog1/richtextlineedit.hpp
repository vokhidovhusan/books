#ifndef RICHTEXTLINEEDIT_HPP
#define RICHTEXTLINEEDIT_HPP
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


#include <QTextEdit>

class QAction;
class QKeyEvent;


class RichTextLineEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit RichTextLineEdit(QWidget *parent=0);

    QString toSimpleHtml() const;

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

public slots:
    void toggleItalic() { setFontItalic(!fontItalic()); }
    void toggleBold() { setFontWeight(fontWeight() > QFont::Normal
                        ? QFont::Normal : QFont::Bold); }

signals:
    void returnPressed();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void customContextMenuRequested(const QPoint &pos);
    void applyTextEffect();
    void applyColor(QAction *action);

private:
    enum Style {Bold, Italic, StrikeOut, NoSuperOrSubscript,
                Subscript, Superscript};

    void createShortcuts();
    void createActions();
    QAction *createAction(const QString &text, const QVariant &data);
    QMenu *createColorMenu();
    void updateContextMenuActions();

    QAction *boldAction;
    QAction *italicAction;
    QAction *strikeOutAction;
    QAction *noSubOrSuperScriptAction;
    QAction *superScriptAction;
    QAction *subScriptAction;
    QAction *colorAction;
};

#endif // RICHTEXTLINEEDIT_HPP
