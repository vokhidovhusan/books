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

#include "pagedata.hpp"
#include <QDialog>
#include <QPrinter>
#include <QSet>


class QPageSetupDialog;
class QPrintPreviewDialog;
class QPushButton;
class QRadioButton;
class QTextDocument;


class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(bool makeSamples_=false, QWidget *parent=0);

private slots:
    void pageSetup();
    void printPreview();
    void print();
    void exportToFile(const QString &sampleFilename=QString());
    void printDocument(QPrinter *printer);
    void makeSamples();

private:
    void createWidgets();
    void createLayout();
    void createConnections();
    void populateDocument(QTextDocument *document);
    bool paintPdfOrPs(const QString &filename);
    bool paintSvg(const QString &filename);
    bool paintImage(const QString &filename);
    bool exportPdfOrPs(const QString &filename,
                       QTextDocument *document);
    bool exportOdf(const QString &filename, QTextDocument *document);
    bool exportHtml(const QString &filename, QTextDocument *document);
    bool exportSvg(const QString &filename, QTextDocument *document);
    bool exportImage(const QString &filename, QTextDocument *document);
    bool suffixInSet(const QString &filename,
                     const QSet<QString> &suffixes);

    QRadioButton *painterRadioButton;
    QRadioButton *htmlRadioButton;
    QRadioButton *cursorRadioButton;
    QPushButton *pageSetupButton;
    QPushButton *printPreviewButton;
    QPushButton *printButton;
    QPushButton *exportButton;
    QPushButton *quitButton;
    QPageSetupDialog *pageSetupDialog;
    QPrintPreviewDialog *printPreviewDialog;
    QPrinter printer;

    PageData pageData;
};

#endif // MAINWINDOW_HPP

