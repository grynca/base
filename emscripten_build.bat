@echo off
set mydir=%cd% 
if not defined EMSDK (
    cd c:\DEV\ENV\emscripten\
    call emsdk activate latest
)
cd %mydir%
mkdir build
cd build
mkdir Emscripten
cd ..
set EMCC_DEBUG=2
call emcc -Itest -Iinclude test/main.cpp -s TOTAL_MEMORY=134217728 -O3 -std=c++14 -DWEB -o build/Emscripten/main.html
