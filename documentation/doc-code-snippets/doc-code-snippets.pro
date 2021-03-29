#
# Project to contain and test various code example snippets for the QCustomPlot documentation
#

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++11
lessThan(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += -std=c++11

TARGET = doc-code-snippets
TEMPLATE = app

HEADERS  += mainwindow.h\
            ../../qcustomplot.h

SOURCES += main.cpp\
        mainwindow.cpp\
        ../../qcustomplot.cpp

FORMS    += mainwindow.ui
