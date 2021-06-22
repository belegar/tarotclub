# Makefile to build WASM TarotClub library

TARGET_EXEC := index.js

OUTDIR := ./build
SRC_DIRS := ./ ../ ../../lib/icl/util ../../lib/icl/json ../../lib/icl/mbedtls/include/

CC = emcc
CXX = em++

TAROT_CORE_SRCS := Card.cpp Deck.cpp BasicClient.cpp Common.cpp Engine.cpp Identity.cpp Score.cpp Protocol.cpp
ICL_SRCS := Log.cpp Util.cpp

SOURCES += main.cpp
SOURCES += $(TAROT_CORE_SRCS)
SOURCES += $(ICL_SRCS)

VPATH := ../ ../../lib/icl/util

OBJECTS += $(addprefix $(OUTDIR)/,$(patsubst %.c, %.o, $(filter %.c,$(SOURCES))))
OBJECTS += $(addprefix $(OUTDIR)/,$(patsubst %.cpp, %.o, $(filter %.cpp,$(SOURCES))))
OBJECTS += $(addprefix $(OUTDIR)/,$(patsubst %.S, %.o, $(filter %.S,$(SOURCES))))
OBJECTS += $(addprefix $(OUTDIR)/,$(patsubst %.s, %.o, $(filter %.s,$(SOURCES))))

DEPENDENCIES := $(patsubst %.o,%.d,$(OBJECTS))

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS += -DUSE_LINUX_OS -s WASM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' 
CPPFLAGS += $(INC_FLAGS) -MMD -MP -std=c++17
LDFLAGS += -s LLD_REPORT_UNDEFINED -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' 

$(OUTDIR)/$(TARGET_EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# c source
$(addprefix $(OUTDIR)/, %.o): %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(addprefix $(OUTDIR)/, %.o): %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -rf $(OUTDIR)

-include $(DEPENDENCIES)

MKDIR_P ?= mkdir -p

# End of Makefile
