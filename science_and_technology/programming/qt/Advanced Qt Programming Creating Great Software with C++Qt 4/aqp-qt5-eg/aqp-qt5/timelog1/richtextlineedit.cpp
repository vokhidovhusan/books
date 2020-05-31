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

#include "alt_key.hpp"
#include "richtextlineedit.hpp"
#include <QAction>
#include <QKeyEvent>
#include <QMenu>
#include <QShortcut>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextFragment>


RichTextLineEdit::RichTextLineEdit(QWidget *parent)
    : QTextEdit(parent)
{
    setLineWrapMode(QTextEdit::NoWrap);
    setWordWrapMode(QTextOption::NoWrap);
    setAcceptRichText(true);
    setTabChangesFocus(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    createShortcuts();
    createActions();
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(customContextMenuRequested(const QPoint&)));
}


void RichTextLineEdit::createShortcuts()
{
    QShortcut *boldShortcut = new QShortcut(QKeySequence::Bold,
            this, SLOT(toggleBold()));
    QShortcut *italicShortcut = new QShortcut(QKeySequence::Italic,
            this, SLOT(toggleItalic()));

    setToolTip(tr("<p>Use %1 to toggle bold, %2 to toggle italic, "
                  "and the context menu for color and other effects.")
            .arg(boldShortcut->key().toString(
                 QKeySequence::NativeText))
            .arg(italicShortcut->key().toString(
                 QKeySequence::NativeText)));
}


void RichTextLineEdit::createActions()
{
    boldAction = createAction(tr("Bold"), Bold);
    italicAction = createAction(tr("Italic"), Italic);
    strikeOutAction = createAction(tr("Strike out"), StrikeOut);
    noSubOrSuperScriptAction = createAction(
            tr("No super or subscript"), NoSuperOrSubscript);
    superScriptAction = createAction(tr("Superscript"), Superscript);
    subScriptAction = createAction(tr("Subscript"), Subscript);

    colorAction = new QAction(tr("Color"), this);
    colorAction->setMenu(createColorMenu());

    addActions(QList<QAction*>() << boldAction << italicAction
            << strikeOutAction << noSubOrSuperScriptAction
            << superScriptAction << subScriptAction << colorAction);
    AQP::accelerateActions(actions());
}


QAction *RichTextLineEdit::createAction(const QString &text,
                                        const QVariant &data)
{
    QAction *action = new QAction(text, this);
    action->setData(data);
    action->setCheckable(true);
    action->setChecked(false);
    connect(action, SIGNAL(triggered()), SLOT(applyTextEffect()));
    return action;
}


QMenu *RichTextLineEdit::createColorMenu()
{
    QMenu *colorMenu = new QMenu(this);
    QPixmap pixmap(22, 22);
    typedef QPair<QColor, QString> ColorPair;
    foreach (const ColorPair &pair, QList<ColorPair>()
            << qMakePair(QColor(Qt::black), tr("Black"))
            << qMakePair(QColor(Qt::blue), tr("Blue"))
            << qMakePair(QColor(Qt::darkBlue), tr("Dark Blue"))
            << qMakePair(QColor(Qt::cyan), tr("Cyan"))
            << qMakePair(QColor(Qt::darkCyan), tr("Dark Cyan"))
            << qMakePair(QColor(Qt::green), tr("Green"))
            << qMakePair(QColor(Qt::darkGreen), tr("Dark Green"))
            << qMakePair(QColor(Qt::magenta), tr("Magenta"))
            << qMakePair(QColor(Qt::darkMagenta), tr("Dark Magenta"))
            << qMakePair(QColor(Qt::red), tr("Red"))
            << qMakePair(QColor(Qt::darkRed), tr("Dark Red"))) {
        pixmap.fill(pair.first);
        QAction *action = colorMenu->addAction(pixmap, pair.second);
        action->setData(pair.first);
    }
    connect(colorMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(applyColor(QAction*)));
    AQP::accelerateMenu(colorMenu);
    return colorMenu;
}


