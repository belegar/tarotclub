# Makefile to build various TarotClub tools

.PHONY: before
before:
	mkdir -p build/tests
	mkdir -p build/desktop
	mkdir -p build/tcds
	mkdir -p build/botclient
	mkdir -p build/console

.PHONY: tests
tests:
	echo "toto"; \
	cd build/tests; \
	qmake ../../tests/testu.pro; \
	make

all: before tests

clean:
	rm -rf build

# End of Makefile
