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

BASE_DIR = $${PWD}

CONFIG(debug, debug|release) {
debug:      DESTDIR = $$BASE_DIR/build-icl_tests/debug
}

CONFIG(release, debug|release) {
release:    DESTDIR = $$BASE_DIR/build-icl_tests/release
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
VPATH += $$BASE_DIR/tests
INCLUDEPATH += $$BASE_DIR/tests

ICL_DIR = $$BASE_DIR
include($$ICL_DIR/icl.pri)

# ------------------------------------------------------------------------------
# COMPILER CONFIG
# ------------------------------------------------------------------------------

QT       += testlib xml
QT       -= gui

TARGET = unit_tests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11  -fprofile-arcs -ftest-coverage

QMAKE_CFLAGS_DEBUG +=  -O0  -ggdb -pedantic -std=c99 -fstrict-aliasing  -fprofile-arcs -ftest-coverage
DEFINES += DUK_OPT_SELF_TESTS
DEFINES += DEAL_TEST

# Specific OS stuff
win32 {
    LIBS +=  libws2_32 libgcov -lpsapi
    DEFINES += USE_WINDOWS_OS
}
unix {
    DEFINES += USE_UNIX_OS
    LIBS += -ldl -lgcov
}

CONFIG(debug, debug|release) {
    DEFINES += TAROT_DEBUG
    DEFINES += DUK_OPT_DEBUG
}

# ------------------------------------------------------------------------------
# Unit test files
# ------------------------------------------------------------------------------

HEADERS +=  tst_database.h \
            tst_hash.h \
            tst_json.h \
            tst_tcp_socket.h \
            tst_utilities.h \
            tst_webstuff.h

SOURCES +=  main.cpp \
            tst_database.cpp \
            tst_hash.cpp \
            tst_json.cpp \
            tst_tcp_socket.cpp \
            tst_utilities.cpp \
            tst_webstuff.cpp

# End of project file
