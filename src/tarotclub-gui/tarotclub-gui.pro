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
BASE_DIR = $${PWD}/../..

CONFIG(debug, debug|release) {
debug:      DESTDIR = $$BASE_DIR/build/tarotclub-gui/debug
}

CONFIG(release, debug|release) {
release:    DESTDIR = $$OUTPUT_DIR/build/tarotclub-gui/release
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

VPATH += $$BASE_DIR/src/tarotclub-gui
VPATH += $$BASE_DIR/src/tarotclub-gui/windows
VPATH += $$BASE_DIR/src/tarotclub-gui/docks
VPATH += $$BASE_DIR/src/tarotclub-gui/forms
VPATH += $$BASE_DIR/src/gfxlib

VPATH += $$BASE_DIR/assets/ai
VPATH += $$BASE_DIR/assets/ai/tarotlib
VPATH += $$BASE_DIR/assets/canvasjs


ICL_DIR = $$BASE_DIR/src/icl
include($$ICL_DIR/icl.pri)

# ------------------------------------------------------------------------------
# Where to find header files
# ------------------------------------------------------------------------------
INCLUDEPATH += $$BASE_DIR/src/tarotclub-gui
INCLUDEPATH += $$BASE_DIR/src/tarotclub-gui/windows
INCLUDEPATH += $$BASE_DIR/src/tarotclub-gui/docks
INCLUDEPATH += $$BASE_DIR/src/tarotclub-gui/forms
INCLUDEPATH += $$BASE_DIR/src/gfxlib

# ------------------------------------------------------------------------------
# Compiler definitions
# ------------------------------------------------------------------------------
QT += svg network qml
RESOURCES = $$BASE_DIR/assets/desktop.qrc
CONFIG += qt warn_on
QMAKE_CXXFLAGS += -std=c++11

# Mainly for Duktape, the only source code in C
QMAKE_CFLAGS_DEBUG      += -O0 -pedantic -std=c99 -fstrict-aliasing -ggdb
QMAKE_CFLAGS_RELEASE    += -Os -pedantic -std=c99 -fstrict-aliasing -fomit-frame-pointer

# ------------------------------------------------------------------------------
# Target definitions
# ------------------------------------------------------------------------------
TARGET = tarotclub-gui # name of the output executable

# Specific OS stuff
win32 {
    RC_FILE = icon.rc
    LIBS +=  libws2_32 -lpsapi
    DEFINES += USE_WINDOWS_OS

    # Let's make everything's static so that we need less DLL
    QMAKE_LFLAGS += -static-libgcc -static-libstdc++ -static -lpthread
}

unix {
    DEFINES += USE_UNIX_OS
    LIBS += -ldl
}

CONFIG(debug, debug|release) {
    DEFINES += TAROT_DEBUG
    DEFINES += DUK_OPT_DEBUG
#    DEFINES += DUK_OPT_DPRINT
#    DEFINES += DUK_OPT_DDPRINT

    unix {
        QMAKE_CXXFLAGS  +=-fsanitize=address -fno-omit-frame-pointer
        QMAKE_CFLAGS    +=-fsanitize=address -fno-omit-frame-pointer
        QMAKE_LFLAGS    +=-fsanitize=address
    }
}


# Duktape defines
DEFINES += DUK_OPT_NO_JX
DEFINES += DUK_OPT_NO_JC

# Common defines for all platforms
DEFINES += DESKTOP_PROJECT


# ------------------------------------------------------------------------------
# Project description files
# ------------------------------------------------------------------------------
OTHER_FILES += HISTORY.md TODO.md README.md

# ------------------------------------------------------------------------------
# JavaScript files
# ------------------------------------------------------------------------------
OTHER_FILES += noob.js \
              card.js \
              util.js \
              deck.js \
              game.js \
              player.js \
              bot.js \
              unit_test.js

#               main.js \

# ------------------------------------------------------------------------------
# Translation files
# ------------------------------------------------------------------------------
TRANSLATIONS = $$BASE_DIR/prj/desktop/tarotclub_fr.ts

# ------------------------------------------------------------------------------
# TarotClub core files
# ------------------------------------------------------------------------------
INCLUDEPATH += $$BASE_DIR/src
VPATH += $$BASE_DIR/src
include($$BASE_DIR/src/tarot_core.pri)

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
    PopupItem.h \
    CheckBoxItem.h \
    ButtonItem.h \
    MessageBoxItem.h \
    IButtonEvent.h \
    ICardEvent.h \
    Translations.h \
    RectBase.h \
    Avatar.h

SOURCES += Canvas.cpp \
    TextBox.cpp \
    TarotWidget.cpp \
    GfxCard.cpp \
    MenuItem.cpp \
    PlayerBox.cpp \
    PopupItem.cpp \
    CheckBoxItem.cpp \
    ButtonItem.cpp \
    MessageBoxItem.cpp \
    Translations.cpp \
    Avatar.cpp

# ------------------------------------------------------------------------------
# Desktop client files
# ------------------------------------------------------------------------------
FORMS += AboutUI.ui \
    AvatarsUI.ui \
    InfosDockUI.ui \
    OptionsUI.ui \
    NumberedDealUI.ui \
    DealEditorUI.ui \
    QuickJoin.ui \
    LobbyUI.ui \
    NewServerUI.ui \
    ImportAvatar.ui \
    NewTurnUI.ui \
    ScoreCalculatorUI.ui

HEADERS += AboutWindow.h \
    MainWindow.h \
    OptionsWindow.h \
    EditorWindow.h \
    ScoresDock.h \
    InfosDock.h \
    ChatDock.h \
    DebugDock.h \
    LobbyDock.h \
    DealsWindow.h \
    ImportAvatarWindow.h \
    NewTurnWindow.h \
    Defines.h \
    ScoreCalculatorWindow.h

SOURCES += AboutWindow.cpp \
    MainWindow.cpp \
    OptionsWindow.cpp \
    EditorWindow.cpp \
    ScoresDock.cpp \
    InfosDock.cpp \
    ChatDock.cpp \
    DebugDock.cpp \    
    LobbyDock.cpp \
    DealsWindow.cpp \
    ImportAvatarWindow.cpp \
    NewTurnWindow.cpp \
    ScoreCalculatorWindow.cpp \
    main.cpp

# End of project file
