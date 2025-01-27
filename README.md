# Description
(Unfinished) 2D Side-scrolling game in SDL2.

# Dependencies
SDL2

# Build
```sh
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
## Webassembly
```sh
make && ./bozohttpd/bozohttpd-20240126/bozohttpd ~/workplace/<GAME_DIR> -f -I 8000 -X  -p ~/workplace/<GAME_DIR>
```
