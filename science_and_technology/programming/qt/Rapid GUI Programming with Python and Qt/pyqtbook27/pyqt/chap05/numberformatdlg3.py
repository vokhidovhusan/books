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

from PyQt4.QtCore import (QRegExp, Qt)
from PyQt4.QtCore import pyqtSignal as Signal
from PyQt4.QtGui import (QCheckBox, QDialog, QGridLayout, QLabel,
        QLineEdit, QRegExpValidator, QSpinBox)


class NumberFormatDlg(QDialog):

    def __init__(self, format, callback, parent=None):
        super(NumberFormatDlg, self).__init__(parent)
        self.format = format
        self.callback = callback
        self.create_widgets()
        self.layout_widgets()
        self.create_connections()
        self.setWindowTitle("Set Number Format (`Live')")


    def create_widgets(self):
        punctuationRe = QRegExp(r"[ ,;:.]")
        self.thousandsLabel = QLabel("&Thousands separator")
        self.thousandsEdit = QLineEdit(self.format["thousandsseparator"])
        self.thousandsLabel.setBuddy(self.thousandsEdit)
        self.thousandsEdit.setMaxLength(1)
        self.thousandsEdit.setValidator(QRegExpValidator(
                punctuationRe, self))
        self.decimalMarkerLabel = QLabel("Decimal &marker")
        self.decimalMarkerEdit = QLineEdit(self.format["decimalmarker"])
        self.decimalMarkerLabel.setBuddy(self.decimalMarkerEdit)
        self.decimalMarkerEdit.setMaxLength(1)
        self.decimalMarkerEdit.setValidator(QRegExpValidator(
                punctuationRe, self))
        self.decimalMarkerEdit.setInputMask("X")
        self.decimalPlacesLabel = QLabel("&Decimal places")
        self.decimalPlacesSpinBox = QSpinBox()
        self.decimalPlacesLabel.setBuddy(self.decimalPlacesSpinBox)
        self.decimalPlacesSpinBox.setRange(0, 6)
        self.decimalPlacesSpinBox.setValue(self.format["decimalplaces"])
        self.redNegativesCheckBox = QCheckBox("&Red negative numbers")
        self.redNegativesCheckBox.setChecked(self.format["rednegatives"])


    def layout_widgets(self):
        grid = QGridLayout()
        grid.addWidget(self.thousandsLabel, 0, 0)
        grid.addWidget(self.thousandsEdit, 0, 1)
        grid.addWidget(self.decimalMarkerLabel, 1, 0)
        grid.addWidget(self.decimalMarkerEdit, 1, 1)
        grid.addWidget(self.decimalPlacesLabel, 2, 0)
        grid.addWidget(self.decimalPlacesSpinBox, 2, 1)
        grid.addWidget(self.redNegativesCheckBox, 3, 0, 1, 2)
        self.setLayout(grid)


    def create_connections(self):
        self.thousandsEdit.textEdited.connect(self.checkAndFix)
        self.decimalMarkerEdit.textEdited.connect(self.checkAndFix)
        self.decimalPlacesSpinBox.valueChanged.connect(self.apply)
        self.redNegativesCheckBox.toggled.connect(self.apply)


    def checkAndFix(self):
        thousands = unicode(self.thousandsEdit.text())
        decimal = unicode(self.decimalMarkerEdit.text())
        if thousands == decimal:
            self.thousandsEdit.clear()
            self.thousandsEdit.setFocus()
        if len(decimal) == 0:
            self.decimalMarkerEdit.setText(".")
            self.decimalMarkerEdit.selectAll()
            self.decimalMarkerEdit.setFocus()
        self.apply()


    def apply(self):
        self.format["thousandsseparator"] = (
                unicode(self.thousandsEdit.text()))
        self.format["decimalmarker"] = (
                unicode(self.decimalMarkerEdit.text()))
        self.format["decimalplaces"] = (
                self.decimalPlacesSpinBox.value())
        self.format["rednegatives"] = (
                self.redNegativesCheckBox.isChecked())
        self.callback()


