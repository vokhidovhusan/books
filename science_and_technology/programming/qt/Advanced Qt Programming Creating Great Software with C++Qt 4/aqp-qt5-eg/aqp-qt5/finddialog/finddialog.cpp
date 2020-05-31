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
#include "finddialog.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSignalTransition>
#include <QState>
#include <QStateMachine>
#include <QTimer>


namespace {

#ifdef Q_WS_MAC
static const int Duration = 1000;
#else
static const int Duration = 1500;
#endif


QVariant fontInterpolator(const QFont &start, const QFont &end,
                          qreal progress)
{
    qreal startSize = start.pointSizeF();
    qreal endSize = end.pointSizeF();
    qreal newSize = startSize + ((endSize - startSize) *
                                qBound(0.0, progress, 1.0));
    QFont font(start);
    font.setPointSizeF(newSize);
    return font;
}

} // anonymous namespace


FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent), margin(0)
{
    qRegisterAnimationInterpolator<QFont>(fontInterpolator);
    createWidgets();
    createLayout();
    createConnections();

    findTextLineEdit->setFocus();
    AQP::accelerateWidget(this);
    QTimer::singleShot(0, this, SLOT(createStateMachines()));
}


void FindDialog::createWidgets()
{
    findTextLineEdit = new QLineEdit;
    caseSensitiveCheckBox = new QCheckBox(tr("Case Sensitive"));
    caseSensitiveCheckBox->setChecked(true);

    wholeWordsCheckBox = new QCheckBox(tr("Whole Words Only"), this);
    wholeWordsCheckBox->setChecked(false);

    syntaxLabel = new QLabel(tr("Syntax:"), this);

    syntaxComboBox = new QComboBox(this);
    syntaxLabel->setBuddy(syntaxComboBox);
    syntaxComboBox->addItem(tr("Literal"), QRegExp::FixedString);
    syntaxComboBox->addItem(tr("Regex"), QRegExp::RegExp2);
    syntaxComboBox->addItem(tr("Wildcard"), QRegExp::Wildcard);
    syntaxComboBox->setCurrentIndex(0);

    extraWidgets << wholeWordsCheckBox << syntaxLabel
                 << syntaxComboBox;

    foreach (QWidget *widget, extraWidgets) {
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect;
        effect->setOpacity(1.0);
        widget->setGraphicsEffect(effect);
    }

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Close,
                                     Qt::Vertical);
    findButton = buttonBox->addButton(tr("Find"),
            QDialogButtonBox::AcceptRole);
    moreButton = buttonBox->addButton(tr("More"),
            QDialogButtonBox::ActionRole);
    moreButton->setCheckable(true);
}


void FindDialog::createLayout()
{
    QFormLayout *topLeftLayout = new QFormLayout;
    topLeftLayout->addRow(tr("Find:"), findTextLineEdit);
    topLeftLayout->addRow(caseSensitiveCheckBox);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addLayout(topLeftLayout);
    layout->addWidget(buttonBox);
    setLayout(layout);

    int left, top, right, bottom;
    layout->getContentsMargins(&left, &top, &right, &bottom);
    margin = qMax(qMax(left, right), qMax(top, bottom));
}


void FindDialog::createStateMachines()
{
    findStateMachine = new QStateMachine(this);
    createFindStates();
    createFindTransitions();
    findStateMachine->setInitialState(nothingToFindState);
    findStateMachine->start();

    extraStateMachine = new QStateMachine(this);
    createShowExtraWidgetsState();
    createHideExtraWidgetsState();
    createShowExtraWidgetsTransitions();
    createHideExtraWidgetsTransitions();
    extraStateMachine->setInitialState(hideExtraWidgetsState);
    extraStateMachine->start();
}


void FindDialog::createFindStates()
{
    nothingToFindState = new QState(findStateMachine);
    nothingToFindState->assignProperty(findButton, "enabled", false);

    somethingToFindState = new QState(findStateMachine);
    somethingToFindState->assignProperty(findButton, "enabled", true);
}


void FindDialog::createFindTransitions()
{
    nothingToFindState->addTransition(this,
            SIGNAL(findTextIsNonEmpty()), somethingToFindState);
    somethingToFindState->addTransition(this,
            SIGNAL(findTextIsEmpty()), nothingToFindState);
}


void FindDialog::createShowExtraWidgetsState()
{
    QSize size = extraSize();
    size.rheight() += minimumSizeHint().height();
    size.setWidth(qMax(size.width(), minimumSizeHint().width()));

    QList<QRectF> rects;
    int y = sizeHint().height() - margin;
    rects << QRectF(margin, y, wholeWordsCheckBox->sizeHint().width(),
                    wholeWordsCheckBox->sizeHint().height());
    y += wholeWordsCheckBox->sizeHint().height() + margin;
    int height = qMax(syntaxLabel->sizeHint().height(),
                      syntaxComboBox->sizeHint().height());
    int width = syntaxLabel->sizeHint().width();
    rects << QRectF(margin, y, width, height);
    int x = margin + syntaxLabel->sizeHint().width() + margin;
    width = qMin(sizeHint().width(), size.width()) - (x + margin);
    rects << QRectF(x, y, width, height);

    showExtraWidgetsState = new QState(extraStateMachine);
    foreach (QWidget *widget, extraWidgets) {
        showExtraWidgetsState->assignProperty(
                widget, "geometry", rects.takeFirst());
        showExtraWidgetsState->assignProperty(
                widget, "font", font());
        showExtraWidgetsState->assignProperty(
                widget->graphicsEffect(), "opacity", 1.0);
    }
    showExtraWidgetsState->assignProperty(this, "minimumSize", size);
}


