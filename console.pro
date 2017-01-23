# TarotClub dedicated server project file
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
BASE_DIR = $${PWD}

CONFIG(debug, debug|release) {
debug:      DESTDIR = $$BASE_DIR/build-console/debug
}

CONFIG(release, debug|release) {
release:    DESTDIR = $$BASE_DIR/build-console/release
}

UI_DIR          = $$DESTDIR/ui
UI_HEADERS_DIR  = $$DESTDIR/include
UI_SOURCES_DIR  = $$DESTDIR/src
OBJECTS_DIR     = $$DESTDIR/obj
RCC_DIR         = $$DESTDIR/rcc
MOC_DIR         = $$DESTDIR/moc

# ------------------------------------------------------------------------------
# Console project: the search path for QtCreator & Make
# ------------------------------------------------------------------------------
VPATH += $$BASE_DIR/src
VPATH += $$BASE_DIR/src/config
VPATH += $$BASE_DIR/ai
VPATH += $$BASE_DIR/ai/tarotlib
VPATH += $$BASE_DIR/console

INCLUDEPATH += $$BASE_DIR/src
INCLUDEPATH += $$BASE_DIR/src/config
INCLUDEPATH += $$BASE_DIR/console


ICL_DIR = $$BASE_DIR/src/icl
include($$ICL_DIR/icl.pri)

# ------------------------------------------------------------------------------
# Compiler definitions
# ------------------------------------------------------------------------------
CONFIG += console warn_on
QMAKE_CXXFLAGS += -std=c++11

# Mainly for Duktape, the only source code in C
QMAKE_CFLAGS_DEBUG      += -O0 -pedantic -std=c99 -fstrict-aliasing -ggdb
QMAKE_CFLAGS_RELEASE    += -Os -pedantic -std=c99 -fstrict-aliasing -fomit-frame-pointer

# ------------------------------------------------------------------------------
# Targer definitions
# ------------------------------------------------------------------------------
TARGET = tarotclub-cli # name of the output executable

# Specific OS stuff
win32 {
    RC_FILE = $$BASE_DIR/tcds/icon.rc
    LIBS +=  libws2_32 -lpsapi
    DEFINES += USE_WINDOWS_OS
    # Let's make everything's static so that we don't need any DLL
    QMAKE_LFLAGS += -static-libgcc -static-libstdc++ -static -lpthread
}

unix {
    DEFINES += USE_UNIX_OS
    LIBS += -ldl
}

debug {
    DEFINES += TAROT_DEBUG
    DEFINES += DUK_OPT_DEBUG
   #  DEFINES += DUK_OPT_DDPRINT
}

# ------------------------------------------------------------------------------
# TarotClub core files
# ------------------------------------------------------------------------------
HEADERS += ServerConfig.h \
    TournamentConfig.h \
    DealFile.h \
    Deck.h \
    Card.h \
    Player.h \
    Bot.h \
    Common.h \
    Engine.h \
    Deal.h \
    Identity.h \
    Score.h \
    Protocol.h \
    PlayingTable.h \
    System.h \
    Users.h \
    Lobby.h \
    BotManager.h \
    Server.h \
    BasicClient.h \
    Network.h

SOURCES += ServerConfig.cpp \
    TournamentConfig.cpp \
    DealFile.cpp \
    Deck.cpp \
    Card.cpp \
    Player.cpp \
    Bot.cpp \
    Common.cpp \
    Engine.cpp \
    Deal.cpp \
    Protocol.cpp \
    PlayingTable.cpp \
    Score.cpp \
    System.cpp \
    Users.cpp \
    Lobby.cpp \
    BotManager.cpp \
    Server.cpp \
    BasicClient.cpp \
    Network.cpp


# ------------------------------------------------------------------------------
# Console files
# ------------------------------------------------------------------------------
SOURCES +=  main.cpp Console.cpp Embedded.cpp ConsoleClient.cpp
HEADERS +=  Console.h Embedded.h ConsoleClient.h

# End of project file
