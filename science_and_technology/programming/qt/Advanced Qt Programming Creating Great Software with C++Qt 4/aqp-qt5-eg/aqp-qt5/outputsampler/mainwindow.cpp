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
#include "mainwindow.hpp"
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QImageWriter>
#include <QPageSetupDialog>
#include <QPainter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QSvgGenerator>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentWriter>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>


MainWindow::MainWindow(bool makeSamples_, QWidget *parent)
    : QDialog(parent), pageSetupDialog(0), printPreviewDialog(0)
{
    createWidgets();
    createLayout();
    createConnections();

    AQP::accelerateWidget(this);
    setWindowTitle(QApplication::applicationName());
    if (makeSamples_)
        QTimer::singleShot(0, this, SLOT(makeSamples()));
}


void MainWindow::createWidgets()
{
    painterRadioButton = new QRadioButton(tr("QPainter"));
    htmlRadioButton = new QRadioButton(tr("HTML/QTextDocument"));
    cursorRadioButton = new QRadioButton(tr("QTextCursor/"
                                            "QTextDocument"));
    cursorRadioButton->setChecked(true);
    pageSetupButton = new QPushButton(tr("Page Setup..."));
    printPreviewButton = new QPushButton(tr("Print Pre&view..."));
    printButton = new QPushButton(tr("Print..."));
    exportButton = new QPushButton(tr("Export..."));
    quitButton = new QPushButton(tr("&Quit"));
}


void MainWindow::createLayout()
{
    QHBoxLayout *radioLayout = new QHBoxLayout;
    radioLayout->addWidget(painterRadioButton);
    radioLayout->addWidget(htmlRadioButton);
    radioLayout->addWidget(cursorRadioButton);
    radioLayout->addStretch();
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(pageSetupButton);
    buttonLayout->addWidget(printPreviewButton);
    buttonLayout->addWidget(printButton);
    buttonLayout->addWidget(exportButton);
    buttonLayout->addWidget(quitButton);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(radioLayout);
    layout->addLayout(buttonLayout);
    setLayout(layout);
}


