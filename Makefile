CC := g++
CPPFLAGS := -g
LDFLAGS := -g
LDLIBS := -lSDL2 -lSDL2_ttf -lSDL2_image -lstdc++
OBJDIR := objdir
OBJS := $(addprefix $(OBJDIR)/,Game.o Common.o Log.o)
INCL_CC := -I./include -I/usr/include/SDL2
SRC_CC := src/Game.cpp src/Common.cpp src/Log.cpp

$(OBJDIR)/%.o : src/%.cpp include/%.h include/Map.h
	$(CC) -c $(INCL_CC) $(CPPFLAGS) $< -o $@

all: game

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

game: $(OBJS)
	mkdir -p build
	g++ $(CPPFLAGS) -o build/game $(OBJS) $(LDLIBS) $(LDFLAGS)
