OBJS = linux_game.cpp
OBJ_NAME = build/main

all: $(OBJS)
	g++ -D ASSET_BUILD -std=c++11 $(OBJS) -w -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -o $(OBJ_NAME)
