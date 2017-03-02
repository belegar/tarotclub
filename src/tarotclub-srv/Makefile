################################################################################
# Makefile for building: tcds (TarotClub Dedicated Server)
################################################################################

# ==============================================================================
# General project paths
# ==============================================================================

# where to find source files

BASEDIR = ../tarotclub/
VPATH  =  $(addprefix $(BASEDIR), src src/library src/tcds src/jsengine src/json prj/tcds src/network src/zip src/lobby)
VPATH += src

# Output directories
OBJECTS_DIR    = build-tcds/release/obj
DESTDIR        = build-tcds/release/

# ==============================================================================
# Parse any make command line arguments
# ==============================================================================

# Platform settings, default (== no args) is good for Linux
ifeq ($(ARCH), mingw)
    SOURCES         = icon.rc
    USER_DEFINES    = -DUSE_WINDOWS_OS -mthreads
    LFLAGS          = -static-libgcc -static-libstdc++ -static -lpthread -Wl,-s -Wl,-subsystem,console 
    LIBS            = -lws2_32 -lpsapi
    DESTDIR_TARGET  = build-tcds/release/tcds.exe
	LINT_INC		= -IC:\\Qt\\Tools\\mingw491_32\\i686-w64-mingw32\\include\\c++ \
					-IC:\\Qt\\Tools\\mingw491_32\\i686-w64-mingw32\\include\\c++\\bits \
					-IC:\\Qt\\Tools\\mingw491_32\\i686-w64-mingw32\\include\\c++\\i686-w64-mingw32

else 
   USER_DEFINES     = -DUSE_UNIX_OS -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0
   LFLAGS           = -pthread -std=c++11
   LIBS		    = -ldl
   DESTDIR_TARGET   = build-tcds/release/tcds
endif

# Debug / release build differences
ifeq ($(TARGET), debug)
    OPTIM           = -O0 -g
    USER_DEFINES    += -DTAROT_DEBUG -DDUK_OPT_DEBUG
else
    OPTIM           = -O2
endif


# ==============================================================================
# Compiler, tools and options
# ==============================================================================

CC            = gcc
CXX           = g++
LINT          = c:\\lint\\lint-nt.exe
LINT_FLAGS    = -zero
PERL          = C:\\StrawberryPerl\\perl\\bin\\perl.exe
DEFINES       = -DUNICODE $(USER_DEFINES)
CFLAGS        = -pipe $(OPTIM) -pedantic -std=c99 -fstrict-aliasing -fomit-frame-pointer -Wall -Wextra -MMD $(DEFINES)
CXXFLAGS      = -pipe -std=c++11 $(OPTIM) -Wall -Wextra -frtti -fexceptions -MMD $(DEFINES)
INCPATH       = $(addprefix -I, $(VPATH))
LINKER        = g++
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

# ==============================================================================
# Files to build
# ==============================================================================

SOURCES += Log.cpp \
		Util.cpp \
		ByteArray.cpp \
		ByteStreamReader.cpp \
		ByteStreamWriter.cpp \
		TcpSocket.cpp \
		TcpServer.cpp \
		TcpServerBase.cpp \
		TcpClient.cpp \
		WebSocket.cpp \
		UniqueId.cpp \
		Value.cpp \
		Base64.cpp \
		Sha1.cpp \
		duktape.c \
		JsonWriter.cpp \
		JsonReader.cpp \
		JSEngine.cpp \
		JsonValue.cpp \
		ServerConfig.cpp \
		DealFile.cpp \
		Deck.cpp \
		Card.cpp \
		Player.cpp \
		Client.cpp \
		NetClient.cpp \
		Bot.cpp \
		Common.cpp \
		TarotEngine.cpp \
		Deal.cpp \
		Protocol.cpp \
		Controller.cpp \
		Score.cpp \
		System.cpp \
		main.cpp \
		Lobby.cpp \
		LobbyServer.cpp \
		Users.cpp \
		DataBase.cpp \
		sqlite3.c \
		CouchDb.cpp \
		Server.cpp \
		Http.cpp \
		Console.cpp \
		BotManager.cpp \
		Zip.cpp

CPP_FILES = $(filter %.cpp, $(SOURCES))
C_FILES = $(filter %.c, $(SOURCES))
RC_FILES = $(filter %.rc, $(SOURCES))

OBJECTS += $(addprefix $(OBJECTS_DIR)/, $(CPP_FILES:.cpp=.o))
OBJECTS += $(addprefix $(OBJECTS_DIR)/, $(C_FILES:.c=.o))
OBJECTS += $(addprefix $(OBJECTS_DIR)/, $(RC_FILES:.rc=.o))

# ==============================================================================
# Makefile targets
# ==============================================================================

all: make_dir tcds

tcds:  $(OBJECTS)
	@echo ' ' 
	@echo Linking $(DESTDIR_TARGET)
	@$(LINKER) $(LFLAGS) -L$(OBJECTS_DIR) -o $(DESTDIR_TARGET) $(OBJECTS) $(LIBS)

make_dir:
	@$(MKDIR) $(DESTDIR) $(OBJECTS_DIR)

clean:
	@echo Cleaning generated files ...
	@$(DEL_FILE) build-tcds/*

.PHONY: lint    
lint: run_lint
	@echo Filtering Lint results to keep project files only...
	@$(PERL) quality/pclint/parse_output.pl

.PHONY: run_lint
run_lint:
	@echo Linting files...
	$(LINT) -iquality/pclint $(LINT_FLAGS) co-gcc.lnt $(DEFINES) $(LINT_INC) $(INCPATH) $(CPP_FILES) > out.txt
    
# ==============================================================================
# Explicit rules
# ==============================================================================

# Clear built-in rules
.SUFFIXES:

$(OBJECTS_DIR)/%.o: %.cpp
	@echo Building $(notdir $<)
	@$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

$(OBJECTS_DIR)/%.o: %.c
	@echo Building $(notdir $<)
	@$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

$(OBJECTS_DIR)/%.o: %.rc
	@echo Creating $(notdir $@) from RC file
	windres -i $< -o $@ --include-dir=./prj/tcds

# ==============================================================================
# Dependency inclusion
# ==============================================================================

-include $(addprefix $(OBJECTS_DIR)/, $(OBJECTS:.o=.d))

# End of Makefile
