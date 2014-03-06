REM ----------------------------------------
REM Script to start 3 TarotClub executables
REM ----------------------------------------

REM Leave the tools directory
cd ..

REM Start the opponent 

cd build-desktop/Bender
start TarotClub.exe
cd ../..

cd build-desktop/Leela
start TarotClub.exe
cd ../..

cd build-desktop/Zoidberg
start TarotClub.exe
cd ../..

REM End of the script
