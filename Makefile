CC = gcc
INCLUDES = -I./src/include
CFLAGS = -g $(INCLUDES)
MEMTEST = echo "\n$@\n" >>memlog && valgrind --leak-check=full --error-exitcode=123 --log-fd=9 9>>memlog
REPORT = (echo $?; cat memlog)

.PHONY: report clear_memlog clean test dist/tests/*

.SILENT:

all: preamble dist/lib/librefc.a

install: all
	echo installing pkg-config metadata...
	cp -f librefc.pc /usr/local/lib/pkgconfig/
	echo installing development headers...
	cp -f src/include/refc.h /usr/local/include/refc.h
	echo installing static library...
	cp -f dist/lib/librefc.a /usr/local/lib/librefc.a
	echo updating library database...
	ldconfig > /dev/null
	echo installing documentation...
	cp -f man3/*.3 /usr/local/share/man/man3/
	echo updating manual database...
	mandb > /dev/null
	echo done

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
	cd man3 && src2man -r librefc ../$< && cd ..
	$(CC) -c -o $@ $^ $(CFLAGS)

dist/lib/librefc.a: dist/lib/refc.o
	ar -rc $@ $^

clean:
	rm dist/tests/*
	rm dist/lib/*
