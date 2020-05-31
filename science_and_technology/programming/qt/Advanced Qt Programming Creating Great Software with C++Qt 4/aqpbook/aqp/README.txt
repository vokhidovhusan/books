Advanced Qt Programming by Mark Summerfield

ISBN: 978-0321635907

All the example programs and modules are copyright (c) Qtrac Ltd. 2009-11.
They are free software: you can redistribute them and/or modify them
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version version 3 of the License, or
(at your option) any later version. They are provided for educational
purposes and are distributed in the hope that they will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public Licenses (in file gpl-3.0.txt) for more details.

All the book's examples are designed to be educational, and many are
also designed to be useful. I hope that you find them helpful, and are
perhaps able to use some of them as starting points for your own
projects.

Most of the icons and sounds are from KDE (The `K' Desktop Environment),
and come under KDE's LGPL license. (Visit http:///www.kde.org for more
information.)

The entire set of examples can be built in one go by running qmake and
then make (or nmake) in their top level directory---the directory that
contains examples.pro.

Note that the Alt_Key library's documentation is here:
http://www.qtrac.eu/alt_key_api.html

Here is the list of programs referred to in the book grouped by chapter:

Chapter 1: Hybrid Desktop/Internet Applications
    weathertrayicon
    rsspanel
    browserwindow
    nyrbviewer
    matrixquiz

Chapter 2: Audio and Video
    moviejingle
    playmusic
    playvideo

Chapter 3: Model/View Table Models
    zipcodes1
    zipcodes2

Chapter 4: Model/View Tree Models
    timelog1
    timelog2

Chapter 5: Model/View Delegates
    folderview
    timelog1
    zipcodes1

Chapter 6: Model/View Views
    censusvisualizer
    tiledlistview [1]

Chapter 7: Threading with QtConcurrent
    image2image
    numbergrid

Chapter 8: Threading with QThread
    crossfader
    findduplicates

Chapter 9: Creating Rich Text Editors
    findduplicates
    xmledit
    timelog1
    textedit

Chapter 10: Creating Rich Text Documents
    outputsampler

Chapter 11: Creating Graphics/View Windows
    petridish1

Chapter 12: Creating Graphics/View Scenes
    pagedesigner1 [2]

Chapter 13: The Animation and State Machine Frameworks
    finddialog
    petridish2
    pagedesigner2 [2]

STOP PRESS:
[1] The TiledListView::calculateRectsIfNecessary() function does not
    play nicely with SQL-based models whose underlying database doesn't
    report its query size. I've now incorporated an alternative
    implementation (which can be switched on by uncommenting the DEFINE
    line in the .pro file) that solves this problem and which is almost
    the same as Johann's suggested solution.
[2] I have now added an alternative implementation of the
    BoxItem::mouseMoveEvent() handler for Page Designer. By default the
    original implementation is used for pagedesigner1 and the
    alternative one for pagedesigner2.

COMPILER NOTE:
If you get compiler errors regarding std::cos() or std::sin() with a
Microsoft compiler add:
    DEFINES += MSVC_COMPILER
to the affected .pro files (i.e., folderview/folderview.pro,
petridish1/petridish1.pro, and petridish2/petridish2.pro).
If you have a better workaround please let me know.

CREDITS:
Thanks to Rémi Faitout for fixes to make the examples build with MSVC8.
Thanks to Jim Clase for spotting and fixing a subtle bug in timelog1.
Thanks to Thomas Gasser for reporting errors when compiling with MSVC9.
Thanks to Johann Anhofer for spotting the problem with SQL-based models
    in the tiledlistview example and for providing a solution.
