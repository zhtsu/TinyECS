@echo off
if not exist build md build
cd build
if exist CMakeCache.txt del CMakeCache.txt
cmake -G "MinGW Makefiles" ..
mingw32-make -f Makefile
cd bin
Alice
pause
