# TarotClub project file
# TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
# Copyright (C) 2003-2999 - Anthony Rabine
# anthony@tarotclub.fr
# This file must be used under the terms of the CeCILL.
# This source file is licensed as described in the file COPYING, which
# you should have received as part of this distribution.  The terms
# are also available at
# http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt


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
VPATH += $${PWD}/../src/client/quazip
VPATH += $${PWD}/include
QT += xml \
    network \
    svg
RESOURCES = $${PWD}/../src/data/data.qrc
CONFIG += qt \
    warn_on
INCLUDEPATH += $${PWD}/include
win32 { 
    INCLUDEPATH += $${PWD}/../lib/zlib-1.2.5
    debug:
    
    # CONFIG += console
    RC_FILE = icon.rc
}
HEADERS += AboutWindow.h \
    MainWindow.h \
    OptionsWindow.h \
    EditorWindow.h \
    ResultWindow.h \
    ScoresDock.h \
    InfosDock.h \
    ChatDock.h \
    RoundDock.h \
    Tapis.h \
    TextBox.h \
    ConfigFile.h \
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
    Score.h
FORMS = AboutUI.ui \
    AvatarsUI.ui \
    EncheresUI.ui \
    InfosDockUI.ui \
    NetClientUI.ui \
    OptionsUI.ui \
    ResultUI.ui \
    ServerManagerUI.ui \
    RoundUI.ui \
    NumberedDealUI.ui \
    DealEditorUI.ui
SOURCES = AboutWindow.cpp \
    MainWindow.cpp \
    OptionsWindow.cpp \
    EditorWindow.cpp \
    ResultWindow.cpp \
    ScoresDock.cpp \
    InfosDock.cpp \
    ChatDock.cpp \
    RoundDock.cpp \
    Tapis.cpp \
    TextBox.cpp \
    ConfigFile.cpp \
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
    Score.cpp
