#-------------------------------------------------
#
# Project created by QtCreator 2015-06-28T19:34:46
#
#-------------------------------------------------

QMAKE_CFLAGS += -Wall -g -O0
QMAKE_CXXFLAGS += -std=c++11

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = listview
TEMPLATE = app


SOURCES += main.cpp \
    directory_model.cpp \
    directory_view.cpp \
    mkdir_dialog.cpp

HEADERS  += \
    directory_model.hpp \
    directory_view.hpp \
    mkdir_dialog.hpp

LIBS += -lboost_system -lboost_filesystem

FORMS +=
