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
#include "scriptform.hpp"
#include "mainwindow.hpp"
#include "newgridform.hpp"
#include "spinbox.hpp"
#include "standarditem.hpp"
#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QItemEditorFactory>
#include <QMenuBar>
#include <QProgressBar>
#include <QScriptEngine>
#include <QScriptValue>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QTableView>
#include <QtConcurrentMap>
#include <QtConcurrentFilter>
#include <QTextStream>
#include <QToolBar>
#include <cstdio> // for snprintf()
#ifdef Q_CC_MSVC
#define snprintf _snprintf
#endif


namespace {

const int StatusTimeout = AQP::MSecPerSecond * 30;
const QString Separator("*");

inline double randomValue()
    { return (qrand() % 200000) - 10000 + ((qrand() % 1000) / 1000.0); }


QString stringForLongDouble(const long double &x)
{
    const int BUFFER_SIZE = 20;
    char longDouble[BUFFER_SIZE + 1];
    int i = snprintf(longDouble, BUFFER_SIZE, "%.3Lf", x);
    if (i < 0 || i >= BUFFER_SIZE) // Error or truncation
        return QString("#####");
    return QString(longDouble);
}


class SurrogateItemMatcher
{
public:
    explicit SurrogateItemMatcher(MatchCriteria matchCriteria_)
        : matchCriteria(matchCriteria_) {}

    typedef bool result_type;

    bool operator()(const SurrogateItem &item)
    {
        switch (matchCriteria.comparisonType) {
            case LessThan:
                return item.value < matchCriteria.value;
            case LessThanOrEqual:
                return item.value <= matchCriteria.value;
            case GreaterThanOrEqual:
                return item.value >= matchCriteria.value;
            case GreaterThan:
                return item.value > matchCriteria.value;
            case ApproximatelyEqual:
                return qFuzzyCompare(item.value, matchCriteria.value);
        }
        Q_ASSERT(false);
        return false;
    }

private:
    MatchCriteria matchCriteria;
};


void itemAccumulator(Results &results, const SurrogateItem &item)
{
    ++results.count;
    results.sum += item.value;
}


class SurrogateItemApplier
{
public:
    explicit SurrogateItemApplier(const QString &script_,
            ThreadSafeErrorInfo *errorInfo_)
        : script(script_), errorInfo(errorInfo_) {}

    typedef SurrogateItem result_type;

    SurrogateItem operator()(const SurrogateItem &item)
    {
        QScriptEngine javaScriptParser;
#if QT_VERSION >= 0x040500
        javaScriptParser.globalObject().setProperty("cellRow",
                                                    item.row);
        javaScriptParser.globalObject().setProperty("cellColumn",
                                                    item.column);
        javaScriptParser.globalObject().setProperty("cellValue",
                                                    item.value);
#else
        QScriptValue cellRowValue(&javaScriptParser, item.row);
        javaScriptParser.globalObject().setProperty("cellRow",
                                                    cellRowValue);
        QScriptValue cellColumnValue(&javaScriptParser, item.column);
        javaScriptParser.globalObject().setProperty("cellColumn",
                                                    cellColumnValue);
        QScriptValue cellValue(&javaScriptParser, item.value);
        javaScriptParser.globalObject().setProperty("cellValue",
                                                    cellValue);
#endif
        QScriptValue result = javaScriptParser.evaluate(script);
        if (javaScriptParser.hasUncaughtException()) {
            QString error = javaScriptParser.uncaughtException()
                            .toString();
            errorInfo->add(error);
            return item;
        }
        return SurrogateItem(item.row, item.column,
                             result.toNumber());
    }

private:
    QString script;
    ThreadSafeErrorInfo *errorInfo;
};

} // anonymous namespace


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), applyToAll(true), cacheIsDirty(true)
{
    progressBar = new QProgressBar(this);
    progressBar->hide();
    statusBar()->addWidget(progressBar);

    createModelAndView();
    setCentralWidget(view);

    createActions();
    createMenusAndToolBar();
    createConnections();

    setWindowTitle(QString("%1[*]")
                   .arg(QApplication::applicationName()));
    statusBar()->showMessage(tr("Ready"), StatusTimeout);
    updateUi();
}


void MainWindow::createModelAndView()
{
    QItemEditorFactory *editorFactory = new QItemEditorFactory;
    QItemEditorCreatorBase *numberEditorCreator = new
            QStandardItemEditorCreator<SpinBox>();
    editorFactory->registerEditor(QVariant::Double,
                                  numberEditorCreator);
    editorFactory->registerEditor(QVariant::Int,
                                  numberEditorCreator);
    QItemEditorFactory::setDefaultFactory(editorFactory);

    view = new QTableView;
    editTriggers = view->editTriggers();
    model = new QStandardItemModel;
    view->setModel(model);
}


