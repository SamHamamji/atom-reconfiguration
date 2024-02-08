# Config
CC := gcc
python := python
python-venv := venv
build-type := dev

# Compiler flags
library-flags := -lm -lpthread
debugging-flags := -Wall -Wextra -Wpedantic -fsanitize=undefined,address -g -Og
optimization-flags := -O3
CFLAGS := -std=c2x $(library-flags)

ifeq ($(build-type), dev)
	CFLAGS += $(debugging-flags)
else ifeq ($(build-type), release)
	CFLAGS += $(optimization-flags)
else ifeq ($(build-type), scan-build)
	CC := scan-build $(CC)
else
	$(error "Invalid build type. Valid options are: dev, release, scan-build")
endif

# Folders
source-folder := src
exec-folder := build
object-folder := obj

# Executables
executables := main unit-test fuzz-test performance-test
visualize-performance-file := $(source-folder)/visualization/main.py

# Source files
lib-folder := $(source-folder)/lib
unit-test-folder := $(source-folder)/unit_test
fuzz-test-folder := $(source-folder)/fuzz_test
performance-test-folder := $(source-folder)/performance_test

main-files := $(source-folder)/main.c
lib-files := $(shell find $(lib-folder) -name "*.c")
unit-test-files := $(shell find $(unit-test-folder) -name "*.c")
fuzz-test-files := $(shell find $(fuzz-test-folder) -name "*.c")
performance-test-files := $(shell find $(performance-test-folder) -name "*.c")

# Object files
source-to-object = $(1:$(source-folder)/%.c=$(object-folder)/%.o)
lib-objects := $(call source-to-object,$(lib-files))

.PHONY: all
all: $(addprefix $(exec-folder)/, $(addsuffix .out, $(executables)))

# Link object files
$(foreach b,$(executables),$(eval $(exec-folder)/$b.out: \
		$$(lib-objects) $$(call source-to-object, $$($b-files)) | $(exec-folder); \
		$$(info Building $$@...) \
		@$$(CC) $$(CFLAGS) -o $$@ $$^))

# Compile source files to object files
$(object-folder)/%.o: $(source-folder)/%.c | $(object-folder)
	@mkdir -p $(@D)
	@$(CC) -c $< -o $@

# Create folders
$(object-folder) $(exec-folder):
	@mkdir -p $@

.PHONY: visualize
visualize: port ?= 8050
visualize: $(file) $(visualize-performance-file)
	@if [ ! -d "$(python-venv)" ]; then \
		$(MAKE) $(python-venv); \
	fi
	@$(python-venv)/bin/python $(visualize-performance-file) --file $(file) --port $(port)

$(python-venv): requirements.txt
	$(info Creating python venv and installing dependencies...)
	@$(python) -m venv $(python-venv)
	@$(python-venv)/bin/pip install -r requirements.txt
	$(info Virtual environment created successfully.)

.PHONY: clean
clean:
	@$(RM) -r $(exec-folder) $(object-folder)
