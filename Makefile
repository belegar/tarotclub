# Makefile to build various TarotClub tools

.PHONY: tests
tests:
	cd build/tests; \
	qmake ../../tests/testu.pro; \
	make

.PHONY: tarotclub-cli
tarotclub-cli:
	cd build/tarotclub-cli; \
	qmake ../../src/tarotclub-cli/tarotclub-cli.pro; \
	make

.PHONY: tarotclub-srv
tarotclub-srv:
    cd build/tarotclub-srv; \
    qmake ../../src/tarotclub-srv/tarotclub-srv.pro; \
    make

all: before tests console tarotclub-srv

# Generate the MSVC project files from the qmake .pro files
.PHONY: msvc_proj
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