void MainWindow::createConnections()
{
    connect(pageSetupButton, SIGNAL(clicked()),
            this, SLOT(pageSetup()));
    connect(printPreviewButton, SIGNAL(clicked()),
            this, SLOT(printPreview()));
    connect(printButton, SIGNAL(clicked()), this, SLOT(print()));
    connect(exportButton, SIGNAL(clicked()),
            this, SLOT(exportToFile()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(accept()));
}


void MainWindow::makeSamples()
{
    QTextStream out(stderr);
    out << "exporting...\n";

    foreach (const QString &filename, QStringList()
            << "sample-paint.pdf"
            << "sample-paint.png"
            << "sample-paint.ps"
            << "sample-paint.svg"
            << "sample-html.html"
            << "sample-cur.html"

            // All these segfault on Linux with Qt 4.6.0:
            << "sample-cur.odt"
            << "sample-cur.pdf"
            << "sample-cur.png"
            << "sample-cur.ps"
            << "sample-cur.svg"
            << "sample-html.odt"
            << "sample-html.pdf"
            << "sample-html.png"
            << "sample-html.ps"
            << "sample-html.svg"
            ) {
        if (filename.contains("paint."))
            painterRadioButton->click();
        else if (filename.contains("cur."))
            cursorRadioButton->click();
        else
            htmlRadioButton->click();
        out << filename << "\n";
        out.flush();
        exportToFile(filename);
    }
    out << "...done\n";
    out.flush();
    cursorRadioButton->setChecked(true);
}


void MainWindow::pageSetup()
{
    if (!pageSetupDialog)
        pageSetupDialog = new QPageSetupDialog(&printer, this);
    pageSetupDialog->open();
}


void MainWindow::printPreview()
{
    if (printPreviewDialog)
        delete printPreviewDialog;
    printPreviewDialog = new QPrintPreviewDialog(&printer, this);
    QSize size = qApp->desktop()->availableGeometry().size();
    size.rwidth() /= 2;
    printPreviewDialog->resize(size);
    if (painterRadioButton->isChecked())
        connect(printPreviewDialog, SIGNAL(paintRequested(QPrinter*)),
                &pageData, SLOT(paintPages(QPrinter*)));
    else
        connect(printPreviewDialog, SIGNAL(paintRequested(QPrinter*)),
                this, SLOT(printDocument(QPrinter*)));
    printPreviewDialog->open();
}


void MainWindow::print()
{
    QPrintDialog dialog(&printer, this);
    if (!dialog.exec())
        return;
    if (painterRadioButton->isChecked())
        pageData.paintPages(&printer, false);
    else
        printDocument(&printer);
}


void MainWindow::exportToFile(const QString &sampleFilename)
{
    QString filename(sampleFilename);
    if (sampleFilename.isEmpty()) {
        QString suffixes = AQP::filenameFilter(tr("Image"),
                QImageWriter::supportedImageFormats());
        suffixes += tr(";;PDF (*.pdf);;PostScript (*.ps);; SVG (*.svg)");
        if (!painterRadioButton->isChecked())
            suffixes.prepend(tr("HTML (*.htm, *.html);;ODF (*.odt);;"));
        filename = QFileDialog::getSaveFileName(this,
                tr("%1 - Export").arg(QApplication::applicationName()),
                ".", suffixes);
        if (filename.isEmpty())
            return;
    }

    bool ok = false;
    if (painterRadioButton->isChecked()) {
        if (filename.endsWith(".pdf") || filename.endsWith(".ps"))
            ok = paintPdfOrPs(filename);
        else if (filename.endsWith(".svg"))
            ok = paintSvg(filename);
        else if (!suffixInSet(filename, QSet<QString>() << "htm"
                 << "html" << "odt"))
            ok = paintImage(filename);
        else
            return;
    }
    else if (!suffixInSet(filename, QSet<QString>() << "htm" << "html"
             << "odt" << "pdf" << "ps")) {
        QTextDocument document;
        document.setPageSize(printer.pageRect().size());
        document.setDocumentMargin(25);
        if (htmlRadioButton->isChecked())
            document.setHtml(pageData.pageAsHtml());
        else {
            document.setDefaultFont(QFont("Times", 14));
            QTextCursor cursor(&document);
            pageData.addPageToDocument(&cursor);
        }
        if (filename.endsWith(".svg"))
            ok = exportSvg(filename, &document);
        else
            ok = exportImage(filename, &document);
    }
    else {
        QTextDocument document;
        populateDocument(&document);
        if (filename.endsWith(".odt"))
            ok = exportOdf(filename, &document);
        else if (filename.endsWith(".pdf") ||
                 filename.endsWith(".ps"))
            ok = exportPdfOrPs(filename, &document);
        else if (filename.endsWith(".htm") ||
                 filename.endsWith(".html"))
            ok = exportHtml(filename, &document);
        else
            Q_ASSERT(false);
    }
    if (ok && sampleFilename.isEmpty())
        AQP::information(this, tr("Export"),
                tr("Successfully exported %1").arg(filename));
}


bool MainWindow::suffixInSet(const QString &filename,
                             const QSet<QString> &suffixes)
{
    int i = filename.lastIndexOf(".");
    if (i < 0)
        return false;
    return suffixes.contains(filename.mid(i + 1));
}


// Deliberately did not refactor the printer setup because I want to
// show each approach independently and self-contained since in a real
// application only one or the other approach would be used, not both.
bool MainWindow::paintPdfOrPs(const QString &filename)
{
    Q_ASSERT(filename.endsWith(".ps") || filename.endsWith(".pdf"));
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFileName(filename);
    printer.setOutputFormat(filename.endsWith(".pdf")
            ? QPrinter::PdfFormat : QPrinter::PostScriptFormat);
    pageData.paintPages(&printer);
    return true;
}


bool MainWindow::exportPdfOrPs(const QString &filename,
                               QTextDocument *document)
{
    Q_ASSERT(filename.endsWith(".ps") || filename.endsWith(".pdf"));
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFileName(filename);
    printer.setOutputFormat(filename.endsWith(".pdf")
            ? QPrinter::PdfFormat : QPrinter::PostScriptFormat);
    document->print(&printer);
    return true;
}


bool MainWindow::paintSvg(const QString &filename)
{
    Q_ASSERT(filename.endsWith(".svg"));
    QSvgGenerator svg;
    svg.setFileName(filename);
    QRect rect = printer.pageRect().adjusted(25, 25, -25, 25);
    svg.setSize(rect.size());
    QPainter painter(&svg);
    painter.setViewport(rect);
    pageData.paintPage(&painter, 0);
    return true;
}


bool MainWindow::exportSvg(const QString &filename,
                           QTextDocument *document)
{
    Q_ASSERT(filename.endsWith(".svg"));
    QSvgGenerator svg;
    svg.setFileName(filename);
    QRect rect = printer.pageRect().adjusted(25, 25, -25, 25);
    svg.setSize(rect.size());
    QPainter painter(&svg);
    painter.setViewport(rect);
    document->drawContents(&painter);
    return true;
}


bool MainWindow::paintImage(const QString &filename)
{
    QImage image(printer.paperRect().size(), QImage::Format_ARGB32);
    QPainter painter(&image);
    painter.setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);
    painter.fillRect(painter.viewport(), Qt::white);
    painter.setViewport(printer.pageRect());
    pageData.paintPage(&painter, 0);
    return image.save(filename);
}


bool MainWindow::exportImage(const QString &filename,
                             QTextDocument *document)
{
    QImage image(printer.paperRect().size(), QImage::Format_ARGB32);
    QPainter painter(&image);
    painter.setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);
    painter.fillRect(painter.viewport(), Qt::white);
    painter.setViewport(printer.paperRect());
    document->drawContents(&painter);
    return image.save(filename);
}


bool MainWindow::exportOdf(const QString &filename,
                           QTextDocument *document)
{
    Q_ASSERT(filename.endsWith(".odt"));
    QTextDocumentWriter writer(filename);
    return writer.write(document);
}


bool MainWindow::exportHtml(const QString &filename,
                            QTextDocument *document)
{
    Q_ASSERT(filename.endsWith(".htm") || filename.endsWith(".html"));
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)) {
        AQP::warning(this, tr("Error"),
                tr("Failed to export %1: %2").arg(filename)
                .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out.setCodec("utf-8");
    out << document->toHtml("utf-8");
    file.close();
    return true;
}


void MainWindow::printDocument(QPrinter *printer)
{
    QTextDocument document;
    populateDocument(&document);
    document.print(printer);
}


void MainWindow::populateDocument(QTextDocument *document)
{
    Q_ASSERT(!painterRadioButton->isChecked());
    if (htmlRadioButton->isChecked())
        pageData.populateDocumentUsingHtml(document);
    else if (cursorRadioButton->isChecked())
        pageData.populateDocumentUsingQTextCursor(document);
}
