#-------------------------------------------------
#
# Project created by QtCreator 2016-01-14T16:31:02
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tetris
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mapview.cpp \
    graphicsview.cpp \
    itemfactory.cpp \
    map.cpp \
    network.cpp \
    networkwidget.cpp

HEADERS  += mainwindow.h \
    mapview.h \
    graphicsview.h \
    itemfactory.h \
    map.h \
    network.h \
    networkwidget.h

OTHER_FILES +=
