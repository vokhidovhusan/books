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
#include "pagedata.hpp"
#include <QPainter>
#include <QPrinter>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentFragment>
#ifdef EMBED_SVG
#include <QtSvg>
#endif


PageData::PageData()
    : QObject()
{
    QString path = AQP::applicationPathOf();

    OnePage page1;
    page1.title = tr("Weather Icons #1");
    foreach (QString name, QStringList()
            << "Cloudy" << "Lightning" << "Overcast" << "Raining") {
        page1.captions << name;
        name.replace(" ", "_");
#ifdef EMBED_SVG
        page1.filenames << path + "/" + name + ".svg";
#else
        page1.filenames << path + "/" + name + ".png";
#endif
    }
    page1.descriptionHtml = QString(
            "Fusce ac turpis sem, ac tincidunt nulla. Donec hendrerit"
            " nisi erat&mdash;quis &eacute;uismod ipsum. Sed nec <font"
            " color=darkblue>euismod</font> mauris. Lorem ipsum dolor"
            " sit amet, <b>consectetur</b> adipiscing elit. Sed id"
            " nunc arcu, id accumsan diam. Morbi nec <u>nulla"
            " egestas</u> lectus &lt;faucibus auctor&gt; at in eros."
            " Lorem ipsum dolor sit amet, consectetur adipiscing elit."
            " Pellentesque <i>libero magna,</i> aliquet in eleifend"
            " non, <b><font color=darkred>porttitor at"
            " mauris.</font></b> Curabitur in felis felis. Nulla"
            " facilisi. Donec est enim, <font color=darkmagenta>"
            "porttitor ut consectetur</font> non,"
            " pretium et nisl. In hac habitasse platea dictumst.");

    OnePage page2;
    page2.title = tr("Weather Icons #2");
    foreach (QString name, QStringList()
            << "Snowing" << "Stormy" << "Sunny and Cloudy"
            << "Sunny") {
        page2.captions << name;
        name.replace(" ", "_");
#ifdef EMBED_SVG
        page2.filenames << path + "/" + name + ".svg";
#else
        page2.filenames << path + "/" + name + ".png";
#endif
    }
    page2.descriptionHtml = QString(
            "Sed non ligula mauris. Nam tincidunt erat vel augue"
            " feugiat sagittis. Etiam <i>vulputate,</i> nunc non"
            " pretium pellentesque, massa nunc pulvinar odio&mdash;ut"
            " bibendum <u>odio diam vitae</u> augue. Pellentesque"
            " <b>habitant</b> morbi tristique senectus et netus et"
            " malesuada fames ac &lt;turpis egestas&gt;. Quisque"
            " <i>consectetur tellus at diam</i> sollicitudin ut"
            " pretium metus scel&eacute;risque. Suspendisse <b><font"
            " color=darkred>luctus consectetur hendrerit.</font></b>"
            " Pellentesque eget orci ipsum, eu porta leo. Integer"
            " <font color=darkgreen>blandit interdum</font> diam,"
            " vitae tincidunt dui scelerisque ac.");

    pages << page1 << page2;
}


void PageData::paintPages(QPrinter *printer, bool noUserInteraction)
{
    if (noUserInteraction)
        printer->setPageMargins(25, 25, 25, 25, QPrinter::Millimeter);
    QPainter painter(printer);
    for (int page = 0; page < pages.count(); ++page) {
        paintPage(&painter, page);
        if (page + 1 < pages.count())
            printer->newPage();
    }
}


void PageData::paintPage(QPainter *painter, int page)
{
    const OnePage &thePage = pages.at(page);

    int y = paintTitle(painter, thePage.title);
    y = paintItems(painter, y, thePage);
    paintHtmlParagraph(painter, y, thePage.descriptionHtml);
    paintFooter(painter, tr("- %1 -").arg(page + 1));
}


int PageData::paintTitle(QPainter *painter, const QString &title)
{
    painter->setFont(QFont("Helvetica", 24, QFont::Bold));
    QRect rect(0, 0, painter->viewport().width(),
               painter->fontMetrics().height());
    painter->drawText(rect, title, QTextOption(Qt::AlignCenter));
    return qRound(painter->fontMetrics().lineSpacing() * 1.5);
}


int PageData::paintItems(QPainter *painter, int y,
                         const OnePage &thePage)
{
    const int ItemHeight = painter->viewport().height() / 3;
    const int ItemWidth = painter->viewport().width() / 2;

    paintItem(painter, QRect(0, y, ItemWidth, ItemHeight),
              thePage.filenames.at(0), thePage.captions.at(0));
    paintItem(painter, QRect(ItemWidth, y, ItemWidth, ItemHeight),
              thePage.filenames.at(1), thePage.captions.at(1));
    y += ItemHeight;
    paintItem(painter, QRect(0, y, ItemWidth, ItemHeight),
              thePage.filenames.at(2), thePage.captions.at(2));
    paintItem(painter, QRect(ItemWidth, y, ItemWidth, ItemHeight),
              thePage.filenames.at(3), thePage.captions.at(3));
    return y + ItemHeight + painter->fontMetrics().height();
}


