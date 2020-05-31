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

from PyQt4.QtCore import (QUrl, Qt)
from PyQt4.QtCore import pyqtSignal as Signal
from PyQt4.QtGui import (QAction, QApplication, QDialog, QIcon,
        QKeySequence, QLabel, QTextBrowser, QToolBar, QVBoxLayout)
import qrc_resources


class HelpForm(QDialog):

    def __init__(self, page, parent=None):
        super(HelpForm, self).__init__(parent)
        self.setAttribute(Qt.WA_DeleteOnClose)
        self.setAttribute(Qt.WA_GroupLeader)
        self.create_widgets()
        self.layout_widgets()
        self.create_connections()
        self.textBrowser.setSearchPaths([":/help"])
        self.textBrowser.setSource(QUrl(page))
        self.resize(400, 600)
        self.setWindowTitle("{0} Help".format(
                QApplication.applicationName()))


    def create_widgets(self):
        self.backAction = QAction(QIcon(":/back.png"), "&Back", self)
        self.backAction.setShortcut(QKeySequence.Back)
        self.homeAction = QAction(QIcon(":/home.png"), "&Home", self)
        self.homeAction.setShortcut("Home")
        self.pageLabel = QLabel()

        self.toolBar = QToolBar()
        self.toolBar.addAction(self.backAction)
        self.toolBar.addAction(self.homeAction)
        self.toolBar.addWidget(self.pageLabel)
        self.textBrowser = QTextBrowser()


    def layout_widgets(self):
        layout = QVBoxLayout()
        layout.addWidget(self.toolBar)
        layout.addWidget(self.textBrowser, 1)
        self.setLayout(layout)


    def create_connections(self):
        self.backAction.triggered.connect(self.textBrowser.backward)
        self.homeAction.triggered.connect(self.textBrowser.home)
        self.textBrowser.sourceChanged.connect(self.updatePageTitle)


    def updatePageTitle(self):
        self.pageLabel.setText(self.textBrowser.documentTitle())


if __name__ == "__main__":
    import sys

    app = QApplication(sys.argv)
    form = HelpForm("index.html")
    form.show()
    app.exec_()

