# TarotClub project file
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

QT += xml network svg script scripttools help
RESOURCES = $${PWD}/../src/data/data.qrc
CONFIG += qt warn_on

INCLUDEPATH += $${PWD}/include
INCLUDEPATH += $${PWD}/../src/client
INCLUDEPATH += $${PWD}/../src/server

# libraries and other annoying OS stuff
win32 {
#   INCLUDEPATH += $${PWD}/../lib/lua/src
#   LIBS += $${PWD}/../lib/win32/liblua.a
   RC_FILE = icon.rc
} else {
#   INCLUDEPATH += /usr/include/lua5.1/
#   LIBS += -llua5.1

   # install
   isEmpty(PREFIX): PREFIX=/opt/tarotclub
   target.path = $${PREFIX}
   cards.path = $${PREFIX}/default
   cards.files = ../src/data/cards/default/*
   copying.path = $${PREFIX}
   copying.files = ../COPYING ../COPYING-FR ../HISTORY ../INSTALL ../README
   INSTALLS += target cards copying
}


HEADERS = AboutWindow.h \
    MainWindow.h \
    OptionsWindow.h \
    EditorWindow.h \
    ResultWindow.h \
    HelpWindow.h \
    ScoresDock.h \
    InfosDock.h \
    ChatDock.h \
    RoundDock.h \
    DebugDock.h \
    Tapis.h \
    TextBox.h \
    ClientConfig.h \
    ServerConfig.h \
    DealEditorFile.h \
    Game.h \
    Jeu.h \
    Deck.h \
    Card.h \
    Player.h \
    Client.h \
    Bot.h \
    defines.h \
    textes.h \
    TarotEngine.h \
    Score.h \
    Identity.h \
    Table.h \
    JoinWizard.h

FORMS = AboutUI.ui \
    AvatarsUI.ui \
    InfosDockUI.ui \
    OptionsUI.ui \
    ResultUI.ui \
    HelpUI.ui \
    ServerManagerUI.ui \
    RoundUI.ui \
    NumberedDealUI.ui \
    DealEditorUI.ui \
    WinUI.ui \
    JoinWizardPage1.ui \
    JoinWizardPage2.ui

SOURCES = AboutWindow.cpp \
    MainWindow.cpp \
    OptionsWindow.cpp \
    EditorWindow.cpp \
    ResultWindow.cpp \
    HelpWindow.cpp \
    ScoresDock.cpp \
    InfosDock.cpp \
    ChatDock.cpp \
    RoundDock.cpp \
    DebugDock.cpp \
    Tapis.cpp \
    TextBox.cpp \
    ClientConfig.cpp \
    ServerConfig.cpp \
    DealEditorFile.cpp \
    Game.cpp \
    main.cpp \
    Jeu.cpp \
    Deck.cpp \
    Card.cpp \
    Player.cpp \
    Client.cpp \
    Bot.cpp \
    TarotEngine.cpp \
    Score.cpp \
    JoinWizard.cpp

