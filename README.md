# Description
(Unfinished) 2D Side-scrolling game in SDL2. Similar in style to old Nintendo games. Works on Native Linux x86_64 and WASM32. Other platforms not supported (yet)

![screenshot](./screenshot.png)

# Dependencies
SDL2, Emscripten, Bozohttpd or equivalent.

See: https://wiki.libsdl.org/SDL2/README/emscripten

See: http://www.eterna23.net/bozohttpd/

# Getting started
## Build
- First compile the SDL2 dependencies.
```sh
./scripts/bootstrap_deps
```
- Then make the MapEditor/Game
```sh
# In the project directory.
make
```
## Run
### Game
```sh
./build/game
```
### MapEditor
```sh
./build/mapeditor
```

## WASM32
### Build EMCC Dependency
```sh
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
git pull
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
make
```

### Run
```
make -B index.html
```
Then head to 0.0.0.0:8000 to see the game. It's in a very immature state and *highly* to break.