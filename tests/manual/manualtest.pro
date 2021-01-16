#
# Project to test various use cases
#

QT += core gui# opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++11
lessThan(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += -std=c++11

TARGET = test
TEMPLATE = app

#DEFINES += QCUSTOMPLOT_USE_OPENGL

SOURCES += main.cpp\
           mainwindow.cpp
HEADERS  += mainwindow.h \
            ../../src/qcp.h
FORMS    += mainwindow.ui
OTHER_FILES += ../../changelog.txt

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../src/ -lqcustomplot
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../src/ -lqcustomplotd
else:unix: LIBS += -L$$PWD/../../src/ -lqcustomplot

INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../src/libqcustomplot.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../src/libqcustomplotd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../src/qcustomplot.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../src/qcustomplotd.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../src/libqcustomplot.a
