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
import urllib2
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
        self.prepare()
        self.setWindowTitle("Currency")


    def create_widgets(self):
        self.dateLabel = QLabel()
        self.fromComboBox = QComboBox()
        self.fromSpinBox = QDoubleSpinBox()
        self.fromSpinBox.setRange(0.01, 10000000.00)
        self.fromSpinBox.setValue(1.00)
        self.toComboBox = QComboBox()
        self.toLabel = QLabel("1.00")


    def layout_widgets(self):
        grid = QGridLayout()
        grid.addWidget(self.dateLabel, 0, 0)
        grid.addWidget(self.fromComboBox, 1, 0)
        grid.addWidget(self.fromSpinBox, 1, 1)
        grid.addWidget(self.toComboBox, 2, 0)
        grid.addWidget(self.toLabel, 2, 1)
        self.setLayout(grid)


    def create_connections(self):
        self.fromComboBox.currentIndexChanged.connect(self.updateUi)
        self.toComboBox.currentIndexChanged.connect(self.updateUi)
        self.fromSpinBox.valueChanged.connect(self.updateUi)


    def prepare(self):
        date = self.getdata()
        self.dateLabel.setText(date)
        rates = sorted(self.rates.keys())
        self.fromComboBox.addItems(rates)
        self.toComboBox.addItems(rates)


    def updateUi(self):
        to = unicode(self.toComboBox.currentText())
        from_ = unicode(self.fromComboBox.currentText())
        if to and from_:
            amount = ((self.rates[from_] / self.rates[to]) *
                    self.fromSpinBox.value())
            self.toLabel.setText("{0:.2f}".format(amount))


    def getdata(self): # Idea taken from the Python Cookbook
        self.rates = {}
        try:
            date = "Unknown"
            fh = urllib2.urlopen("http://www.bankofcanada.ca"
                                 "/en/markets/csv/exchange_eng.csv")
            for line in fh:
                line = line.rstrip()
                if not line or line.startswith(("#", "Closing ")):
                    continue
                fields = line.split(",")
                if line.startswith("Date "):
                    date = fields[-1]
                else:
                    try:
                        value = float(fields[-1])
                        self.rates[unicode(fields[0])] = value
                    except ValueError:
                        pass
            return "Exchange Rates Date: " + date
        except Exception, err:
            return "Failed to download:\n{0}".format(err)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    form = Form()
    form.show()
    app.exec_()

