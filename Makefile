CC := g++
CPPFLAGS := -g -std=c++20 -fpermissive
LDFLAGS := -g
LDLIBS := -L$(HOME)/.local/share/thirdparty_opensource/lib -lSDL2 -lSDL2_ttf -lSDL2_image -lstdc++ -Wl,-rpath,$(HOME)/.local/share/thirdparty_opensource/lib
OBJDIR := objdir

GAME_OBJS := $(addprefix $(OBJDIR)/,Game.o Common.o Log.o)
MAPEDITOR_OBJS := $(addprefix $(OBJDIR)/,MapEditor.o Common.o Log.o Algorithm.o)

# Note: $(HOME)/.local/share/thirdparty_opensource is defined in scripts/build.sh
INCL_CC := -I./include -I$(HOME)/.local/share/thirdparty_opensource/include/SDL2/
SRC_CC := src/Game.cpp src/Common.cpp src/Log.cpp

all: build/game build/mapeditor

$(OBJDIR)/Algorithm.o: src/Algorithm.cpp include/Algorithm.h
	$(CC) $(CPPFLAGS) -c $(INCL_CC) src/Algorithm.cpp -o $(OBJDIR)/Algorithm.o

$(OBJDIR)/Game.o: src/Game.cpp include/Map.h include/Types.h
	$(CC) $(CPPFLAGS) -c $(INCL_CC) src/Game.cpp -o $(OBJDIR)/Game.o

$(OBJDIR)/Common.o: src/Common.cpp include/Map.h
	$(CC) $(CPPFLAGS) -c $(INCL_CC) src/Common.cpp -o $(OBJDIR)/Common.o

$(OBJDIR)/Log.o: src/Log.cpp
	$(CC) $(CPPFLAGS) -c $(INCL_CC) src/Log.cpp -o $(OBJDIR)/Log.o

$(OBJDIR)/MapEditor.o: src/MapEditor.cpp include/MapEditor.h include/Algorithm.h include/Types.h
	$(CC) $(CPPFLAGS) -c $(INCL_CC) src/MapEditor.cpp -o $(OBJDIR)/MapEditor.o

$(GAME_OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

build/mapeditor: $(MAPEDITOR_OBJS)
	mkdir -p build
	g++ $(CPPFLAGS) -o build/mapeditor $(MAPEDITOR_OBJS) $(LDLIBS) $(LDFLAGS)

build/game: $(GAME_OBJS)
	mkdir -p build
	g++ $(CPPFLAGS) -o build/game $(GAME_OBJS) $(LDLIBS) $(LDFLAGS)

index.html: $(OBJS)
	emcc -I./include src/Game.cpp src/Log.cpp src/Common.cpp  --emrun --embed-file assets@/assets -sUSE_SDL=2 -sUSE_SDL_TTF=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -o index.html 
