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

#include "option_parser.hpp"
#include <QCoreApplication>
#include <QTextStream>


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QTextStream out(stdout);
    AQP::OptionParser parser(app.arguments(),
            "usage: {program} [options] <files>\n"
            "\nA program to do various things.\n",
            "\nVersion 1.0.0 Copyright (c) 2009-10 Qtrac Ltd. "
            "All rights reserved.");
    AQP::BooleanOptionPtr verboseOpt =
            parser.addBooleanOption("v", "verbose");
    verboseOpt->setDefaultValue(false);
    AQP::StringOptionPtr infileOpt = parser.addStringOption(
            "i", "infile");
    infileOpt->setHelp("input file");
    AQP::StringOptionPtr outfileOpt = parser.addStringOption(
            "o", "outfile");
    outfileOpt->setRequired();
    outfileOpt->setHelp("output file");
    AQP::IntegerOptionPtr limitOpt = parser.addIntegerOption(
            "l", "limit");
    limitOpt->setRequired();
    limitOpt->setHelp("limiting factor");
    limitOpt->setDefaultValue(50);
    limitOpt->setMaximum(1000);
    AQP::IntegerOptionPtr widthOpt = parser.addIntegerOption(
            "w", "width");
    widthOpt->setHelp("page width");
    widthOpt->setDefaultValue(100);
    widthOpt->setRange(200, 500);
    AQP::IntegerOptionPtr heightOpt = parser.addIntegerOption(
            QString(), "height");
    heightOpt->setHelp("page height");
    heightOpt->setDefaultValue(200);
    heightOpt->setAcceptableValues(QSet<int>() << 100 << 150 << 200
            << 250 << 300 << 350);
    AQP::IntegerOptionPtr depthOpt = parser.addIntegerOption(
            "d", "depth");
    depthOpt->setHelp("the depth factor which can be pretty well "
                      "anything");
    AQP::RealOptionPtr pointSizeOpt = parser.addRealOption(
            "p", "pointsize");
    pointSizeOpt->setHelp("size in points");
    pointSizeOpt->setRange(6.0, 144.0);
    AQP::RealOptionPtr significanceOpt =
            parser.addRealOption("s");
    significanceOpt->setHelp("significance");
    AQP::StringOptionPtr colorOpt = parser.addStringOption(
            QString(), "color");
    colorOpt->setHelp("color name");
    colorOpt->setAcceptableValues(QStringList() << "red" << "green"
                                  << "blue");
    AQP::IntegerOptionPtr sizeOpt = parser.addIntegerOption(
            QString(), "size");
    sizeOpt->setHelp("size");
    sizeOpt->setMinimum(5);
    AQP::StringOptionPtr moodOpt = parser.addStringOption("m", "mood");
    moodOpt->setHelp("mood name");
    moodOpt->setDefaultValue("Happy");
    moodOpt->setAcceptableValues(QStringList() << "Happy" << "Sad"
                                  << "Neutral" << "Gloomy" << "Joyful");
    if (!parser.parse())
        return 2;

    out
        << "colour="
        << (colorOpt->hasValue() ? colorOpt->value() : "") << "\n"
        << "depth="
        << (depthOpt->hasValue() ? QString::number(depthOpt->value())
                                 : "") << "\n"
        << "height="
        << (heightOpt->hasValue() ? QString::number(heightOpt->value())
                                  : "") << "\n"
        << "infile="
        << (parser.hasValue("infile") ? parser.string("infile") : "")
        << "\n"
        << "limit=" << parser.integer("limit") << "\n"
        << "mood=" << parser.string("mood") << "\n"
        << "outfile=" << parser.string("outfile") << "\n"
        << "pointsize="
        << (parser.hasValue("pointsize")
                ? QString::number(parser.real("pointsize")) : "")
        << "\n"
        << "s=" << (parser.hasValue("s")
                ? QString::number(parser.real("s")) : "") << "\n"
        << "size="
        << (sizeOpt->hasValue() ? QString::number(sizeOpt->value())
                                : "") << "\n"
        << "verbose="
        << (parser.hasValue("verbose")
                ? (parser.boolean("verbose") ? "True" : "False") : "")
        << "\n"
        << "width="
        << (parser.hasValue("width")
                ? QString::number(parser.integer("width")) : "")
        << "\n"
        ;

    QStringList remainder = parser.remainder();
    if (remainder.isEmpty())
        out << "* no other arguments\n";
    else {
        out << "* other arguments: ";
        foreach (const QString &arg, remainder)
            out << arg << " ";
        out << "\n";
    }

    return 0;
}
