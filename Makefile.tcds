#############################################################################
# Makefile for building: tcds
#############################################################################

####### General project paths, where to find source files
VPATH  =   src src/library src/tcds src/jsengine src/json prj/tcds


# vpath %.cpp src/library

####### Compiler, tools and options

# Parse any make command line arguments
ifeq ($(ARCH), mingw)
    SOURCES         = icon.rc
    USER_DEFINES    = -DUSE_WINDOWS_OS
    LIBS            = -lws2_32
endif

# debug:  -DTAROT_DEBUG  -DDUK_OPT_DEBUG

CC            = gcc
CXX           = g++
DEFINES       = -DUNICODE $(USER_DEFINES)
CFLAGS        = -pipe -fno-keep-inline-dllexport -O2 -Os -pedantic -std=c99 -fstrict-aliasing -fomit-frame-pointer -Wall -Wextra -MMD $(DEFINES)
CXXFLAGS      = -pipe -fno-keep-inline-dllexport -std=c++11 -O2 -Wall -Wextra -frtti -fexceptions -mthreads -MMD $(DEFINES)
INCPATH       = -Iprj -I'src' -I'src/library' -I'src/tcds' -I'src/jsengine' -I'src/json' -I'.'
LINKER        = g++
LFLAGS        = -static-libgcc -static-libstdc++ -static -lpthread -Wl,-s -Wl,-subsystem,console -mthreads
COPY          = cp -f
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
DEL_FILE      = rm -rf
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
INSTALL_FILE    = cp -f
INSTALL_PROGRAM = cp -f
INSTALL_DIR     = $(COPY_DIR)

####### Output directory

OBJECTS_DIR    = build-server/release/obj
DESTDIR        = build-server/release/
DESTDIR_TARGET = build-server/release/tcds.exe

####### Files

SOURCES += Log.cpp \
		Util.cpp \
		ByteArray.cpp \
		ByteStreamReader.cpp \
		ByteStreamWriter.cpp \
		TcpSocket.cpp \
		TcpServer.cpp \
		TcpClient.cpp \
		UniqueId.cpp \
		Value.cpp \
		duktape.c \
		JsonWriter.cpp \
		JsonReader.cpp \
		JSEngine.cpp \
		JsonValue.cpp \
		ServerConfig.cpp \
		DealFile.cpp \
		TarotDeck.cpp \
		Deck.cpp \
		Card.cpp \
		Player.cpp \
		Client.cpp \
		Bot.cpp \
		Common.cpp \
		TarotEngine.cpp \
		Deal.cpp \
		Protocol.cpp \
		Controller.cpp \
		Table.cpp \
		Score.cpp \
		System.cpp \
		main.cpp \
		Lobby.cpp

CPP_FILES = $(filter %.cpp, $(SOURCES))
C_FILES = $(filter %.c, $(SOURCES))
RC_FILES = $(filter %.rc, $(SOURCES))

OBJECTS += $(addprefix $(OBJECTS_DIR)/, $(CPP_FILES:.cpp=.o))
OBJECTS += $(addprefix $(OBJECTS_DIR)/, $(C_FILES:.c=.o))
OBJECTS += $(addprefix $(OBJECTS_DIR)/, $(RC_FILES:.rc=.o))

####### Build rules

all: make_dir tcds

tcds:  $(OBJECTS)
	@echo ' ' 
	@echo Linking $(DESTDIR_TARGET)
	@$(LINKER) $(LFLAGS) -L$(OBJECTS_DIR) -o $(DESTDIR_TARGET) $(OBJECTS) $(LIBS)

make_dir:
	@$(MKDIR) $(DESTDIR) $(OBJECTS_DIR)
    
clean:
	@echo Cleaning generated files ...
	@$(DEL_FILE) build-server/*
  
####### Explicit rules

# Clear built-in rules
.SUFFIXES:

$(OBJECTS_DIR)/%.o: %.cpp
	@echo Building $(notdir $<)
	@$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

$(OBJECTS_DIR)/%.o: %.c
	@echo Building $(notdir $<)
	@$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

$(OBJECTS_DIR)/%.o: %.rc
	@echo Crating $(notdir $@) from RC file
	@windres -i $< -o $@ --include-dir=./prj/tcds $(DEFINES)

####### Dependency inclusion
-include $(addprefix $(OBJECTS_DIR)/, $(OBJECTS:.o=.d))
