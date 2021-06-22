# Test plan project file
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
debug:      DESTDIR = $$BASE_DIR/build/tests/debug
}

CONFIG(release, debug|release) {
release:    DESTDIR = $$BASE_DIR/build/tests/release
}


UI_DIR          = $$DESTDIR/ui
OBJECTS_DIR     = $$DESTDIR/obj
RCC_DIR         = $$DESTDIR/rcc
MOC_DIR         = $$DESTDIR/moc


# ------------------------------------------------------------------------------
# The search path to find supplied files
# ------------------------------------------------------------------------------
VPATH += $$BASE_DIR/ai
VPATH += $$BASE_DIR/ai/tarotlib


# ------------------------------------------------------------------------------
# COMPILER CONFIG
# ------------------------------------------------------------------------------
QT       += testlib xml
QT       -= gui

TARGET = tarotclub-tst
CONFIG += console
CONFIG -= app_bundle
CONFIG += icl_http

TEMPLATE = app
RESOURCES = $$BASE_DIR/assets/testu.qrc

QMAKE_CXXFLAGS += -std=c++17  -fprofile-arcs -ftest-coverage

QMAKE_CFLAGS_DEBUG +=  -O0  -ggdb -pedantic -std=c99 -fstrict-aliasing

DEFINES += UNIT_TEST

# Specific OS stuff
win32 {
    LIBS +=  libws2_32 libgcov -lpsapi
    DEFINES += USE_WINDOWS_OS
}
unix {
    DEFINES += USE_LINUX_OS
    LIBS += -ldl -lgcov
}

CONFIG(debug, debug|release) {
    DEFINES += TAROT_DEBUG
    DEFINES += DUK_USE_DEBUG
}

# ------------------------------------------------------------------------------
# TarotClub core files
# ------------------------------------------------------------------------------
VPATH += $$BASE_DIR/src
INCLUDEPATH += $$BASE_DIR/src

include($$BASE_DIR/src/tarot_core.pri)

# ------------------------------------------------------------------------------
# ICL files
# ------------------------------------------------------------------------------
CONFIG += icl_zip icl_database
ICL_DIR = $$BASE_DIR/lib/icl
include($$ICL_DIR/icl.pri)

# ------------------------------------------------------------------------------
# Unit test files
# ------------------------------------------------------------------------------
HEADERS +=  tst_tarot_base.h \
            tst_tarot_rules.h \
            tst_tarot_protocol.h \
            tst_tarot_3_and_5_players.h

SOURCES +=  main.cpp \
            tst_tarot_base.cpp \
            tst_tarot_rules.cpp \
            tst_tarot_protocol.cpp \
            tst_tarot_3_and_5_players.cpp

# End of project file
