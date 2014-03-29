# TarotClub project file
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
UI_DIR = ./ui
UI_HEADERS_DIR = ./include
UI_SOURCES_DIR = ./src
OBJECTS_DIR = ./obj
DESTDIR = ./bin
BASE_DIR = $${PWD}/..

# ------------------------------------------------------------------------------
# The search path to find supplied files
# ------------------------------------------------------------------------------
VPATH += $$BASE_DIR/src
VPATH += $$BASE_DIR/src/library
VPATH += $$BASE_DIR/src/android
VPATH += $$BASE_DIR/src/jsengine
VPATH += $$BASE_DIR/src/json
VPATH += $$BASE_DIR/src/gfxlib
VPATH += $$BASE_DIR/lib
VPATH += $$BASE_DIR/ai
VPATH += $$BASE_DIR/ai/tarotlib

# ------------------------------------------------------------------------------
# Where to find header files
# ------------------------------------------------------------------------------
INCLUDEPATH += $$BASE_DIR/src
INCLUDEPATH += $$BASE_DIR/src/library
INCLUDEPATH += $$BASE_DIR/src/android
INCLUDEPATH += $$BASE_DIR/src/jsengine
INCLUDEPATH += $$BASE_DIR/src/json
INCLUDEPATH += $$BASE_DIR/src/gfxlib

# ------------------------------------------------------------------------------
# Compiler definitions
# ------------------------------------------------------------------------------
QT += xml svg help qml
RESOURCES = $$BASE_DIR/assets/android.qrc
CONFIG += qt warn_on
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CFLAGS_DEBUG +=  -O0  -ggdb -pedantic -std=c99 -fstrict-aliasing


# ------------------------------------------------------------------------------
# Targer definitions
# ------------------------------------------------------------------------------
TARGET = TarotClub # name of the output executable


DEFINES += USE_UNIX_OS
DEFINES += USE_ANDROID_OS

debug {
    DEFINES += TAROT_DEBUG
#    DEFINES += DUK_OPT_DEBUG
}

# ------------------------------------------------------------------------------
# JavaScript files
# ------------------------------------------------------------------------------
OTHER_FILES += beginner.js \
              system.js \
              card.js \
              util.js \
              deck.js \
              game.js \
              player.js \
              unit_test.js

# ------------------------------------------------------------------------------
# Android specific
# ------------------------------------------------------------------------------
OTHER_FILES += android/AndroidManifest.xml
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

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
    Common.h \
    System.h

SOURCES += Log.cpp \
    Util.cpp \
    ByteArray.cpp \
    ByteStreamReader.cpp \
    ByteStreamWriter.cpp \
    TcpSocket.cpp \
    TcpServer.cpp \
    TcpClient.cpp \
    UniqueId.cpp \
    Common.cpp \
    System.cpp

# ------------------------------------------------------------------------------
# JSEngine and JSON files
# ------------------------------------------------------------------------------

HEADERS += duktape.h \
    JsonWriter.h \
    JsonReader.h \
    JSEngine.h \
    IScriptEngine.h \
    JSValue.h \
    JsonValue.h

SOURCES += duktape.c \
    JsonWriter.cpp \
    JsonReader.cpp \
    JSEngine.cpp \
    JSValue.cpp \
    JsonValue.cpp

# ------------------------------------------------------------------------------
# TarotClub core files
# ------------------------------------------------------------------------------
HEADERS += DealFile.h \
    TarotDeck.h \
    Deck.h \
    Card.h \
    Player.h \
    Client.h \
    Bot.h \
    TarotEngine.h \
    Deal.h \
    Identity.h \
    Game.h \
    Score.h \
    Protocol.h \
    Controller.h \
    ClientConfig.h \
    ServerConfig.h \
    Table.h

SOURCES += DealFile.cpp \
    TarotDeck.cpp \
    Deck.cpp \
    Card.cpp \
    Player.cpp \
    Client.cpp \
    Bot.cpp \
    TarotEngine.cpp \
    Deal.cpp \
    Game.cpp \
    Protocol.cpp \
    Controller.cpp \
    ClientConfig.cpp \
    ServerConfig.cpp \
    Table.cpp \
    Score.cpp

# ------------------------------------------------------------------------------
# Gfx library files
# ------------------------------------------------------------------------------
HEADERS += Canvas.h \
    TextBox.h \
    GfxCard.h \
    MenuItem.h \
    TarotWidget.h \
    PlayerBox.h \
    CustomTypes.h \
    PopupItem.h

SOURCES += Canvas.cpp \
    TextBox.cpp \
    TarotWidget.cpp \
    GfxCard.cpp \
    MenuItem.cpp \
    PlayerBox.cpp \
    PopupItem.cpp


# ------------------------------------------------------------------------------
# Android client files
# ------------------------------------------------------------------------------
SOURCES += main.cpp

# End of project file
