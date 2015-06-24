#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T01:16:16
#
#-------------------------------------------------

QMAKE_CFLAGS += -g
QMAKE_CXXFLAGS += -std=c++11 -Wall

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = aexplor
TEMPLATE = app


SOURCES += main.cpp \
    directory_view.cpp

HEADERS  += \
    directory_view.hpp

LIBS += -lboost_system -lboost_filesystem
