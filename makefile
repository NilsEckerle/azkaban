# Define variables
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c17
SRCDIR = src
BUILDDIR = build
TARGET = azkaban

# Source files
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

# Default TARGET
all: clean setup $(TARGET)

clean:
	@echo "Cleaning up..."
	@if [ -d $(BUILDDIR) ]; then rm -r $(BUILDDIR); fi

setup:
	@echo "Setting up directories..."
	@mkdir -p $(BUILDDIR)

# Comp9ile target: build the executable
$(TARGET): $(OBJ)
	@echo "Linking..."
	$(CC) $(CFLAGS) -o $@ $^

# Compile object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo "Comppiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Phony targets
.PHONY: all clean setup
