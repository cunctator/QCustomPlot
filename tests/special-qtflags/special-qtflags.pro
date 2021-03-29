QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++11
lessThan(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += -std=c++11

TARGET = special-qtflags
TEMPLATE = app

# These are the special defines that are tested here:
DEFINES += QT_NO_KEYWORDS QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII

if (equals(QT_MAJOR_VERSION, 4):lessThan(QT_MINOR_VERSION, 8)): {
  DEFINES += QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS
} else {
  DEFINES += QT_USE_STRING_BUILDER
}

HEADERS += qcustomplot.h
SOURCES += qcustomplot.cpp main.cpp 
