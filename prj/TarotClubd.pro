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

# Qt build configuration
# The search path to find supplied files
VPATH += $${PWD}/../src
VPATH += $${PWD}/../src/server
VPATH += $${PWD}/include

QT += xml network script scripttools
CONFIG += qt console warn_on

INCLUDEPATH += $${PWD}/include
INCLUDEPATH += $${PWD}/../src/client
INCLUDEPATH += $${PWD}/../src/server

# libraries and other annoying OS stuff
win32 {
   RC_FILE = server.rc
}

HEADERS = TarotEngine.h \
    defines.h \
    Identity.h \
    Card.h \
    Deck.h \
    Deal.h \
    TarotDeck.h \
    Player.h \
    NetPlayer.h \
    Client.h \
    Bot.h \
    Score.h \
    Table.h \
    DealFile.h \
    ServerConfig.h \
    Lobby.h \
    Server.h \
    Protocol.h \
    Game.h

SOURCES = main.cpp \
    TarotEngine.cpp \
    Card.cpp \
    Deck.cpp \
    Deal.cpp \
    TarotDeck.cpp \
    Player.cpp \
    NetPlayer.cpp \
    Client.cpp \
    Bot.cpp \
    Score.cpp \
    DealFile.cpp \
    ServerConfig.cpp \
    Lobby.cpp \
    Server.cpp \
    Protocol.cpp \
    Game.cpp \
    Table.cpp
