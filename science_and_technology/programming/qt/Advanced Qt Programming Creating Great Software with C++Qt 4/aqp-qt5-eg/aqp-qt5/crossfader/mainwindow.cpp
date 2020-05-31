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
#include "crossfader.hpp"
#include "mainwindow.hpp"
#include "statusbuttonbar.hpp"
#include <QApplication>
#include <QCheckBox>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QImageReader>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStatusBar>
#include <QUrl>
#include <QVBoxLayout>


namespace {
const int StatusTimeout = AQP::MSecPerSecond * 10;
const int StopWait = 100;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), canceled(false)
{
    createWidgets();
    createLayout();
    createConnections();

    AQP::accelerateWidget(this);
    firstButton->setFocus();
    statusBar->showMessage(tr("Ready"), StatusTimeout);
    setWindowTitle(QApplication::applicationName());
}


void MainWindow::createWidgets()
{
    firstButton = new QPushButton(tr("First Image:"));
    firstLabel = new QLabel;
    firstLabel->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
    lastButton = new QPushButton(tr("Last Image:"));
    lastLabel = new QLabel;
    lastLabel->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
    baseNameLabel = new QLabel(tr("Base Name:"));
    baseNameEdit = new QLineEdit(tr("Image-"));
    baseNameLabel->setBuddy(baseNameEdit);
    numberLabel = new QLabel(tr("Number:"));
    numberSpinBox = new QSpinBox;
    numberLabel->setBuddy(numberSpinBox);
    numberSpinBox->setRange(1, 14);
    numberSpinBox->setValue(5);
    numberSpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

    progressWidget = new QWidget;
    QGridLayout *progressLayout = new QGridLayout;
    progressWidget->setLayout(progressLayout);
    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(progressWidget);
    scrollArea->setWidgetResizable(true);

    generateOrCancelButton = new QPushButton(tr("G&enerate"));
    generateOrCancelButton->setEnabled(false);
    generateOrCancelButton->setDefault(true);
    quitButton = new QPushButton(tr("Quit"));
}


void MainWindow::createLayout()
{
    QHBoxLayout *row1Layout = new QHBoxLayout;
    row1Layout->addWidget(firstButton);
    row1Layout->addWidget(firstLabel, 1);
    QHBoxLayout *row2Layout = new QHBoxLayout;
    row2Layout->addWidget(lastButton);
    row2Layout->addWidget(lastLabel, 1);
    QHBoxLayout *row3Layout = new QHBoxLayout;
    row3Layout->addWidget(baseNameLabel);
    row3Layout->addWidget(baseNameEdit, 1);
    row3Layout->addWidget(numberLabel);
    row3Layout->addWidget(numberSpinBox);

    statusBar = new StatusButtonBar;
    statusBar->buttonBox()->addButton(generateOrCancelButton,
            QDialogButtonBox::ActionRole);
    statusBar->buttonBox()->addButton(quitButton,
            QDialogButtonBox::AcceptRole);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(row1Layout);
    layout->addLayout(row2Layout);
    layout->addLayout(row3Layout);
    layout->addWidget(scrollArea, 1);
    layout->addWidget(statusBar);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
}


