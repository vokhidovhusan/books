#ifndef TEXTEDIT_HPP
#define TEXTEDIT_HPP
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
#include <QWidget>


class QColorDialog;
class QDoubleSpinBox;
class QFontComboBox;
class QTextCharFormat;
class QToolBar;


class TextEdit : public QWidget
{
    Q_OBJECT

public:
    explicit TextEdit(QWidget *parent=0);

    QString toHtml() const { return textEdit->toHtml(); }
    QString toPlainText() const { return textEdit->toPlainText(); }
    QTextDocument *document() const { return textEdit->document(); }

signals:
    void textChanged();

public slots:
    void setHtml(const QString &html) { textEdit->setHtml(html); }

private slots:
    void setBold(bool on)
        { textEdit->setFontWeight(on ? QFont::Bold : QFont::Normal); }
    void setColor();
    void updateColor(const QColor &color);
    void setFontPointSize(double points)
        { textEdit->setFontPointSize(static_cast<qreal>(points)); }
    void setFontFamily(const QFont &font)
        { textEdit->setFontFamily(font.family()); }
    void currentCharFormatChanged(const QTextCharFormat &format);
#ifdef SUPPORT_ALIGNMENT
    void alignLeft() { textEdit->setAlignment(Qt::AlignLeft); }
    void alignCenter() { textEdit->setAlignment(Qt::AlignCenter); }
    void alignJustify() { textEdit->setAlignment(Qt::AlignJustify); }
    void alignRight() { textEdit->setAlignment(Qt::AlignRight); }
    void cursorPositionChanged();
#endif

private:
    void createWidgets();
    void createLayout();
    void createConnections();
    void updateColorSwatch();

    QToolBar *fontToolBar;
    QAction *boldAction;
    QAction *italicAction;
    QAction *colorAction;
    QColorDialog *colorDialog;
    QFontComboBox *fontComboBox;
    QDoubleSpinBox *fontSizeSpinBox;
#ifdef SUPPORT_ALIGNMENT
    QToolBar *alignmentToolBar;
    QAction *alignLeftAction;
    QAction *alignCenterAction;
    QAction *alignJustifyAction;
    QAction *alignRightAction;
#endif
    QTextEdit *textEdit;
};

#endif // TEXTEDIT_HPP
