#-------------------------------------------------
#
# Project created by QtCreator 2013-11-15T22:36:45
#
#-------------------------------------------------

VPATH += $${PWD}/../../src
INCLUDEPATH += $${PWD}/../../src

QT       += testlib

QT       -= gui

TARGET = tst_utilities
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11


SOURCES += tst_utilities.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
