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
import os
import re
import sys
from PyQt4.QtCore import (QMutex, QMutexLocker, QReadLocker, QThread,
        QWriteLocker, Qt, QString)
from PyQt4.QtCore import pyqtSignal as Signal


class Walker(QThread):

    finished = Signal(bool)
    indexed = Signal(QString)

    COMMON_WORDS_THRESHOLD = 250
    MIN_WORD_LEN = 3
    MAX_WORD_LEN = 25
    INVALID_FIRST_OR_LAST = frozenset("0123456789_")
    STRIPHTML_RE = re.compile(r"<[^>]*?>", re.IGNORECASE|re.MULTILINE)
    ENTITY_RE = re.compile(r"&(\w+?);|&#(\d+?);")
    SPLIT_RE = re.compile(r"\W+", re.IGNORECASE|re.MULTILINE)

    def __init__(self, lock, parent=None):
        super(Walker, self).__init__(parent)
        self.lock = lock
        self.stopped = False
        self.mutex = QMutex()
        self.path = None
        self.completed = False


    def initialize(self, path, filenamesForWords, commonWords):
        self.stopped = False
        self.path = path
        self.filenamesForWords = filenamesForWords
        self.commonWords = commonWords
        self.completed = False


    def stop(self):
        with QMutexLocker(self.mutex):
            self.stopped = True


    def isStopped(self):
        with QMutexLocker(self.mutex):
            return self.stopped


    def run(self):
        self.processFiles(self.path)
        self.stop()
        self.finished.emit(self.completed)


    def processFiles(self, path):
        for root, dirs, files in os.walk(path):
            if self.isStopped():
                return
            for name in [name for name in files
                         if name.endswith((".htm", ".html"))]:
                fname = os.path.join(root, name)
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
                with QReadLocker(self.lock):
                    new = word not in self.commonWords
                if new:
                    words.add(word)
        if self.isStopped():
            return
        for word in words:
            with QWriteLocker(self.lock):
                files = self.filenamesForWords[word]
                if len(files) > self.COMMON_WORDS_THRESHOLD:
                    del self.filenamesForWords[word]
                    self.commonWords.add(word)
                else:
                    files.add(unicode(fname))
        self.indexed.emit(fname)


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

