#ifndef FINDDIALOG_HPP
#define FINDDIALOG_HPP
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


#include <QDialog>


class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QGraphicsOpacityEffect;
class QLabel;
class QLineEdit;
class QPushButton;
class QSignalTransition;
class QState;
class QStateMachine;


class FindDialog : public QDialog
{
    Q_OBJECT

public:
    FindDialog(QWidget *parent=0);

signals:
    void findTextIsEmpty();
    void findTextIsNonEmpty();
    void showExtra();
    void hideExtra();

private slots:
    void createStateMachines();
    void findTextChanged(const QString &text);
    void showOrHideExtra(bool on);
#ifdef SCREENSHOTS
    void takeScreenshot();
    void reject();
#endif

private:
    void createWidgets();
    void createLayout();
    void createFindStates();
    void createShowExtraWidgetsState();
    void createHideExtraWidgetsState();
    void createFindTransitions();
    void createShowExtraWidgetsTransitions();
    void createHideExtraWidgetsTransitions();
    void createCommonTransitions(QSignalTransition *transition);
    void createConnections();
    QSize extraSize() const;

    QLineEdit *findTextLineEdit;
    QCheckBox *caseSensitiveCheckBox;
    QPushButton *findButton;
    QPushButton *moreButton;
    QDialogButtonBox *buttonBox;

    QCheckBox *wholeWordsCheckBox;
    QLabel *syntaxLabel;
    QComboBox *syntaxComboBox;
    QList<QWidget*> extraWidgets;

    QStateMachine *findStateMachine;
    QState *nothingToFindState;
    QState *somethingToFindState;

    QStateMachine *extraStateMachine;
    QState *showExtraWidgetsState;
    QState *hideExtraWidgetsState;

    int margin;
#ifdef SCREENSHOTS
    QList<QPixmap> screenshots;
#endif
};

#endif // FINDDIALOG_HPP
