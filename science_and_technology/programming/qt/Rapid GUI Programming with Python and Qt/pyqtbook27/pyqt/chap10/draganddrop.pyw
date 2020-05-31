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

import os
import sys
from PyQt4.QtCore import Qt
from PyQt4.QtGui import (QApplication, QDialog, QHBoxLayout, QIcon,
        QListWidget, QListWidgetItem, QSplitter, QTableWidget)


class Form(QDialog):

    def __init__(self, parent=None):
        super(Form, self).__init__(parent)
        self.create_widgets()
        self.layout_widgets()
        self.setWindowTitle("Drag and Drop")


    def create_widgets(self):
        self.listWidget = QListWidget()
        self.listWidget.setAcceptDrops(True)
        self.listWidget.setDragEnabled(True)
        path = os.path.dirname(__file__)
        for image in sorted(os.listdir(os.path.join(path, "images"))):
            if image.endswith(".png"):
                item = QListWidgetItem(image.split(".")[0].capitalize())
                item.setIcon(QIcon(os.path.join(path,
                                   "images/{0}".format(image))))
                self.listWidget.addItem(item)
        self.iconListWidget = QListWidget()
        self.iconListWidget.setAcceptDrops(True)
        self.iconListWidget.setDragEnabled(True)
        self.iconListWidget.setViewMode(QListWidget.IconMode)
        self.tableWidget = QTableWidget()
        self.tableWidget.setRowCount(5)
        self.tableWidget.setColumnCount(2)
        self.tableWidget.setHorizontalHeaderLabels(["Column #1",
                "Column #2"])
        self.tableWidget.setAcceptDrops(True)
        self.tableWidget.setDragEnabled(True)


    def layout_widgets(self):
        splitter = QSplitter(Qt.Horizontal)
        splitter.addWidget(self.listWidget)
        splitter.addWidget(self.iconListWidget)
        splitter.addWidget(self.tableWidget)
        layout = QHBoxLayout()
        layout.addWidget(splitter)
        self.setLayout(layout)



if __name__ == "__main__":
    app = QApplication(sys.argv)
    form = Form()
    form.show()
    app.exec_()
