QT      += xml network
DESTDIR = ../../bin
CONFIG += qt console warn_on static

win32 {
    RC_FILE = icon.rc
}

HEADERS	= ServerConsole.h \
        ServerConsole.h \
        TarotEngine.h \
        ../defines.h \
        ../base/Card.h \
        ../base/Deck.h \
        ../base/Jeu.h \
        ../base/Player.h \
        ../base/Score.h

SOURCES	= main.cpp \
        ServerConsole.cpp \
        TarotEngine.cpp \
        ../base/Card.cpp \
        ../base/Deck.cpp \
        ../base/Jeu.cpp \
        ../base/Player.cpp \
        ../base/Score.cpp

