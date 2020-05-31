#ifndef PAGEDATA_HPP
#define PAGEDATA_HPP
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

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextTableCell>


class QFont;
class QPainter;
class QPrinter;
class QRect;
class QTextCharFormat;
class QTextCursor;
class QTextDocument;


struct OnePage
{
    QString title;
    QStringList filenames;
    QStringList captions;
    QString descriptionHtml;
};


class PageData : public QObject
{
    Q_OBJECT

public:
    explicit PageData();

public slots:
    void paintPages(QPrinter *printer, bool noUserInteraction=true);

public:
    void paintPage(QPainter *painter, int page);
    void populateDocumentUsingHtml(QTextDocument *document);
    QString pageAsHtml(int page=0, bool selfContained=true);
    void populateDocumentUsingQTextCursor(QTextDocument *document);
    void addPageToDocument(QTextCursor *cursor, int page=0);

private:
    int paintTitle(QPainter *painter, const QString &title);
    int paintItems(QPainter *painter, int y, const OnePage &thePage);
    void paintItem(QPainter *painter, const QRect &rect,
                   const QString &filename, const QString &caption);
    int paintHtmlParagraph(QPainter *painter, int y,
                           const QString &html);
    int paintWord(QPainter *painter, int x, int y,
                  const QString &word, const QFont &paragraphFont,
                  const QTextCharFormat &format);
    void paintFooter(QPainter *painter, const QString &footer);
    QString itemsAsHtmlTable(const OnePage &thePage);
    void addTitleToDocument(QTextCursor *cursor, const QString &title);
    void addItemsToDocument(QTextCursor *cursor,
                            const OnePage &thePage);
    QTextTableFormat tableFormat();
    void populateTableCell(QTextTableCell tableCell,
                           const OnePage &thePage, int index);
    void addRuleToDocument(QTextCursor *cursor);

    QList<OnePage> pages;
};



#endif // PAGEDATA_HPP
