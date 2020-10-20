/* ========================================= */
//     $File: win32_platform.h
//     $Date: October 18th 2017 08:58 pm 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */

#if !defined(WIN32_GAME_H)

#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
#include <SDL2\SDL_ttf.h>
#include <SDL2\SDL_mixer.h>

// TODO(msokolov): this needs to be in a better place
static const char* SpritePath = "..\\data\\sprites\\";
static const char* SoundPath  = "..\\data\\sound\\";
static const char* FontPath   = "..\\data\\Karmina-Bold.otf";

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


#define WIN32_GAME_H
#endif


