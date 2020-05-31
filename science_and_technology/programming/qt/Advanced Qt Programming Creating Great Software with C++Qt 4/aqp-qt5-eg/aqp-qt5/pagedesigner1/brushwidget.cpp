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
#include "swatch.hpp"
#include "brushwidget.hpp"
#include <QComboBox>
#include <QFormLayout>


BrushWidget::BrushWidget(QWidget *parent)
    : QWidget(parent)
{
    createWidgets();
    setBrush(QBrush());
    createLayout();
    createConnections();
    setFixedSize(minimumSizeHint());
}


void BrushWidget::createWidgets()
{
    colorComboBox = new QComboBox;
    foreach (const QString &name, QColor::colorNames()) {
        QColor color(name);
        colorComboBox->addItem(colorSwatch(color), name, color);
    }

    styleComboBox = new QComboBox;
    typedef QPair<QString, Qt::BrushStyle> BrushPair;
    foreach (const BrushPair &pair, QList<BrushPair>()
            << qMakePair(tr("No Brush"), Qt::NoBrush)
            << qMakePair(tr("Solid"), Qt::SolidPattern)
            << qMakePair(tr("Dense #1"), Qt::Dense1Pattern)
            << qMakePair(tr("Dense #2"), Qt::Dense2Pattern)
            << qMakePair(tr("Dense #3"), Qt::Dense3Pattern)
            << qMakePair(tr("Dense #4"), Qt::Dense4Pattern)
            << qMakePair(tr("Dense #5"), Qt::Dense5Pattern)
            << qMakePair(tr("Dense #6"), Qt::Dense6Pattern)
            << qMakePair(tr("Horizontal"), Qt::HorPattern)
            << qMakePair(tr("Vertical"), Qt::VerPattern)
            << qMakePair(tr("Cross"), Qt::CrossPattern)
            << qMakePair(tr("Diagonal /"), Qt::BDiagPattern)
            << qMakePair(tr("Diagonal \\"), Qt::FDiagPattern)
            << qMakePair(tr("Diagonal Cross"), Qt::DiagCrossPattern))
        styleComboBox->addItem(brushSwatch(pair.second), pair.first,
                               pair.second);
}


void BrushWidget::createLayout()
{
    QFormLayout *layout = new QFormLayout;
    layout->addRow(tr("Color"), colorComboBox);
    layout->addRow(tr("Style"), styleComboBox);
    setLayout(layout);
}


void BrushWidget::createConnections()
{
    connect(colorComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateColor(int)));
    connect(styleComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateStyle(int)));
}


void BrushWidget::setBrush(const QBrush &brush)
{
    if (m_brush != brush) {
        m_brush = brush;
        colorComboBox->setCurrentIndex(
                colorComboBox->findData(m_brush.color()));
        styleComboBox->setCurrentIndex(
                styleComboBox->findData(
                        static_cast<int>(m_brush.style())));
    }
}


void BrushWidget::updateColor(int index)
{
    m_brush.setColor(colorComboBox->itemData(index).value<QColor>());
    updateSwatches();
    emit brushChanged(m_brush);
}


void BrushWidget::updateStyle(int index)
{
    m_brush.setStyle(static_cast<Qt::BrushStyle>(
                     styleComboBox->itemData(index).toInt()));
    emit brushChanged(m_brush);
}


void BrushWidget::updateSwatches()
{
    QColor color = colorComboBox->itemData(
            colorComboBox->currentIndex()).value<QColor>();
    for (int i = 0; i < styleComboBox->count(); ++i)
        styleComboBox->setItemIcon(i, brushSwatch(
                static_cast<Qt::BrushStyle>(
                    styleComboBox->itemData(i).toInt()), color));
}
