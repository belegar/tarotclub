# Makefile to build various TarotClub tools

.PHONY: tests
tests:
	cd build/tests; \
	qmake ../../tests/testu.pro; \
	make

.PHONY: console
console:
	cd build/console; \
	qmake ../../src/console/console.pro; \
	make

all: before tests console

clean:
	rm -rf build

.PHONY: before
before:
	mkdir -p build/tests; \
	mkdir -p build/desktop; \
	mkdir -p build/tcds; \
	mkdir -p build/botclient; \
	mkdir -p build/console;
	
# End of Makefile
