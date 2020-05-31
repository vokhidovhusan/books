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
#include "boxitem.hpp"
#include "brushwidget.hpp"
#include "graphicsview.hpp"
#include "mainwindow.hpp"
#include "penwidget.hpp"
#include "smileyitem.hpp"
#include "textitemdialog.hpp"
#include "textitem.hpp"
#include "transformwidget.hpp"
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QImageWriter>
#include <QMenu>
#include <QMenuBar>
#ifdef ANIMATE_ALIGNMENT
#include <QPropertyAnimation>
#endif
#ifdef ANIMATE_IN_PARALLEL
#include <QParallelAnimationGroup>
#endif
#include <QPrintDialog>
#include <QSettings>
#include <QStatusBar>
#include <QSvgGenerator>
#include <QTimer>
#include <QToolBar>
#ifdef USE_STL
#include <algorithm>
#endif
#include <cmath>
#include <limits>


namespace {

const int StatusTimeout = AQP::MSecPerSecond * 30;
const int OffsetIncrement = 5;
const qint32 MagicNumber = 0x5A93DE5;
const qint16 VersionNumber = 1;
const QString ShowGrid("ShowGrid");
const QString MostRecentFile("MostRecentFile");
const QString MimeType = "application/vnd.qtrac.pagedesigner";


#ifndef USE_STL
template<template<typename T> class S, typename T>
T min(const S<T> &sequence)
{
    Q_ASSERT(!sequence.isEmpty());
    T minimum = sequence.first();
    foreach (const T &item, sequence)
        if (item < minimum)
            minimum = item;
    return minimum;
}

template<template<typename T> class S, typename T>
T max(const S<T> &sequence)
{
    Q_ASSERT(!sequence.isEmpty());
    T maximum = sequence.first();
    foreach (const T &item, sequence)
        if (item > maximum)
            maximum = item;
    return maximum;
}
#endif

#ifdef NO_DYNAMIC_CAST
QObject *qObjectFrom(QGraphicsItem *item)
{
    if (!item)
        return 0;
    // Types not inheriting QGraphicsObject must be handled explicitly
    if (item->type() == BoxItemType)
        return qobject_cast<QObject*>(static_cast<BoxItem*>(item));
    // Types inheriting QGraphicsObject can be handled generically
    return item->toGraphicsObject();
}
#endif

} // anonymous namespace


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), gridGroup(0), addOffset(OffsetIncrement),
      pasteOffset(OffsetIncrement)
{
    printer = new QPrinter(QPrinter::HighResolution);

    createSceneAndView();
    createActions();
    createMenusAndToolBars();
    createDockWidgets();
    createConnections();

    QSettings settings;
    viewShowGridAction->setChecked(
            settings.value(ShowGrid, true).toBool());
    QString filename = settings.value(MostRecentFile).toString();
    if (filename.isEmpty() || filename == tr("Unnamed"))
        QTimer::singleShot(0, this, SLOT(fileNew()));
    else {
        setWindowFilePath(filename);
        QTimer::singleShot(0, this, SLOT(loadFile()));
    }
}


QSize MainWindow::sizeHint() const
{
    QSize size = printer->paperSize(QPrinter::Point).toSize() * 1.2;
    size.rwidth() += brushWidget->sizeHint().width();
    return size.boundedTo(
            QApplication::desktop()->availableGeometry().size());
}