void PageData::paintItem(QPainter *painter, const QRect &rect,
        const QString &filename, const QString &caption)
{
    painter->drawRect(rect);

    const int Margin = 20;
    painter->setFont(QFont("Helvetica", 18));
    const int LineHeight = painter->fontMetrics().lineSpacing();

    QRect imageRect(rect);
    imageRect.adjust(Margin, Margin, -Margin, -(Margin + LineHeight));
#ifdef EMBED_SVG
    QSvgRenderer svg(filename);
    QSize size(svg.defaultSize());
    size.scale(imageRect.size(), Qt::KeepAspectRatio);
    imageRect.setSize(size);
    const int Xoffset = (imageRect.width() - size.width()) / 2;
    imageRect.moveTo(imageRect.x() + Xoffset, imageRect.y());
    svg.render(painter, imageRect);
#else
    QImage image(filename);
    QSize size(image.size());
    size.scale(imageRect.size(), Qt::KeepAspectRatio);
    imageRect.setSize(size);
    const int Xoffset = (imageRect.width() - size.width()) / 2;
    imageRect.moveTo(imageRect.x() + Xoffset, imageRect.y());
    painter->drawImage(imageRect, image);
#endif

    int y = rect.y() + rect.height() - LineHeight;
    QRect captionRect(rect.x(), y, rect.width(), LineHeight);
    painter->drawText(captionRect, caption,
                      QTextOption(Qt::AlignCenter));
}


int PageData::paintHtmlParagraph(QPainter *painter, int y,
                                 const QString &html)
{
    const QFont ParagraphFont("Times", 15);
    painter->setFont(ParagraphFont);
    QTextDocument document;
    document.setHtml(html);
#ifdef PAINT_USING_QTEXTDOCUMENT
    document.setDefaultFont(ParagraphFont);
    document.setUseDesignMetrics(true);
    document.setTextWidth(painter->viewport().width());
    QRect rect(0, y, painter->viewport().width(),
               painter->viewport().height());
    painter->save();
    painter->setViewport(rect);
    document.drawContents(painter);
    painter->restore();
    return y + document.documentLayout()->documentSize().height() +
           painter->fontMetrics().lineSpacing();
#else
    QTextBlock block = document.begin();
    Q_ASSERT(block.isValid());
    int x = 0;
    for (QTextBlock::iterator i = block.begin(); !i.atEnd(); ++i) {
        QTextFragment fragment = i.fragment();
        if (fragment.isValid()) {
            QTextCharFormat format = fragment.charFormat();
            foreach (QString word,
                     fragment.text().split(QRegExp("\\s+"))) {
                int width = painter->fontMetrics().width(word);
                if (x + width > painter->viewport().width()) {
                    x = 0;
                    y += painter->fontMetrics().lineSpacing();
                }
                else if (x != 0)
                    word.prepend(" ");
                x += paintWord(painter, x, y, word, ParagraphFont,
                               format);
            }
        }
    }
    return y + painter->fontMetrics().lineSpacing();
#endif
}


int PageData::paintWord(QPainter *painter, int x, int y,
        const QString &word, const QFont &paragraphFont,
        const QTextCharFormat &format)
{
    QFont font(format.font());
    font.setFamily(paragraphFont.family());
    font.setPointSize(paragraphFont.pointSize());
    painter->setFont(font);
    painter->setPen(format.foreground().color());
    painter->drawText(x, y, word);
    return painter->fontMetrics().width(word);
}


void PageData::paintFooter(QPainter *painter, const QString &footer)
{
    painter->setFont(QFont("Helvetica", 11));
    painter->setPen(Qt::black);
    const int LineHeight = painter->fontMetrics().lineSpacing();
    int y = painter->viewport().height() - LineHeight;
    painter->drawLine(0, y, painter->viewport().width(), y);

    y += LineHeight / 10;
    painter->drawText(
            QRect(0, y, painter->viewport().width(), LineHeight),
            footer, QTextOption(Qt::AlignCenter));
}


void PageData::populateDocumentUsingHtml(QTextDocument *document)
{
    QString html("<html>\n<body>\n");
    for (int page = 0; page < pages.count(); ++page) {
        html += pageAsHtml(page, false);
        if (page + 1 < pages.count())
            html += "<br style='page-break-after:always;'/>\n";
    }
    html += "</body>\n</html>\n";
    document->setHtml(html);
}


