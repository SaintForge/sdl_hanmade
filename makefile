OBJS = win32_platform.cpp
OBJ_NAME = build/win32_platform.exe

all: $(OBJS)
	g++ $(OBJS) -D ASSET_BUILD -std=c++11 -w -I"D:\WorkSpace\Libs\SDL2_gcc\SDL2\include" -L"D:\WorkSpace\Libs\SDL2_gcc\SDL2\lib" -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -o $(OBJ_NAME)