void MainWindow::createSceneAndView()
{
    view = new GraphicsView;
    scene = new QGraphicsScene(this);
    QSize pageSize = printer->paperSize(QPrinter::Point).toSize();
    scene->setSceneRect(0, 0, pageSize.width(), pageSize.height());
    view->setScene(scene);
    setCentralWidget(view);
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
    fileExportAction = new QAction(QIcon(":/fileexport.png"),
            tr("Export..."), this);
    filePrintAction = new QAction(QIcon(":/fileprint.png"),
            tr("Print..."), this);
    fileQuitAction = new QAction(QIcon(":/filequit.png"),
            tr("Quit"), this);
#if QT_VERSION >= 0x040600
    fileQuitAction->setShortcuts(QKeySequence::Quit);
#else
    fileQuitAction->setShortcut(QKeySequence("Ctrl+Q"));
#endif

    editSelectedItemAction = new QAction(
            QIcon(":/editselecteditem.png"),
            tr("Edit Selected Item..."), this);
    editAddTextAction = new QAction(QIcon(":/editaddtext.png"),
            tr("Add Text..."), this);
    editAddTextAction->setData(TextItemType);
    editAddBoxAction = new QAction(QIcon(":/editaddbox.png"),
            tr("Add Box"), this);
    editAddBoxAction->setData(BoxItemType);
    editAddSmileyAction = new QAction(QIcon(":/editaddsmiley.png"),
            tr("Add Smiley"), this);
    editAddSmileyAction->setData(SmileyItemType);
    editCopyAction = new QAction(QIcon(":/editcopy.png"), tr("&Copy"),
                                 this);
    editCopyAction->setShortcuts(QKeySequence::Copy);
    editCutAction = new QAction(QIcon(":/editcut.png"), tr("Cu&t"),
                                this);
    editCutAction->setShortcuts(QList<QKeySequence>()
            << QKeySequence::Cut << Qt::Key_Delete);
    editPasteAction = new QAction(QIcon(":/editpaste.png"),
                                  tr("&Paste"), this);
    editPasteAction->setShortcuts(QKeySequence::Paste);

    editAlignmentAction = new QAction(QIcon(":/align-left.png"),
                                      tr("Alignment"), this);
    editAlignmentAction->setData(Qt::AlignLeft);
    editAlignLeftAction = new QAction(QIcon(":/align-left.png"),
                                      tr("Left"), this);
    editAlignLeftAction->setData(Qt::AlignLeft);
    editAlignRightAction = new QAction(QIcon(":/align-right.png"),
                                       tr("Right"), this);
    editAlignRightAction->setData(Qt::AlignRight);
    editAlignTopAction = new QAction(QIcon(":/align-top.png"),
                                     tr("Top"), this);
    editAlignTopAction->setData(Qt::AlignTop);
    editAlignBottomAction = new QAction(QIcon(":/align-bottom.png"),
                                        tr("Bottom"), this);
    editAlignBottomAction->setData(Qt::AlignBottom);
    editClearTransformsAction = new
            QAction(QIcon(":/editcleartransforms.png"),
                    tr("Clear Transformations"), this);

    viewZoomInAction = new QAction(QIcon(":/zoom-in.png"),
                                   tr("Zoom In"), this);
    viewZoomInAction->setShortcut(tr("+"));
    viewZoomOutAction = new QAction(QIcon(":/zoom-out.png"),
                                    tr("Zoom Out"), this);
    viewZoomOutAction->setShortcut(tr("-"));
    viewShowGridAction = new QAction(QIcon(":/showgrid.png"),
            tr("Show Grid"), this);
    viewShowGridAction->setCheckable(true);
    viewShowGridAction->setChecked(true);
}


