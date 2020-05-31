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
#include "browserwindow.hpp"
#include <QAction>
#include <QFontMetrics>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QProgressBar>
#include <QSet>
#include <QSpinBox>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebHistory>
#include <QWebPage>


namespace {
const int MaxHistoryMenuItems = 20;
const int MaxMenuWidth = 300;
const int ZoomStepSize = 5;
}


#if QT_VERSION < 0x040700
inline bool qHash(const QUrl &url) { return qHash(url.toString()); }
#endif


BrowserWindow::BrowserWindow(const QString &url, QWebPage *webPage,
                             QWidget *parent, Qt::WindowFlags flags)
    : QFrame(parent, flags)
{
    setFrameStyle(QFrame::Box|QFrame::Raised);

    webView = new QWebView;
    if (webPage)
        webView->setPage(webPage);
    load(url);

    createActions();
    createToolBar();
    createLayout();
    createConnections();
}


void BrowserWindow::createActions()
{
    zoomOutAction = new QAction(QIcon(":/zoomout.png"),
                                tr("Zoom Out"), this);
    zoomOutAction->setShortcuts(QKeySequence::ZoomOut);
    zoomInAction = new QAction(QIcon(":/zoomin.png"),
                               tr("Zoom In"), this);
    zoomInAction->setShortcuts(QKeySequence::ZoomIn);
    setUrlAction = new QAction(QIcon(":/seturl.png"),
                               tr("Set URL..."), this);
    setUrlAction->setShortcut(QKeySequence("F2"));
    historyAction = new QAction(QIcon(":/history.png"),
                                tr("History..."), this);

    QList<QAction*> actions;
    actions << webView->pageAction(QWebPage::Back)
            << webView->pageAction(QWebPage::Forward)
            << webView->pageAction(QWebPage::Reload)
            << webView->pageAction(QWebPage::Stop)
            << zoomOutAction << zoomInAction << setUrlAction
            << historyAction;
#ifdef DEBUG
    actions << webView->pageAction(QWebPage::InspectElement);
#endif
    AQP::accelerateActions(actions);
    webView->addActions(actions);
    webView->setContextMenuPolicy(Qt::ActionsContextMenu);
}


void BrowserWindow::createToolBar()
{
    toolBar = new QToolBar(tr("Navigation"), this);
#ifdef Q_WS_MAC
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
#endif
    toolBar->addAction(webView->pageAction(QWebPage::Back));
    toolBar->addAction(webView->pageAction(QWebPage::Forward));
    toolBar->addAction(webView->pageAction(QWebPage::Reload));
    toolBar->addAction(webView->pageAction(QWebPage::Stop));
    toolBar->addSeparator();
    toolBar->addAction(zoomOutAction);
    toolBar->addAction(zoomInAction);
    toolBar->addSeparator();
    zoomSpinBox = new QSpinBox;
    zoomSpinBox->setSingleStep(ZoomStepSize);
    zoomSpinBox->setRange(25, 250);
    zoomSpinBox->setValue(100);
    zoomSpinBox->setSuffix(tr("%"));
    zoomSpinBox->setFocusPolicy(Qt::NoFocus);
    QLabel *zoomLabel = new QLabel(tr("Zoom"));
    zoomLabel->setMargin(4);
    progressBar = new QProgressBar;
    progressLabel = new QLabel(tr("Progress"));
    progressLabel->setMargin(4);
#ifndef Q_WS_MAC
    toolBar->addWidget(zoomLabel);
    toolBar->addWidget(zoomSpinBox);
    toolBar->addSeparator();
    toolBar->addAction(setUrlAction);
    toolBar->addAction(historyAction);
    toolBar->addWidget(progressLabel);
    toolBar->addWidget(progressBar);
#else
    QWidget *zoomWidget = new QWidget;
    QVBoxLayout *zoomLayout = new QVBoxLayout;
    zoomLayout->addWidget(zoomSpinBox);
    zoomLayout->addWidget(zoomLabel);
    zoomLayout->setAlignment(zoomLabel, Qt::AlignCenter);
    zoomWidget->setLayout(zoomLayout);
    toolBar->addWidget(zoomWidget);
    toolBar->addSeparator();
    toolBar->addAction(setUrlAction);
    toolBar->addAction(historyAction);
    QWidget *progressWidget = new QWidget;
    QVBoxLayout *progressLayout = new QVBoxLayout;
    progressLayout->setAlignment(Qt::AlignCenter);
    progressLayout->addWidget(progressBar);
    progressLayout->addWidget(progressLabel);
    progressLayout->setAlignment(progressLabel, Qt::AlignCenter);
    progressWidget->setLayout(progressLayout);
    toolBar->addWidget(progressWidget);
#endif
}


