UI_DIR  = windows/ui
QT      += xml network
DESTDIR = ../bin
RESOURCES = resources/resources.qrc
CONFIG += qt warn_on static

win32 {
    debug {
        CONFIG += console
    }
    RC_FILE = icon.rc
}


HEADERS	= windows/AboutWindow.h \
	windows/MainWindow.h \
	windows/OptionsWindow.h \
	windows/AvatarsWindow.h \
	windows/ResultWindow.h \
    windows/ScoresDock.h \
    windows/InfosDock.h \
    windows/ChatDock.h \
	windows/Tapis.h \
	windows/TextBox.h \
	ConfigFile.h \
	Game.h \
	defines.h \
    textes.h \
	base/Jeu.h \
	base/Deck.h \
    base/Card.h \
	base/Player.h \
    base/Client.h \
    base/Bot.h
        
FORMS   =   windows/ui/AboutUI.ui \
            windows/ui/ResultUI.ui \
            windows/ui/OptionsUI.ui \
            windows/ui/NetClientUI.ui \
            windows/ui/AvatarsUI.ui

SOURCES	= windows/AboutWindow.cpp \
	windows/MainWindow.cpp \
	windows/OptionsWindow.cpp \
	windows/AvatarsWindow.cpp \
	windows/ResultWindow.cpp \
    windows/ScoresDock.cpp \
    windows/InfosDock.cpp \
    windows/ChatDock.cpp \
	windows/Tapis.cpp \
	windows/TextBox.cpp \
	ConfigFile.cpp \
	Game.cpp \
	main.cpp \
    base/Jeu.cpp \
	base/Deck.cpp \
    base/Card.cpp \
	base/Player.cpp \
    base/Client.cpp \
    base/Bot.cpp
