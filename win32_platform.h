/* win32_game.h --- 
 * 
 * Filename: win32_game.h
 * Author: 
 * Created: Ср окт 18 20:58:16 2017 (+0400)
 * Last-Updated: Пт окт 20 16:50:56 2017 (+0300)
 *           By: Sierra
 */
#if !defined(WIN32_GAME_H)

#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
#include <SDL2\SDL_ttf.h>
#include <SDL2\SDL_mixer.h>

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