QSize RichTextLineEdit::sizeHint() const
{
    QFontMetrics fm(font());
    return QSize(document()->idealWidth() + fm.width("W"),
                 fm.height() + 5);
}


QSize RichTextLineEdit::minimumSizeHint() const
{
    QFontMetrics fm(font());
    return QSize(fm.width("WWWW"), fm.height() + 5);
}


void RichTextLineEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter ||
        event->key() == Qt::Key_Return) {
        emit returnPressed();
        event->accept();
    }
    else
        QTextEdit::keyPressEvent(event);
}


void RichTextLineEdit::customContextMenuRequested(const QPoint &pos)
{
    updateContextMenuActions();

    QMenu menu(this);
    menu.addActions(actions());
    menu.exec(mapToGlobal(pos));
}



void RichTextLineEdit::updateContextMenuActions()
{
    boldAction->setChecked(fontWeight() > QFont::Normal);
    italicAction->setChecked(fontItalic());
    const QTextCharFormat &format = currentCharFormat();
    strikeOutAction->setChecked(format.fontStrikeOut());
    noSubOrSuperScriptAction->setChecked(format.verticalAlignment() ==
            QTextCharFormat::AlignNormal);
    superScriptAction->setChecked(format.verticalAlignment() ==
                                  QTextCharFormat::AlignSuperScript);
    subScriptAction->setChecked(format.verticalAlignment() ==
                                QTextCharFormat::AlignSubScript);
}


void RichTextLineEdit::applyTextEffect()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        Style style = static_cast<Style>(action->data().toInt());
        QTextCharFormat format = currentCharFormat();
        switch (style) {
            case Bold: toggleBold(); return;
            case Italic: toggleItalic(); return;
            case StrikeOut:
                format.setFontStrikeOut(!format.fontStrikeOut());
                break;
            case NoSuperOrSubscript:
                format.setVerticalAlignment(
                        QTextCharFormat::AlignNormal);
                break;
            case Superscript:
                format.setVerticalAlignment(
                        QTextCharFormat::AlignSuperScript);
                break;
            case Subscript:
                format.setVerticalAlignment(
                        QTextCharFormat::AlignSubScript);
                break;
        }
        mergeCurrentCharFormat(format);
    }
}


void RichTextLineEdit::applyColor(QAction *action)
{
    Q_ASSERT(action);
    setTextColor(action->data().value<QColor>());
}


QString RichTextLineEdit::toSimpleHtml() const
{
    QString html;
    for (QTextBlock block = document()->begin(); block.isValid();
         block = block.next()) {
        for (QTextBlock::iterator i = block.begin(); !i.atEnd();
             ++i) {
            QTextFragment fragment = i.fragment();
            if (fragment.isValid()) {
                QTextCharFormat format = fragment.charFormat();
                QColor color = format.foreground().color();
                //QString text = Qt::escape(fragment.text()); //deleted for Qt5
                QString text = QString(fragment.text()).toHtmlEscaped(); //added for Qt5

                QStringList tags;
                if (format.verticalAlignment() ==
                    QTextCharFormat::AlignSubScript)
                    tags << "sub";
                else if (format.verticalAlignment() ==
                         QTextCharFormat::AlignSuperScript)
                    tags << "sup";
                if (format.fontItalic())
                    tags << "i";
                if (format.fontWeight() > QFont::Normal)
                    tags << "b";
                if (format.fontStrikeOut())
                    tags << "s";
                while (!tags.isEmpty())
                    text = QString("<%1>%2</%1>")
                                   .arg(tags.takeFirst()).arg(text);
                if (color != QColor(Qt::black))
                    text = QString("<font color=\"%1\">%2</font>")
                                   .arg(color.name()).arg(text);
                html += text;
            }
        }
    }
    return html;
}
