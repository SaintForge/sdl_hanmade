/* date = June 23rd 2020 11:37 pm */

#ifndef MAC_PLATFORM_H
#define MAC_PLATFORM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

// TODO(msokolov): this needs to be in a better place
static const char* SpritePath = "../data/sprites/";
static const char* SoundPath  = "../data/sound/";
static const char* FontPath   = "../data/Karmina-Bold.otf";

struct sdl_offscreen_buffer
{
    SDL_Texture *Texture;
    unsigned int *Memory;
    int Width;
    int Height;
    int Pitch;
};

struct window_dimension
{
    int Width;
    int Height;
};

#endif //MAC_PLATFORM_H