void MainWindow::createActions()
{
    fileNewAction = new QAction(QIcon(":/filenew.png"),
            tr("New..."), this);
    fileNewAction->setShortcuts(QKeySequence::New);
    fileOpenAction = new QAction(QIcon(":/fileopen.png"),
            tr("Open..."), this);
    fileOpenAction->setShortcuts(QKeySequence::Open);
    fileSaveAction = new QAction(QIcon(":/filesave.png"),
            tr("Save"), this);
    fileSaveAction->setShortcuts(QKeySequence::Save);
    fileSaveAsAction = new QAction(QIcon(":/filesave.png"),
            tr("Save As..."), this);
#if QT_VERSION >= 0x040500
    fileSaveAsAction->setShortcuts(QKeySequence::SaveAs);
#endif
    fileQuitAction = new QAction(QIcon(":/filequit.png"),
            tr("Quit"), this);
#if QT_VERSION >= 0x040600
    fileQuitAction->setShortcuts(QKeySequence::Quit);
#else
    fileQuitAction->setShortcut(QKeySequence("Ctrl+Q"));
#endif
    editCountAction = new QAction(QIcon(":/editcount.png"),
            tr("Count..."), this);
    editSelectAction = new QAction(QIcon(":/editselect.png"),
            tr("Select..."), this);
    editApplyScriptAction = new QAction(
            QIcon(":/editapplyscript.png"), tr("Apply Script..."),
            this);
    editStopAction = new QAction(QIcon(":/editstop.png"),
            tr("Stop"), this);
    editStopAction->setEnabled(false);
}


void MainWindow::createMenusAndToolBar()
{
    setUnifiedTitleAndToolBarOnMac(true);

    QMenu *fileMenu = menuBar()->addMenu(tr("File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
#ifdef Q_WS_MAC
    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
#endif
    foreach (QAction *action, QList<QAction*>()
            << fileNewAction << fileOpenAction << fileSaveAction
            << fileSaveAsAction) {
        fileMenu->addAction(action);
        if (action != fileSaveAsAction)
            fileToolBar->addAction(action);
    }
    fileMenu->addSeparator();
    fileMenu->addAction(fileQuitAction);

    QMenu *editMenu = menuBar()->addMenu(tr("Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));
#ifdef Q_WS_MAC
    editToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
#endif
    foreach (QAction *action, QList<QAction*>()
            << editCountAction << editSelectAction
            << editApplyScriptAction << editStopAction) {
        editMenu->addAction(action);
        editToolBar->addAction(action);
    }
    AQP::accelerateMenu(menuBar());
}


void MainWindow::createConnections()
{
    connect(fileNewAction, SIGNAL(triggered()),
            this, SLOT(fileNew()));
    connect(fileOpenAction, SIGNAL(triggered()),
            this, SLOT(fileOpen()));
    connect(fileSaveAction, SIGNAL(triggered()),
            this, SLOT(fileSave()));
    connect(fileSaveAsAction, SIGNAL(triggered()),
            this, SLOT(fileSaveAs()));
    connect(fileQuitAction, SIGNAL(triggered()),
            this, SLOT(close()));
    connect(editCountAction, SIGNAL(triggered()),
            this, SLOT(editCount()));
    connect(editSelectAction, SIGNAL(triggered()),
            this, SLOT(editSelect()));
    connect(editApplyScriptAction, SIGNAL(triggered()),
            this, SLOT(editApplyScript()));

    connect(model,
        SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SLOT(setDirty()));

    connect(editStopAction, SIGNAL(triggered()),
            &selectWatcher, SLOT(cancel()));
    connect(editStopAction, SIGNAL(triggered()),
            &countWatcher, SLOT(cancel()));
    connect(editStopAction, SIGNAL(triggered()),
            &applyScriptWatcher, SLOT(cancel()));

    connect(&selectWatcher, SIGNAL(finished()),
            this, SLOT(finishedSelecting()));
    connect(&countWatcher, SIGNAL(finished()),
            this, SLOT(finishedCounting()));
    connect(&applyScriptWatcher, SIGNAL(finished()),
            this, SLOT(finishedApplyingScript()));
}


void MainWindow::setDirty(bool on)
{
    setWindowModified(on);
    cacheIsDirty = true;
    updateUi();
}


void MainWindow::updateUi()
{
    fileSaveAction->setEnabled(isWindowModified());
    fileSaveAsAction->setEnabled(model->rowCount());
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (okToClearData())
        event->accept();
    else
        event->ignore();
}


void MainWindow::fileNew()
{
    if (!okToClearData())
        return;
    NewGridForm newGridForm(this);
    if (!newGridForm.exec())
        return;
    GridSpecification gridSpecification = newGridForm.result();
    filename.clear();
    model->clear();
    model->setRowCount(gridSpecification.rows);
    model->setColumnCount(gridSpecification.columns);
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int column = 0; column < model->columnCount();
             ++column) {
            double value = gridSpecification.randomInitialValue ?
                    randomValue(): gridSpecification.initialValue;
            StandardItem *item = new StandardItem(value);
            model->setItem(row, column, item);
        }
    }
    view->resizeColumnsToContents();
    statusBar()->showMessage(tr("Created a %1 %2 %3 grid")
            .arg(gridSpecification.rows).arg(QChar(0xD7)) // # x
            .arg(gridSpecification.columns), StatusTimeout);
    setWindowTitle(tr("%1 - Untitled[*]")
                   .arg(QApplication::applicationName()));
    setDirty(true);
}


bool MainWindow::okToClearData()
{
    if (isWindowModified())
        return AQP::okToClearData(&MainWindow::fileSave, this,
                tr("Unsaved changes"), tr("Save unsaved changes?"));
    return true;
}


void MainWindow::fileOpen()
{
    if (!okToClearData())
        return;
    QString name = QFileDialog::getOpenFileName(this,
            tr("%1 - Open").arg(QApplication::applicationName()));
    if (name.isEmpty())
        return;
    filename = name;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        AQP::warning(this, tr("Error"), tr("Failed to open file: %1")
                                        .arg(file.errorString()));
        return;
    }
    QRegExp separator(QString("[%1,\\t]|\\s+").arg(Separator));
    model->clear();
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QStandardItem*> items;
        foreach (const QString &text, line.split(separator))
            items << new StandardItem(text.toDouble());
        model->appendRow(items);
    }
    view->resizeColumnsToContents();
    setWindowTitle(tr("%1 - %2[*]")
            .arg(QApplication::applicationName()).arg(filename));
    statusBar()->showMessage(tr("Loaded %1").arg(filename),
                             StatusTimeout);
    setDirty(false);
}


