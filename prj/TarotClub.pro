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

# directories for temp files
UI_DIR = ./ui
UI_HEADERS_DIR = ./include
UI_SOURCES_DIR = ./src
OBJECTS_DIR = ./obj
DESTDIR = ./bin

# Qt build configuration
# The search path to find supplied files
VPATH += $${PWD}/../src
VPATH += $${PWD}/../src/client
VPATH += $${PWD}/../src/server
VPATH += $${PWD}/include
VPATH += $${PWD}/../lib
VPATH += $${PWD}/../ai
VPATH += $${PWD}/../ai/tarotlib

QT += xml network svg script scripttools help
RESOURCES = $${PWD}/../src/data/data.qrc
CONFIG += qt warn_on

INCLUDEPATH += $${PWD}/include
INCLUDEPATH += $${PWD}/../src/client
INCLUDEPATH += $${PWD}/../src/server

# Libraries and other annoying OS stuff
win32 {
   RC_FILE = icon.rc
} else {
   # Install under Unix
   isEmpty(PREFIX): PREFIX=/opt/tarotclub
   target.path = $${PREFIX}
   cards.path = $${PREFIX}/default
   cards.files = ../src/data/cards/default/*
   copying.path = $${PREFIX}
   copying.files = ../COPYING ../COPYING-FR ../HISTORY ../INSTALL ../README
   INSTALLS += target cards copying
}

OTHER_FILES = beginner.js \
              system.js \
              card.js \
              util.js \
              deck.js \
              game.js \
              player.js

TRANSLATIONS = tarotclub_fr.ts \
               tarotclub_en.ts

HEADERS = AboutWindow.h \
    MainWindow.h \
    OptionsWindow.h \
    EditorWindow.h \
    ResultWindow.h \
    HelpWindow.h \
    ScoresDock.h \
    InfosDock.h \
    ChatDock.h \
    DebugDock.h \
    Canvas.h \
    TextBox.h \
    ClientConfig.h \
    ServerConfig.h \
    DealFile.h \
    TarotClub.h \
    TarotDeck.h \
    Deck.h \
    Card.h \
    Player.h \
    Client.h \
    Bot.h \
    defines.h \
    Tools.h \
    TarotEngine.h \
    Deal.h \
    Identity.h \
    JoinWizard.h \
    Game.h \
    Score.h \
    Protocol.h \
    Server.h \
    Table.h \
    NetPlayer.h \
    GfxCard.h \
    MenuItem.h \
    PlayerBox.h \
    CustomTypes.h

FORMS = AboutUI.ui \
    AvatarsUI.ui \
    InfosDockUI.ui \
    OptionsUI.ui \
    ResultUI.ui \
    HelpUI.ui \
    NumberedDealUI.ui \
    DealEditorUI.ui \
    WinUI.ui \
    JoinWizardPage1.ui \
    JoinWizardPage2.ui \
    QuickJoin.ui

SOURCES = AboutWindow.cpp \
    MainWindow.cpp \
    OptionsWindow.cpp \
    EditorWindow.cpp \
    ResultWindow.cpp \
    HelpWindow.cpp \
    ScoresDock.cpp \
    InfosDock.cpp \
    ChatDock.cpp \
    DebugDock.cpp \
    Canvas.cpp \
    TextBox.cpp \
    ClientConfig.cpp \
    ServerConfig.cpp \
    DealFile.cpp \
    TarotClub.cpp \
    main.cpp \
    TarotDeck.cpp \
    Deck.cpp \
    Card.cpp \
    Player.cpp \
    Client.cpp \
    Bot.cpp \
    TarotEngine.cpp \
    Deal.cpp \
    JoinWizard.cpp \
    Game.cpp \
    Protocol.cpp \
    Server.cpp \
    Table.cpp \
    Score.cpp \
    NetPlayer.cpp \
    GfxCard.cpp \
    MenuItem.cpp \
    PlayerBox.cpp

