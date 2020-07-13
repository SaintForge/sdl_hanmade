/* ========================================= */
//     $File: tetroman_platform.h
//     $Date: June 19th 2020 1:15 pm 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */

#ifndef TETROMAN_PLATFORM_H
#define TETROMAN_PLATFORM_H

#include <stdint.h>
#include <math.h>

// TODO(msokolov): this 2 need to be resolved
#include <string.h>
#include <string>

// TODO(msokolov): we need to get rid of this
#include <vector>

#include <stdint.h>
#include <stddef.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define LOGICAL_GAME_WIDTH 1980
#define LOGICAL_GAME_HEIGHT 1080

using namespace std;

typedef SDL_Rect    game_rect;
typedef SDL_Point   game_point;
typedef SDL_Color   game_color;
typedef SDL_Texture game_texture;
typedef SDL_Surface game_surface;
typedef Mix_Chunk   game_sound;
typedef Mix_Music   game_music;
typedef TTF_Font    game_font;

typedef SDL_RendererFlip figure_flip;

typedef int8_t   s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef int32_t b32;

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float r32;
typedef double r64;

typedef size_t memory_index;

struct game_offscreen_buffer
{
    s32 ScreenWidth;
    s32 ScreenHeight;
    
    s32 Width;
    s32 Height;
    
    SDL_Renderer *Renderer;
};

struct game_button_state
{
    bool EndedDown;
    bool EndedUp;
};

struct game_keyboard_input
{
    union
    {
        game_button_state Buttons[24];
        struct
        {
            game_button_state Up;
            game_button_state Down;
            game_button_state Left;
            game_button_state Right;
            game_button_state LeftClick;
            game_button_state RightClick;
            game_button_state Escape;
            game_button_state BackQuote;
            game_button_state Q_Button;
            game_button_state E_Button;
            game_button_state S_Button;
            game_button_state Tab;
            
            game_button_state Zero;
            game_button_state One;
            game_button_state Two;
            game_button_state Three;
            game_button_state Four;
            game_button_state Five;
            game_button_state Six;
            game_button_state Seven;
            game_button_state Eight;
            game_button_state Nine;
            game_button_state BackSpace;
            game_button_state Enter;
            game_button_state LeftShift;
        };
    };
};

struct game_input
{
    r32 TimeElapsedMs;
    r32 dtForFrame;
    
    s32 MouseX, MouseY;
    s32 MouseRelX, MouseRelY;
    
    game_button_state MouseButtons[2];
    game_keyboard_input Keyboard;
};


struct game_memory
{
    void *TransientStorage;
    u64 TransientStorageSize;
    
    void *PermanentStorage;
    u64 PermanentStorageSize;
    
    void *AssetStorage;
    u64 AssetStorageSize;
    
    void *LevelStorage;
    u64 LevelStorageSize;
    
    bool IsInitialized;
};

#endif //TETROMAN_PLATFORM_H
