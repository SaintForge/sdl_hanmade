/* date = June 19th 2020 1:15 pm */

#ifndef TETROMAN_PLATFORM_H
#define TETROMAN_PLATFORM_H

#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
#include <SDL2\SDL_ttf.h>
#include <SDL2\SDL_mixer.h>

#include <stdint.h>
#include <math.h>

// TODO(msokolov): this 2 need to be resolved
#include <string.h>
#include <string>

// TODO(msokolov): we need to get rid of this
#include <vector>

#include <stdint.h>
#include <stddef.h>

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

#define Assert(Expression) if(!(Expression)) { LogErrorLine( __FILE__, __LINE__); *(int *)0 = 0;  }

void LogErrorLine(const char* Message, int Line)
{
    fprintf(stderr, "Assert fail in %s: %d\n",Message, Line);
}

struct game_offscreen_buffer
{
    s32 Width;
    s32 Height;
    
    s32 ReferenceWidth;
    s32 ReferenceHeight;
    
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
        game_button_state Buttons[23];
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
    
    s32 MouseX, MouseY;
    s32 MouseRelX, MouseRelY;
    
    game_button_state MouseButtons[2];
    game_keyboard_input Keyboard;
};

// TODO(msokolov): this needs to be moved into tetroman.h or something
enum game_mode
{
    LEVEL, LEVEL_MENU, MAIN_MENU
};

struct game_memory
{
    /* DEBUG only code */
    
    game_rect PadRect;
    
    s32 RefPadWidth;
    s32 RefPadHeight;
    
    s32 RefWidth;
    s32 RefHeight;
    
    b32 EditorMode;
    
    /*              */
    
    game_mode CurrentState;
    
    u32 CurrentLevelIndex;
    u32 LevelMemoryAmount;
    u32 LevelMemoryReserved;
    
    void *TransientStorage;
    u64 TransientStorageSize;
    
    void *PermanentStorage;
    u64 PermanentStorageSize;
    
    void *LocalMemoryStorage;
    void *GlobalMemoryStorage;
    void *EditorMemoryStorage;
    
    void *AssetStorage;
    u32 AssetsSpaceAmount;
    
    // TODO(Max): This should not be here!!!
    game_font *LevelNumberFont;
    
    bool IsInitialized;
    bool AssetsInitialized;
    bool ToggleMenu;
};

#endif //TETROMAN_PLATFORM_H
