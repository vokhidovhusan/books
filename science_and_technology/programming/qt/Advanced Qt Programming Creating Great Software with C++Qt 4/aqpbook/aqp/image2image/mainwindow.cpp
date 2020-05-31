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

#include "aqp.hpp"
#include "alt_key.hpp"
#ifndef USE_QTCONCURRENT
#include "convertimagetask.hpp"
#endif
#include "mainwindow.hpp"
#include <QApplication>
#include <QCloseEvent>
#include <QCompleter>
#include <QComboBox>
#include <QDirIterator>
#include <QDirModel>
#include <QGridLayout>
#include <QImageReader>
#include <QImageWriter>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QThreadPool>
#include <QTimer>
#ifdef USE_QTCONCURRENT
#include <QtConcurrentRun>
#endif


namespace {

const int PollTimeout = 100;


#ifdef USE_QTCONCURRENT
struct ProgressEvent : public QEvent
{
    enum {EventId = QEvent::User};

    explicit ProgressEvent(bool saved_, const QString &message_)
        : QEvent(static_cast<Type>(EventId)),
          saved(saved_), message(message_) {}

    const bool saved;
    const QString message;
};


void convertImages(QObject *receiver, volatile bool *stopped,
        const QStringList &sourceFiles, const QString &targetType)
{
    foreach (const QString &source, sourceFiles) {
        if (*stopped)
            return;
        QImage image(source);
        QString target(source);
        target.chop(QFileInfo(source).suffix().length());
        target += targetType.toLower();
        if (*stopped)
            return;
        bool saved = image.save(target);

        QString message = saved
                ? QObject::tr("Saved '%1'")
                              .arg(QDir::toNativeSeparators(target))
                : QObject::tr("Failed to convert '%1'")
                              .arg(QDir::toNativeSeparators(source));
        QApplication::postEvent(receiver,
                                new ProgressEvent(saved, message));
    }
}
#endif


#ifdef USE_CUSTOM_DIR_MODEL
// Taken from Qt's completer example
class DirModel : public QDirModel
{
public:
    explicit DirModel(QObject *parent=0) : QDirModel(parent) {}

    QVariant data(const QModelIndex &index,
                  int role=Qt::DisplayRole) const
    {
        if (role == Qt::DisplayRole && index.column() == 0) {
            QString path = QDir::toNativeSeparators(filePath(index));
            if (path.endsWith(QDir::separator()))
                path.chop(1);
            return path;
        }
        return QDirModel::data(index, role);
    }
};
#endif

} // anonymous namespace


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), stopped(true)
{
    createWidgets();
    createLayout();
    createConnections();

    AQP::accelerateWidget(this);
    updateUi();
    directoryEdit->setFocus();
    setWindowTitle(QApplication::applicationName());
}


void MainWindow::createWidgets()
{
    directoryLabel = new QLabel(tr("Path:"));
    QCompleter *directoryCompleter = new QCompleter(this);
#ifndef Q_WS_X11
    directoryCompleter->setCaseSensitivity(Qt::CaseInsensitive);
#endif
#ifdef USE_CUSTOM_DIR_MODEL
    directoryCompleter->setModel(new DirModel(directoryCompleter));
#else
    directoryCompleter->setModel(new QDirModel(directoryCompleter));
#endif
    directoryEdit = new QLineEdit(QDir::toNativeSeparators(
                                  QDir::homePath()));
    directoryEdit->setCompleter(directoryCompleter);
    directoryLabel->setBuddy(directoryEdit);

    sourceTypeLabel = new QLabel(tr("Source type:"));
    sourceTypeComboBox = new QComboBox;
    foreach (const QByteArray &ba,
             QImageReader::supportedImageFormats())
        sourceTypeComboBox->addItem(QString(ba).toUpper());
    sourceTypeComboBox->setCurrentIndex(0);
    sourceTypeLabel->setBuddy(sourceTypeComboBox);

    targetTypeLabel = new QLabel(tr("Target type:"));
    targetTypeComboBox = new QComboBox;
    targetTypeLabel->setBuddy(targetTypeComboBox);
    sourceTypeChanged(sourceTypeComboBox->currentText());

    logEdit = new QPlainTextEdit;
    logEdit->setReadOnly(true);
    logEdit->setPlainText(tr("Choose a path, source type and target "
                             "file type, and click Convert."));

    convertOrCancelButton = new QPushButton(tr("&Convert"));
    quitButton = new QPushButton(tr("Quit"));
}


