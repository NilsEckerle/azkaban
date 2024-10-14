# # Define variables
# CC = gcc
# CFLAGS = -Wall -Wextra -pedantic -std=c17 -D_GNU_SOURCE -g -I/usr/include/ -I/usr/include/x86_64-linux-gnu
# CFLAGS = -std=c17 -D_GNU_SOURCE -g -I/usr/include/ -I/usr/include/x86_64-linux-gnu
# CFLAGS_END = -lsqlcipher -lykpiv -lSDL2
# # MacOS
# CFLAGS = -std=c17 -D_GNU_SOURCE -g -I/opt/homebrew/Cellar/sdl2/2.30.8/include -I/opt/homebrew/Cellar/sdl2_image/2.8.2_2/include -I/usr/local/include/ -I/opt//homebrew/Cellar/sqlcipher/4.6.1/include 
# SRCDIR = src
# BUILDDIR = build
CLI_TARGET = azkaban-cli
TUI_TARGET = azkaban_tui
GUI_TARGET = azkaban
#
# # Source files
# SRC = $(wildcard $(SRCDIR)/*.c)
# OBJ = $(SRC:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)



# Define variables
CC = gcc
CFLAGS_DEBUG = -Wall -Wextra -pedantic -g
CFLAGS_COMMON = $(CFLAGS_DEBUG) -std=c17 -D_GNU_SOURCE
LIBS_COMMON = -lsqlcipher -lykpiv

# Linux configuration
CFLAGS_LINUX = $(CFLAGS_COMMON) -I/usr/include/ -I/usr/include/x86_64-linux-gnu
LIBS_LINUX = $(LIBS_COMMON) -lSDL2

# MacOS configuration
CFLAGS_MAC = -I/opt/homebrew/Cellar/sqlcipher/4.6.1/include -I/opt/homebrew/Cellar/sdl2/2.30.8/include -I/opt/homebrew/Cellar/sdl2_image/2.8.2_2/include -I/opt/homebrew/Cellar/sdl2_ttf/2.22.0/include $(CFLAGS_COMMON) 
LIBS_MAC = $(LIBS_COMMON) -lSDL2

# Source files
SRCDIR = src
BUILDDIR = build
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

# Platform-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CFLAGS = $(CFLAGS_LINUX)
	LIBS = $(LIBS_LINUX)
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS = $(CFLAGS_MAC)
	LIBS = $(LIBS_MAC)
endif

CFLAGS_END = $(LIBS)



# Default TARGET
all: clean setup $(GUI_TARGET) $(TUI_TARGET) $(CLI_TARGET)

clean:
	@echo "Cleaning up..."
	@if [ -d $(BUILDDIR) ]; then rm -r $(BUILDDIR); fi

setup:
	@echo "Setting up directories..."
	@mkdir -p $(BUILDDIR)

# Compile target: build the executable
$(GUI_TARGET): $(filter-out $(BUILDDIR)/$(CLI_TARGET).o $(BUILDDIR)/$(TUI_TARGET).o, $(OBJ))
	@echo "Linking for gui..."
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $^ $(CFLAGS_END)

# Compile target: build the executable
$(CLI_TARGET): $(filter-out $(BUILDDIR)/$(TUI_TARGET).o $(BUILDDIR)/$(GUI_TARGET).o, $(OBJ))
	@echo "Linking for cli..."
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $^ $(CFLAGS_END)

# Compile target: build the executable
$(TUI_TARGET): $(filter-out $(BUILDDIR)/$(CLI_TARGET).o $(BUILDDIR)/$(GUI_TARGET).o, $(OBJ))
	@echo "Linking for tui..."
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $^ $(CFLAGS_END)

# Compile object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo "Comppiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@ $(CFLAGS_END)

# Phony targets
.PHONY: all clean setup


