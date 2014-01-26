# TarotClubServer project file
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

# directories for temp files
UI_DIR  = ./ui
UI_HEADERS_DIR = ./include
UI_SOURCES_DIR = ./src
OBJECTS_DIR = ./obj
DESTDIR = ./bin

# The search path to find supplied files
VPATH += $${PWD}/../src
VPATH += $${PWD}/../src/library
VPATH += $${PWD}/../src/server
VPATH += $${PWD}/../lib
VPATH += $${PWD}/../ai
VPATH += $${PWD}/../ai/tarotlib

# Where to find header files
INCLUDEPATH += $${PWD}/../src
INCLUDEPATH += $${PWD}/../src/library
INCLUDEPATH += $${PWD}/../src/server

QT += xml qml
CONFIG += qt console warn_on
QMAKE_CXXFLAGS += -std=c++11
TARGET = tcds

# Specific OS stuff
win32 {
    RC_FILE = server.rc
    LIBS +=  libws2_32
    DEFINES += USE_WINDOWS_OS
}
unix {
    DEFINES += USE_UNIX_OS
}

# -------------------------------------------------------------
# Library files
# -------------------------------------------------------------
HEADERS += Log.h \
    Observer.h \
    ThreadQueue.h \
    ByteStreamReader.h \
    ByteStreamWriter.h \
    ByteArray.h \
    TcpSocket.h \
    TcpServer.h \
    TcpClient.h \
    UniqueId.h

SOURCES += Log.cpp \
    ByteArray.cpp \
    ByteStreamReader.cpp \
    ByteStreamWriter.cpp \
    TcpSocket.cpp \
    TcpServer.cpp \
    TcpClient.cpp \
    UniqueId.cpp


# -------------------------------------------------------------
# TarotClub core files
# -------------------------------------------------------------
HEADERS += ServerConfig.h \
    DealFile.h \
    TarotDeck.h \
    Deck.h \
    Card.h \
    Player.h \
    Client.h \
    Bot.h \
    defines.h \
    TarotEngine.h \
    Deal.h \
    Identity.h \
    Game.h \
    Score.h \
    Protocol.h \
    Controller.h \
    Table.h

SOURCES += ServerConfig.cpp \
    DealFile.cpp \
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
    Table.cpp \
    Score.cpp


# -------------------------------------------------------------
# Server files
# -------------------------------------------------------------
HEADERS += Lobby.h

SOURCES += main.cpp \
    Lobby.cpp \
