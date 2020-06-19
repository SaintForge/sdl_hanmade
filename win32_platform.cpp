// win32_game.cpp --- 
// 
// Filename: win32_game.cpp
// Author: 
// Created: Ср окт 18 20:51:21 2017 (+0400)
// Last-Updated: Пт окт 27 09:46:39 2017 (+0300)
//           By: Sierra
//

// TODO(msokolov): this needs to be in a better place
static const char* SpritePath = "..\\data\\sprites\\";
static const char* SoundPath  = "..\\data\\sound\\";

#include "win32_platform.h"
#include "tetroman_platform.h"

#include "tetroman.cpp"
#include "tetroman_test.cpp"

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
                Input->MouseX = Event->motion.x;
                Input->MouseY = Event->motion.y;
                
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
                    else if(KeyCode == SDLK_TAB)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.Tab, IsDown, WasDown);
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
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    s32 img_flags = IMG_INIT_PNG;
    s32 return_flags = IMG_Init(img_flags);
    if ((return_flags&img_flags) != img_flags) {
        printf("IMG_Init: Failed to init required jpg and png support!\n");
        printf("IMG_Init: %s\n", IMG_GetError());
    }
    
    SDL_version compiled;
    SDL_version linked;
    
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    printf("We compiled against SDL version %d.%d.%d ...\n",
           compiled.major, compiled.minor, compiled.patch);
    printf("But we are linking against SDL version %d.%d.%d.\n",
           linked.major, linked.minor, linked.patch);
    
    TTF_Init();
    
    // NOTE(msokolov): this is just for testing
    
    memory_test mem_test = init_memory();
    
    write_test_data(&mem_test);
    level_test *level = (level_test*) mem_test.transient_storage;
    
    level->figures->units[10].pos_x = 1488;
    level->figures->units[0].pos_x  = 1488;
    
    // NOTE(msokolov): this is just for testing
    
    SDL_DisplayMode Display = {};
    SDL_GetDesktopDisplayMode(0, &Display);
    
    b32 VSyncOn = true;
    s32 FrameLimit = 60;
    
    s32 ScreenWidth  = 1920;
    s32 ScreenHeight = 1080;
    s32 ReferenceWidth  = 800;
    s32 ReferenceHeight = 600;
    
    SDL_Window* Window = SDL_CreateWindow("This is window",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          Display.w, Display.h,
                                          SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    
    SDL_SetWindowSize(Window, ScreenWidth, ScreenHeight);
    SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    
    if(Window)
    {
        //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        //SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
        
        if(VSyncOn)
        {
            SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
        }
        
        SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1,
                                                    SDL_RENDERER_TARGETTEXTURE|SDL_RENDERER_ACCELERATED);
        
        SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderSetLogicalSize(Renderer, ScreenWidth, ScreenHeight);
        
        
        if(Renderer)
        {
            bool IsRunning = true;
            window_dimension Dimension = SDLGetWindowDimension(Window);
            
            sdl_offscreen_buffer BackBuffer = {};
            BackBuffer.Width  = ScreenWidth;
            BackBuffer.Height = ScreenHeight;
            
            game_offscreen_buffer Buffer = {};
            Buffer.Renderer = Renderer;
            
            Buffer.Width    = BackBuffer.Width;
            Buffer.Height   = BackBuffer.Height;
            
            Buffer.ReferenceWidth  = ReferenceWidth;
            Buffer.ReferenceHeight = ReferenceHeight;
            printf("Ready!\n");
            
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
            
            r32 TimeElapsed = 0.0f;
            r32 PreviousTimeTick = SDL_GetTicks();
            
            s32 OldMouseX = 0;
            s32 OldMouseY = 0;
            
            r32 FPSCountMs = 0.0f;
            
            
            r32 FrameLimitMs = 1.0f / (r32)FrameLimit;
            r32 TimeSinceLastFrameMs = 0.0f;
            
            while (IsRunning)
            {
                PreviousTimeTick = SDL_GetTicks();
                
                game_input Input = {};
                Input.MouseX = OldMouseX;
                Input.MouseY = OldMouseY;
                Input.TimeElapsedMs = TimeElapsed;
                
                SDL_Event Event = {};
                if(SDLHandleEvent(&Event, &Input))
                {
                    IsRunning = false;
                }
                
                if(Memory.AssetsInitialized)
                {
                    if(GameUpdateAndRender(&Memory, &Input, &Buffer))
                    {
                        IsRunning = false;
                        
                        free(Memory.AssetStorage);
                    }
                }
                
                SDLUpdateWindow(Window, Renderer, &BackBuffer);
                
                OldMouseX = Input.MouseX;
                OldMouseY = Input.MouseY;
                
                r32 CurrentTimeTick = SDL_GetTicks();
                TimeElapsed = (CurrentTimeTick - PreviousTimeTick) / 1000.0f;
                
                if(!VSyncOn)
                {
                    if(TimeElapsed < FrameLimitMs)
                    {
                        r32 TimeLeftMs = FrameLimitMs - TimeElapsed;
                        u32 SleepMs = roundf((TimeLeftMs) * 1000.0f);
                        
                        if(SleepMs > 0)
                        {
                            SDL_Delay(SleepMs);
                            TimeElapsed += TimeLeftMs;
                        }
                    }
                    
                }
                
                s32 FpsCounter = roundf((1.0f / TimeElapsed));
                
                FPSCountMs += TimeElapsed;
                
                if(FPSCountMs >= 0.2f)
                {
                    FPSCountMs = 0.0f;
                    
                    char WindowTitle[128] = {};
                    
                    char Number[32] = {};
                    strcpy(WindowTitle, "Time: ");
                    sprintf(Number, "%d", FpsCounter);
                    strcat(WindowTitle, Number);
                    strcat(WindowTitle, "fps, ");
                    
                    sprintf(Number, "%.3f", TimeElapsed);
                    strcat(WindowTitle, Number);
                    strcat(WindowTitle, "s");
                    
                    SDL_SetWindowTitle(Window, WindowTitle);
                }
                
            }
        }
    }
    
    return 0;
}