void MainWindow::createLayout()
{
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(directoryLabel, 0, 0);
    layout->addWidget(directoryEdit, 0, 1, 1, 5);
    layout->addWidget(sourceTypeLabel, 1, 0);
    layout->addWidget(sourceTypeComboBox, 1, 1);
    layout->addWidget(targetTypeLabel, 1, 2);
    layout->addWidget(targetTypeComboBox, 1, 3);
    layout->addWidget(convertOrCancelButton, 1, 4);
    layout->addWidget(quitButton, 1, 5);
    layout->addWidget(logEdit, 2, 0, 1, 6);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
}


void MainWindow::createConnections()
{
    connect(directoryEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(updateUi()));
    connect(sourceTypeComboBox,
            SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(sourceTypeChanged(const QString&)));
    connect(sourceTypeComboBox, SIGNAL(activated(const QString&)),
            this, SLOT(sourceTypeChanged(const QString&)));
    connect(convertOrCancelButton, SIGNAL(clicked()),
            this, SLOT(convertOrCancel()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
}


void MainWindow::updateUi()
{
    if (stopped) {
        convertOrCancelButton->setText(tr("&Convert"));
        convertOrCancelButton->setEnabled(
                !directoryEdit->text().isEmpty());
    }
    else {
        convertOrCancelButton->setText(tr("&Cancel"));
        convertOrCancelButton->setEnabled(true);
    }
}


void MainWindow::sourceTypeChanged(const QString &sourceType)
{
    QStringList targetTypes;
    if (targetTypes.isEmpty()) {
        foreach (const QByteArray &ba,
                 QImageWriter::supportedImageFormats()) {
            const QString targetType = QString(ba).toUpper();
            if (targetType != sourceType)
                targetTypes << targetType.toUpper();
        }
    }
    targetTypes.sort();
    targetTypeComboBox->clear();
    targetTypeComboBox->addItems(targetTypes);
}


void MainWindow::convertOrCancel()
{
    stopped = true;
    if (QThreadPool::globalInstance()->activeThreadCount())
        QThreadPool::globalInstance()->waitForDone();
    if (convertOrCancelButton->text() == tr("&Cancel")) {
        updateUi();
        return;
    }

    QString sourceType = sourceTypeComboBox->currentText();
    QStringList sourceFiles;
    QDirIterator i(directoryEdit->text(), QDir::Files|QDir::Readable);
    while (i.hasNext()) {
        const QString &filenameAndPath = i.next();
        if (i.fileInfo().suffix().toUpper() == sourceType)
            sourceFiles << filenameAndPath;
    }
    if (sourceFiles.isEmpty())
        AQP::warning(this, tr("No Images Error"),
                     tr("No matching files found"));
    else {
        logEdit->clear();
        convertFiles(sourceFiles);
    }
}


void MainWindow::convertFiles(const QStringList &sourceFiles)
{
    stopped = false;
    updateUi();
    total = sourceFiles.count();
    done = 0;
    const QVector<int> sizes = AQP::chunkSizes(sourceFiles.count(),
            QThread::idealThreadCount());

    int offset = 0;
    foreach (const int chunkSize, sizes) {
#ifdef USE_QTCONCURRENT
        QtConcurrent::run(convertImages, this, &stopped,
                sourceFiles.mid(offset, chunkSize),
                targetTypeComboBox->currentText());
#else
        ConvertImageTask *convertImageTask = new ConvertImageTask(
                this, &stopped, sourceFiles.mid(offset, chunkSize),
                targetTypeComboBox->currentText());
        QThreadPool::globalInstance()->start(convertImageTask);
#endif
        offset += chunkSize;
    }
    checkIfDone();
}


void MainWindow::checkIfDone()
{
    if (QThreadPool::globalInstance()->activeThreadCount())
        QTimer::singleShot(PollTimeout, this, SLOT(checkIfDone()));
    else {
        QString message;
        if (done == total)
            message = tr("All %n image(s) converted", "", done);
        else
            message = tr("Converted %n/%1 image(s)", "", done)
                      .arg(total);
        logEdit->appendPlainText(message);
        stopped = true;
        updateUi();
    }
}


void MainWindow::announceProgress(bool saved, const QString &message)
{
    if (stopped)
        return;
    logEdit->appendPlainText(message);
    if (saved)
        ++done;
}


#ifdef USE_QTCONCURRENT
bool MainWindow::event(QEvent *event)
{
    if (!stopped && event->type() ==
            static_cast<QEvent::Type>(ProgressEvent::EventId)) {
        ProgressEvent *progressEvent =
                static_cast<ProgressEvent*>(event);
        Q_ASSERT(progressEvent);
        logEdit->appendPlainText(progressEvent->message);
        if (progressEvent->saved)
            ++done;
        return true;
    }
    return QMainWindow::event(event);
}
#endif


void MainWindow::closeEvent(QCloseEvent *event)
{
    stopped = true;
    if (QThreadPool::globalInstance()->activeThreadCount())
        QThreadPool::globalInstance()->waitForDone();
    event->accept();
}
