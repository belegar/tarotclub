#
# TarotClubServer project file
#
# TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
# Copyright (C) 2003-2999 - Anthony Rabine
# anthony@tarotclub.fr
#
# This file must be used under the terms of the CeCILL.
# This source file is licensed as described in the file COPYING, which
# you should have received as part of this distribution.  The terms
# are also available at
# http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
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
VPATH += $${PWD}/../lib

QT += xml network
CONFIG += qt console warn_on

INCLUDEPATH += $${PWD}/include
INCLUDEPATH += $${PWD}/../src/client
INCLUDEPATH += $${PWD}/../src/server

# libraries and other annoying OS stuff
win32 {
   RC_FILE = server.rc
   INCLUDEPATH += $${PWD}/../lib/lua/src
   LIBS += $${PWD}/../lib/win32/liblua.a

} else {
   INCLUDEPATH += /usr/include/lua5.1/
   LIBS += -llua5.1 -ldl -lpthread

   # install
   isEmpty(PREFIX): PREFIX=/opt/tarotclubd
   target.path = $${PREFIX}
   copying.path = $${PREFIX}
   copying.files = ../COPYING ../COPYING-FR ../HISTORY ../INSTALL ../README
   INSTALLS += target cards copying
}

HEADERS = ServerConsole.h \
   TarotEngine.h \
   defines.h \
   Identity.h \
   Card.h \
   Deck.h \
   Jeu.h \
   Player.h \
   Client.h \
   Bot.h \
   LuaBot.h \
   Score.h \
   Table.h \
   DealEditorFile.h \
   ServerConfig.h \
   Lobby.h \
   TarotServer.h

SOURCES = main.cpp \
   ServerConsole.cpp \
   TarotEngine.cpp \
   Card.cpp \
   Deck.cpp \
   Jeu.cpp \
   Player.cpp \
   Client.cpp \
   Bot.cpp \
   LuaBot.cpp \
   Score.cpp \
   DealEditorFile.cpp \
   ServerConfig.cpp \
   Lobby.cpp \
   TarotServer.cpp
