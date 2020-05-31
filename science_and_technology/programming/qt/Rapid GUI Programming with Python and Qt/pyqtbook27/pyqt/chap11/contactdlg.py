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
from PyQt4.QtCore import (QVariant, Qt)
from PyQt4.QtGui import (QApplication, QComboBox, QDialog,
        QDialogButtonBox, QGridLayout, QLabel, QLineEdit, QVBoxLayout)


class ContactDlg(QDialog):

    StyleSheet = """
QComboBox { color: darkblue; }
QLineEdit { color: darkgreen; }
QLineEdit[mandatory="true"] {
    background-color: rgb(255, 255, 127);
    color: darkblue;
}
"""

    def __init__(self, parent=None):
        super(ContactDlg, self).__init__(parent)
        self.create_widgets()
        self.layout_widgets()
        self.create_connections()
        self.lineedits = (self.forenameEdit, self.surnameEdit,
                self.companyEdit, self.phoneEdit, self.emailEdit)
        for lineEdit in self.lineedits:
            lineEdit.setProperty("mandatory", QVariant(True))
            lineEdit.textEdited.connect(self.updateUi)
        self.categoryComboBox.activated[int].connect(self.updateUi)
        self.setStyleSheet(ContactDlg.StyleSheet)
        self.setWindowTitle("Add Contact")

# An alternative would be to not create the QLabels but instead use a
# QFormLayout

    def create_widgets(self):
        self.forenameLabel = QLabel("&Forename:")
        self.forenameEdit = QLineEdit()
        self.forenameLabel.setBuddy(self.forenameEdit)
        self.surnameLabel = QLabel("&Surname:")
        self.surnameEdit = QLineEdit()
        self.surnameLabel.setBuddy(self.surnameEdit)
        self.categoryLabel = QLabel("&Category:")
        self.categoryComboBox = QComboBox()
        self.categoryLabel.setBuddy(self.categoryComboBox)
        self.categoryComboBox.addItems(["Business", "Domestic",
                                        "Personal"])
        self.companyLabel = QLabel("C&ompany:")
        self.companyEdit = QLineEdit()
        self.companyLabel.setBuddy(self.companyEdit)
        self.addressLabel = QLabel("A&ddress:")
        self.addressEdit = QLineEdit()
        self.addressLabel.setBuddy(self.addressEdit)
        self.phoneLabel = QLabel("&Phone:")
        self.phoneEdit = QLineEdit()
        self.phoneLabel.setBuddy(self.phoneEdit)
        self.mobileLabel = QLabel("&Mobile:")
        self.mobileEdit = QLineEdit()
        self.mobileLabel.setBuddy(self.mobileEdit)
        self.faxLabel = QLabel("Fa&x:")
        self.faxEdit = QLineEdit()
        self.faxLabel.setBuddy(self.faxEdit)
        self.emailLabel = QLabel("&Email:")
        self.emailEdit = QLineEdit()
        self.emailLabel.setBuddy(self.emailEdit)
        self.buttonBox = QDialogButtonBox(QDialogButtonBox.Ok|
                                          QDialogButtonBox.Cancel)
        addButton = self.buttonBox.button(QDialogButtonBox.Ok)
        addButton.setText("&Add")
        addButton.setEnabled(False)


    def layout_widgets(self):
        grid = QGridLayout()
        grid.addWidget(self.forenameLabel, 0, 0)
        grid.addWidget(self.forenameEdit, 0, 1)
        grid.addWidget(self.surnameLabel, 0, 2)
        grid.addWidget(self.surnameEdit, 0, 3)
        grid.addWidget(self.categoryLabel, 1, 0)
        grid.addWidget(self.categoryComboBox, 1, 1)
        grid.addWidget(self.companyLabel, 1, 2)
        grid.addWidget(self.companyEdit, 1, 3)
        grid.addWidget(self.addressLabel, 2, 0)
        grid.addWidget(self.addressEdit, 2, 1, 1, 3)
        grid.addWidget(self.phoneLabel, 3, 0)
        grid.addWidget(self.phoneEdit, 3, 1)
        grid.addWidget(self.mobileLabel, 3, 2)
        grid.addWidget(self.mobileEdit, 3, 3)
        grid.addWidget(self.faxLabel, 4, 0)
        grid.addWidget(self.faxEdit, 4, 1)
        grid.addWidget(self.emailLabel, 4, 2)
        grid.addWidget(self.emailEdit, 4, 3)
        layout = QVBoxLayout()
        layout.addLayout(grid)
        layout.addWidget(self.buttonBox)
        self.setLayout(layout)


    def create_connections(self):
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)


    def updateUi(self):
        mandatory = self.companyEdit.property("mandatory").toBool()
        if self.categoryComboBox.currentText() == "Business":
            if not mandatory:
                self.companyEdit.setProperty("mandatory", QVariant(True))
        elif mandatory:
            self.companyEdit.setProperty("mandatory", QVariant(False))
        if (mandatory !=
            self.companyEdit.property("mandatory").toBool()):
            self.setStyleSheet(ContactDlg.StyleSheet)
        enable = True
        for lineEdit in self.lineedits:
            if (lineEdit.property("mandatory").toBool() and
                lineEdit.text().isEmpty()):
                enable = False
                break
        self.buttonBox.button(QDialogButtonBox.Ok).setEnabled(enable)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    form = ContactDlg()
    form.show()
    app.exec_()
