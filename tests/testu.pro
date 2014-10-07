# Test plan project file
#
# TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
# Copyright (C) 2003-2999 - Anthony Rabine
# anthony@tarotclub.fr
#
# TarotClub is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# TarotClub is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
#

# ------------------------------------------------------------------------------
# Directories for generated files and base directory
# ------------------------------------------------------------------------------

BASE_DIR = $${PWD}/..

CONFIG(debug, debug|release) {
debug:      DESTDIR = $$BASE_DIR/build-testu/debug
}

CONFIG(release, debug|release) {
release:    DESTDIR = $$BASE_DIR/build-testu/release
}

UI_DIR          = $$DESTDIR/ui
UI_HEADERS_DIR  = $$DESTDIR/include
UI_SOURCES_DIR  = $$DESTDIR/src
OBJECTS_DIR     = $$DESTDIR/obj
RCC_DIR         = $$DESTDIR/rcc
MOC_DIR         = $$DESTDIR/moc


# ------------------------------------------------------------------------------
# The search path to find supplied files
# ------------------------------------------------------------------------------
VPATH += $$BASE_DIR/src
VPATH += $$BASE_DIR/src/library
VPATH += $$BASE_DIR/src/jsengine
VPATH += $$BASE_DIR/src/json
VPATH += $$BASE_DIR/src/database
VPATH += $$BASE_DIR/src/gfxlib
VPATH += $$BASE_DIR/lib
VPATH += $$BASE_DIR/ai
VPATH += $$BASE_DIR/ai/tarotlib
VPATH += $$BASE_DIR/tests/src

# ------------------------------------------------------------------------------
# Where to find header files
# ------------------------------------------------------------------------------
INCLUDEPATH += $$BASE_DIR/src
INCLUDEPATH += $$BASE_DIR/src/library
INCLUDEPATH += $$BASE_DIR/src/database
INCLUDEPATH += $$BASE_DIR/src/gfxlib
INCLUDEPATH += $$BASE_DIR/src/jsengine
INCLUDEPATH += $$BASE_DIR/src/json
INCLUDEPATH += $$BASE_DIR/tests/src


QT       += testlib xml
QT       -= gui

TARGET = tst_utilities
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
RESOURCES = $$BASE_DIR/assets/testu.qrc

QMAKE_CXXFLAGS += -std=c++11  -fprofile-arcs -ftest-coverage

QMAKE_CFLAGS_DEBUG +=  -O0  -ggdb -pedantic -std=c99 -fstrict-aliasing  -fprofile-arcs -ftest-coverage
DEFINES += DUK_OPT_SELF_TESTS

# Specific OS stuff
win32 {
    LIBS +=  libws2_32 libgcov -lpsapi
    DEFINES += USE_WINDOWS_OS
}
unix {
    DEFINES += USE_UNIX_OS
    LIBS += -ldl -lgcov
}

CONFIG(debug, debug|release) {
    DEFINES += TAROT_DEBUG
    DEFINES += DUK_OPT_DEBUG
}

# ------------------------------------------------------------------------------
# Library files
# ------------------------------------------------------------------------------
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
    Base64.h \
    Sha1.h \
    WebSocket.h \
    Value.h

SOURCES += Log.cpp \
    Util.cpp \
    ByteArray.cpp \
    ByteStreamReader.cpp \
    ByteStreamWriter.cpp \
    TcpSocket.cpp \
    TcpServer.cpp \
    TcpClient.cpp \
    UniqueId.cpp \
    Base64.cpp \
    Sha1.cpp \
    WebSocket.cpp \
    Value.cpp

# ------------------------------------------------------------------------------
# UI utility classes
# ------------------------------------------------------------------------------
HEADERS += MiniDeck.h
SOURCES += MiniDeck.cpp

# ------------------------------------------------------------------------------
# Database management files
# ------------------------------------------------------------------------------
HEADERS += sqlite3.h \
    DataBase.h

SOURCES += sqlite3.c \
    DataBase.cpp

# ------------------------------------------------------------------------------
# JSEngine and JSON files
# ------------------------------------------------------------------------------
HEADERS += duktape.h \
    JsonWriter.h \
    JsonReader.h \
    JSEngine.h \
    IScriptEngine.h \
    JsonValue.h

SOURCES += duktape.c \
    JsonWriter.cpp \
    JsonReader.cpp \
    JSEngine.cpp \
    JsonValue.cpp

# ------------------------------------------------------------------------------
# TarotClub core files
# ------------------------------------------------------------------------------
HEADERS += Deck.h \
    Card.h \
    Player.h \
    ServerConfig.h \
    Common.h \
    System.h \
    Deal.h \
    Score.h

SOURCES += Deck.cpp \
    Card.cpp \
    Player.cpp \
    ServerConfig.cpp \
    Common.cpp \
    System.cpp \
    Deal.cpp \
    Score.cpp

# ------------------------------------------------------------------------------
# Unit test files
# ------------------------------------------------------------------------------

HEADERS +=  tst_tarot_base.h \
            tst_tarot_rules.h \
            tst_utilities.h \
            tst_json.h \
            tst_hash.h \
            tst_websocket.h \
            tst_database.h

SOURCES +=  main.cpp \
            tst_utilities.cpp \
            tst_tarot_base.cpp \
            tst_tarot_rules.cpp \
            tst_json.cpp \
            tst_hash.cpp \
            tst_websocket.cpp \
            tst_database.cpp

# End of project file
