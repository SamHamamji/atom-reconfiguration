exec = linear-assignment.out

source-files = $(shell find src -name "*.c")

libraries-flags = -lm
install-path = ~/.local/bin/$(exec:.out=)


build: $(source-files)
	clang -g -o $(exec) $(source-files) $(libraries-flags)

run:
	make build
	./$(exec)

clean: $(exec)
	rm $(exec)

install:
	make
	cp $(exec) $(install-path)

uninstall: $(install-path)
	rm $(install-path)
