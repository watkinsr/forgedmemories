CC := g++
CPPFLAGS := -g -std=c++20
LDFLAGS := -g
LDLIBS := -lSDL2 -lSDL2_ttf -lSDL2_image -lstdc++
OBJDIR := objdir
OBJS := $(addprefix $(OBJDIR)/,Game.o Common.o Log.o)
INCL_CC := -I./include -I/usr/include/SDL2
SRC_CC := src/Game.cpp src/Common.cpp src/Log.cpp

$(OBJDIR)/%.o : src/%.cpp include/%.h include/Map.h
	$(CC) $(CPPFLAGS) -c $(INCL_CC) $< -o $@

all: game

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

game: $(OBJS)
	mkdir -p build
	g++ $(CPPFLAGS) -o build/game $(OBJS) $(LDLIBS) $(LDFLAGS)

game_wa:
	emcc -I./include src/Game.cpp src/Log.cpp src/Common.cpp  --emrun --embed-file assets@/assets -sUSE_SDL=2 -sUSE_SDL_TTF=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -o index.html 
