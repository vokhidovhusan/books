#!/usr/bin/env python
# Copyright (c) 2008-14 Qtrac Ltd. All rights reserved.
# This program or module is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
# by the Free Software Foundation, either version 2 of the License, or
# version 3 of the License, or (at your option) any later version. It is
# provided for educational purposes and is distributed in the hope that
# it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
# the GNU General Public License for more details.

from __future__ import division
from __future__ import print_function
from __future__ import unicode_literals
from future_builtins import *

import collections
import sys
from PyQt4.QtCore import (QDir, QReadLocker, QReadWriteLock, Qt)
from PyQt4.QtCore import pyqtSignal as Signal
from PyQt4.QtGui import (QApplication, QDialog, QFileDialog, QFrame,
        QHBoxLayout, QLCDNumber, QLabel, QLineEdit, QListWidget,
        QPushButton, QVBoxLayout)
import walker_27 as walker


class Form(QDialog):

    def __init__(self, parent=None):
        super(Form, self).__init__(parent)
        self.fileCount = 0
        self.filenamesForWords = collections.defaultdict(set)
        self.commonWords = set()
        self.lock = QReadWriteLock()
        self.path = QDir.homePath()
        self.create_widgets()
        self.layout_widgets()
        self.create_connections()
        self.setWindowTitle("Page Indexer")


    def create_widgets(self):
        self.pathLabel = QLabel("Indexing path:")
        self.pathResultLabel = QLabel()
        self.pathResultLabel.setFrameStyle(QFrame.StyledPanel|QFrame.Sunken)
        self.pathButton = QPushButton("Set &Path...")
        self.pathButton.setAutoDefault(False)
        self.findLabel = QLabel("&Find word:")
        self.findEdit = QLineEdit()
        self.findLabel.setBuddy(self.findEdit)
        self.commonWordsLabel = QLabel("&Common words:")
        self.commonWordsListWidget = QListWidget()
        self.commonWordsLabel.setBuddy(self.commonWordsListWidget)
        self.filesLabel = QLabel("Files containing the &word:")
        self.filesListWidget = QListWidget()
        self.filesLabel.setBuddy(self.filesListWidget)
        self.filesIndexedLabel = QLabel("Files indexed")
        self.filesIndexedLCD = QLCDNumber()
        self.filesIndexedLCD.setSegmentStyle(QLCDNumber.Flat)
        self.wordsIndexedLabel = QLabel("Words indexed")
        self.wordsIndexedLCD = QLCDNumber()
        self.wordsIndexedLCD.setSegmentStyle(QLCDNumber.Flat)
        self.commonWordsLCDLabel = QLabel("Common words")
        self.commonWordsLCD = QLCDNumber()
        self.commonWordsLCD.setSegmentStyle(QLCDNumber.Flat)
        self.statusLabel = QLabel("Click the 'Set Path' "
                                  "button to start indexing")
        self.statusLabel.setFrameStyle(QFrame.StyledPanel|QFrame.Sunken)


    def layout_widgets(self):
        topLayout = QHBoxLayout()
        topLayout.addWidget(self.pathLabel)
        topLayout.addWidget(self.pathResultLabel, 1)
        topLayout.addWidget(self.pathButton)
        topLayout.addWidget(self.findLabel)
        topLayout.addWidget(self.findEdit, 1)
        leftLayout = QVBoxLayout()
        leftLayout.addWidget(self.filesLabel)
        leftLayout.addWidget(self.filesListWidget)
        rightLayout = QVBoxLayout()
        rightLayout.addWidget(self.commonWordsLabel)
        rightLayout.addWidget(self.commonWordsListWidget)
        middleLayout = QHBoxLayout()
        middleLayout.addLayout(leftLayout, 1)
        middleLayout.addLayout(rightLayout)
        bottomLayout = QHBoxLayout()
        bottomLayout.addWidget(self.filesIndexedLabel)
        bottomLayout.addWidget(self.filesIndexedLCD)
        bottomLayout.addWidget(self.wordsIndexedLabel)
        bottomLayout.addWidget(self.wordsIndexedLCD)
        bottomLayout.addWidget(self.commonWordsLCDLabel)
        bottomLayout.addWidget(self.commonWordsLCD)
        bottomLayout.addStretch()
        layout = QVBoxLayout()
        layout.addLayout(topLayout)
        layout.addLayout(middleLayout)
        layout.addLayout(bottomLayout)
        layout.addWidget(self.statusLabel)
        self.setLayout(layout)


    def create_connections(self):
        self.walker = walker.Walker(self.lock, self)
        self.walker.indexed.connect(self.indexed)
        self.walker.finished.connect(self.finished)
        self.pathButton.clicked.connect(self.setPath)
        self.findEdit.returnPressed.connect(self.find)


    def setPath(self):
        self.pathButton.setEnabled(False)
        if self.walker.isRunning():
            self.walker.stop()
            self.walker.wait()
        path = QFileDialog.getExistingDirectory(self,
                    "Choose a Path to Index", self.path)
        if path.isEmpty():
            self.statusLabel.setText("Click the 'Set Path' "
                                     "button to start indexing")
            self.pathButton.setEnabled(True)
            return
        self.path = QDir.toNativeSeparators(path)
        self.findEdit.setFocus()
        self.pathResultLabel.setText(self.path)
        self.statusLabel.clear()
        self.filesListWidget.clear()
        self.fileCount = 0
        self.filenamesForWords = collections.defaultdict(set)
        self.commonWords = set()
        self.walker.initialize(unicode(self.path),
                self.filenamesForWords, self.commonWords)
        self.walker.start()


    def find(self):
        word = unicode(self.findEdit.text())
        if not word:
            self.statusLabel.setText("Enter a word to find in files")
            return
        self.statusLabel.clear()
        self.filesListWidget.clear()
        word = word.lower()
        if " " in word:
            word = word.split()[0]
        with QReadLocker(self.lock):
            found = word in self.commonWords
        if found:
            self.statusLabel.setText(
                    "Common words like '{0}' are not indexed".format(word))
            return
        with QReadLocker(self.lock):
            files = self.filenamesForWords.get(word, set()).copy()
        if not files:
            self.statusLabel.setText(
                    "No indexed file contains the word '{0}'".format(word))
            return
        files = [QDir.toNativeSeparators(name) for name in
                 sorted(files, key=unicode.lower)]
        self.filesListWidget.addItems(files)
        self.statusLabel.setText(
                "{0} indexed files contain the word '{1}'".format(
                len(files), word))


    def indexed(self, fname):
        self.statusLabel.setText(fname)
        self.fileCount += 1
        if self.fileCount % 25 == 0:
            self.filesIndexedLCD.display(self.fileCount)
            with QReadLocker(self.lock):
                indexedWordCount = len(self.filenamesForWords)
                commonWordCount = len(self.commonWords)
            self.wordsIndexedLCD.display(indexedWordCount)
            self.commonWordsLCD.display(commonWordCount)
        elif self.fileCount % 101 == 0:
            self.commonWordsListWidget.clear()
            with QReadLocker(self.lock):
                words = self.commonWords.copy()
            self.commonWordsListWidget.addItems(sorted(words))


    def finished(self, completed):
        self.statusLabel.setText("Indexing complete"
                                 if completed else "Stopped")
        self.finishedIndexing()


    def reject(self):
        if self.walker.isRunning():
            self.walker.stop()
            self.finishedIndexing()
        else:
            self.accept()


    def closeEvent(self, event=None):
        self.walker.stop()
        self.walker.wait()


    def finishedIndexing(self):
        self.walker.wait()
        self.filesIndexedLCD.display(self.fileCount)
        self.wordsIndexedLCD.display(len(self.filenamesForWords))
        self.commonWordsLCD.display(len(self.commonWords))
        self.pathButton.setEnabled(True)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    form = Form()
    form.show()
    app.exec_()
