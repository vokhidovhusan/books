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

#include <QLabel>
#include <QList>
#include <QMap>
#include <QMainWindow>
#include <QPointer>
#include <QProgressBar>


class CrossFader;
class StatusButtonBar;
class QLineEdit;
class QPushButton;
class QScrollArea;
class QSpinBox;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=0);

private slots:
    void setFirstImage();
    void setLastImage();
    void generateOrCancelImages();
    void saving(const QString &filename);
    void saved(bool saved, const QString &filename);
    void finished();
    void updateUi();
    void quit();

private:
    enum StopState {Stopping, Terminating};

    void createWidgets();
    void createLayout();
    void createConnections();
    void setImageFile(QLabel *targetLabel, const QString &which);
    void createAndRunACrossFader(int number, const QImage &firstImage,
                                 const QImage &lastImage);
    void cleanUp(StopState stopState=Stopping);

    QPushButton *firstButton;
    QLabel *firstLabel;
    QLabel *lastLabel;
    QLabel *baseNameLabel;
    QLineEdit *baseNameEdit;
    QLabel *numberLabel;
    QSpinBox *numberSpinBox;
    QPushButton *lastButton;
    QPushButton *generateOrCancelButton;
    QPushButton *quitButton;
    StatusButtonBar *statusBar;
    QScrollArea *scrollArea;
    QWidget *progressWidget;

    QMap<QString, QPointer<QProgressBar> > progressBarForFilename;
    QList<QPointer<QLabel> > progressLabels;
    QList<QPointer<CrossFader> > crossFaders;
    bool canceled;
};

#endif // MAINWINDOW_HPP
