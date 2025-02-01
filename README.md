# Description
(Unfinished) 2D Side-scrolling game in SDL2. Tested on Linux x86_64. Not sure about other platforms.

# Dependencies
SDL2, Emscripten, Bozohttpd or equivalent.

See: https://wiki.libsdl.org/SDL2/README/emscripten

See: http://www.eterna23.net/bozohttpd/

# Build
## Bozohttpd
```sh
tar -xf bozohttpd-20240126.tar.bz2
cd bozohttpd-20240126
make -f Makefile.boot
```

## Webassembly
```sh
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
git pull
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

## General
```sh
# In the project directory.
make
```

# Run
## Native
```sh
./build/game
```

```sh
./build/mapeditor
```

## WASM32
You can probably serve it from somewhere else but `bozohttpd` seems lightweight and works.
See: http://www.eterna23.net/bozohttpd/
```
./serve.sh
```
Then head to localhost:8000 to see the game.
