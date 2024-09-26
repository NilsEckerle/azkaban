# Define variables
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c17 -D_GNU_SOURCE -g -I/usr/include/ -I/usr/include/x86_64-linux-gnu
CFLAGS = -std=c17 -D_GNU_SOURCE -g -I/usr/include/ -I/usr/include/x86_64-linux-gnu
# MacOS
CFLAGS = -std=c17 -D_GNU_SOURCE -g -I/usr/local/include/ -I/opt//homebrew/Cellar/sqlcipher/4.6.1/include -I/Library/Developer/CommandLineTools/SDKs/MacOSX13.3.sdk/usr/include/libxml/threads.h
CFLAGS_END = -lsqlcipher -lykpiv
SRCDIR = src
BUILDDIR = build
CLI_TARGET = azkaban-cli
TUI_TARGET = azkaban_tui
GUI_TARGET = azkaban

# Source files
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

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


