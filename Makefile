CC := g++
CPPFLAGS := -g -std=c++20
LDFLAGS := -g
LDLIBS := -lSDL2 -lSDL2_ttf -lSDL2_image -lstdc++
OBJDIR := objdir
OBJS := $(addprefix $(OBJDIR)/,Game.o Common.o Log.o)
MAPEDITOR_OBJS := $(addprefix $(OBJDIR)/,MapEditor.o Common.o Log.o)
INCL_CC := -I./include -I/usr/include/SDL2
SRC_CC := src/Game.cpp src/Common.cpp src/Log.cpp

all: game mapeditor

$(OBJDIR)/Game.o: src/Game.cpp include/Map.h
	$(CC) $(CPPFLAGS) -c $(INCL_CC) src/Game.cpp -o $(OBJDIR)/Game.o

$(OBJDIR)/Common.o: src/Common.cpp include/Map.h
	$(CC) $(CPPFLAGS) -c $(INCL_CC) src/Common.cpp -o $(OBJDIR)/Common.o

$(OBJDIR)/Log.o: src/Log.cpp
	$(CC) $(CPPFLAGS) -c $(INCL_CC) src/Log.cpp -o $(OBJDIR)/Log.o

$(OBJDIR)/MapEditor.o: src/MapEditor.cpp include/MapEditor.h
	$(CC) $(CPPFLAGS) -c $(INCL_CC) src/MapEditor.cpp -o $(OBJDIR)/MapEditor.o

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

mapeditor: $(MAPEDITOR_OBJS)
	mkdir -p build
	g++ $(CPPFLAGS) -o build/mapeditor $(MAPEDITOR_OBJS) $(LDLIBS) $(LDFLAGS)

game: $(OBJS)
	mkdir -p build
	g++ $(CPPFLAGS) -o build/game $(OBJS) $(LDLIBS) $(LDFLAGS)

game_wa:
	emcc -I./include src/Game.cpp src/Log.cpp src/Common.cpp  --emrun --embed-file assets@/assets -sUSE_SDL=2 -sUSE_SDL_TTF=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -o index.html 
