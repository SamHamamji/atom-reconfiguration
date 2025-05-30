# Config
CC := gcc
BUILD_TYPE := dev
PYTHON := python
PYTHON_VENV := .venv

# Directories
SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := obj

# Binaries
BINS := main unit_test linear_fuzz_test grid_fuzz_test linear_performance_test grid_performance_test

# Source files
SRC_lib := $(shell find $(SRC_DIR)/lib -name "*.c")
SRC_main := $(SRC_DIR)/main.c
SRC_unit_test := $(shell find $(SRC_DIR)/unit_test -name "*.c")
SRC_linear_fuzz_test := $(shell find $(SRC_DIR)/fuzz_test/linear_solver -name "*.c")
SRC_grid_fuzz_test := $(shell find $(SRC_DIR)/fuzz_test/grid_solver -name "*.c")
SRC_linear_performance_test := $(shell find $(SRC_DIR)/performance_test/linear_solver -name "*.c")
SRC_grid_performance_test := $(shell find $(SRC_DIR)/performance_test/grid_solver -name "*.c")

SRC_TO_OBJ = $(1:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
PYTHON_VISUALIZATION_FILE := $(SRC_DIR)/visualization/main.py

# Compiler flags
LIB_FLAGS := -lm -lpthread
WARNING_FLAGS := -Wall -Wextra -Wpedantic
DEBUGGING_FLAGS := -fsanitize=undefined,address -Og -g3
STATIC_ANALYSIS_FLAGS := -fanalyzer -Wno-analyzer-use-of-uninitialized-value -Wno-analyzer-possible-null-dereference -Wno-analyzer-possible-null-argument
OPTIMIZATION_FLAGS := -O3 -DNDEBUG
CFLAGS := -std=gnu2x $(LIB_FLAGS) $(WARNING_FLAGS)

ifeq ($(BUILD_TYPE), dev)
	CFLAGS += $(DEBUGGING_FLAGS)
else ifeq ($(BUILD_TYPE), release)
	CFLAGS += $(OPTIMIZATION_FLAGS)
else ifeq ($(BUILD_TYPE), static-analysis)
	CFLAGS += $(STATIC_ANALYSIS_FLAGS)
else
$(error "Invalid build type. Valid options are: dev, release, static-analysis")
endif

.PHONY: all
all: $(addprefix $(BIN_DIR)/, $(addsuffix .out, $(BINS)))

# Link object files
$(foreach b,$(BINS),$(eval $(BIN_DIR)/$b.out: \
	$(call SRC_TO_OBJ,$(SRC_lib)) $$(call SRC_TO_OBJ,$$(SRC_$b)) | $(BIN_DIR); \
	$$(info Building $$@...) \
	@$$(CC) -o $$@ $$^ $$(CFLAGS)))

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(@D)
	@$(CC) -c $< -o $@ $(CFLAGS)

# Create directories
$(OBJ_DIR) $(BIN_DIR):
	@mkdir -p $@

.PHONY: visualize
visualize: port ?= 8050
visualize: $(file) $(PYTHON_VISUALIZATION_FILE)
	@if [ ! -d "$(PYTHON_VENV)" ]; then \
		$(MAKE) $(PYTHON_VENV); \
	fi
	@$(PYTHON_VENV)/bin/python $(PYTHON_VISUALIZATION_FILE) --file $(file) --port $(port)

$(PYTHON_VENV): requirements.txt
	$(info Creating python venv and installing dependencies...)
	@$(PYTHON) -m venv $(PYTHON_VENV)
	@$(PYTHON_VENV)/bin/pip install -r requirements.txt
	$(info Virtual environment created successfully.)

.PHONY: clean
clean:
	@$(RM) -r $(BIN_DIR) $(OBJ_DIR)
