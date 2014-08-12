@echo off
REM This batch script will build a release version of the TarotClub Dedicated Server (TCDS).
REM It is intended to be run under a Windows command prompt.
REM The following software must be available in the PATH: mingw32-make, MSYS, g++ (MinGW)

mingw32-make -j4 ARCH=mingw %1
