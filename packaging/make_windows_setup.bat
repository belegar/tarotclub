REM -------------------------------------------------------------
REM Manual step: download a tagged zip of source code from BitBucket
REM then unzip to generate a ./windows/tarotclub directory
REM -------------------------------------------------------------

echo off
cd windows

REM -------------------------------------------------------------
REM Build a release version of TarotClub
REM -------------------------------------------------------------

REM rm -rf tarotclub
rm *.exe
REM tar xf tarotclub.tgz
cd tarotclub
qmake prj\desktop.pro
make
qmake prj\botclient.pro
make

REM -------------------------------------------------------------
REM Make the setup executable
REM -------------------------------------------------------------
cd ..
compil32 /cc install_innosetup.iss
