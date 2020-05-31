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

#include <QHash>
#include <QMainWindow>
#include <QMap>
#include <QString>


class BrowserWindow;
class LinkFetcher;
class QComboBox;
class QLabel;
class QPushButton;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=0);

private slots:
    void populateIssueComboBox(bool ok);
    void currentIssueIndexChanged(int index);
    void populateCache(bool ok);
    void currentArticleIndexChanged(int index);
    void networkTimeout();

private:
    void createWidgets();
    void createLayout();
    void createConnections();
    void populateArticleComboBox();
    void populateAComboBox(const QString &statusText,
            const QMap<QString, QString> &namesForUrls,
            QComboBox *comboBox);
    QString scriptPathAndName(const QString &filename);

    QLabel *issueLabel;
    QComboBox *issueComboBox;
    QLabel *articleLabel;
    QComboBox *articleComboBox;
    QPushButton *quitButton;
    BrowserWindow *browser;
    LinkFetcher *issueLinkFetcher;
    LinkFetcher *articleLinkFetcher;

    QHash<int, QMap<QString, QString> > namesForUrlsForIssueCache;
};

#endif // MAINWINDOW_HPP
