mkdir build
cd build
mkdir Emscripten
cd ..
set EMCC_DEBUG=2
emcc test/main.cpp -O2 -std=c++11 -DWEB -o build/Emscripten/main.html
