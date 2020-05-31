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
#include "getmd5sthread.hpp"
#include "mainwindow.hpp"
#include <QApplication>
#include <QCloseEvent>
#include <QCompleter>
#include <QDirIterator>
#include <QDirModel>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QTreeView>
#include <QVBoxLayout>


namespace {

const int StatusTimeout = AQP::MSecPerSecond * 5;
const int StopWait = 100;


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
    : QMainWindow(parent), stopped(false)
{
    createWidgets();

    model = new QStandardItemModel;
    view->setModel(model);

    createLayout();
    createConnections();

    AQP::accelerateWidget(this);
    rootDirectoryEdit->setFocus();
    statusBar()->showMessage(tr("Set a root directory "
                "then press Find"), StatusTimeout);
    setWindowTitle(QApplication::applicationName());
}


void MainWindow::createWidgets()
{
    rootDirectoryLabel = new QLabel(tr("Root Directory:"));
    rootDirectoryEdit = new QLineEdit(QDir::toNativeSeparators(
                                      QDir::homePath()));
    rootDirectoryLabel->setBuddy(rootDirectoryEdit);
    QCompleter *directoryCompleter = new QCompleter(this);
#ifndef Q_WS_X11
    directoryCompleter->setCaseSensitivity(Qt::CaseInsensitive);
#endif
#ifdef USE_CUSTOM_DIR_MODEL
    directoryCompleter->setModel(new DirModel(directoryCompleter));
#else
    directoryCompleter->setModel(new QDirModel(directoryCompleter));
#endif
    rootDirectoryEdit->setCompleter(directoryCompleter);

    findButton = new QPushButton(tr("Find"));
    cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->hide();
    quitButton = new QPushButton(tr("Quit"));

    view = new QTreeView;
}


void MainWindow::createLayout()
{
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(rootDirectoryLabel);
    topLayout->addWidget(rootDirectoryEdit, 1);
    topLayout->addWidget(findButton);
    topLayout->addWidget(cancelButton);
    topLayout->addWidget(quitButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(topLayout);
    layout->addWidget(view, 1);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
}


void MainWindow::createConnections()
{
    connect(rootDirectoryEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(updateUi()));
    connect(findButton, SIGNAL(clicked()), this, SLOT(find()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
}


void MainWindow::updateUi()
{
    const QString &root = rootDirectoryEdit->text();
    findButton->setEnabled(!root.isEmpty() &&
                             QFileInfo(root).isDir());
}


void MainWindow::find()
{
    stopThreads();

    rootDirectoryEdit->setEnabled(false);
    view->setSortingEnabled(false);
    model->clear();
    model->setColumnCount(2);
    model->setHorizontalHeaderLabels(QStringList() << tr("File")
                                                   << tr("Size"));
    findButton->hide();
    cancelButton->show();
    cancelButton->setEnabled(true);
    cancelButton->setFocus();

    stopped = false;
    prepareToProcess();
}


void MainWindow::prepareToProcess()
{
    statusBar()->showMessage(tr("Reading files..."));
    QStringList directories;
    directories << rootDirectoryEdit->text();
    QDirIterator i(directories.first());
    while (!stopped && i.hasNext()) {
        const QString &pathAndFilename = i.next();
        const QFileInfo &info = i.fileInfo();
        if (info.isDir() && !info.isSymLink() &&
            i.fileName() != "." && i.fileName() != "..")
            directories << pathAndFilename;
    }
    if (stopped)
        return;
    processDirectories(directories);
}


void MainWindow::processDirectories(const QStringList &directories)
{
    const QVector<int> sizes = AQP::chunkSizes(directories.count(),
            QThread::idealThreadCount());
    int offset = 0;
    foreach (const int chunkSize, sizes) {
        QPointer<GetMD5sThread> thread = QPointer<GetMD5sThread>(
                new GetMD5sThread(&stopped, directories.first(),
                        directories.mid(offset, chunkSize),
                        &filesForMD5));
        threads << thread;
        connect(thread, SIGNAL(readOneFile()),
                this, SLOT(readOneFile()));
        connect(thread, SIGNAL(finished()), this, SLOT(finished()));
        thread->start();
        offset += chunkSize;
    }
}


void MainWindow::readOneFile()
{
    statusBar()->showMessage(tr("Read %Ln file(s)", "",
                                filesForMD5.count()));
}


void MainWindow::processResults()
{
    stopThreads();

    qint64 maximumSize;
    forever {
        bool more;
        QStringList files = filesForMD5.takeOne(&more);
        if (!more)
            break;
        if (files.count() < 2)
            continue;
        addOneResult(files, &maximumSize);
    }
    updateView(maximumSize);
    statusBar()->showMessage(tr("Found %Ln duplicate file(s)", "",
                             model->rowCount()));
    completed();
}


void MainWindow::addOneResult(const QStringList &files,
                              qint64 *maximumSize)
{
    QFileInfo info(files.first());
    if (info.size() > *maximumSize)
        *maximumSize = info.size();
    QStandardItem *parentItem = model->invisibleRootItem();
    QStandardItem *treeItem = new QStandardItem(info.fileName());
    QStandardItem *sizeItem = new QStandardItem(
            QString("%L1").arg(info.size(), 20, 10, QChar(' ')));
    sizeItem->setTextAlignment(Qt::AlignVCenter|Qt::AlignRight);
    parentItem->appendRow(QList<QStandardItem*>() << treeItem
                                                  << sizeItem);
    foreach (const QString &filename, files)
        treeItem->appendRow(new QStandardItem(
                QDir::toNativeSeparators(filename)));
}


void MainWindow::updateView(qint64 maximumSize)
{
    if (model->rowCount()) {
        model->sort(0, Qt::AscendingOrder);
        view->expand(model->invisibleRootItem()->child(0)->index());
        QFontMetrics fm(font());
        int sizeWidth = fm.width(QString("W%L1W").arg(maximumSize));
        view->setColumnWidth(1, sizeWidth);
        sizeWidth += fm.width("W");
        view->setColumnWidth(0, view->width() - (sizeWidth +
                view->verticalScrollBar()->sizeHint().width()));
    }
}


void MainWindow::finished()
{
    foreach (QPointer<GetMD5sThread> thread, threads)
        if (thread && thread->isRunning())
            return;
    processResults();
}


void MainWindow::completed()
{
    view->setSortingEnabled(true);
    cancelButton->setEnabled(false);
    cancelButton->hide();
    findButton->show();
    findButton->setEnabled(true);
    findButton->setFocus();
    rootDirectoryEdit->setEnabled(true);
}


void MainWindow::stopThreads()
{
    stopped = true;
    while (threads.count()) {
        QMutableListIterator<QPointer<GetMD5sThread> > i(threads);
        while (i.hasNext()) {
            QPointer<GetMD5sThread> thread = i.next();
            if (thread) {
                if (thread->wait(StopWait)) {
                    delete thread;
                    i.remove();
                }
            }
            else
                i.remove();
        }
    }
    Q_ASSERT(threads.isEmpty());
}


void MainWindow::cancel()
{
    stopThreads();
    completed();
    statusBar()->showMessage(tr("Canceled"), StatusTimeout);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    stopThreads();
    event->accept();
}
