#Config
c-compiler := gcc
python := python
python-venv := venv
default-visualization-port := 8050
ifndef $(port)
	port := $(default-visualization-port)
endif

# Compiler flags
library-flags := -lm -lpthread
debugging-flags := -Wall -Wextra # -pedantic -fsanitize=undefined,address -pg
optimization-flags := -O3
all-flags := $(library-flags) $(debugging-flags) $(optimization-flags)

# Folders
source-folder := src
exec-folder := build
object-folder := obj
lib-folder := $(source-folder)/lib
unit-test-folder := $(source-folder)/unit_test
performance-test-folder := $(source-folder)/performance_test

# Executables
exec-main := $(exec-folder)/main.out
exec-unit-tests := $(exec-folder)/unit-test.out
exec-performance-tests := $(exec-folder)/performance-test.out
visualize-performance-file := $(source-folder)/visualization/visualize_performance.py

# Source files
main-file := $(source-folder)/main.c
lib-files := $(shell find $(lib-folder) -name "*.c")
unit-test-files := $(shell find $(unit-test-folder) -name "*.c")
performance-test-files := $(shell find $(performance-test-folder) -name "*.c")

# Object files
lib-objects := $(patsubst $(source-folder)/%.c, $(object-folder)/%.o, $(lib-files))
unit-test-objects := $(patsubst $(source-folder)/%.c, $(object-folder)/%.o, $(unit-test-files))
performance-test-objects := $(patsubst $(source-folder)/%.c, $(object-folder)/%.o, $(performance-test-files))

# Build all executables
all: $(exec-main) $(exec-unit-tests) $(exec-performance-tests)

# Build main executable
$(exec-main): $(lib-objects) $(main-file) | $(exec-folder)
	@$(c-compiler) $^ -o $@ $(all-flags)

# Build unit tests
$(exec-unit-tests): $(lib-objects) $(unit-test-objects) | $(exec-folder)
	@$(c-compiler) $^ -o $@ $(all-flags)

# Build performance tests
$(exec-performance-tests): $(lib-objects) $(performance-test-objects) | $(exec-folder)
	@$(c-compiler) $^ -o $@ $(all-flags)

# Create folders
$(object-folder) $(exec-folder):
	@mkdir -p $@

# Build object files
$(object-folder)/%.o: $(source-folder)/%.c | $(object-folder)
	@mkdir -p $(@D)
	@$(c-compiler) -c $< -o $@

.PHONY: clean visualize

visualize: $(file) $(visualize-performance-file) | $(python-venv)
	@$(python-venv)/bin/python $(visualize-performance-file) --file $(file) --port $(port)

$(python-venv): requirements.txt
	@echo -e "Creating python venv and installing dependencies...\\n"
	@$(python) -m venv $(python-venv)
	@$(python-venv)/bin/pip install -r requirements.txt
	@echo -e "Virtual environment created successfully.\\n"

clean:
	rm -rdf $(exec-folder) $(object-folder)
