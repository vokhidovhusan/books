#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
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

#include <QMainWindow>


class QCloseEvent;
class QComboBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=0);

private slots:
    void convertOrCancel();
    void updateUi();
    void checkIfDone();
    void announceProgress(bool saved, const QString &message);
    void sourceTypeChanged(const QString &sourceType);

protected:
#ifdef USE_QTCONCURRENT
    bool event(QEvent *event);
#endif
    void closeEvent(QCloseEvent *event);

private:
    void createWidgets();
    void createLayout();
    void createConnections();
    void convertFiles(const QStringList &sourceFiles);

    QLabel *directoryLabel;
    QLineEdit *directoryEdit;
    QLabel *sourceTypeLabel;
    QComboBox *sourceTypeComboBox;
    QLabel *targetTypeLabel;
    QComboBox *targetTypeComboBox;
    QPlainTextEdit *logEdit;
    QPushButton *convertOrCancelButton;
    QPushButton *quitButton;

    int total;
    int done;
    volatile bool stopped;
};

#endif // MAINWINDOW_HPP
