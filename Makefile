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

# Generate the MSVC project files from the qmake .pro files
msvc_proj:
	cd src/tarotclub-cli; \
	qmake -spec win32-msvc2013 -tp vc

clean:
	rm -rf build

.PHONY: before
before:
	mkdir -p build/tests; \
	mkdir -p build/tarotclub-gui; \
	mkdir -p build/tarotclub-srv; \
	mkdir -p build/botclient; \
	mkdir -p build/tarotclub-cli;

# End of Makefile
