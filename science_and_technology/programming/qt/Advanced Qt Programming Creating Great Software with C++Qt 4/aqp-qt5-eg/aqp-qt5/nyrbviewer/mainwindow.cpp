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
#include "browserwindow.hpp"
#include "linkfetcher.hpp"
#include "mainwindow.hpp"
#include <QApplication>
#include <QComboBox>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextDocument>
#include <QTimer>
#include <QVBoxLayout>


namespace {
const QString NYRBUrl("http://www.nybooks.com");
const QString InitialMessage(
        QObject::tr("Attempting to connect to the network..."));
const QString FailMessage(
        QObject::tr("No issues or articles available"));
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createWidgets();
    createLayout();
    issueLinkFetcher = new LinkFetcher(NYRBUrl,
            scriptPathAndName("fetch_issue_links.js"), this);
    articleLinkFetcher = new LinkFetcher(NYRBUrl,
            scriptPathAndName("fetch_article_links.js"), this);
    createConnections();

    AQP::accelerateWidget(this);
    issueComboBox->setFocus();
    issueLinkFetcher->load(NYRBUrl);
    setWindowTitle(QApplication::applicationName());
    QTimer::singleShot(1000 * 60, this, SLOT(networkTimeout()));
}


QString MainWindow::scriptPathAndName(const QString &filename)
{
    QString name = filename;
    QString path = AQP::applicationPathOf() + "/";
    if (QFile::exists(path + name))
        return path + name;
    return QString(":/%1").arg(name);
}


void MainWindow::networkTimeout()
{
    const QString text = browser->toPlainText().trimmed();
    if (text == InitialMessage || text == FailMessage)
        browser->setHtml("<h3><font color=red>Failed to connect "
                "to the network</font></h3>Perhaps the proxy "
                "settings are wrong, or maybe a proxy is needed. "
                "Try:<br><tt>nyrbviewer --help</tt>");
}


void MainWindow::createWidgets()
{
    issueLabel = new QLabel(tr("Issue:"));
    issueComboBox = new QComboBox;
    issueLabel->setBuddy(issueComboBox);
    issueComboBox->addItem(tr("+ fetching the list of issues +"));
    articleLabel = new QLabel(tr("Article:"));
    articleComboBox = new QComboBox;
    articleLabel->setBuddy(articleComboBox);
    articleComboBox->addItem(tr("- no issue selected -"));
    quitButton = new QPushButton("Quit");
    browser = new BrowserWindow;
    browser->setHtml(tr("<h3><font color=red>%1</font></h3>")
                     .arg(InitialMessage));
}


void MainWindow::createLayout()
{
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(issueLabel);
    topLayout->addWidget(issueComboBox);
    topLayout->addWidget(articleLabel);
    topLayout->addWidget(articleComboBox, 1);
    topLayout->addStretch();
    topLayout->addWidget(quitButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(topLayout);
    layout->addWidget(browser);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
}


void MainWindow::createConnections()
{
    connect(issueLinkFetcher, SIGNAL(finished(bool)),
            this, SLOT(populateIssueComboBox(bool)));
    connect(articleLinkFetcher, SIGNAL(finished(bool)),
            this, SLOT(populateCache(bool)));
    connect(issueComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(currentIssueIndexChanged(int)));
    connect(articleComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(currentArticleIndexChanged(int)));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
}


void MainWindow::populateIssueComboBox(bool ok)
{
    if (ok)
        populateAComboBox(tr("- no issue selected -"),
                issueLinkFetcher->namesForUrls(), issueComboBox);
    else {
        issueComboBox->clear();
        issueComboBox->addItem(tr("- no issues available -"));
    }
}


void MainWindow::populateAComboBox(const QString &statusText,
        const QMap<QString, QString> &namesForUrls,
        QComboBox *comboBox)
{
    comboBox->clear();
    comboBox->addItem(statusText);
    QMapIterator<QString, QString> i(namesForUrls);
    i.toBack();
    while (i.hasPrevious()) {
        i.previous();
        comboBox->addItem(i.value(), i.key());
    }
    if (comboBox->count() > 1)
        comboBox->setCurrentIndex(1);
}


void MainWindow::currentIssueIndexChanged(int index)
{
    articleComboBox->clear();
    if (index == 0) {
        articleComboBox->addItem(tr("- no issue selected -"));
        return;
    }
    if (namesForUrlsForIssueCache.contains(index))
        populateArticleComboBox();
    else {
        articleComboBox->addItem(
                tr("+ fetching the list of articles +"));
        browser->setHtml(tr("<h3><font color=red>"
                "Fetching the list of articles...</font></h3>"));
        QString url = issueComboBox->itemData(index).toString();
        articleLinkFetcher->load(url);
    }
}


void MainWindow::populateCache(bool ok)
{
    if (!ok || issueComboBox->count() == 1) {
        articleComboBox->setItemText(0,
                tr("- no articles available -"));
        browser->setHtml(tr("<h3><font color=red>%1</font></h3>")
                         .arg(FailMessage));
        return;
    }
    QTextDocument document;
    QMap<QString, QString> namesForUrls =
            articleLinkFetcher->namesForUrls();
    QMutableMapIterator<QString, QString> i(namesForUrls);
    while (i.hasNext()) {
        i.next();
        document.setHtml(i.value());
        i.setValue(document.toPlainText());
    }
    namesForUrlsForIssueCache[issueComboBox->currentIndex()] =
            namesForUrls;
    populateArticleComboBox();
}


void MainWindow::populateArticleComboBox()
{
    int index = issueComboBox->currentIndex();
    if (index > 0)
        populateAComboBox(tr("- no article selected -"),
                namesForUrlsForIssueCache[index], articleComboBox);
}


void MainWindow::currentArticleIndexChanged(int index)
{
    if (index == 0)
        return;
    QString url = articleComboBox->itemData(index).toString();
    browser->load(url);
    browser->setFocus();
}
