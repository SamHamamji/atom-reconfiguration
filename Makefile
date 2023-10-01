exec-folder = build/
exec = $(exec-folder)linear-assignment.out
exec-unit-tests = $(exec-folder)unit-tests.out
exec-performance-tests = $(exec-folder)performance-tests.out
visualize-performance-file = visualization/visualize_performance.py

main-file = src/main.c
source-files = $(shell find src -name "*.c" -not -path $(main-file))
unit-test-files = $(shell find test/unit_test -name "*.c")
performance-test-files = $(shell find test/performance_test -name "*.c")
unity-file = unity/unity.c

libraries-flags = -lm
install-path = ~/.local/bin/$(exec:.out=)

check-build-folder:
	mkdir -p $(exec-folder)

build-main: $(main-file) $(source-files)
	make check-build-folder
	clang -o $(exec) $(main-file) $(source-files) $(libraries-flags)

build-unit-tests: $(source-files) $(unit-test-files) $(unity-file)
	make check-build-folder
	clang -o $(exec-unit-tests) $(source-files) $(unit-test-files) $(unity-file) $(libraries-flags)

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
	python $(visualize-performance-file) $(file)

clean:
	rm -rd $(exec-folder)

install:
	make build
	cp $(exec) $(install-path)

uninstall: $(install-path)
	rm $(install-path)