bool MainWindow::fileSave()
{
    if (filename.isEmpty())
        return fileSaveAs();
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
        return false;
    QTextStream out(&file);
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int column = 0; column < model->columnCount();
             ++column) {
            out << model->item(row, column)->text();
            if (column + 1 < model->columnCount())
                out << Separator;
        }
        out << "\n";
    }
    file.close();
    setWindowTitle(tr("%1 - %2[*]")
            .arg(QApplication::applicationName()).arg(filename));
    setDirty(false);
    return true;
}


bool MainWindow::fileSaveAs()
{
    filename = QFileDialog::getSaveFileName(this,
            tr("%1 - Save As").arg(QApplication::applicationName()));
    if (filename.isEmpty())
        return false;
    return fileSave();
}


void MainWindow::editSelect()
{
    MatchForm matchForm(MatchForm::Select, this);
    if (matchForm.exec()) {
        MatchCriteria matchCriteria = matchForm.result();
        stop();
        view->setEditTriggers(QAbstractItemView::NoEditTriggers);
        QList<SurrogateItem> items = allSurrogateItems();
        QFuture<SurrogateItem> future = QtConcurrent::filtered(items,
                SurrogateItemMatcher(matchCriteria));
        selectWatcher.setFuture(future);
        setUpProgressBar(selectWatcher);
        editStopAction->setEnabled(true);
    }
}


void MainWindow::finishedSelecting()
{
    editStopAction->setEnabled(false);
    progressBar->hide();
    if (!selectWatcher.isCanceled()) {
        view->clearSelection();
        QItemSelectionModel *selectionModel = view->selectionModel();
        const QList<SurrogateItem> items = selectWatcher.future()
                                                        .results();
        QListIterator<SurrogateItem> i(items);
        while (i.hasNext()) {
            const SurrogateItem &item = i.next();
            selectionModel->select(
                    model->index(item.row, item.column),
                    QItemSelectionModel::Select);
        }
        statusBar()->showMessage(
                tr("Selected %Ln cell(s)", "", items.count()),
                StatusTimeout);
    }
    view->setEditTriggers(editTriggers);
}


void MainWindow::editCount()
{
    MatchForm matchForm(MatchForm::Count, this);
    if (matchForm.exec()) {
        countCriteria = matchForm.result();
        stop();
        view->setEditTriggers(QAbstractItemView::NoEditTriggers);
        applyToAll = countCriteria.applyToAll;
        QList<SurrogateItem> items = applyToAll ? allSurrogateItems()
                                           : selectedSurrogateItems();
        QFuture<Results> future = QtConcurrent::filteredReduced(
                items, SurrogateItemMatcher(countCriteria),
                itemAccumulator);
        countWatcher.setFuture(future);
        setUpProgressBar(countWatcher);
        editStopAction->setEnabled(true);
    }
}


