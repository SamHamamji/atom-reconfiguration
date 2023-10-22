exec-folder := build/
exec := $(exec-folder)linear-assignment.out
exec-unit-tests := $(exec-folder)unit-tests.out
exec-performance-tests := $(exec-folder)performance-tests.out
visualize-performance-file := visualization/visualize_performance.py

main-file := src/main.c
source-files := $(shell find src -name "*.c" -not -path $(main-file))
unit-test-files := $(shell find test/unit_test -name "*.c")
performance-test-files := $(shell find test/performance_test -name "*.c")

libraries-flags := -lm
install-path := ~/.local/bin/$(exec:.out=)

default-visualization-port := 8050
ifndef $(port)
	port := $(default-visualization-port)
endif

check-build-folder:
	mkdir -p $(exec-folder)

build-main: $(main-file) $(source-files)
	make check-build-folder
	clang -o $(exec) $(main-file) $(source-files) $(libraries-flags)

build-unit-tests: $(source-files) $(unit-test-files)
	make check-build-folder
	clang -o $(exec-unit-tests) $(source-files) $(unit-test-files) $(libraries-flags)

build-performance-tests: $(source-files) $(performance-test-files)
	make check-build-folder
	clang -o $(exec-performance-tests) $(source-files) $(performance-test-files) $(libraries-flags)

run-main: $(exec)
	./$(exec)

run-unit-tests: $(exec-unit-tests)
	./$(exec-unit-tests)

run-performance-tests: $(exec-performance-tests)
	./$(exec-performance-tests)

visualize: $(file) $(visualize-performance-file)
	python $(visualize-performance-file) --file $(file) --port $(port)

clean:
	rm -rd $(exec-folder)

install:
	make build
	cp $(exec) $(install-path)

uninstall: $(install-path)
	rm $(install-path)
