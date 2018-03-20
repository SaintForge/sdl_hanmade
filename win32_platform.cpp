// win32_game.cpp --- 
// 
// Filename: win32_game.cpp
// Author: 
// Created: Ср окт 18 20:51:21 2017 (+0400)
// Last-Updated: Пт окт 27 09:46:39 2017 (+0300)
//           By: Sierra
//

#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
#include <SDL2\SDL_ttf.h>
#include <SDL2\SDL_mixer.h>

#include <stdint.h>
#include <math.h>
#include <string.h>
#include <string>
#include <vector>
#include <windows.h>

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

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float r32;
typedef double r64;
typedef int32_t b32;

#include "win32_platform.h"

static const char* SpritePath = "..\\data\\sprites\\";
static const char* SoundPath  = "..\\data\\sound\\";

#include "game.cpp"


static void
SDLChangeBufferColor(sdl_offscreen_buffer *Buffer, u8 Red, u8 Green, u8 Blue, u8 Alpha)
{
    if(Buffer->Memory)
    {
        u32 *Pixel = (u32*)Buffer->Memory;
        u32 PixelAmount = Buffer->Width * Buffer->Height;
        for (u32 i = 0;
             i < PixelAmount; ++i)
        {
            *Pixel++ = ((Red << 24) | (Green << 16) | (Blue << 8) | (Alpha));
        }
    }
}

static void
SDLCreateBufferTexture(sdl_offscreen_buffer *Buffer, SDL_Renderer* Renderer, int Width, int Height)
{
    int BytesPerPixel = 4;
    Buffer->Width  = Width;
    Buffer->Height = Height;
    Buffer->Pitch  = Width * BytesPerPixel;
    
    Buffer->Memory = (u32*)calloc(Width * Height, BytesPerPixel);
    if(Buffer->Memory)
    {
        Buffer->Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888,
                                            SDL_TEXTUREACCESS_TARGET,
                                            Width, Height);
        if(Buffer->Texture)
        {
        }
        else
        {
            printf("Failed to create SDL_Texture!%s\n", SDL_GetError());
        }
    }
    else
    {
        printf("Failed to malloc Buffer->Memory!\n");
    }
}

static SDL_Texture*
SDLUploadTexture(SDL_Renderer *&Renderer, game_rect *Quad, char* FileName)
{
    SDL_Surface *Surface = IMG_Load(FileName);
    Quad->w = Surface->w;
    Quad->h = Surface->h;
    SDL_Texture *Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
    
    SDL_FreeSurface(Surface);
    
    return (Texture);
}


window_dimension SDLGetWindowDimension(SDL_Window* Window)
{
    window_dimension WindowQuad = {};
    SDL_GetWindowSize(Window, &WindowQuad.Width, &WindowQuad.Height);
    
    return (WindowQuad);
}


static void
SDLProcessKeyPress(game_button_state *NewState, bool IsDown, bool WasDown)
{
    if(NewState->EndedDown != IsDown)
    {
        NewState->EndedDown = IsDown;
    }
    if(NewState->EndedUp != WasDown)
    {
        NewState->EndedUp = WasDown;
    }
}

bool SDLHandleEvent(SDL_Event *Event, game_input *Input)
{
    bool ShouldQuit = false;
    
    while(SDL_PollEvent(Event))
    {
        switch(Event->type)
        {
            case SDL_MOUSEMOTION:
            {
                Input->MouseX = Event->motion.x;
                Input->MouseY = Event->motion.y;
                
                Input->MouseRelX += Event->motion.xrel;
                Input->MouseRelY += Event->motion.yrel;
            } break;
            
            case SDL_QUIT:
            {
                printf("SDL_QUIT\n");
                ShouldQuit = true;
                break;
            } break;
            
            case SDL_MOUSEBUTTONDOWN: 
            case SDL_MOUSEBUTTONUP: 
            {
                
                u8 Button = Event->button.button;
                
                bool IsDown  = (Event->button.state == SDL_PRESSED);
                bool WasDown = (Event->button.state == SDL_RELEASED);
                
                
                if(Event->button.clicks != 0)
                {
                    if(Button == SDL_BUTTON_LEFT)
                    {
                        SDLProcessKeyPress(&Input->MouseButtons[0], IsDown, WasDown);
                    }
                    else if(Button == SDL_BUTTON_RIGHT)
                    {
                        SDLProcessKeyPress(&Input->MouseButtons[1], IsDown, WasDown);
                    }
                }
                
            } break;
            
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                SDL_Keycode KeyCode = Event->key.keysym.sym;
                
                bool IsDown  = (Event->key.state == SDL_PRESSED);
                bool WasDown = (Event->key.state == SDL_RELEASED);
                
                if (Event->key.repeat != 0)
                {
                    WasDown = true;
                }
                
                if(Event->key.repeat == 0)
                {
                    if(KeyCode == SDLK_q)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Q_Button, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_e)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.E_Button, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_ESCAPE)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Escape, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_BACKQUOTE)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.BackQuote, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_w)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Up, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_a)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Left, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_s)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Down, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_d)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Right, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_0)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Zero, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_1)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.One, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_2)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Two, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_3)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Three, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_4)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Four, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_5)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Five, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_6)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Six, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_7)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Seven, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_8)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Eight, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_9)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Nine, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_RETURN)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Enter, IsDown, WasDown);
                    }
                    else if(KeyCode == SDLK_LSHIFT)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.LeftShift, IsDown, WasDown);
                    }
                    
                }										 
                
            } break;
        }
    }
    
    return (ShouldQuit);
}

