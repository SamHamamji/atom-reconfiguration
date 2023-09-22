exec-folder = build/
exec = $(exec-folder)linear-assignment.out
exec-tests = $(exec-folder)tests.out

main-file = src/main.c
source-files = $(shell find src -name "*.c" -not -path $(main-file))
test-files = $(shell find test -name "*.c")
unity-file = unity/unity.c

libraries-flags = -lm
install-path = ~/.local/bin/$(exec:.out=)

check-build-folder:
	mkdir -p $(exec-folder)

build: $(main-file) $(source-files)
	make check-build-folder
	clang -o $(exec) $(main-file) $(source-files) $(libraries-flags)

build-tests:
	make check-build-folder
	clang -o $(exec-tests) $(source-files) $(test-files) $(unity-file) $(libraries-flags)

run:
	make build
	./$(exec)

run-tests:
	make build-tests
	./$(exec-tests)

clean:
	rm -rd $(exec-folder)

install:
	make build
	cp $(exec) $(install-path)

uninstall: $(install-path)
	rm $(install-path)
