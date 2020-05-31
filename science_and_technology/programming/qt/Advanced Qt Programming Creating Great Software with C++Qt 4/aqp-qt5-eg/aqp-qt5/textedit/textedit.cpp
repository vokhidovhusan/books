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

#include "swatch.hpp"
#include "textedit.hpp"
#include <QAction>
#include <QApplication>
#include <QColorDialog>
#include <QDoubleSpinBox>
#include <QFontComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextCharFormat>
#include <QToolBar>
#include <QVBoxLayout>


TextEdit::TextEdit(QWidget *parent)
    : QWidget(parent), colorDialog(0)
{
    createWidgets();
    createLayout();
    createConnections();
    updateColorSwatch();
    textEdit->setFocus();
}


void TextEdit::createWidgets()
{
    QFont font("Helvetica", 11);
    textEdit = new QTextEdit;
    textEdit->setAcceptRichText(true);
    textEdit->setTabChangesFocus(true);
    textEdit->setCurrentFont(font);

    fontToolBar = new QToolBar(tr("Font"), this);
    boldAction = fontToolBar->addAction(QIcon(":/bold.png"),
                                        tr("Bold"));
    boldAction->setCheckable(true);
    boldAction->setShortcut(tr("Ctrl+B"));
    italicAction = fontToolBar->addAction(QIcon(":/italic.png"),
                                          tr("Italic"));
    italicAction->setCheckable(true);
    italicAction->setShortcut(tr("Ctrl+I"));
    colorAction = fontToolBar->addAction(QIcon(":/color.png"),
                                         tr("Color"));
    fontToolBar->addWidget(new QLabel("Font:"));
    fontComboBox = new QFontComboBox;
    fontComboBox->setCurrentFont(font);
    fontToolBar->addWidget(fontComboBox);
    fontSizeSpinBox = new QDoubleSpinBox;
    fontSizeSpinBox->setDecimals(0);
    fontSizeSpinBox->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    fontSizeSpinBox->setRange(6, 280);
    fontSizeSpinBox->setValue(font.pointSize());
    fontToolBar->addWidget(new QLabel("Size:"));
    fontToolBar->addWidget(fontSizeSpinBox);

#ifdef SUPPORT_ALIGNMENT
    alignmentToolBar = new QToolBar(tr("Alignment"), this);
    alignLeftAction = alignmentToolBar->addAction(
            QIcon(":/align-text-left.png"), tr("Left"));
    alignLeftAction->setShortcut(tr("Ctrl+L"));
    alignCenterAction = alignmentToolBar->addAction(
            QIcon(":/align-text-center.png"), tr("Center"));
    alignCenterAction->setShortcut(tr("Ctrl+E"));
    alignJustifyAction = alignmentToolBar->addAction(
            QIcon(":/align-text-justify.png"), tr("Justify"));
    alignRightAction = alignmentToolBar->addAction(
            QIcon(":/align-text-right.png"), tr("Right"));
    alignRightAction->setShortcut(tr("Ctrl+R"));
    QActionGroup *group = new QActionGroup(this);
    foreach (QAction *action, QList<QAction*>() << alignLeftAction
            << alignCenterAction << alignJustifyAction
            << alignRightAction) {
        action->setCheckable(true);
        group->addAction(action);
    }
    alignLeftAction->setChecked(true);
#endif
}


void TextEdit::createLayout()
{
    QHBoxLayout *toolbarLayout = new QHBoxLayout;
    toolbarLayout->setContentsMargins(2, 2, 2, 2);
    toolbarLayout->addWidget(fontToolBar);
#ifdef SUPPORT_ALIGNMENT
    toolbarLayout->addWidget(alignmentToolBar);
#endif
    QFrame *frame = new QFrame;
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);
    frame->setLayout(toolbarLayout);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(3);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(frame);
    layout->addWidget(textEdit);
    setLayout(layout);
}


void TextEdit::createConnections()
{
    connect(fontComboBox, SIGNAL(currentFontChanged(const QFont&)),
            this, SLOT(setFontFamily(const QFont&)));
    connect(fontSizeSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(setFontPointSize(double)));
    connect(boldAction, SIGNAL(toggled(bool)),
            this, SLOT(setBold(bool)));
    connect(italicAction, SIGNAL(toggled(bool)),
            textEdit, SLOT(setFontItalic(bool)));
    connect(colorAction, SIGNAL(triggered()), this, SLOT(setColor()));
#ifdef SUPPORT_ALIGNMENT
    connect(alignLeftAction, SIGNAL(triggered()),
            this, SLOT(alignLeft()));
    connect(alignCenterAction, SIGNAL(triggered()),
            this, SLOT(alignCenter()));
    connect(alignJustifyAction, SIGNAL(triggered()),
            this, SLOT(alignJustify()));
    connect(alignRightAction, SIGNAL(triggered()),
            this, SLOT(alignRight()));
#endif
    connect(textEdit, SIGNAL(currentCharFormatChanged(
                             const QTextCharFormat&)),
            this, SLOT(currentCharFormatChanged(
                       const QTextCharFormat&)));
#ifdef SUPPORT_ALIGNMENT
    connect(textEdit, SIGNAL(cursorPositionChanged()),
            this, SLOT(cursorPositionChanged()));
#endif
    connect(textEdit, SIGNAL(textChanged()),
            this, SIGNAL(textChanged()));
}


void TextEdit::currentCharFormatChanged(
        const QTextCharFormat &format)
{
    fontComboBox->setCurrentFont(format.font());
    fontSizeSpinBox->setValue(format.fontPointSize());
    boldAction->setChecked(format.fontWeight() == QFont::Bold);
    italicAction->setChecked(format.fontItalic());
    updateColorSwatch();
}


#ifdef SUPPORT_ALIGNMENT
void TextEdit::cursorPositionChanged()
{
    QTextCursor cursor = textEdit->textCursor();
    QTextBlockFormat format = cursor.blockFormat();
    switch (format.alignment()) {
        case Qt::AlignLeft:
            alignLeftAction->setChecked(true); break;
        case Qt::AlignCenter:
            alignCenterAction->setChecked(true); break;
        case Qt::AlignJustify:
            alignJustifyAction->setChecked(true); break;
        case Qt::AlignRight:
            alignRightAction->setChecked(true); break;
    }
}
#endif


#if QT_VERSION >= 0x040600 && defined(Q_WS_MAC)
void TextEdit::setColor()
{
    QColor color = QColorDialog::getColor(textEdit->textColor(),
            this, tr("%1 - Choose Color")
                  .arg(qApp->applicationName()));
    if (color.isValid())
        updateColor(color);
}

#else

void TextEdit::setColor()
{
    if (!colorDialog) {
        colorDialog = new QColorDialog(this);
        connect(colorDialog, SIGNAL(colorSelected(const QColor&)),
                this, SLOT(updateColor(const QColor&)));
    }
    colorDialog->setCurrentColor(textEdit->textColor());
    colorDialog->open();
}
#endif


void TextEdit::updateColor(const QColor &color)
{
    textEdit->setTextColor(color);
    updateColorSwatch();
}


void TextEdit::updateColorSwatch()
{
    colorAction->setIcon(colorSwatch(textEdit->textColor(),
                                     QSize(48, 48)));
}
