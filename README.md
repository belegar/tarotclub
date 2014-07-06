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

Thanks for your help and your support.

## Features

TarotClub provides a collection of software:

  * A desktop client/server executable made with Qt. It should work on any decent operating system supporting Qt.
  * A dedicated server without any GUI, written in raw C++0x11. It should compile with any decent compiler supporting C++0x11
  * A test engine to run automatic unit tests
  
The dedicated server should be used to setup a game server. It provides several game rooms that are available for everybody 
  
Current features are:

  * Quick or tournament game modes
  * Join or host network games
  * Play against the computer AI scripts written in JavaScript
  * JavaScript library to help you build your own AI scripts
  * Numbered deal
  * Deal editor
  * Game options including players and network parameters
  * Full Tarot cards images in vector format (SVG)
  * Dedicated server with multi game rooms, independent executable, no external libraries developed in a pure portable C++0x11 code
  * Advanced logging system with game events and memorized played deal reports
  
## Status

The current desktop version is stable and fully featured. Currently work in progress is focusing on:

  * API improvements
  * Dedicated server development
  
The highly possible next tasks will focus on the development on phone version, starting with Android. Please refer to 
the website to learn more on the next delivery contents.

# Installation guide

## Installation on Windows

Please use the provided setup executable. It should guide you seamlessly for a proper installation of the game.

## Installation on Debian

Please use the provided .deb package. It has been tested on the last version of Ubuntu and is dependant of the Qt libraries (Qt5.x).

## Installation on other system

For the moment, no any specific package is available. Linux users other than Debian-like operating system can use the tarball archive.
Since I have no Macintosh computer, I cannot provide the game on this system (try to build from the source).

# Building TarotClub

## Getting the source code

The source code is available on the Mercurial BitBucket depot. To get the source code, execute the following command:
```
hg clone https://tarotclub@bitbucket.org/tarotclub/tarotclub
```

You will need to install the Mercurial source code management client, available here [http://mercurial.selenic.com] or from your Linux distribution software center.
It is also possible to see the code by using the Web interface: [https://bitbucket.org/tarotclub/tarotclub/overview]

## How to compile TarotClub under Ubuntu

The best option is to use QtCreator since qmake projects (.pro files) are available. The following commands will help you to install everything:

  * apt-get install build-essential
  * apt-get install libglw1-mesa-dev
  * apt-get install libegl1-mesa-dev
  * apt-get install libgl1-mesa-dev
  * apt-get install qt5-default qttools5-dev-tools qttools5-doc

# Glossary and translations

  * Hearts = Coeur
  * Spades = Pic
  * Diamonds = Carreau
  * Clubs = Trèfle
  * Trumps = Atout
  * Oudler = Bout
  * Handler = Poignée
  * Slam = chelem
  * Trick = tour
  * Round = partie

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

# Network protocol

## Packet format

```
|  Header   |  Data  |
```

Header format is:

  * Size of message (2 bytes)
  * Protocol version (1 byte)
  * UUID (1 byte)
  * Command (1 byte)

Packet size is minimum 5 bytes (command without data).

## Cards and deck format

The Tarot cards are modeled using a string format. This format is used internally and by the Javascript IA scripts.
One card is represented as follow:

```
XX-Y
```
  * XX is the card facial value on two digits with a left leading zero for values inferior to ten. Fool is equal to zero, court cards  follow the ten in their standard order (Jack = 11, knight = 12, queen = 13, king = 14)
  * Y is the suit represented by the first suit letter (Trumps = T, Spades = S, Diamonds = D, Hearts = H, Clubs = C)

Examples:
```
7 of trumps is 07-T
queen of spades is 13-S
Fool of trumps is 00-T
One of diamonds is 01-D
```

# Javascript IA interface

The artificial intelligence of bot players are programmed using the JavaScript language. It is the same than the one for the Web 
except that some global objects specific to the browser are not available such as "window". The JavaScript follow the ECMA Script standard 5.1.

## Entry points

TODO.



