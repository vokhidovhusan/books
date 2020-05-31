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

import sys
from PyQt4.QtCore import (QFile, QString, QTimer, Qt)
from PyQt4.QtCore import pyqtSignal as Signal
from PyQt4.QtGui import (QApplication, QDialog, QHBoxLayout, QLabel,
        QMessageBox, QPushButton, QSplitter, QTableView, QVBoxLayout,
        QWidget)
import ships

MAC = True
try:
    from PyQt4.QtGui import qt_mac_set_native_menubar
except ImportError:
    MAC = False


class MainForm(QDialog):

    def __init__(self, parent=None):
        super(MainForm, self).__init__(parent)
        self.model = ships.ShipTableModel(QString("ships.dat"))
        self.create_widgets()
        self.layout_widgets()
        self.create_connections()
        self.setWindowTitle("Ships (delegate)")
        QTimer.singleShot(0, self.initialLoad)


    def create_widgets(self):
        self.tableLabel1 = QLabel("Table &1")
        self.tableView1 = QTableView()
        self.tableLabel1.setBuddy(self.tableView1)
        self.tableView1.setModel(self.model)
        self.tableView1.setItemDelegate(ships.ShipDelegate(self))
        self.tableLabel2 = QLabel("Table &2")
        self.tableView2 = QTableView()
        self.tableLabel2.setBuddy(self.tableView2)
        self.tableView2.setModel(self.model)
        self.tableView2.setItemDelegate(ships.ShipDelegate(self))

        self.addShipButton = QPushButton("&Add Ship")
        self.removeShipButton = QPushButton("&Remove Ship")
        self.quitButton = QPushButton("&Quit")
        if not MAC:
            self.addShipButton.setFocusPolicy(Qt.NoFocus)
            self.removeShipButton.setFocusPolicy(Qt.NoFocus)
            self.quitButton.setFocusPolicy(Qt.NoFocus)


    def layout_widgets(self):
        buttonLayout = QHBoxLayout()
        buttonLayout.addWidget(self.addShipButton)
        buttonLayout.addWidget(self.removeShipButton)
        buttonLayout.addStretch()
        buttonLayout.addWidget(self.quitButton)
        splitter = QSplitter(Qt.Horizontal)
        vbox = QVBoxLayout()
        vbox.addWidget(self.tableLabel1)
        vbox.addWidget(self.tableView1)
        widget = QWidget()
        widget.setLayout(vbox)
        splitter.addWidget(widget)
        vbox = QVBoxLayout()
        vbox.addWidget(self.tableLabel2)
        vbox.addWidget(self.tableView2)
        widget = QWidget()
        widget.setLayout(vbox)
        splitter.addWidget(widget)
        layout = QVBoxLayout()
        layout.addWidget(splitter)
        layout.addLayout(buttonLayout)
        self.setLayout(layout)


    def create_connections(self):
        for tableView in (self.tableView1, self.tableView2):
            header = tableView.horizontalHeader()
            header.sectionClicked.connect(self.sortTable)
        self.addShipButton.clicked.connect(self.addShip)
        self.removeShipButton.clicked.connect(self.removeShip)
        self.quitButton.clicked.connect(self.accept)


    def initialLoad(self):
        if not QFile.exists(self.model.filename):
            for ship in ships.generateFakeShips():
                self.model.ships.append(ship)
                self.model.owners.add(unicode(ship.owner))
                self.model.countries.add(unicode(ship.country))
            self.model.reset()
            self.model.dirty = False
        else:
            try:
                self.model.load()
            except IOError, err:
                QMessageBox.warning(self, "Ships - Error",
                        "Failed to load: {0}".format(err))
        self.model.sortByName()
        self.resizeColumns()


    def resizeColumns(self):
        self.tableView1.resizeColumnsToContents()
        self.tableView2.resizeColumnsToContents()


    def reject(self):
        self.accept()


    def accept(self):
        if (self.model.dirty and
            QMessageBox.question(self, "Ships - Save?",
                    "Save unsaved changes?",
                    QMessageBox.Yes|QMessageBox.No) ==
                    QMessageBox.Yes):
            try:
                self.model.save()
            except IOError, err:
                QMessageBox.warning(self, "Ships - Error",
                        "Failed to save: {0}".format(err))
        QDialog.accept(self)

    
    def sortTable(self, section):
        if section in (ships.OWNER, ships.COUNTRY):
            self.model.sortByCountryOwner()
        else:
            self.model.sortByName()
        self.resizeColumns()


    def addShip(self):
        row = self.model.rowCount()
        self.model.insertRows(row)
        index = self.model.index(row, 0)
        tableView = self.tableView1
        if self.tableView2.hasFocus():
            tableView = self.tableView2
        tableView.setFocus()
        tableView.setCurrentIndex(index)
        tableView.edit(index)


    def removeShip(self):
        tableView = self.tableView1
        if self.tableView2.hasFocus():
            tableView = self.tableView2
        index = tableView.currentIndex()
        if not index.isValid():
            return
        row = index.row()
        name = self.model.data(
                    self.model.index(row, ships.NAME)).toString()
        owner = self.model.data(
                    self.model.index(row, ships.OWNER)).toString()
        country = self.model.data(
                    self.model.index(row, ships.COUNTRY)).toString()
        if (QMessageBox.question(self, "Ships - Remove", 
                (QString("Remove %1 of %2/%3?").arg(name).arg(owner)
                        .arg(country)),
                QMessageBox.Yes|QMessageBox.No) ==
                QMessageBox.No):
            return
        self.model.removeRows(row)
        self.resizeColumns()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    form = MainForm()
    form.show()
    app.exec_()
