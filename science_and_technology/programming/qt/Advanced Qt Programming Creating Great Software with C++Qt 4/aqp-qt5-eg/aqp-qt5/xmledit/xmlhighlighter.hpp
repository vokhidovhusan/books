#ifndef XMLHIGHLIGHTER_HPP
#define XMLHIGHLIGHTER_HPP
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
#include <QRegExp>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>


class XmlHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit XmlHighlighter(QTextDocument *parent=0);

protected:
    void highlightBlock(const QString &text);

private:
    enum State {Normal=0x01, InComment=0x02};
    enum Type {Tag, Attribute, Entity, Comment};

    void addRegex(Type type, const QString &pattern,
                  bool minimal=true);

    void highlightPatterns(const QString &text);
    void highlightComments(const QString &text);

    QTextCharFormat tagFormat;
    QTextCharFormat attributeNameFormat;
    QTextCharFormat attributeValueFormat;
    QTextCharFormat entityFormat;
    QTextCharFormat commentFormat;
    QMultiHash<Type, QRegExp> regexForType;
};

#endif // XMLHIGHLIGHTER_HPP
