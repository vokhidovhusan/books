#!/usr/bin/env python
# Copyright (c) 2007-14 Qtrac Ltd. All rights reserved.
# This program or module is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
# by the Free Software Foundation, either version 2 of the License, or
# version 3 of the License, or (at your option) any later version. It is
# provided for educational purposes and is distributed in the hope that
# it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
# the GNU General Public License for more details.

import sys
from PyQt4.QtCore import *
Signal = pyqtSignal
from PyQt4.QtGui import *

MAC = "qt_mac_set_native_menubar" in dir()


class StringListDlg(QDialog):

    acceptedList = Signal(QStringList)

    def __init__(self, name, stringlist=None, parent=None):
        super(StringListDlg, self).__init__(parent)
        self.name = name
        self.sortOrder = Qt.AscendingOrder
        self.create_widgets(stringlist)
        self.layout_widgets()
        self.setWindowTitle("Edit {0} List".format(self.name))


    def create_widgets(self, stringlist):
        self.listWidget = QListWidget()
        self.stringlist = QStringList()
        if stringlist is not None:
            self.listWidget.addItems(stringlist)
            self.listWidget.setCurrentRow(0)
            self.stringlist = stringlist[:]


    def layout_widgets(self): 
        buttonLayout = QVBoxLayout()
        for text, slot in (("&Add...", self.add),
                           ("&Edit...", self.edit),
                           ("&Remove...", self.remove),
                           ("&Up", self.up),
                           ("&Down", self.down),
                           ("&Sort", self.sort),
                           ("Close", self.accept)):
            button = QPushButton(text)
            if not MAC:
                button.setFocusPolicy(Qt.NoFocus)
            if text == "Close":
                buttonLayout.addStretch()
            buttonLayout.addWidget(button)
            button.clicked.connect(slot)
        layout = QHBoxLayout()
        layout.addWidget(self.listWidget)
        layout.addLayout(buttonLayout)
        self.setLayout(layout)


    def add(self):
        row = self.listWidget.currentRow()
        title = "Add {0}".format(self.name)
        string, ok = QInputDialog.getText(self, title, title)
        if ok and not string.isEmpty():
            self.listWidget.insertItem(row, string)


    def edit(self):
        row = self.listWidget.currentRow()
        item = self.listWidget.item(row)
        if item is not None:
            title = "Edit {0}".format(self.name)
            string, ok = QInputDialog.getText(self, title, title,
                    QLineEdit.Normal, item.text())
            if ok and not string.isEmpty():
                item.setText(string)


    def remove(self):
        row = self.listWidget.currentRow()
        item = self.listWidget.item(row)
        if item is None:
            return
        reply = QMessageBox.question(self, "Remove {0}".format(
                self.name), "Remove {0} `{1}'?".format(
                self.name, unicode(item.text())),
                QMessageBox.Yes|QMessageBox.No)
        if reply == QMessageBox.Yes:
            item = self.listWidget.takeItem(row)
            del item


    def up(self):
        row = self.listWidget.currentRow()
        if row >= 1:
            thisItem = self.listWidget.item(row)
            prevItem = self.listWidget.item(row - 1)
            text = item.text()
            item.setText(prevItem.text())
            prevItem.setText(text)
            self.listWidget.setCurrentItem(prevItem)


    def down(self):
        row = self.listWidget.currentRow()
        if row < self.listWidget.count() - 1:
            thisItem = self.listWidget.item(row)
            nextItem = self.listWidget.item(row + 1)
            text = thisItem.text()
            thisItem.setText(nextItem.text())
            nextItem.setText(text)
            self.listWidget.setCurrentItem(nextItem)


    def sort(self):
        self.listWidget.sortItems(self.sortOrder)
        if self.sortOrder == Qt.AscendingOrder:
            self.sortOrder = Qt.DescendingOrder
        else:
            self.sortOrder = Qt.AscendingOrder


    def reject(self):
        self.accept()


    def accept(self):
        self.stringlist = QStringList()
        for row in range(self.listWidget.count()):
            self.stringlist.append(self.listWidget.item(row).text())
        self.acceptedList.emit(self.stringlist)
        QDialog.accept(self)


if __name__ == "__main__":
    fruit = ["Banana", "Apple", "Elderberry", "Clementine", "Fig",
             "Guava", "Mango", "Honeydew Melon", "Date", "Watermelon",
             "Tangerine", "Ugli Fruit", "Juniperberry", "Kiwi",
             "Lemon", "Nectarine", "Plum", "Raspberry", "Strawberry",
             "Orange"]
    app = QApplication(sys.argv)
    form = StringListDlg("Fruit", fruit)
    form.exec_()
    print "\n".join([unicode(x) for x in form.stringlist])

