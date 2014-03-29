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
  * Dedicated server with multi game rooms, independent executable
  * Advanced logging system with game events and memorized played deal reports
  
## Status

The current desktop version is stable and fully featured. Currently work in progress is focusing on:

  * API improvements
  * Dedicated server development
  
The highly possible next tasks will focus on the development on phone version, starting with Android. Please refer to 
the website to learn more on the next delivery contents.

# TarotClub network protocol

# Installation guide

## Installation on Windows

Please use the provided setup executable. It should guide you seamlessly for a proper installation of the game.

## Installation on Debian

Please use the provided .deb package. It has been tested on the last version of Ubuntu and is dependant of the Qt libraries (Qt5.x).

## Installation on other system

For the moment, no any specific package is available. Linux users other than Debian-like operating system can use the tarball archive.
Since I have no Macintosh computer, I cannot provide the game on this system (try to build from the source).

# Building TarotClub

## Building using Qt

It is the best option since qmake projects are available for QtCreator.