void MainWindow::createConnections()
{
    connect(firstButton, SIGNAL(clicked()),
            this, SLOT(setFirstImage()));
    connect(lastButton, SIGNAL(clicked()),
            this, SLOT(setLastImage()));
    connect(generateOrCancelButton, SIGNAL(clicked()),
            this, SLOT(generateOrCancelImages()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(quit()));

}


void MainWindow::updateUi()
{
    generateOrCancelButton->setEnabled(
            !(firstLabel->text().isEmpty() ||
              lastLabel->text().isEmpty()));
}


void MainWindow::setFirstImage()
{
    setImageFile(firstLabel, tr("first"));
    lastButton->setFocus();
}


void MainWindow::setLastImage()
{
    setImageFile(lastLabel, tr("last"));
    baseNameEdit->setFocus();
}


void MainWindow::setImageFile(QLabel *targetLabel,
                              const QString &which)
{
//    QString dir(QDesktopServices::storageLocation(
//                QDesktopServices::PicturesLocation)); //deleted for Qt5
    QString dir(QStandardPaths::PicturesLocation); //added for Qt5

    if (which == tr("first") && !firstLabel->text().isEmpty())
        dir = QFileInfo(firstLabel->text()).path();
    else if (which == tr("last")) {
        if (!lastLabel->text().isEmpty())
            dir = QFileInfo(lastLabel->text()).path();
        else if (!firstLabel->text().isEmpty())
            dir = QFileInfo(firstLabel->text()).path();
    }
    QString filename = QFileDialog::getOpenFileName(this,
            tr("Choose the %1 image").arg(which), dir,
            AQP::filenameFilter(tr("Images"),
                    QImageReader::supportedImageFormats()));
    targetLabel->setText(QDir::toNativeSeparators(filename));
    updateUi();
}


void MainWindow::generateOrCancelImages()
{
    if (generateOrCancelButton->text() == tr("G&enerate")) {
        generateOrCancelButton->setEnabled(false);
        statusBar->showMessage(tr("Generating..."));
        canceled = false;
        cleanUp();
        QImage firstImage(firstLabel->text());
        QImage lastImage(lastLabel->text());
        for (int i = 0; i < numberSpinBox->value(); ++i)
            createAndRunACrossFader(i, firstImage, lastImage);
        generateOrCancelButton->setText(tr("Canc&el"));
    }
    else {
        canceled = true;
        cleanUp();
        generateOrCancelButton->setText(tr("G&enerate"));
    }
    updateUi();
}


void MainWindow::createAndRunACrossFader(int number,
        const QImage &firstImage, const QImage &lastImage)
{
    QString filename = QString("%1%2.png").arg(baseNameEdit->text())
                       .arg(number + 1, 2, 10, QChar('0'));
    QLabel *progressLabel = new QLabel(filename);
    progressLabels << progressLabel;
    QProgressBar *progressBar = new QProgressBar;
    progressBar->setRange(0, 100);
    progressBarForFilename[filename] = progressBar;
    QGridLayout *layout = qobject_cast<QGridLayout*>(
            progressWidget->layout());
    Q_ASSERT(layout);
    layout->addWidget(progressLabel, number, 0);
    layout->addWidget(progressBar, number, 1);

    double firstWeight = (number + 1) /
            static_cast<double>(numberSpinBox->value() + 1);
    double secondWeight = 1.0 - firstWeight;
    CrossFader *crossFader = new CrossFader(filename, firstImage,
            firstWeight, lastImage, secondWeight, this);
    crossFaders << crossFader;

    connect(crossFader, SIGNAL(progress(int)),
            progressBar, SLOT(setValue(int)));
    connect(crossFader, SIGNAL(saving(const QString&)),
            this, SLOT(saving(const QString&)));
    connect(crossFader, SIGNAL(saved(bool, const QString&)),
            this, SLOT(saved(bool, const QString&)));
    connect(crossFader, SIGNAL(finished()),
            this, SLOT(finished()));
    crossFader->start();
}


void MainWindow::cleanUp(StopState stopState)
{
#ifdef SLOW_STOP
    foreach (CrossFader *crossFader, crossFaders) { // Naive!
        crossFader->stop();
        crossFader->wait();
        crossFader->deleteLater();
    }
    crossFaders.clear();
#else
    foreach (CrossFader *crossFader, crossFaders)
        crossFader->stop();
    while (crossFaders.count()) {
        QMutableListIterator<QPointer<CrossFader> > i(crossFaders);
        while (i.hasNext()) {
            CrossFader *crossFader = i.next();
            if (crossFader) {
                if (crossFader->wait(StopWait)) {
                    delete crossFader;
                    i.remove();
                }
            }
            else
                i.remove();
        }
    }
#endif
    Q_ASSERT(crossFaders.isEmpty());
    if (stopState == Terminating)
        return;
    foreach (QProgressBar *progressBar, progressBarForFilename)
        if (progressBar)
            progressBar->deleteLater();
    progressBarForFilename.clear();
    foreach (QLabel *progressLabel, progressLabels)
        if (progressLabel)
            progressLabel->deleteLater();
    progressLabels.clear();
}


void MainWindow::saving(const QString &filename)
{
    statusBar->showMessage(tr("Saving '%1'").arg(filename),
                           StatusTimeout);
    if (QProgressBar *progressBar = progressBarForFilename[filename])
        progressBar->setRange(0, 0);
}


void MainWindow::saved(bool saved, const QString &filename)
{
    const QString message = saved ? tr("Saved '%1'")
                                  : tr("Failed to save '%1'");
    statusBar->showMessage(message.arg(filename), StatusTimeout);
    if (QProgressBar *progressBar =
            progressBarForFilename[filename]) {
        progressBar->setRange(0, 1);
        progressBar->setValue(saved ? 1 : 0);
        progressBar->setEnabled(false);
    }
}


void MainWindow::finished()
{
    foreach (CrossFader *crossFader, crossFaders)
        if (crossFader && !crossFader->isFinished())
            return;
    generateOrCancelButton->setText(tr("G&enerate"));
    if (canceled)
        statusBar->showMessage(tr("Canceled"), StatusTimeout);
    else {
        statusBar->showMessage(tr("Finished"));
        if (statusBar->checkBox()->isChecked())
            QDesktopServices::openUrl(QUrl::fromLocalFile(
                    firstLabel->text()));
    }
}


void MainWindow::quit()
{
    cleanUp(Terminating);
    close();
}
