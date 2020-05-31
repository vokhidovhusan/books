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
from PyQt4.QtCore import (QFile, QString, QVariant, Qt)
from PyQt4.QtCore import pyqtSignal as Signal
from PyQt4.QtGui import (QApplication, QDialog, QDialogButtonBox, QMenu,
        QMessageBox, QTableView, QVBoxLayout)
from PyQt4.QtSql import (QSqlDatabase, QSqlQuery, QSqlTableModel)

MAC = True
try:
    from PyQt4.QtGui import qt_mac_set_native_menubar
except ImportError:
    MAC = False

ID, CATEGORY, SHORTDESC, LONGDESC = range(4)


class ReferenceDataDlg(QDialog):

    def __init__(self, parent=None):
        super(ReferenceDataDlg, self).__init__(parent)
        self.create_widgets()
        self.layout_widgets()
        self.create_connections()
        self.setWindowTitle("Reference Data")


    def create_widgets(self):
        self.model = QSqlTableModel(self)
        self.model.setTable("reference")
        self.model.setSort(ID, Qt.AscendingOrder)
        self.model.setHeaderData(ID, Qt.Horizontal, QVariant("ID"))
        self.model.setHeaderData(CATEGORY, Qt.Horizontal,
                QVariant("Category"))
        self.model.setHeaderData(SHORTDESC, Qt.Horizontal,
                QVariant("Short Desc."))
        self.model.setHeaderData(LONGDESC, Qt.Horizontal,
                QVariant("Long Desc."))
        self.model.select()

        self.view = QTableView()
        self.view.setModel(self.model)
        self.view.setSelectionMode(QTableView.SingleSelection)
        self.view.setSelectionBehavior(QTableView.SelectRows)
        self.view.setColumnHidden(ID, True)
        self.view.resizeColumnsToContents()

        self.buttonBox = QDialogButtonBox()
        self.addButton = self.buttonBox.addButton("&Add",
                QDialogButtonBox.ActionRole)
        self.deleteButton = self.buttonBox.addButton("&Delete",
                QDialogButtonBox.ActionRole)
        self.sortButton = self.buttonBox.addButton("&Sort",
                QDialogButtonBox.ActionRole)
        if not MAC:
            self.addButton.setFocusPolicy(Qt.NoFocus)
            self.deleteButton.setFocusPolicy(Qt.NoFocus)
            self.sortButton.setFocusPolicy(Qt.NoFocus)

        menu = QMenu(self)
        self.sortByCategoryAction = menu.addAction("Sort by &Category")
        self.sortByDescriptionAction = menu.addAction(
                "Sort by &Description")
        self.sortByIDAction = menu.addAction("Sort by &ID")
        self.sortButton.setMenu(menu)
        self.closeButton = self.buttonBox.addButton(QDialogButtonBox.Close)


    def layout_widgets(self):
        layout = QVBoxLayout()
        layout.addWidget(self.view)
        layout.addWidget(self.buttonBox)
        self.setLayout(layout)


    def create_connections(self):
        self.addButton.clicked.connect(self.addRecord)
        self.deleteButton.clicked.connect(self.deleteRecord)
        self.sortByCategoryAction.triggered.connect(
                lambda: self.sort(CATEGORY))
        self.sortByDescriptionAction.triggered.connect(
                lambda: self.sort(SHORTDESC))
        self.sortByIDAction.triggered.connect(lambda: self.sort(ID))
        self.closeButton.clicked.connect(self.accept)


    def addRecord(self):
        row = self.model.rowCount()
        self.model.insertRow(row)
        index = self.model.index(row, CATEGORY)
        self.view.setCurrentIndex(index)
        self.view.edit(index)


    def deleteRecord(self):
        index = self.view.currentIndex()
        if not index.isValid():
            return
        record = self.model.record(index.row())
        category = record.value(CATEGORY).toString()
        desc = record.value(SHORTDESC).toString()
        if (QMessageBox.question(self, "Reference Data",
                QString("Delete %1 from category %2?")
                .arg(desc).arg(category),
                QMessageBox.Yes|QMessageBox.No) ==
                QMessageBox.No):
            return
        self.model.removeRow(index.row())
        self.model.submitAll()


    def sort(self, column):
        self.model.setSort(column, Qt.AscendingOrder)
        self.model.select()


def main():
    app = QApplication(sys.argv)

    filename = os.path.join(os.path.dirname(__file__), "reference.db")
    create = not QFile.exists(filename)

    db = QSqlDatabase.addDatabase("QSQLITE")
    db.setDatabaseName(filename)
    if not db.open():
        QMessageBox.warning(None, "Reference Data",
            QString("Database Error: %1").arg(db.lastError().text()))
        sys.exit(1)

    if create:
        query = QSqlQuery()
        query.exec_("""CREATE TABLE reference (
                id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL,
                category VARCHAR(30) NOT NULL,
                shortdesc VARCHAR(20) NOT NULL,
                longdesc VARCHAR(80))""")

    form = ReferenceDataDlg()
    form.show()
    sys.exit(app.exec_())


main()