QSize FindDialog::extraSize() const
{
    const int Width = syntaxLabel->sizeHint().width() +
            syntaxComboBox->sizeHint().width() + (2 * margin);
    const int Height = wholeWordsCheckBox->sizeHint().height() +
            qMax(syntaxLabel->sizeHint().height(),
                 syntaxComboBox->sizeHint().height()) + (2 * margin);
    return QSize(Width, Height);
}


void FindDialog::createHideExtraWidgetsState()
{
    QRectF rect = QRectF(buttonBox->x() + (moreButton->width() / 2),
            buttonBox->height() - (moreButton->height() / 2), 1, 1);
    QFont smallFont(font());
    smallFont.setPointSizeF(1.0);

    hideExtraWidgetsState = new QState(extraStateMachine);
    foreach (QWidget *widget, extraWidgets) {
        hideExtraWidgetsState->assignProperty(
                widget, "geometry", rect);
        hideExtraWidgetsState->assignProperty(
                widget, "font", smallFont);
        hideExtraWidgetsState->assignProperty(
                widget->graphicsEffect(), "opacity", 0.0);
    }
    hideExtraWidgetsState->assignProperty(
            this, "minimumSize", minimumSizeHint());
}


void FindDialog::createShowExtraWidgetsTransitions()
{
    QSignalTransition *transition =
            hideExtraWidgetsState->addTransition(this,
                    SIGNAL(showExtra()), showExtraWidgetsState);

    createCommonTransitions(transition);

    QPropertyAnimation *animation;
    animation = new QPropertyAnimation(this, "minimumSize");
    animation->setDuration(Duration / 3);
    transition->addAnimation(animation);

    animation = new QPropertyAnimation(this, "size");
    animation->setDuration(Duration / 3);
    QSize size = extraSize();
    size = QSize(qMax(size.width(), width()),
                 sizeHint().height() + size.height());
    animation->setEndValue(size);
    transition->addAnimation(animation);
}


void FindDialog::createCommonTransitions(
        QSignalTransition *transition)
{
    QPropertyAnimation *animation;
    foreach (QWidget *widget, extraWidgets) {
        animation = new QPropertyAnimation(widget, "geometry");
        animation->setDuration(Duration);
        transition->addAnimation(animation);
        animation = new QPropertyAnimation(widget, "font");
        animation->setDuration(Duration);
        transition->addAnimation(animation);
        if (QGraphicsOpacityEffect *effect =
                static_cast<QGraphicsOpacityEffect*>(
                        widget->graphicsEffect())) {
            animation = new QPropertyAnimation(effect, "opacity");
            animation->setDuration(Duration);
            animation->setEasingCurve(QEasingCurve::OutInCirc);
            transition->addAnimation(animation);
        }
    }
}


void FindDialog::createHideExtraWidgetsTransitions()
{
    QSignalTransition *transition =
            showExtraWidgetsState->addTransition(this,
                    SIGNAL(hideExtra()), hideExtraWidgetsState);

    createCommonTransitions(transition);

    QPropertyAnimation *animation = new QPropertyAnimation(this,
                                                           "size");
    animation->setDuration(Duration);
    animation->setEndValue(sizeHint());
    transition->addAnimation(animation);
}


void FindDialog::createConnections()
{
    connect(findTextLineEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(findTextChanged(const QString&)));
    connect(moreButton, SIGNAL(toggled(bool)),
            this, SLOT(showOrHideExtra(bool)));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}


void FindDialog::findTextChanged(const QString &text)
{
    if (text.isEmpty())
        emit findTextIsEmpty();
    else
        emit findTextIsNonEmpty();
}


void FindDialog::showOrHideExtra(bool on)
{
    if (on)
        emit showExtra();
    else
        emit hideExtra();
#ifdef SCREENSHOTS
    takeScreenshot();
#endif
}


#ifdef SCREENSHOTS
void FindDialog::takeScreenshot()
{
    screenshots << QPixmap::grabWidget(this);
    if (screenshots.count() < 33)
        QTimer::singleShot(50, this, SLOT(takeScreenshot()));
}


void FindDialog::reject()
{
    for (int i = 0; i < screenshots.count(); ++i)
        screenshots.at(i).save(QString("finddialog%1.png")
                               .arg(i + 1, 2, 10, QChar('0')));
    QDialog::reject();
}
#endif
