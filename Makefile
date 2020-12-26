CC = gcc
INCLUDES = -I./src/include
CFLAGS = -g $(INCLUDES)
MEMTEST = echo "\n$@\n" >>memlog && valgrind --leak-check=full --error-exitcode=123 --log-fd=9 9>>memlog
REPORT = (echo $?; cat memlog)

.PHONY: report clear_memlog clean test dist/tests/*

.SILENT:

all: preamble dist/lib/librefc.a

install: all
	cp -vf src/include/refc.h /usr/local/include/refc.h
	cp -vf dist/lib/librefc.a /usr/local/lib/librefc.a
	ldconfig

preamble:
	mkdir -p dist/tests
	mkdir -p dist/lib

test: clear_memlog dist/tests/refc
	echo
	echo ALL TESTS AND MEMORY CHECKS PASSED!
	echo

clear_memlog:
	echo > memlog

dist/tests/refc: src/tests/refc.c dist/lib/librefc.a
	$(CC) -o $@ $^ $(CFLAGS)
	$(MEMTEST) ./$@ || $(REPORT)

dist/lib/refc.o: src/lib/refc.c
	$(CC) -c -o $@ $^ $(CFLAGS)

dist/lib/librefc.a: dist/lib/refc.o
	ar -rc $@ $^

clean:
	rm dist/tests/*
	rm dist/lib/*
