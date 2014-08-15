#!/bin/sh

# Shell command line argument is passed to the makefile
# Example: build_tcds.sh clean
make -f Makefile.tcds -j4 $1

# End of shell script

