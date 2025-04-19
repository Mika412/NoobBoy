BUILD_ROOT   := build
DESKTOP_DIR  := $(BUILD_ROOT)/desktop
WEB_DIR      := $(BUILD_ROOT)/web
TERM_DIR     := $(BUILD_ROOT)/terminal

# CMake Options for Different Targets
DESKTOP_OPTS := -DBUILD_DESKTOP=ON -DCMAKE_BUILD_TYPE=Release
WEB_OPTS     := -DBUILD_WEB=ON -DCMAKE_BUILD_TYPE=Release
TERM_OPTS    := -DBUILD_TERMINAL=ON -DCMAKE_BUILD_TYPE=Release

# Default target: build the desktop version
.PHONY: all
all: desktop

####################################
# Desktop Build
####################################

$(DESKTOP_DIR):
	@echo "Creating desktop build directory..."
	@mkdir -p $(DESKTOP_DIR)

.PHONY: desktop
desktop: $(DESKTOP_DIR)
	@echo "Configuring Desktop Build..."
	@cd $(DESKTOP_DIR) && cmake $(CURDIR) $(DESKTOP_OPTS)
	@echo "Building Desktop Version..."
	@$(MAKE) -C $(DESKTOP_DIR)
	@echo "Desktop build complete. Executable is in $(DESKTOP_DIR)"

####################################
# Clean Target
####################################
.PHONY: clean
clean:
	@echo "Removing all build directories..."
	@rm -rf $(BUILD_ROOT)
