# ==============================================================================
# Pinch - Makefile
# ==============================================================================
# Pinch is a utility for cutting the middle out of log/text files.
# This Makefile builds the project using C++17.

# Compiler and Flags
CXX          := g++
CXXFLAGS     := -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 \
                -Wcast-align -Wconversion -Wsign-conversion -Wnull-dereference \
                -Iinclude -Iinclude/extern -MMD -MP

# Optimization & Debug Flags
OPTFLAGS     := -O3
DEBUGFLAGS   := -g

# Directories
SRC_DIR      := src
BUILD_DIR    := build

# Installation paths
PREFIX       ?= /usr/local
BINDIR       ?= $(PREFIX)/bin
MANDIR       ?= $(PREFIX)/share/man/man1

# Target executable name
TARGET       := pinch

# Source and Object files
SRCS         := $(wildcard $(SRC_DIR)/*.cpp)
OBJS         := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS         := $(OBJS:.o=.d)

# ------------------------------------------------------------------------------
# Build Rules
# ------------------------------------------------------------------------------

# Default target (Release build)
all: CXXFLAGS += $(OPTFLAGS)
all: $(TARGET)

# Debug target (adds debug symbols)
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(TARGET)

# Rule to link the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "=============================================="
	@echo " Successfully built: $@"
	@echo "=============================================="

# Rule to compile source files to object files in the build directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Include generated dependency files (so changes to headers trigger rebuilds)
-include $(DEPS)

# ------------------------------------------------------------------------------
# Cleaning Rules
# ------------------------------------------------------------------------------

# Clean object files and dependency files
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build objects and dependencies."

# Clean everything including the executable
fclean: clean
	rm -f $(TARGET)
	@echo "Cleaned executable: $(TARGET)"

# Rebuild target
re: fclean all

# ------------------------------------------------------------------------------
# Installation Rules
# ------------------------------------------------------------------------------

# Install the application and the man page
install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	mkdir -p $(DESTDIR)$(MANDIR)
	cp -f $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)
	chmod 755 $(DESTDIR)$(BINDIR)/$(TARGET)
	cp -f pinch.1 $(DESTDIR)$(MANDIR)/pinch.1
	chmod 644 $(DESTDIR)$(MANDIR)/pinch.1
	@echo "Installed $(TARGET) to $(DESTDIR)$(BINDIR) and man page to $(DESTDIR)$(MANDIR)"

# Uninstall the application and the man page
uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)
	rm -f $(DESTDIR)$(MANDIR)/pinch.1
	@echo "Uninstalled $(TARGET) and man page."

.PHONY: all debug clean fclean re install uninstall
