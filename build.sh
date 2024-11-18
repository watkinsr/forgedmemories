#!/usr/bin/env sh

set -x

g++ -Wall -Wextra -pedantic -O2 src/Common.cpp src/MapEditor.cpp -lSDL2  -lSDL2_mixer  -lSDL2  -lSDL2_ttf  -lSDL2  -lSDL2_image  -lSDL2  -lSDL2_mixer  -lSDL2_ttf  -lSDL2_image -g -I./include/ -I/usr/include/SDL2 -std=c++20 -o MapEditor
