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

#include "xmlhighlighter.hpp"


XmlHighlighter::XmlHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    tagFormat.setForeground(Qt::darkBlue);
    attributeNameFormat.setForeground(Qt::blue);
    attributeValueFormat.setForeground(Qt::darkYellow);
    entityFormat.setForeground(Qt::darkRed);
    commentFormat.setForeground(Qt::darkGreen);
    commentFormat.setFontItalic(true);

    addRegex(Tag, "<[!?]?\\w+(?:/>)?", false);
    addRegex(Tag, "(?:</\\w+)?[?]?>");
    addRegex(Attribute, "(\\w+(?::\\w+)?)=(\"[^\"]+\"|'[^\']+')");
    addRegex(Entity, "&(:?#\\d+|\\w+);");
    addRegex(Comment, "<!--.*-->");
}


void XmlHighlighter::addRegex(Type type, const QString &pattern,
                              bool minimal)
{
    QRegExp regex(pattern);
    regex.setPatternSyntax(QRegExp::RegExp2);
    regex.setMinimal(minimal);
    regexForType.insert(type, regex);
}


void XmlHighlighter::highlightBlock(const QString &text)
{
    setCurrentBlockState(Normal);

    highlightPatterns(text);
    highlightComments(text);
}


void XmlHighlighter::highlightPatterns(const QString &text)
{
    QHashIterator<Type, QRegExp> i(regexForType);
    while (i.hasNext()) {
        i.next();
        Type type = i.key();
        const QRegExp &regex = i.value();
        int index = regex.indexIn(text);
        while (index > -1) {
            int length = regex.matchedLength();
            if (type == Tag)
                setFormat(index, length, tagFormat);
            else if (type == Attribute) {
                setFormat(index, regex.pos(2) - index - 1,
                          attributeNameFormat);
                setFormat(regex.pos(2) + 1, regex.cap(2).length() - 2,
                          attributeValueFormat);
            }
            else if (type == Entity)
                setFormat(index, length, entityFormat);
            else if (type == Comment)
                setFormat(index, length, commentFormat);
            index = regex.indexIn(text, index + length);
        }
    }
}


void XmlHighlighter::highlightComments(const QString &text)
{
    const QString StartOfComment("<!--");
    const QString EndOfComment("-->");

    if (previousBlockState() > -1 &&
        (previousBlockState() & InComment) == InComment) {
        int end = text.indexOf(EndOfComment);
        if (end == -1) {
            setFormat(0, text.length(), commentFormat);
            setCurrentBlockState(currentBlockState() | InComment);
            return;
        }
        else
            setFormat(0, end + EndOfComment.length(), commentFormat);
    }

    int start = text.lastIndexOf(StartOfComment);
    if (start != -1) {
        int end = text.lastIndexOf(EndOfComment);
        if (end < start) {
            setFormat(start, text.length(), commentFormat);
            setCurrentBlockState(currentBlockState() | InComment);
        }
    }
}
