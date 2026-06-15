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

# Debian packaging settings
PACKAGE_NAME    := pinch
PACKAGE_VERSION ?= 1.0.0
PACKAGE_ARCH    := $(shell dpkg --print-architecture 2>/dev/null || echo "amd64")
DEB_STAGE_DIR   := build/deb_pkg
DIST_DIR        := dist
DEB_FILE        := $(DIST_DIR)/$(PACKAGE_NAME)_$(PACKAGE_VERSION)_$(PACKAGE_ARCH).deb

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

# Clean everything including the executable and packaged binaries
fclean: clean
	rm -f $(TARGET)
	rm -rf $(DIST_DIR)
	@echo "Cleaned executable and distribution files."

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

# ------------------------------------------------------------------------------
# Debian Packaging
# ------------------------------------------------------------------------------

# Build a Debian package in the dist/ folder
deb: all
	@echo "Creating Debian package structure..."
	rm -rf $(DEB_STAGE_DIR)
	mkdir -p $(DEB_STAGE_DIR)/DEBIAN
	mkdir -p $(DEB_STAGE_DIR)/usr/bin
	mkdir -p $(DEB_STAGE_DIR)/usr/share/man/man1
	mkdir -p $(DEB_STAGE_DIR)/usr/share/doc/$(PACKAGE_NAME)
	# Copy files to staging area
	cp -f $(TARGET) $(DEB_STAGE_DIR)/usr/bin/$(TARGET)
	chmod 755 $(DEB_STAGE_DIR)/usr/bin/$(TARGET)
	cp -f pinch.1 $(DEB_STAGE_DIR)/usr/share/man/man1/pinch.1
	chmod 644 $(DEB_STAGE_DIR)/usr/share/man/man1/pinch.1
	cp -f LICENSE $(DEB_STAGE_DIR)/usr/share/doc/$(PACKAGE_NAME)/copyright
	chmod 644 $(DEB_STAGE_DIR)/usr/share/doc/$(PACKAGE_NAME)/copyright
	# Generate DEBIAN/control file
	@echo "Package: $(PACKAGE_NAME)" > $(DEB_STAGE_DIR)/DEBIAN/control
	@echo "Version: $(PACKAGE_VERSION)" >> $(DEB_STAGE_DIR)/DEBIAN/control
	@echo "Section: utils" >> $(DEB_STAGE_DIR)/DEBIAN/control
	@echo "Priority: optional" >> $(DEB_STAGE_DIR)/DEBIAN/control
	@echo "Architecture: $(PACKAGE_ARCH)" >> $(DEB_STAGE_DIR)/DEBIAN/control
	@echo "Maintainer: Nikole Smith <appsolutionz.com>" >> $(DEB_STAGE_DIR)/DEBIAN/control
	@echo "Description: Truncates and displays the start and end of text streams." >> $(DEB_STAGE_DIR)/DEBIAN/control
	@echo " Pinch is a high-performance command-line utility that extracts" >> $(DEB_STAGE_DIR)/DEBIAN/control
	@echo " the head and tail of an input stream and discards the middle," >> $(DEB_STAGE_DIR)/DEBIAN/control
	@echo " ensuring a constant memory footprint even with multi-gigabyte logs." >> $(DEB_STAGE_DIR)/DEBIAN/control
	# Build the package
	mkdir -p $(DIST_DIR)
	dpkg-deb --build $(DEB_STAGE_DIR) $(DEB_FILE)
	@echo "Debian package successfully built: $(DEB_FILE)"

.PHONY: all debug clean fclean re install uninstall deb