static void
SDLUpdateWindow(SDL_Window* Window, SDL_Renderer *Renderer, sdl_offscreen_buffer *Buffer)
{
    SDL_RenderPresent(Renderer);
    SDL_RenderClear(Renderer);
}

static void
SDLReloadFontTexture(TTF_Font *&Font, SDL_Texture *&Texture, SDL_Rect *Quad,
                     char* Text, SDL_Renderer*& Renderer)
{
    SDL_Surface *Surface = 0;
    
    if(Texture)
    {
        SDL_DestroyTexture(Texture);
    }
    
    Surface = TTF_RenderUTF8_Blended(Font, Text, {255,255,255});
    Quad->w = Surface->w;
    Quad->h = Surface->h;
    
    Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
    Assert(Texture);
    SDL_FreeSurface(Surface);
}

#undef main //NOTE(Max): Because SDL_main doesn't work on some windows versions 
int main(int argc, char **argv)
{
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    //SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    
    SDL_DisplayMode Display = {};
    SDL_GetDesktopDisplayMode(0, &Display);
    
    /*
    
    320 x 480	Alcatel pixi 3, LG Wine Smart
    240 x 320	Nokia 230, Nokia 215, Samsung Xcover 550, LG G350
    480 x 800	Samsung Galaxy J1 (2016), Samsung Z1, Samsung Z2, Lumia 435, Alcatel Pixi 4, LG Joy, ZTE Blade G
    480 x 854	Huawei Y635, Nokia Lumia 635, Sony Xperia E3
    540 x 960	Samsung Galaxy J2, Moto E 2nd Gen, Sony Xperia E4, HTC Desire 526
    640 x 960	iPhone 4, iPhone 4S
    640 x 1136	iPhone 5, iPhone 5S, iPhone 5C, iPhone SE
    720 x 1280	Samsung Galaxy J5, Samsung Galaxy J3, Moto G4 Play, Xiaomi Redmi 3, Moto G 3rd Gen, Sony Xperia M4 Aqua
    750 x 1334	iPhone 6, iPhone 6S, iPhone 7
    1080 x 1920	iPhone 6S Plus, iPhone 6 Plus, iPhone 7 Plus, Huawei P9, Sony Xperia Z5, Samsung Galaxy A5, Samsung Galaxy A7, Samsung Galaxy S5, Samsung Galaxy A9, HTC One M9, Sony Xperia M5
    
    */
    
    SDL_Window *Window = SDL_CreateWindow("This is window",
                                          0,
                                          0,
                                          800, 600,
                                          SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    
    if(Window)
    {
        SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1,
                                                    SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
        SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
        
        if(Renderer)
        {
            bool IsRunning = true;
            window_dimension Dimension = SDLGetWindowDimension(Window);
            
            sdl_offscreen_buffer BackBuffer = {};
            BackBuffer.Width = Dimension.Width;
            BackBuffer.Height = Dimension.Height;
            
#if ASSET_BUILD
            // NOTE: This is for packaging data to the disk
            SDLAssetBuildBinaryFile();
            printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n");
            printf("BUILDED!!!\n");
            printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n");
#endif
            game_memory Memory = {};
            
            u64 TotalAssetSize = SDLSizeOfBinaryFile("package1.bin");
            SDL_Thread *AssetThread = SDL_CreateThread(SDLAssetLoadBinaryFile, "LoadingThread",
                                                       (void*)&Memory);
            
            r32 PreviousTimeTick = SDL_GetTicks();
            
            while(IsRunning)
            {
                r32 CurrentTimeTick = SDL_GetTicks();
                r32 TimeElapsed = (CurrentTimeTick - PreviousTimeTick) / 1000.0f;
                PreviousTimeTick = CurrentTimeTick;
                
                game_input Input = {};
                Input.TimeElapsedMs = TimeElapsed;
                SDL_GetMouseState(&Input.MouseX, &Input.MouseY);
                
                SDL_Event Event;
                if(SDLHandleEvent(&Event, &Input))
                {
                    IsRunning = false;
                }
                
                game_offscreen_buffer Buffer = {};
                Buffer.Renderer = Renderer;
                Buffer.Memory   = BackBuffer.Texture;
                Buffer.Width    = BackBuffer.Width;
                Buffer.Height   = BackBuffer.Height;
                
                if(Memory.AssetsInitialized)
                {
                    if(GameUpdateAndRender(&Memory, &Input, &Buffer))
                    {
                        IsRunning = false;
                        
                        free(Memory.AssetStorage);
                    }
                }
                
                SDLUpdateWindow(Window, Renderer, &BackBuffer);
            }
        }
    }
    
    return 0;
}
