# TarotClub project file

# directories for temp files
UI_DIR  = ./ui
UI_HEADERS_DIR = ./include
UI_SOURCES_DIR = ./src
OBJECTS_DIR = ./obj
DESTDIR = ./bin

# Qt build configuration
VPATH += $${PWD}/../src
QT      += xml network
RESOURCES = $${PWD}/../src/resources/resources.qrc
CONFIG += qt warn_on static

win32 {
    debug {
        CONFIG += console
    }
    RC_FILE = icon.rc
}

# generated
HEADERS = ui_AboutUI.h

HEADERS	+=  AboutWindow.h MainWindow.h OptionsWindow.h AvatarsWindow.h \
            ResultWindow.h ScoresDock.h InfosDock.h ChatDock.h \
            Tapis.h TextBox.h ConfigFile.h Game.h Jeu.h \
            Deck.h Card.h Player.h Client.h Bot.h

FORMS = AboutUI.ui ResultUI.ui OptionsUI.ui NetClientUI.ui AvatarsUI.ui

SOURCES	=   AboutWindow.cpp MainWindow.cpp OptionsWindow.cpp AvatarsWindow.cpp \
            ResultWindow.cpp ScoresDock.cpp InfosDock.cpp ChatDock.cpp \
            Tapis.cpp TextBox.cpp ConfigFile.cpp Game.cpp main.cpp Jeu.cpp \
            Deck.cpp Card.cpp Player.cpp Client.cpp Bot.cpp