void BrowserWindow::createLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(webView);
    layout->addWidget(toolBar);
    setLayout(layout);
}


void BrowserWindow::createConnections()
{
    connect(webView, SIGNAL(loadProgress(int)),
            progressBar, SLOT(setValue(int)));
    connect(webView, SIGNAL(urlChanged(const QUrl&)),
            this, SLOT(urlChange(const QUrl&)));
    connect(webView, SIGNAL(loadFinished(bool)),
            this, SLOT(loadFinish(bool)));
    connect(setUrlAction, SIGNAL(triggered()), this, SLOT(setUrl()));
    connect(historyAction, SIGNAL(triggered()),
            this, SLOT(popUpHistoryMenu()));
    connect(zoomOutAction, SIGNAL(triggered()),
            this, SLOT(zoomOut()));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(zoomSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(setZoomFactor(int)));
}


void BrowserWindow::load(const QString &url)
{
    if (url.isEmpty())
        return;
    QString theUrl = url;
    if (!theUrl.contains("://"))
        theUrl.prepend("http://");
    webView->load(theUrl);
}


void BrowserWindow::enableActions(bool enable)
{
    foreach (QAction *action, webView->actions())
        action->setEnabled(enable);
    toolBar->setEnabled(enable);
    webView->setContextMenuPolicy(enable ? Qt::ActionsContextMenu
                                         : Qt::NoContextMenu);
}


void BrowserWindow::setUrl()
{
    load(QInputDialog::getText(this, tr("Set URL"), tr("&URL:")));
}


void BrowserWindow::zoomOut()
{
    zoomSpinBox->setValue(zoomSpinBox->value() - ZoomStepSize);
}


void BrowserWindow::zoomIn()
{
    zoomSpinBox->setValue(zoomSpinBox->value() + ZoomStepSize);
}


void BrowserWindow::setZoomFactor(int zoom)
{
#if QT_VERSION >= 0x040500
    webView->setZoomFactor(zoom / 100.0);
#else
    webView->setTextSizeMultiplier(zoom / 100.0);
#endif
}


void BrowserWindow::loadFinish(bool ok)
{
    emit loadFinished(ok);
    progressLabel->setText(ok ? tr("Loaded") : tr("Canceled"));
#ifdef DEBUG
    qDebug("loadFinished(%s)", ok ? "true" : "false");
#endif
}

void BrowserWindow::urlChange(const QUrl &url)
{
    emit urlChanged(url);
    progressLabel->setText(tr("Loading"));
#ifdef DEBUG
    qDebug("urlChanged(\"%s\")", qPrintable(url.toString()));
#endif
}


void BrowserWindow::popUpHistoryMenu()
{
    QFontMetrics fontMetrics(font());
    QMenu menu;
    QSet<QUrl> uniqueUrls;
    QListIterator<QWebHistoryItem> i(webView->history()->items());
    i.toBack();
    while (i.hasPrevious() &&
           uniqueUrls.count() < MaxHistoryMenuItems) {
        const QWebHistoryItem &item = i.previous();
        if (uniqueUrls.contains(item.url()))
            continue;
        uniqueUrls << item.url();
        QString title = fontMetrics.elidedText(item.title(),
                Qt::ElideRight, MaxMenuWidth);
#if QT_VERSION >= 0x040500
        QAction *action = new QAction(item.icon(), title, &menu);
#else
        QAction *action = new QAction(title, &menu);
#endif
        action->setData(item.url());
        menu.addAction(action);
    }
    AQP::accelerateMenu(&menu);
    if (QAction *action = menu.exec(QCursor::pos()))
        webView->load(action->data().toUrl());
}
