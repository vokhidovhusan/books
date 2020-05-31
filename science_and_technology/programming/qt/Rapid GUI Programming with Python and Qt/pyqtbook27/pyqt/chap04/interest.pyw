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
from PyQt4.QtCore import Qt
from PyQt4.QtCore import pyqtSignal as Signal
from PyQt4.QtGui import (QApplication, QComboBox, QDialog,
        QDoubleSpinBox, QGridLayout, QLabel)


class Form(QDialog):

    def __init__(self, parent=None):
        super(Form, self).__init__(parent)
        self.create_widgets()
        self.layout_widgets()
        self.create_connections()
        self.setWindowTitle("Interest")
        self.updateUi()


    def create_widgets(self):
        self.principalLabel = QLabel("Principal:")
        self.principalSpinBox = QDoubleSpinBox()
        self.principalSpinBox.setRange(1, 1000000000)
        self.principalSpinBox.setValue(1000)
        self.principalSpinBox.setPrefix("$ ")
        self.rateLabel = QLabel("Rate:")
        self.rateSpinBox = QDoubleSpinBox()
        self.rateSpinBox.setRange(1, 100)
        self.rateSpinBox.setValue(5)
        self.rateSpinBox.setSuffix(" %")
        self.yearsLabel = QLabel("Years:")
        self.yearsComboBox = QComboBox()
        self.yearsComboBox.addItem("1 year")
        self.yearsComboBox.addItems(
                ["{0} years".format(x) for x in range(2, 26)])
        self.amountLabel = QLabel("Amount")
        self.amountResultLabel = QLabel()


    def layout_widgets(self):
        grid = QGridLayout()
        grid.addWidget(self.principalLabel, 0, 0)
        grid.addWidget(self.principalSpinBox, 0, 1)
        grid.addWidget(self.rateLabel, 1, 0)
        grid.addWidget(self.rateSpinBox, 1, 1)
        grid.addWidget(self.yearsLabel, 2, 0)
        grid.addWidget(self.yearsComboBox, 2, 1)
        grid.addWidget(self.amountLabel, 3, 0)
        grid.addWidget(self.amountResultLabel, 3, 1)
        self.setLayout(grid)


    def create_connections(self):
        self.principalSpinBox.valueChanged.connect(self.updateUi)
        self.rateSpinBox.valueChanged.connect(self.updateUi)
        self.yearsComboBox.currentIndexChanged.connect(self.updateUi)


    def updateUi(self):
        """Calculates compound interest"""
        principal = self.principalSpinBox.value()
        rate = self.rateSpinBox.value()
        years = self.yearsComboBox.currentIndex() + 1
        amount = principal * ((1 + (rate / 100.0)) ** years)
        self.amountResultLabel.setText("$ {0:.2f}".format(amount))


if __name__ == "__main__":
    app = QApplication(sys.argv)
    form = Form()
    form.show()
    app.exec_()