void MainWindow::createMenusAndToolBars()
{
    QAction *separator = 0;
    setUnifiedTitleAndToolBarOnMac(true);

    QMenu *fileMenu = menuBar()->addMenu(tr("File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    populateMenuAndToolBar(fileMenu, fileToolBar, QList<QAction*>()
            << fileNewAction << fileOpenAction << fileSaveAction
            << fileExportAction << separator << filePrintAction);
    fileMenu->insertAction(fileExportAction, fileSaveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(fileQuitAction);

    QMenu *alignmentMenu = new QMenu(tr("Align"), this);
    foreach (QAction *action, QList<QAction*>()
            << editAlignLeftAction << editAlignRightAction
            << editAlignTopAction << editAlignBottomAction)
        alignmentMenu->addAction(action);
    editAlignmentAction->setMenu(alignmentMenu);

    QMenu *editMenu = menuBar()->addMenu(tr("Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));
    populateMenuAndToolBar(editMenu, editToolBar, QList<QAction*>()
            << editSelectedItemAction << separator
            << editAddTextAction << editAddBoxAction
            << editAddSmileyAction << separator << editCopyAction
            << editCutAction << editPasteAction << separator
            << editAlignmentAction << editClearTransformsAction);

    QMenu *viewMenu = menuBar()->addMenu(tr("View"));
    QToolBar *viewToolBar = addToolBar(tr("View"));
    populateMenuAndToolBar(viewMenu, viewToolBar, QList<QAction*>()
            << viewZoomInAction << viewZoomOutAction << separator
            << viewShowGridAction);

    AQP::accelerateMenu(menuBar());
}


void MainWindow::populateMenuAndToolBar(QMenu *menu,
        QToolBar *toolBar, QList<QAction*> actions)
{
#ifdef Q_WS_MAC
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
#endif
    foreach (QAction *action, actions) {
        if (!action) {
            menu->addSeparator();
            toolBar->addSeparator();
        }
        else {
            menu->addAction(action);
            toolBar->addAction(action);
        }
    }
}


void MainWindow::createDockWidgets()
{
    setDockOptions(QMainWindow::AnimatedDocks);
    QDockWidget::DockWidgetFeatures features =
            QDockWidget::DockWidgetMovable|
            QDockWidget::DockWidgetFloatable;

    transformWidget = new TransformWidget;
    QDockWidget *transformDockWidget = new QDockWidget(
            tr("Transform"), this);
    transformDockWidget->setFeatures(features);
    transformDockWidget->setWidget(transformWidget);
    addDockWidget(Qt::RightDockWidgetArea, transformDockWidget);

    brushWidget = new BrushWidget;
    QDockWidget *brushDockWidget = new QDockWidget(tr("Brush (Fill)"),
                                                   this);
    brushDockWidget->setFeatures(features);
    brushDockWidget->setWidget(brushWidget);
    addDockWidget(Qt::RightDockWidgetArea, brushDockWidget);

    penWidget = new PenWidget;
    QDockWidget *penDockWidget = new QDockWidget(tr("Pen (Outline)"),
                                                 this);
    penDockWidget->setFeatures(features);
    penDockWidget->setWidget(penWidget);
    addDockWidget(Qt::RightDockWidgetArea, penDockWidget);
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
    connect(fileExportAction, SIGNAL(triggered()),
            this, SLOT(fileExport()));
    connect(filePrintAction, SIGNAL(triggered()),
            this, SLOT(filePrint()));
    connect(fileQuitAction, SIGNAL(triggered()),
            this, SLOT(close()));
    connect(editSelectedItemAction, SIGNAL(triggered()),
            this, SLOT(editSelectedItem()));
    connect(editAddTextAction, SIGNAL(triggered()),
            this, SLOT(editAddItem()));
    connect(editAddBoxAction, SIGNAL(triggered()),
            this, SLOT(editAddItem()));
    connect(editAddSmileyAction, SIGNAL(triggered()),
            this, SLOT(editAddItem()));
    connect(editCopyAction, SIGNAL(triggered()),
            this, SLOT(editCopy()));
    connect(editCutAction, SIGNAL(triggered()),
            this, SLOT(editCut()));
    connect(editPasteAction, SIGNAL(triggered()),
            this, SLOT(editPaste()));
    connect(QApplication::clipboard(), SIGNAL(dataChanged()),
            this, SLOT(updateUi()));
    foreach (QAction *action, QList<QAction*>()
            << editAlignmentAction << editAlignLeftAction
            << editAlignRightAction << editAlignTopAction
            << editAlignBottomAction)
        connect(action, SIGNAL(triggered()), this, SLOT(editAlign()));
    connect(editClearTransformsAction, SIGNAL(triggered()),
            this, SLOT(editClearTransforms()));
    connect(scene, SIGNAL(selectionChanged()),
            this, SLOT(selectionChanged()));
    connect(viewShowGridAction, SIGNAL(toggled(bool)),
            this, SLOT(viewShowGrid(bool)));
    connect(viewZoomInAction, SIGNAL(triggered()),
            view, SLOT(zoomIn()));
    connect(viewZoomOutAction, SIGNAL(triggered()),
            view, SLOT(zoomOut()));
}


void MainWindow::setDirty(bool on)
{
    setWindowModified(on);
    updateUi();
}


void MainWindow::updateUi()
{
    fileSaveAction->setEnabled(isWindowModified());
    bool hasItems = sceneHasItems();
    fileSaveAsAction->setEnabled(hasItems);
    fileExportAction->setEnabled(hasItems);
    filePrintAction->setEnabled(hasItems);
    int selected = scene->selectedItems().count();
    editSelectedItemAction->setEnabled(selected == 1);
    editCopyAction->setEnabled(selected >= 1);
    editCutAction->setEnabled(selected >= 1);
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    editPasteAction->setEnabled(mimeData &&
            (mimeData->hasFormat(MimeType) || mimeData->hasHtml() ||
             mimeData->hasText()));
    editAlignmentAction->setEnabled(selected >= 2);
    editClearTransformsAction->setEnabled(selected >= 1);
    transformWidget->setEnabled(selected >= 1);
    bool hasBrushProperty;
    bool hasPenProperty;
    getSelectionProperties(&hasBrushProperty, &hasPenProperty);
    brushWidget->setEnabled(hasBrushProperty);
    penWidget->setEnabled(hasPenProperty);
}


bool MainWindow::sceneHasItems() const
{
    foreach (QGraphicsItem *item, scene->items())
        if (item != gridGroup && item->group() != gridGroup)
            return true;
    return false;
}


void MainWindow::getSelectionProperties(bool *hasBrushProperty,
                                        bool *hasPenProperty) const
{
    Q_ASSERT(hasBrushProperty && hasPenProperty);
    *hasBrushProperty = false;
    *hasPenProperty = false;
    foreach (QGraphicsItem *item, scene->selectedItems()) {
#ifdef NO_DYNAMIC_CAST
        if (QObject *object = qObjectFrom(item)) {
#else
        if (QObject *object = dynamic_cast<QObject*>(item)) {
#endif
            const QMetaObject *metaObject = object->metaObject();
            if (metaObject->indexOfProperty("brush") > -1)
                *hasBrushProperty = true;
            if (metaObject->indexOfProperty("pen") > -1)
                *hasPenProperty = true;
            if (*hasBrushProperty && *hasPenProperty)
                break;
        }
    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (okToClearData()) {
#ifdef SCREENSHOTS
        for (int i = 0; i < screenshots.count(); ++i)
            screenshots.at(i).save(QString("pagedesigner%1.png")
                                   .arg(i + 1, 2, 10, QChar('0')));
#endif
        QSettings settings;
        settings.setValue(ShowGrid, viewShowGridAction->isChecked());
        settings.setValue(MostRecentFile, windowFilePath());
        event->accept();
    }
    else
        event->ignore();
}


void MainWindow::fileNew()
{
    if (!okToClearData())
        return;
    clear();
    setWindowFilePath(tr("Unnamed"));
    setDirty(false);
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
    const QString &filename = QFileDialog::getOpenFileName(this,
            tr("%1 - Open").arg(QApplication::applicationName()),
            ".", tr("Page Designer (*.pd)"));
    if (filename.isEmpty())
        return;
    setWindowFilePath(filename);
    loadFile();
}


void MainWindow::loadFile()
{
    QFile file(windowFilePath());
    QDataStream in;
    if (!openPageDesignerFile(&file, in))
        return;
    in.setVersion(QDataStream::Qt_4_5);
    clear();
    readItems(in);
    statusBar()->showMessage(tr("Loaded %1").arg(windowFilePath()),
                             StatusTimeout);
    setDirty(false);
}


bool MainWindow::openPageDesignerFile(QFile *file, QDataStream &in)
{
    if (!file->open(QIODevice::ReadOnly)) {
        AQP::warning(this, tr("Error"), tr("Failed to open file: %1")
                                        .arg(file->errorString()));
        return false;
    }
    in.setDevice(file);
    qint32 magicNumber;
    in >> magicNumber;
    if (magicNumber != MagicNumber) {
        AQP::warning(this, tr("Error"),
                tr("%1 is not a %2 file").arg(file->fileName())
                .arg(QApplication::applicationName()));
        return false;
    }
    qint16 versionNumber;
    in >> versionNumber;
    if (versionNumber > VersionNumber) {
        AQP::warning(this, tr("Error"),
                tr("%1 needs a more recent version of %2")
                .arg(file->fileName())
                .arg(QApplication::applicationName()));
        return false;
    }
    return true;
}


void MainWindow::clear()
{
    scene->clear();
    gridGroup = 0;
    viewShowGrid(viewShowGridAction->isChecked());
}


void MainWindow::readItems(QDataStream &in, int offset, bool select)
{
    QSet<QGraphicsItem*> items;
    qint32 itemType;
    QGraphicsItem *item = 0;
    while (!in.atEnd()) {
        in >> itemType;
        switch (itemType) {
            case BoxItemType: {
                BoxItem *boxItem = new BoxItem(QRect(), scene);
                in >> *boxItem;
                connectItem(boxItem);
                item = boxItem;
                break;
            }
            case SmileyItemType: {
                SmileyItem *smileyItem = new SmileyItem(QPoint(),
                                                        scene);
                in >> *smileyItem;
                connectItem(smileyItem);
                item = smileyItem;
                break;
            }
            case TextItemType: {
                TextItem *textItem = new TextItem(QPoint(), scene);
                in >> *textItem;
                connectItem(textItem);
                item = textItem;
                break;
            }
        }
        if (item) {
            item->moveBy(offset, offset);
            if (select)
                items << item;
            item = 0;
        }
    }
    if (select)
        selectItems(items);
    else
        selectionChanged();
}


void MainWindow::selectItems(const QSet<QGraphicsItem*> &items)
{
    scene->clearSelection();
    foreach (QGraphicsItem *item, items)
        item->setSelected(true);
    selectionChanged();
}


bool MainWindow::fileSave()
{
    const QString filename = windowFilePath();
    if (filename.isEmpty() || filename == tr("Unnamed"))
        return fileSaveAs();
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    QDataStream out(&file);
    out << MagicNumber << VersionNumber;
    out.setVersion(QDataStream::Qt_4_5);
    writeItems(out, scene->items());
    file.close();
    setDirty(false);
    return true;
}


void MainWindow::writeItems(QDataStream &out,
                            const QList<QGraphicsItem*> &items)
{
    foreach (QGraphicsItem *item, items) {
        if (item == gridGroup || item->group() == gridGroup)
            continue;
        qint32 type = static_cast<qint32>(item->type());
        out << type;
        switch (type) {
            case BoxItemType:
                    out << *static_cast<BoxItem*>(item); break;
            case SmileyItemType:
                    out << *static_cast<SmileyItem*>(item); break;
            case TextItemType:
                    out << *static_cast<TextItem*>(item); break;
            default: Q_ASSERT(false);
        }
    }
}


bool MainWindow::fileSaveAs()
{
    QString filename = QFileDialog::getSaveFileName(this,
            tr("%1 - Save As").arg(QApplication::applicationName()),
            ".", tr("Page Designer (*.pd)"));
    if (filename.isEmpty())
        return false;
    if (!filename.toLower().endsWith(".pd"))
        filename += ".pd";
    setWindowFilePath(filename);
    return fileSave();
}


void MainWindow::fileExport()
{
    QString suffixes = AQP::filenameFilter(tr("Bitmap image"),
            QImageWriter::supportedImageFormats());
    suffixes += tr(";;Vector image (*.svg)");
    const QString filename = QFileDialog::getSaveFileName(this,
            tr("%1 - Export").arg(QApplication::applicationName()),
            ".", suffixes);
    if (filename.isEmpty())
        return;
    if (filename.toLower().endsWith(".svg"))
        exportSvg(filename);
    else
        exportImage(filename);
}


void MainWindow::exportSvg(const QString &filename)
{
    QSvgGenerator svg;
    svg.setFileName(filename);
    svg.setSize(printer->paperSize(QPrinter::Point).toSize());
    {
        QPainter painter(&svg);
        paintScene(&painter);
    }
    statusBar()->showMessage(tr("Exported %1").arg(filename),
                             StatusTimeout);
}


void MainWindow::exportImage(const QString &filename)
{
    QImage image(printer->paperSize(QPrinter::Point).toSize(),
                 QImage::Format_ARGB32);
    {
        QPainter painter(&image);
        painter.setRenderHints(QPainter::Antialiasing|
                               QPainter::TextAntialiasing);
        paintScene(&painter);
    }
    if (image.save(filename))
        statusBar()->showMessage(tr("Exported %1").arg(filename),
                                 StatusTimeout);
    else
        AQP::warning(this, tr("Error"), tr("Failed to export: %1")
                                        .arg(filename));
}


void MainWindow::paintScene(QPainter *painter)
{
    bool showGrid = viewShowGridAction->isChecked();
    if (showGrid)
        viewShowGrid(false);
    QList<QGraphicsItem*> items = scene->selectedItems();
    scene->clearSelection();

    scene->render(painter);

    if (showGrid)
        viewShowGrid(true);
    foreach (QGraphicsItem *item, items)
        item->setSelected(true);
    selectionChanged();
}


void MainWindow::filePrint()
{
    QPrintDialog dialog(printer);
    if (dialog.exec()) {
        {
            QPainter painter(printer);
            paintScene(&painter);
        }
        statusBar()->showMessage(tr("Printed %1")
                .arg(windowFilePath()), StatusTimeout);
    }
}


QPoint MainWindow::position()
{
    QPoint point = mapFromGlobal(QCursor::pos());
    if (!view->geometry().contains(point)) {
        point = previousPoint.isNull()
                ? view->pos() + QPoint(10, 10) : previousPoint;
    }
    if (!previousPoint.isNull() && point == previousPoint) {
        point += QPoint(addOffset, addOffset);
        addOffset += OffsetIncrement;
    }
    else {
        addOffset = OffsetIncrement;
        previousPoint = point;
    }
    return view->mapToScene(point - view->pos()).toPoint();
}


void MainWindow::editSelectedItem()
{
    QList<QGraphicsItem*> items = scene->selectedItems();
    if (items.count() != 1)
        return;
#ifdef NO_DYNAMIC_CAST
    if (QObject *item = qObjectFrom(items.at(0))) {
#else
    if (QObject *item = dynamic_cast<QObject*>(items.at(0))) {
#endif
        const QMetaObject *metaObject = item->metaObject();
        metaObject->invokeMethod(item, "edit", Qt::DirectConnection);
    }
}


void MainWindow::editAddItem()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;
    QObject *item = 0;
    int type = action->data().toInt();
    if (type == BoxItemType)
        item = new BoxItem(QRect(position(), QSize(90, 30)), scene);
    else if (type == SmileyItemType)
        item = new SmileyItem(position(), scene);
    else if (type == TextItemType) {
        TextItemDialog dialog(0, position(), scene, this);
        if (dialog.exec())
            item = dialog.textItem();
    }
    if (item) {
        connectItem(item);
        setDirty(true);
    }
}


void MainWindow::connectItem(QObject *item)
{
    connect(item, SIGNAL(dirty()), this, SLOT(setDirty()));
    const QMetaObject *metaObject = item->metaObject();
    if (metaObject->indexOfProperty("brush") > -1)
        connect(brushWidget, SIGNAL(brushChanged(const QBrush&)),
                item, SLOT(setBrush(const QBrush&)));
    if (metaObject->indexOfProperty("pen") > -1)
        connect(penWidget, SIGNAL(penChanged(const QPen&)),
                item, SLOT(setPen(const QPen&)));
    if (metaObject->indexOfProperty("angle") > -1) {
        connect(transformWidget, SIGNAL(angleChanged(double)),
                item, SLOT(setAngle(double)));
        connect(transformWidget, SIGNAL(shearChanged(double, double)),
                item, SLOT(setShear(double, double)));
    }
}


void MainWindow::viewShowGrid(bool on)
{
    if (!gridGroup) {
        const int GridSize = 40;
        QPen pen(QColor(175, 175, 175, 127));
        gridGroup = new QGraphicsItemGroup;
        const int MaxX = static_cast<int>(std::ceil(scene->width())
                / GridSize) * GridSize;
        const int MaxY = static_cast<int>(std::ceil(scene->height())
                / GridSize) * GridSize;
        for (int x = 0; x <= MaxX; x += GridSize) {
            QGraphicsLineItem *item = new QGraphicsLineItem(x, 0, x,
                                                            MaxY);
            item->setPen(pen);
            item->setZValue(std::numeric_limits<int>::min());
            gridGroup->addToGroup(item);
        }
        for (int y = 0; y <= MaxY; y += GridSize) {
            QGraphicsLineItem *item = new QGraphicsLineItem(0, y,
                                                            MaxX, y);
            item->setPen(pen);
            item->setZValue(-1000);
            gridGroup->addToGroup(item);
        }
        scene->addItem(gridGroup);
    }
    gridGroup->setVisible(on);
}


void MainWindow::editCopy()
{
    QList<QGraphicsItem*> items = scene->selectedItems();
    if (items.isEmpty())
        return;
    pasteOffset = OffsetIncrement;
    copyItems(items);
    updateUi();
}


void MainWindow::copyItems(const QList<QGraphicsItem*> &items)
{
    QByteArray copiedItems;
    QDataStream out(&copiedItems, QIODevice::WriteOnly);
    writeItems(out, items);
    QMimeData *mimeData = new QMimeData;
    mimeData->setData(MimeType, copiedItems);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);
}


void MainWindow::editCut()
{
    QList<QGraphicsItem*> items = scene->selectedItems();
    if (items.isEmpty())
        return;
    copyItems(items);
    QListIterator<QGraphicsItem*> i(items);
    while (i.hasNext()) {
#if QT_VERSION >= 0x040600
        QScopedPointer<QGraphicsItem> item(i.next());
        scene->removeItem(item.data());
#else
        QGraphicsItem *item = i.next();
        scene->removeItem(item);
        delete item;
#endif
    }
    setDirty(true);
}


void MainWindow::editPaste()
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if (!mimeData)
        return;

    if (mimeData->hasFormat(MimeType)) {
        QByteArray copiedItems = mimeData->data(MimeType);
        QDataStream in(&copiedItems, QIODevice::ReadOnly);
        readItems(in, pasteOffset, true);
        pasteOffset += OffsetIncrement;
    }
    else if (mimeData->hasHtml() || mimeData->hasText()) {
        TextItem *textItem = new TextItem(position(), scene);
        connectItem(textItem);
        if (mimeData->hasHtml())
            textItem->setHtml(mimeData->html());
        else
            textItem->setPlainText(mimeData->text());
    }
    else
        return;
    setDirty(true);
}


#ifndef ANIMATE_ALIGNMENT
void MainWindow::editAlign()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    Qt::Alignment alignment = static_cast<Qt::Alignment>(
            action->data().toInt());
    if (action != editAlignmentAction) {
        editAlignmentAction->setData(action->data());
        editAlignmentAction->setIcon(action->icon());
    }

    QList<QGraphicsItem*> items = scene->selectedItems();
    QVector<double> coordinates;
    populateCoordinates(alignment, &coordinates, items);
    double offset;
    if (alignment == Qt::AlignLeft || alignment == Qt::AlignTop)
#ifdef USE_STL
        offset = *std::min_element(coordinates.constBegin(),
                                   coordinates.constEnd());
    else
        offset = *std::max_element(coordinates.constBegin(),
                                   coordinates.constEnd());
#else
        offset = min(coordinates);
    else
        offset = max(coordinates);
#endif

    if (alignment == Qt::AlignLeft || alignment == Qt::AlignRight) {
        for (int i = 0; i < items.count(); ++i)
            items.at(i)->moveBy(offset - coordinates.at(i), 0);
    }
    else {
        for (int i = 0; i < items.count(); ++i)
            items.at(i)->moveBy(0, offset - coordinates.at(i));
    }
    setDirty(true);
}

#else

void MainWindow::editAlign()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    Qt::Alignment alignment = static_cast<Qt::Alignment>(
            action->data().toInt());
    if (action != editAlignmentAction) {
        editAlignmentAction->setData(action->data());
        editAlignmentAction->setIcon(action->icon());
    }

    QList<QGraphicsItem*> items = scene->selectedItems();
    QVector<double> coordinates;
    populateCoordinates(alignment, &coordinates, items);
    double offset;
    if (alignment == Qt::AlignLeft || alignment == Qt::AlignTop)
#ifdef USE_STL
        offset = *std::min_element(coordinates.constBegin(),
                                   coordinates.constEnd());
    else
        offset = *std::max_element(coordinates.constBegin(),
                                   coordinates.constEnd());
#else
        offset = min(coordinates);
    else
        offset = max(coordinates);
#endif
#ifdef SCREENSHOTS
    takeScreenshot();
#endif

    QList<QPointF> positions;
    if (alignment == Qt::AlignLeft || alignment == Qt::AlignRight) {
        for (int i = 0; i < items.count(); ++i)
            positions << items.at(i)->pos() +
                         QPointF(offset - coordinates.at(i), 0);
    }
    else {
        for (int i = 0; i < items.count(); ++i)
            positions << items.at(i)->pos() +
                         QPointF(0, offset - coordinates.at(i));
    }

    animateAlignment(items, positions);
    setDirty(true);
}


void MainWindow::animateAlignment(const QList<QGraphicsItem*> &items,
                                  const QList<QPointF> &positions)
{
    int duration = ((qApp->keyboardModifiers() & Qt::ShiftModifier)
                    != Qt::ShiftModifier) ? 1000 : 5000;

#ifdef ANIMATE_IN_PARALLEL
    QParallelAnimationGroup *group = new QParallelAnimationGroup;
#endif
    for (int i = 0; i < items.count(); ++i) {
#ifdef NO_DYNAMIC_CAST
        QObject *object = qObjectFrom(items.at(i));
#else
        QObject *object = dynamic_cast<QObject*>(items.at(i));
#endif
        if (!object)
            continue;
        QPropertyAnimation *animation = new QPropertyAnimation(
                object, "pos", this);
        animation->setDuration(duration);
        animation->setEasingCurve(QEasingCurve::InOutBack);
#ifdef ANIMATE_IN_PARALLEL
        animation->setStartValue(items.at(i)->pos());
        animation->setEndValue(positions.at(i));
        group->addAnimation(animation);
#else
        animation->setKeyValueAt(0.0, items.at(i)->pos());
        animation->setKeyValueAt(1.0, positions.at(i));
        animation->start(QAbstractAnimation::DeleteWhenStopped);
#endif
    }
#ifdef ANIMATE_IN_PARALLEL
    group->start(QAbstractAnimation::DeleteWhenStopped);
#endif
}

#endif

#ifdef SCREENSHOTS
void MainWindow::takeScreenshot()
{
    screenshots << QPixmap::grabWidget(view);
    if (screenshots.count() < 49)
        QTimer::singleShot(50, this, SLOT(takeScreenshot()));
}
#endif


void MainWindow::populateCoordinates(const Qt::Alignment &alignment,
        QVector<double> *coordinates,
        const QList<QGraphicsItem*> &items)
{
    QListIterator<QGraphicsItem*> i(items);
    while (i.hasNext()) {
        QRectF rect = i.next()->sceneBoundingRect();
        switch (alignment) {
            case Qt::AlignLeft:
                coordinates->append(rect.x()); break;
            case Qt::AlignRight:
                coordinates->append(rect.x() + rect.width()); break;
            case Qt::AlignTop:
                coordinates->append(rect.y()); break;
            case Qt::AlignBottom:
                coordinates->append(rect.y() + rect.height()); break;
        }
    }
}


void MainWindow::editClearTransforms()
{
    QList<QGraphicsItem*> items = scene->selectedItems();
    Q_ASSERT(!items.isEmpty());
    QListIterator<QGraphicsItem*> i(items);
    while (i.hasNext()) {
#ifdef NO_DYNAMIC_CAST
        if (QObject *item = qObjectFrom(i.next())) {
#else
        if (QObject *item = dynamic_cast<QObject*>(i.next())) {
#endif
            if (item->property("angle").isValid()) {
                item->setProperty("angle", 0.0);
                item->setProperty("shearHorizontal", 0.0);
                item->setProperty("shearVertical", 0.0);
            }
        }
    }
    transformWidget->setAngle(0.0);
    transformWidget->setShear(0.0, 0.0);
    setDirty(true);
}


void MainWindow::selectionChanged()
{
    QList<QGraphicsItem*> items = scene->selectedItems();
    if (items.count() == 1) {
#ifdef NO_DYNAMIC_CAST
        if (QObject *item = qObjectFrom(items.at(0))) {
#else
        if (QObject *item = dynamic_cast<QObject*>(items.at(0))) {
#endif
            if (item->property("brush").isValid())
                brushWidget->setBrush(
                        item->property("brush").value<QBrush>());
            if (item->property("pen").isValid())
                penWidget->setPen(
                        item->property("pen").value<QPen>());
            if (item->property("angle").isValid()) {
                transformWidget->setAngle(
                        item->property("angle").toDouble());
                transformWidget->setShear(
                        item->property("shearHorizontal").toDouble(),
                        item->property("shearVertical").toDouble());
            }
        }
    }
    updateUi();
}
