#-------------------------------------------------
#
# Project created by QtCreator 2013-11-15T22:36:45
#
#-------------------------------------------------

VPATH += $${PWD}/../../src
VPATH += $${PWD}/../../src/library
INCLUDEPATH += $${PWD}/../../src
INCLUDEPATH += $${PWD}/../../src/library

QT       += testlib

QT       -= gui

TARGET = tst_utilities
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

# Specific OS stuff
win32 {
    LIBS +=  libws2_32
    DEFINES += USE_WINDOWS_OS
}
unix {
    DEFINES += USE_UNIX_OS
}

debug {
    DEFINES += TAROT_DEBUG
}

HEADERS +=  Card.h \
            tst_tarot_base.h \
            tst_utilities.h \
            Deck.h \
            TarotDeck.h \
            ByteArray.h \
            ByteStreamReader.h \
            ByteStreamWriter.h \
            defines.h

SOURCES +=  tst_utilities.cpp \
            main.cpp \
            tst_tarot_base.cpp \
            Card.cpp \
            Deck.cpp \
            TarotDeck.cpp
