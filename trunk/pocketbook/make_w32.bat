@echo off

set OUTPUT=wdominos.exe
set SOURCES=dmain.cpp dominos.cpp help.cpp

echo "WARNING: you can not build working PocketBook application in Windows"
echo "   Only win32 emulation supported from windows!"
echo

set P=C:\PBSDK
set PATH=%P%\bin;%PATH%

set INCLUDE=-I../include
set LIBS=-linkview -lfreetype -ljpeg -lz -lgdi32


g++ -static -Wall -O2 -fomit-frame-pointer %INCLUDE% -o %OUTPUT% %SOURCES% %IMAGES% %LIBS%
