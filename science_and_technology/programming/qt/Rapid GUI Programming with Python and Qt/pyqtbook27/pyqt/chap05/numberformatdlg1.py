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

from PyQt4.QtCore import Qt
from PyQt4.QtCore import pyqtSignal as Signal
from PyQt4.QtGui import (QCheckBox, QDialog, QDialogButtonBox,
        QGridLayout, QLabel, QLineEdit, QMessageBox, QSpinBox)


class NumberFormatDlg(QDialog):

    def __init__(self, format, parent=None):
        super(NumberFormatDlg, self).__init__(parent)
        self.format = format.copy()
        self.create_widgets()
        self.layout_widgets()
        self.create_connections()


    def create_widgets(self):
        self.thousandsLabel = QLabel("&Thousands separator")
        self.thousandsEdit = QLineEdit(self.format["thousandsseparator"])
        self.thousandsLabel.setBuddy(self.thousandsEdit)
        self.decimalMarkerLabel = QLabel("Decimal &marker")
        self.decimalMarkerEdit = QLineEdit(self.format["decimalmarker"])
        self.decimalMarkerLabel.setBuddy(self.decimalMarkerEdit)
        self.decimalPlacesLabel = QLabel("&Decimal places")
        self.decimalPlacesSpinBox = QSpinBox()
        self.decimalPlacesLabel.setBuddy(self.decimalPlacesSpinBox)
        self.decimalPlacesSpinBox.setRange(0, 6)
        self.decimalPlacesSpinBox.setValue(self.format["decimalplaces"])
        self.redNegativesCheckBox = QCheckBox("&Red negative numbers")
        self.redNegativesCheckBox.setChecked(self.format["rednegatives"])
        self.buttonBox = QDialogButtonBox(QDialogButtonBox.Ok|
                                          QDialogButtonBox.Cancel)

    
    def layout_widgets(self):
        grid = QGridLayout()
        grid.addWidget(self.thousandsLabel, 0, 0)
        grid.addWidget(self.thousandsEdit, 0, 1)
        grid.addWidget(self.decimalMarkerLabel, 1, 0)
        grid.addWidget(self.decimalMarkerEdit, 1, 1)
        grid.addWidget(self.decimalPlacesLabel, 2, 0)
        grid.addWidget(self.decimalPlacesSpinBox, 2, 1)
        grid.addWidget(self.redNegativesCheckBox, 3, 0, 1, 2)
        grid.addWidget(self.buttonBox, 4, 0, 1, 2)
        self.setLayout(grid)


    def create_connections(self):
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)
        self.setWindowTitle("Set Number Format (Modal)")


    def accept(self):
        class ThousandsError(Exception): pass
        class DecimalError(Exception): pass
        Punctuation = frozenset(" ,;:.")

        thousands = unicode(self.thousandsEdit.text())
        decimal = unicode(self.decimalMarkerEdit.text())
        try:
            if len(decimal) == 0:
                raise DecimalError, ("The decimal marker may not be "
                                     "empty.")
            if len(thousands) > 1:
                raise ThousandsError, ("The thousands separator may "
                                       "only be empty or one character.")
            if len(decimal) > 1:
                raise DecimalError, ("The decimal marker must be "
                                     "one character.")
            if thousands == decimal:
                raise ThousandsError, ("The thousands separator and "
                              "the decimal marker must be different.")
            if thousands and thousands not in Punctuation:
                raise ThousandsError, ("The thousands separator must "
                                       "be a punctuation symbol.")
            if decimal not in Punctuation:
                raise DecimalError, ("The decimal marker must be a "
                                     "punctuation symbol.")
        except ThousandsError, err:
            QMessageBox.warning(self, "Thousands Separator Error",
                                unicode(err))
            self.thousandsEdit.selectAll()
            self.thousandsEdit.setFocus()
            return
        except DecimalError, err:
            QMessageBox.warning(self, "Decimal Marker Error",
                                unicode(err))
            self.decimalMarkerEdit.selectAll()
            self.decimalMarkerEdit.setFocus()
            return

        self.format["thousandsseparator"] = thousands
        self.format["decimalmarker"] = decimal
        self.format["decimalplaces"] = (
                self.decimalPlacesSpinBox.value())
        self.format["rednegatives"] = (
                self.redNegativesCheckBox.isChecked())
        QDialog.accept(self)


    def numberFormat(self):
        return self.format


