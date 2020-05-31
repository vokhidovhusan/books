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

import codecs
import htmlentitydefs
import re
import sys
from PyQt4.QtCore import (QMutex, QThread, Qt, QString)
from PyQt4.QtCore import pyqtSignal as Signal


class Walker(QThread):

    finished = Signal(bool, int)
    indexed = Signal(QString, int)

    COMMON_WORDS_THRESHOLD = 250
    MIN_WORD_LEN = 3
    MAX_WORD_LEN = 25
    INVALID_FIRST_OR_LAST = frozenset("0123456789_")
    STRIPHTML_RE = re.compile(r"<[^>]*?>", re.IGNORECASE|re.MULTILINE)
    ENTITY_RE = re.compile(r"&(\w+?);|&#(\d+?);")
    SPLIT_RE = re.compile(r"\W+", re.IGNORECASE|re.MULTILINE)

    def __init__(self, index, lock, files, filenamesForWords,
                 commonWords, parent=None):
        super(Walker, self).__init__(parent)
        self.index = index
        self.lock = lock
        self.files = files
        self.filenamesForWords = filenamesForWords
        self.commonWords = commonWords
        self.stopped = False
        self.mutex = QMutex()
        self.completed = False


    def stop(self):
        try:
            self.mutex.lock()
            self.stopped = True
        finally:
            self.mutex.unlock()


    def isStopped(self):
        try:
            self.mutex.lock()
            return self.stopped
        finally:
            self.mutex.unlock()


    def run(self):
        self.processFiles()
        self.stop()
        self.finished.emit(self.completed, self.index)


    def processFiles(self):
        for fname in self.files:
            if self.isStopped():
                return
            self.processOneFile(fname)
        self.completed = True


    def processOneFile(self, fname):
        words = set()
        text = self.readFile(fname)
        if text is None or self.isStopped():
            return
        text = self.STRIPHTML_RE.sub("", text)
        text = self.ENTITY_RE.sub(unichrFromEntity, text)
        text = text.lower()
        for word in self.SPLIT_RE.split(text):
            if (self.MIN_WORD_LEN <= len(word) <=
                self.MAX_WORD_LEN and
                word[0] not in self.INVALID_FIRST_OR_LAST and
                word[-1] not in self.INVALID_FIRST_OR_LAST):
                try:
                    self.lock.lockForRead()
                    new = word not in self.commonWords
                finally:
                    self.lock.unlock()
                if new:
                    words.add(word)
        if self.isStopped():
            return
        for word in words:
            try:
                self.lock.lockForWrite()
                files = self.filenamesForWords[word]
                if len(files) > self.COMMON_WORDS_THRESHOLD:
                    del self.filenamesForWords[word]
                    self.commonWords.add(word)
                else:
                    files.add(unicode(fname))
            finally:
                self.lock.unlock()
        self.indexed.emit(fname, self.index)


    def readFile(self, fname):
        try:
            with codecs.open(fname, "r", "UTF8", "ignore") as file:
                return file.read()
        except (IOError, OSError), err:
            sys.stderr.write("Error: {0}\n".format(err))


def unichrFromEntity(match):
    text = match.group(match.lastindex)
    if text.isdigit():
        return unichr(int(text))
    u = htmlentitydefs.name2codepoint.get(text)
    return unichr(u) if u is not None else ""

