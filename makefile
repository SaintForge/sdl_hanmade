OBJS = win32_game.cpp
OBJ_NAME = build/main

all: $(OBJS)
	g++ -std=c++11 $(OBJS) -w -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -o $(OBJ_NAME)
