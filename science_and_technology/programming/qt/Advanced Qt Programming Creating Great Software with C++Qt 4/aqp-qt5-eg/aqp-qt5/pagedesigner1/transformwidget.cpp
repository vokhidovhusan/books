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
#include "aqp.hpp"
#include "global.hpp"
#include "transformwidget.hpp"
#include <QDoubleSpinBox>
#include <QFormLayout>


namespace {
const QChar Degree(0xB0);
#ifdef Q_WS_WIN
const QChar HorizontalArrow(0x2194);
const QChar VerticalArrow(0x2195);
#else
const QChar HorizontalArrow(0x21C6);
const QChar VerticalArrow(0x21C5);
#endif
}


TransformWidget::TransformWidget(QWidget *parent)
    : QWidget(parent)
{
    createWidgets();
    createLayout();
    createConnections();
    setFixedSize(minimumSizeHint());
}


void TransformWidget::createWidgets()
{
    angleSpinBox = new QDoubleSpinBox;
    angleSpinBox->setRange(0.0, 360.0);
    angleSpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    angleSpinBox->setDecimals(1);
    angleSpinBox->setSuffix(Degree);
    shearHorizontalSpinBox = new QDoubleSpinBox;
    shearHorizontalSpinBox->setRange(-5.0, 5.0);
    shearHorizontalSpinBox->setSingleStep(0.1);
    shearHorizontalSpinBox->setAlignment(Qt::AlignVCenter|
                                         Qt::AlignRight);
    shearHorizontalSpinBox->setDecimals(1);
    shearHorizontalSpinBox->setSuffix(QString(" %1")
                                      .arg(HorizontalArrow));
    shearVerticalSpinBox = new QDoubleSpinBox;
    shearVerticalSpinBox->setRange(-5.0, 5.0);
    shearVerticalSpinBox->setSingleStep(0.1);
    shearVerticalSpinBox->setAlignment(Qt::AlignVCenter|
                                       Qt::AlignRight);
    shearVerticalSpinBox->setSuffix(QString(" %1")
                                    .arg(VerticalArrow));
    shearVerticalSpinBox->setDecimals(1);
}


void TransformWidget::createLayout()
{
    QFormLayout *layout = new QFormLayout;
    layout->addRow(tr("Angle"), angleSpinBox);
    QHBoxLayout *shearLayout = new QHBoxLayout;
    shearLayout->addWidget(shearHorizontalSpinBox);
    shearLayout->addWidget(shearVerticalSpinBox);
    layout->addRow(tr("Shear"), shearLayout);
    setLayout(layout);
}


void TransformWidget::createConnections()
{
    connect(angleSpinBox, SIGNAL(valueChanged(double)),
            this, SIGNAL(angleChanged(double)));
    connect(shearHorizontalSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(updateShear()));
    connect(shearVerticalSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(updateShear()));
}


void TransformWidget::setAngle(double angle)
{
    if (!qFuzzyCompare(angle, angleSpinBox->value())) {
        blockSignals(true);
        angleSpinBox->setValue(angle);
        blockSignals(false);
    }
}


void TransformWidget::setShear(double shearHorizontal,
                               double shearVertical)
{
    if (!qFuzzyCompare(shearHorizontal,
                       shearHorizontalSpinBox->value()) ||
        !qFuzzyCompare(shearVertical,
                       shearVerticalSpinBox->value())) {
        blockSignals(true);
        shearHorizontalSpinBox->setValue(shearHorizontal);
        shearVerticalSpinBox->setValue(shearVertical);
        blockSignals(false);
    }
}


void TransformWidget::updateShear()
{
    emit shearChanged(shearHorizontalSpinBox->value(),
                      shearVerticalSpinBox->value());
}
