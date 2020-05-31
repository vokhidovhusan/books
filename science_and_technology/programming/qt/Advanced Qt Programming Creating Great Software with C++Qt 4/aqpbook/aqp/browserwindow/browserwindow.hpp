#ifndef BROWSERWINDOW_HPP
#define BROWSERWINDOW_HPP
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


#include <QFrame>
#include <QToolBar>
#include <QWebFrame>
#include <QWebView>


class QAction;
class QLabel;
class QProgressBar;
class QSpinBox;
class QUrl;
class QWebPage;


class BrowserWindow : public QFrame
{
    Q_OBJECT

public:
    explicit BrowserWindow(const QString &url=QString(),
            QWebPage *webPage=0, QWidget *parent=0,
            Qt::WindowFlags flags=0);

    QString toHtml() const
        { return webView->page()->mainFrame()->toHtml(); }
    QString toPlainText() const
        { return webView->page()->mainFrame()->toPlainText(); }

signals:
    void loadFinished(bool ok);
    void urlChanged(const QUrl &url);

public slots:
    void load(const QString &url);
    void setHtml(const QString &html) { webView->setHtml(html); }
    void showToolBar(bool on) { toolBar->setVisible(on); }
    void enableActions(bool enable);

protected:
    void focusInEvent(QFocusEvent*) { webView->setFocus(); }

private slots:
    void zoomOut();
    void zoomIn();
    void setUrl();
    void setZoomFactor(int zoom);
    void popUpHistoryMenu();
    void loadFinish(bool ok);
    void urlChange(const QUrl &url);

private:
    void createActions();
    void createToolBar();
    void createLayout();
    void createConnections();

    QAction *zoomOutAction;
    QAction *zoomInAction;
    QAction *setUrlAction;
    QAction *historyAction;

    QWebView *webView;
    QToolBar *toolBar;
    QSpinBox *zoomSpinBox;
    QProgressBar *progressBar;
    QLabel *progressLabel;
};

#endif // BROWSERWINDOW_HPP
