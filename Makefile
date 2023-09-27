exec-folder = build/
exec = $(exec-folder)linear-assignment.out
exec-unit-tests = $(exec-folder)unit-tests.out

main-file = src/main.c
source-files = $(shell find src -name "*.c" -not -path $(main-file))
unit-test-files = $(shell find test/unit_test -name "*.c")
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

run-main: $(exec)
	./$(exec)

run-unit-tests: $(exec-unit-tests)
	./$(exec-unit-tests)

clean:
	rm -rd $(exec-folder)

install:
	make build
	cp $(exec) $(install-path)

uninstall: $(install-path)
	rm $(install-path)
