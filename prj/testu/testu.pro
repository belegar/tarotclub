#-------------------------------------------------
#
# Project created by QtCreator 2013-11-15T22:36:45
#
#-------------------------------------------------

VPATH += $${PWD}/../../src
VPATH += $${PWD}/../../src/library
VPATH += $${PWD}/../../src/jsengine
VPATH += $${PWD}/../../src/json

INCLUDEPATH += $${PWD}/../../src
INCLUDEPATH += $${PWD}/../../src/library
INCLUDEPATH += $${PWD}/../../src/jsengine
INCLUDEPATH += $${PWD}/../../src/json

QT       += testlib
QT       -= gui

TARGET = tst_utilities
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

QMAKE_CFLAGS_DEBUG +=  -O0  -ggdb -pedantic -std=c99 -fstrict-aliasing
DEFINES += DUK_OPT_SELF_TESTS

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
    DEFINES += DUK_OPT_DEBUG
}


# -------------------------------------------------------------
# Library files
# -------------------------------------------------------------
HEADERS += Log.h \
    Observer.h \
    Util.h \
    ThreadQueue.h \
    ByteStreamReader.h \
    ByteStreamWriter.h \
    ByteArray.h \
    TcpSocket.h \
    TcpServer.h \
    TcpClient.h \
    UniqueId.h \
    Common.h

SOURCES += Log.cpp \
    Util.cpp \
    ByteArray.cpp \
    ByteStreamReader.cpp \
    ByteStreamWriter.cpp \
    TcpSocket.cpp \
    TcpServer.cpp \
    TcpClient.cpp \
    UniqueId.cpp \
    Common.cpp

# -------------------------------------------------------------
# JSEngine and JSON files
# -------------------------------------------------------------

HEADERS += duktape.h \
    JsonWriter.h \
    JsonReader.h
#    JSEngine.h

SOURCES += duktape.c \
    JsonWriter.cpp \
    JsonReader.cpp

# -------------------------------------------------------------
# TarotClub core files
# -------------------------------------------------------------
HEADERS += TarotDeck.h \
    Deck.h \
    Card.h \
    Player.h \
    defines.h \

SOURCES += TarotDeck.cpp \
    Deck.cpp \
    Card.cpp \
    Player.cpp

# -------------------------------------------------------------
# Unit test files
# -------------------------------------------------------------

HEADERS +=  tst_tarot_base.h \
            tst_utilities.h \
            tst_jsengine.h

SOURCES +=  tst_utilities.cpp \
            tst_tarot_base.cpp \
            tst_main.cpp \
            tst_jsengine.cpp
