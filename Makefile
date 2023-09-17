exec = linear-assignment.out

source-files = src/main.c \
		src/interval/interval.c src/interval/interval_formatting.c \
		src/mapping/mapping.c src/mapping/mapping_formatting.c \
		src/solvers/iterative_solver.c
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
