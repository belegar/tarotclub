1. Introduction
---------------

TarotClub is a multi-system compatible game, so please use 
the appropriate package to install it under your OS.

This build help is intented for developers only. TarotClub does 
not use autoconf and probably never will because it is an insane 
mess created by alien brains.

The client project file is "tarotclub/prj/TarotClub.pro"
The server project file is "tarotclub/prj/TarotClubd.pro"

2. Obtaining source code
------------------------

hg clone https://tarotclub@bitbucket.org/tarotclub/tarotclub

3. Dependencies
---------------

 - Qt library, 5.0 minimum.
 - Prefer the usage of QtCreator while hacking TarotClub

4. Compiling under Windows
--------------------------

The best way is to install the official Qt development tools 
from qt.nokia.com and use the QtCreator IDE. So simple.

5. Compiling under Linux
------------------------
 
Install the following packages to setup a development environment:

Debian: apt-get install build-essential qt4-dev-tools

Then, in TarotClub/pjt directory:

qmake <project-file>.pro
make
make install

The default install path is /opt/tarotclub, it can be overloaded 
by using the PREFIX=/your/path argument to qmake.
Warning: the /opt directory is probably restricted to the root. Use 
"sudo make install".

6. Compiling under MacOS X
--------------------------

TODO