void MainWindow::finishedCounting()
{
    editStopAction->setEnabled(false);
    progressBar->hide();
    if (!countWatcher.isCanceled()) {
        Results results = countWatcher.result();
        QString selected(applyToAll ? QString()
                                    : tr(" from those selected"));
        AQP::information(this, tr("Count"),
                tr("A total of %Ln cell(s)%2 are %3 %4.\n"
                   "Their total value is %L5.", "", results.count)
                .arg(selected)
                .arg(comparisonName(countCriteria.comparisonType))
                .arg(countCriteria.value)
                .arg(stringForLongDouble(results.sum)));
    }
    view->setEditTriggers(editTriggers);
}


void MainWindow::editApplyScript()
{
    ScriptForm scriptForm(script, this);
    if (scriptForm.exec()) {
        script = scriptForm.script();
        stop();
        view->setEditTriggers(QAbstractItemView::NoEditTriggers);
        errorInfo.clear();
        applyToAll = scriptForm.applyToAll();
        QList<SurrogateItem> items = applyToAll ? allSurrogateItems()
                                           : selectedSurrogateItems();
        QFuture<SurrogateItem> future = QtConcurrent::mapped(items,
                SurrogateItemApplier(script, &errorInfo));
        applyScriptWatcher.setFuture(future);
        setUpProgressBar(applyScriptWatcher);
        editStopAction->setEnabled(true);
    }
}


void MainWindow::finishedApplyingScript()
{
    editStopAction->setEnabled(false);
    progressBar->hide();
    if (!applyScriptWatcher.isCanceled() &&
        (errorInfo.isEmpty() || applyDespiteErrors())) {
        const QList<SurrogateItem> items = applyScriptWatcher.future()
                                                           .results();
        QListIterator<SurrogateItem> i(items);
        while (i.hasNext()) {
            const SurrogateItem &item = i.next();
            model->item(item.row, item.column)->setData(item.value,
                                                        Qt::EditRole);
        }
        QString selected(applyToAll ? QString()
                                    : tr(" from those selected"));
        statusBar()->showMessage(tr("Finished applying script "
                "to %Ln cell(s)%1", "", items.count())
                .arg(selected), StatusTimeout);
    }
    view->setEditTriggers(editTriggers);
}


bool MainWindow::applyDespiteErrors()
{
    const int MaxErrorStrings = 15;

    QStringList errors = errorInfo.errors();
    if (errors.count() > MaxErrorStrings) {
        errors = errors.mid(0, MaxErrorStrings);
        errors.append(tr("(and %L1 others...)")
                         .arg(errorInfo.count() - MaxErrorStrings));
    }
    return AQP::question(this, tr("Apply Script Error"),
            tr("%Ln error(s) occurred:\n%1", "", errorInfo.count())
               .arg(errors.join("\n")),
            "", tr("&Apply Anyway"), tr("&Don't Apply"));
}


const QList<SurrogateItem> MainWindow::allSurrogateItems() const
{
    static QList<SurrogateItem> items;
    if (cacheIsDirty) {
        items.clear();
        for (int row = 0; row < model->rowCount(); ++row) {
            for (int column = 0; column < model->columnCount();
                 ++column) {
                double value = model->item(row, column)->
                        data(Qt::EditRole).toDouble();
                items << SurrogateItem(row, column, value);
            }
        }
        cacheIsDirty = false;
    }
    return items;
}


QList<SurrogateItem> MainWindow::selectedSurrogateItems() const
{
    QList<SurrogateItem> items;
    QItemSelectionModel *selectionModel = view->selectionModel();
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int column = 0; column < model->columnCount();
             ++column) {
            QStandardItem *item = model->item(row, column);
            if (selectionModel->isSelected(item->index())) {
                double value = item->data(Qt::EditRole).toDouble();
                items << SurrogateItem(row, column, value);
            }
        }
    }
    return items;
}


void MainWindow::stop()
{
    editStopAction->setEnabled(false);
    if (selectWatcher.isRunning())
        selectWatcher.cancel();
    if (countWatcher.isRunning())
        countWatcher.cancel();
    if (applyScriptWatcher.isRunning())
        applyScriptWatcher.cancel();

    if (selectWatcher.isRunning())
        selectWatcher.waitForFinished();
    if (countWatcher.isRunning())
        countWatcher.waitForFinished();
    if (applyScriptWatcher.isRunning())
        applyScriptWatcher.waitForFinished();
}


template<typename T>
void MainWindow::setUpProgressBar(QFutureWatcher<T> &futureWatcher)
{
    progressBar->setRange(futureWatcher.progressMinimum(),
                          futureWatcher.progressMaximum());
    connect(&futureWatcher, SIGNAL(progressRangeChanged(int,int)),
            progressBar, SLOT(setRange(int,int)));
    connect(&futureWatcher, SIGNAL(progressValueChanged(int)),
            progressBar, SLOT(setValue(int)));
    progressBar->show();
}
