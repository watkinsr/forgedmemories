all: game

game: src/Game.cpp src/Log.cpp src/Common.cpp
	gcc src/Game.cpp src/Log.cpp src/Common.cpp -I./include -I/usr/include/SDL2 -lSDL2 -lSDL2_ttf -lSDL2_image -lstdc++ -o build/game
