all: main.c interval/interval.c interval/mapping.c
	clang -o a.out main.c interval/interval.c
clean: a.out
	rm a.out