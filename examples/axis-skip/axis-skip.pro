#-------------------------------------------------
#
# Project created by QtCreator 2016-11-09T07:24:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = axis-skip
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../qcustomplot.cpp

HEADERS  += mainwindow.h \
    ../../qcustomplot.h

FORMS    += mainwindow.ui
