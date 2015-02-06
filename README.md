# TarotClub, the free French Tarot card game!

## Introduction

TarotClub is a Tarot card game (not the divination one) playable on any computer 
with Mac OS X, Windows or Linux.
The Tarot card game is mostly played in french-spoken countries. It uses a specific 
deck of cards including standard card figures plus the Knight and 22 trumps. The complete 
rules of the Tarot card game is available in Wikipedia.

This project is a tentative to create a nice open source version of this game, playable in 
various operating system and with a modular design.

Project: https://bitbucket.org/tarotclub/tarotclub
Website: http://www.tarotclub.fr
Contact: support@tarotclub.fr

Thanks for your help and your support.
  
# Installation guide

## Installation on Windows

Please use the provided setup executable. It should guide you seamlessly for a proper installation of the game.

## Installation on Debian / Arch / Fedora

Please use the provided package. It has been tested on the last version of each system and is dependant of the Qt libraries (Qt5.x).

## Installation on other systems

For the moment, no any specific package is available for other Linux or BSD distribution. Consider building from the source code.
Since I have no Macintosh computer, I cannot provide the game on this system (try to build from the source).

# Building the game

See the official documentation on http://www.tarotclub.fr

# Credits / contributions

  * Hizoka, for all of his remarks, game testing and contributions
  * cledesol, for all his remarks and game testing
  * Duktape author, for his Javascript engine and helps



# Generated files

TarotClub generates some files to store the user configuration and various game information to find bugs or retreive past data.
The files are generated in the user's home directory:

  * in  ~/<username>/.tarotclub for Linux
  * in c:\users\<username>\.tarotclub for Windows

## Configuration files

Two configuration files are generated: tarotclub.json for the client preferences and tcds.json for the server configuration. Try to not modify by hand these files since everything is editable through the TarotClub GUI.

## Deal files

Each played deal generates a file in the folder *.tarotclub/games* in JSON format. This file contains the deal parameters, the scoring details and every trick played.

## Log files

One file per day is generated under *.tarotclub/logs*. The format is CSV (comma-separated values) so that it can be easily parsed or opened
 by spreadsheet editors such as Microsoft Excel or LibreOffice Calc.







