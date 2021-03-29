QT += testlib
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++11
lessThan(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += -std=c++11

TEMPLATE = app
TARGET = benchmark
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += ../../qcustomplot.h benchmark.moc
SOURCES += benchmark.cpp ../../qcustomplot.cpp
