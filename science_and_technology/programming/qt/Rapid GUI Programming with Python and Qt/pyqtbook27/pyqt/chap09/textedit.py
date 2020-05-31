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
from PyQt4.QtCore import (QFile, QFileInfo, QIODevice, QString,
        QTextStream, Qt)
from PyQt4.QtGui import (QFileDialog, QMessageBox, QTextEdit)


class TextEdit(QTextEdit):

    NextId = 1

    def __init__(self, filename=QString(), parent=None):
        super(TextEdit, self).__init__(parent)
        self.setAttribute(Qt.WA_DeleteOnClose)
        self.filename = filename
        if self.filename.isEmpty():
            self.filename = QString("Unnamed-{0}.txt".format(
                                    TextEdit.NextId))
            TextEdit.NextId += 1
        self.document().setModified(False)
        self.setWindowTitle(QFileInfo(self.filename).fileName())

    
    def closeEvent(self, event):
        if (self.document().isModified() and 
            QMessageBox.question(self,
                   "Text Editor - Unsaved Changes",
                   "Save unsaved changes in {0}?".format(self.filename),
                   QMessageBox.Yes|QMessageBox.No) ==
                QMessageBox.Yes):
            try:
                self.save()
            except (IOError, OSError), err:
                QMessageBox.warning(self,
                        "Text Editor -- Save Error",
                        "Failed to save {0}: {1}".format(self.filename, err))


    def isModified(self):
        return self.document().isModified()


    def save(self):
        if self.filename.startsWith("Unnamed"):
            filename = QFileDialog.getSaveFileName(self,
                    "Text Editor -- Save File As", self.filename,
                    "Text files (*.txt *.*)")
            if filename.isEmpty():
                return
            self.filename = filename
        self.setWindowTitle(QFileInfo(self.filename).fileName())
        with codecs.open(self.filename, "wt", encoding="utf-8") as file:
            file.write(self.toPlainText())
            self.document().setModified(False)


    def load(self):
        with codecs.open(self.filename, encoding="utf-8") as file:
            self.setPlainText(file.read())
            self.document().setModified(False)
