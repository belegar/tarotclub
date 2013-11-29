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

HEADERS +=  Card.h \
            Deck.h \
            TarotDeck.h

SOURCES +=  tst_utilities.cpp \
            main.cpp \
            tst_tarot_base.cpp \
            Card.cpp \
            Deck.cpp \
            TarotDeck.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    tst_tarot_base.h \
    tst_utilities.h