QString PageData::pageAsHtml(int page, bool selfContained)
{
    const OnePage &thePage = pages.at(page);

    QString html;
    if (selfContained)
        html += "<html>\n<body>\n";
    html += QString("<h1 align='center'>%1</h1>\n")
                  //.arg(Qt::escape(thePage.title)); //escape obsoleted in Qt5
                    .arg(QString(thePage.title).toHtmlEscaped()); //changed for Qt5
    html += "<p>";
    html += itemsAsHtmlTable(thePage);
    html += "</p>\n";
    html += QString("<p style='font-size:15pt;font-family:times'>"
                    "%1</p><hr>\n").arg(thePage.descriptionHtml);
    if (selfContained)
        html += "</body>\n</html>\n";
    return html;
}


QString PageData::itemsAsHtmlTable(const OnePage &thePage)
{
    QString html("<table border='1' cellpadding='20' width='100%'>");
    for (int i = 0; i < thePage.filenames.count(); ++i) {
        if (i % 2 == 0)
            html += "<tr>\n";
        html += QString("<td align='center'><img src='%1'/>"
                        "<p style='font-size:18pt'>%2</p></td>\n")
                        .arg(thePage.filenames.at(i))
                      //.arg(Qt::escape(thePage.captions.at(i))); //escape obsoleted in Qt5
                        .arg(QString(thePage.captions.at(i)).toHtmlEscaped()); //changed for Qt5
        if (i % 2 != 0)
            html += "</tr>\n";
    }
    if (!html.endsWith("</tr>\n"))
        html += "</tr>\n";
    html += "</table>\n";
    return html;
}


void PageData::populateDocumentUsingQTextCursor(
        QTextDocument *document)
{
    document->setDefaultFont(QFont("Times", 15));
    QTextCursor cursor(document);
    for (int page = 0; page < pages.count(); ++page) {
        addPageToDocument(&cursor, page);
        if (page + 1 < pages.count()) {
            QTextBlockFormat blockFormat;
            blockFormat.setPageBreakPolicy(
                    QTextFormat::PageBreak_AlwaysAfter);
            cursor.mergeBlockFormat(blockFormat);
        }
    }
}


void PageData::addPageToDocument(QTextCursor *cursor, int page)
{
    const OnePage &thePage = pages.at(page);

    addTitleToDocument(cursor, thePage.title);
    addItemsToDocument(cursor, thePage);
    cursor->insertHtml(thePage.descriptionHtml);
    addRuleToDocument(cursor);
}


void PageData::addTitleToDocument(QTextCursor *cursor,
                                  const QString &title)
{
    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignHCenter);
    blockFormat.setTopMargin(0);
    cursor->insertBlock(blockFormat);
    QTextCharFormat charFormat;
    charFormat.setFont(QFont("Helvetica", 24, QFont::Bold));
    cursor->insertText(title, charFormat);
}


void PageData::addItemsToDocument(QTextCursor *cursor,
                                  const OnePage &thePage)
{
    QTextDocument tableDocument;
    QTextCursor tableCursor(&tableDocument);
    QTextTable *table = tableCursor.insertTable(2, 2, tableFormat());
    for (int i = 0; i < thePage.filenames.count(); ++i)
        populateTableCell(table->cellAt(i / 2, i % 2), thePage, i);
    cursor->insertFragment(QTextDocumentFragment(&tableDocument));
}


QTextTableFormat PageData::tableFormat()
{
    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignCenter);
    tableFormat.setCellPadding(10);
    tableFormat.setTopMargin(10);
    tableFormat.setBottomMargin(10);
    QVector<QTextLength> widths;
    widths << QTextLength(QTextLength::PercentageLength, 50)
           << QTextLength(QTextLength::PercentageLength, 50);
    tableFormat.setColumnWidthConstraints(widths);
    return tableFormat;
}


void PageData::populateTableCell(QTextTableCell tableCell,
                                 const OnePage &thePage, int index)
{
    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignHCenter);
    QTextCursor cursor = tableCell.firstCursorPosition();
    cursor.insertBlock(blockFormat);
    cursor.insertImage(thePage.filenames.at(index));
    blockFormat.setTopMargin(30);
    cursor.insertBlock(blockFormat);
    QTextCharFormat charFormat;
    charFormat.setFont(QFont("Helvetica", 18));
    cursor.insertText(thePage.captions.at(index), charFormat);
}


void PageData::addRuleToDocument(QTextCursor *cursor)
{
    QTextBlockFormat blockFormat;
    blockFormat.setProperty(
            QTextFormat::BlockTrailingHorizontalRulerWidth, 1);
    cursor->insertBlock(blockFormat);
}
