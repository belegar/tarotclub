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
VPATH += $${PWD}/../src
VPATH += $${PWD}/../src/server
QT      += xml network
CONFIG += qt console warn_on static


win32 {
    RC_FILE = icon.rc
}

HEADERS	=   ServerConsole.h ServerConsole.h TarotEngine.h defines.h Card.h \
            Deck.h Jeu.h Player.h Score.h

SOURCES	=   main.cpp ServerConsole.cpp TarotEngine.cpp Card.cpp Deck.cpp \
            Jeu.cpp Player.cpp Score.cpp

