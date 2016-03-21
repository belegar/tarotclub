# TCDS  - TarotClub Dedicated Server #

This repository contains project files to build the TarotClub dedicated server executable.

# Building TarotClub Dedicated Server (TCDS)

There are three projects available:

  * A Visual Studio 2013 project file (Windows only)
  * A Qt project file
  * A hand-made Makefile

## Building using Visual Studio

Just open the solution file and build the target.
  
## Building using Qt

Either use QtCreator IDE or run on the command line:

```
qmake tcds.pro
make
```

## Building using the hand-made Makefile 
Another way is to use the hand-made "Makefile" available at the directory root. Some batch/sh command-line 
scripts are available.

Example under Windows:
```
cd tcds
build_tcds.bat
```

# TODO

  * Add a maximum play delay for network players, if timeout a random card is played
  * Log all the messages (per table and of the hall, one file per day)
  * Add memory stats and players stats in the JSON status command result
  
# HISTORY OF CHANGES

## Version 1.2.0

  * Added a default Log limit to 50MB per log file
  * Corrected a critical bug in the TCP server (bad socket management)
  * Added WebSocket protocol support

## Version 1.1.0

  * Integration of the CouchDb driver and deal JSON file upload at each end of deal
  * New tournament configuration (each deal can be configured individually)
  * Updated SQLite to 3.8.10.1
  * Removed dedicated server web interface
  * Added new console TCP/IP interface
  * Integration of AI tournament

## Version 1.0.0

  * Moved bot configuration into client parameters
  * Use of dedicated version for TCDS
  
