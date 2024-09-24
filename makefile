# Define variables
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c17 -D_GNU_SOURCE -g -I/usr/include/ -I/usr/include/x86_64-linux-gnu
CFLAGS = -std=c17 -D_GNU_SOURCE -g -I/usr/include/ -I/usr/include/x86_64-linux-gnu
CFLAGS_END = -lsqlcipher -lykpiv
SRCDIR = src
BUILDDIR = build
TARGET = azkaban
TARGET-CLI = azkaban-cli

# Source files
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

# Default TARGET
all: clean setup $(TARGET) $(TARGET-CLI)

clean:
	@echo "Cleaning up..."
	@if [ -d $(BUILDDIR) ]; then rm -r $(BUILDDIR); fi

setup:
	@echo "Setting up directories..."
	@mkdir -p $(BUILDDIR)

# Compile target: build the executable
$(TARGET): $(OBJ)
	@echo "Linking..."
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $(filter-out $(BUILDDIR)/$(TARGET-CLI).o, $^) $(CFLAGS_END)

$(TARGET-CLI): $(OBJ)
	@echo "Linking..."
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $(filter-out $(BUILDDIR)/$(TARGET).o, $^) $(CFLAGS_END)

# Compile object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo "Comppiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@ $(CFLAGS_END)

# Phony targets
.PHONY: all clean setup


