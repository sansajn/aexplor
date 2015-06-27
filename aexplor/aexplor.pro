#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T01:16:16
#
#-------------------------------------------------

QMAKE_CFLAGS += -g -O0 -Wall
QMAKE_CXXFLAGS += -std=c++11

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = aexplor
TEMPLATE = app


SOURCES += main.cpp \
    directory_view.cpp \
    directory_widget.cpp \
    mkdir_dialog.cpp

HEADERS  += \
    directory_view.hpp \
    directory_widget.hpp \
    mkdir_dialog.hpp

LIBS += -lboost_system -lboost_filesystem -lboost_regex
