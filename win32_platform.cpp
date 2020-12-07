/* ========================================= */
//     $File: win32_platform.cpp
//     $Date: October 10th 2017 10:32 pm 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */

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

window_dimension SDLGetWindowDimension(SDL_Window* Window)
{
    window_dimension WindowQuad = {};
    SDL_GetWindowSize(Window, &WindowQuad.Width, &WindowQuad.Height);
    
    return (WindowQuad);
}

inline static v2 
CalculateViewport(r32 ScreenWidth, r32 ScreenHeight) {
    
    v2 Result = {};
    
    r32 AspectRatio = (r32) VIRTUAL_GAME_WIDTH / (r32) VIRTUAL_GAME_HEIGHT;
    
    r32 ViewportWidth = ScreenWidth;
    r32 ViewportHeight = roundf(ViewportWidth / AspectRatio);
    
    if (ViewportHeight > ScreenHeight) {
        ViewportHeight = ScreenHeight;
        ViewportWidth  = roundf(ViewportHeight * AspectRatio);
    }
    
    Result.w = ViewportWidth;
    Result.h = ViewportHeight;
    
    return (Result);
}

static void
SDLProcessKeyPress(game_button_state *NewState, bool IsDown, bool WasDown)
{
    if (!NewState->EndedDown)
    {
        NewState->EndedDown = IsDown;
    }
    if (!NewState->EndedUp)
    {
        NewState->EndedUp = WasDown;
    }
}

bool SDLHandleEvent(SDL_Event *Event, game_input *Input, SDL_Window *Window, game_offscreen_buffer *Buffer)
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
                    else if(KeyCode == SDLK_s)
                    {
                        SDLProcessKeyPress(&Input->Keyboard.S_Button, IsDown, WasDown);
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
            
            case SDL_WINDOWEVENT:
            {
                switch(Event->window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                    {
                        SDL_GetWindowSize(Window, &Buffer->ScreenWidth, &Buffer->ScreenHeight);
                        
                        v2 Viewport = CalculateViewport(Buffer->ScreenWidth, Buffer->ScreenHeight);
                        
                        Buffer->ViewportWidth  = Viewport.w;
                        Buffer->ViewportHeight = Viewport.h;
                        
                    } break;
                }
            } break;
        }
    }
    
    return (ShouldQuit);
}

static void
SetWindowFullscreen(SDL_Window *Window, b32 ToggleFullscreen)
{
    if (ToggleFullscreen)
        SDL_SetWindowFullscreen(Window, SDL_WINDOW_FULLSCREEN);
    else
        SDL_SetWindowFullscreen(Window, 0);
}

static window_dimension
SetWindowResolution(SDL_Window *Window, game_resolution Resolution)
{
    window_dimension Result = {};
    
    switch(Resolution)
    {
        case HD:
        {
            Result.Width = 1280;
            Result.Height = 720;
        } break;
        
        case FULLHD:
        {
            Result.Width = 1920;
            Result.Height = 1080;
        } break;
        case QFULLHD:
        {
            Result.Width = 2560;
            Result.Height = 1440;
        } break;
    }
    
    SDL_SetWindowSize(Window, Result.Width, Result.Height);
    SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    
    return (Result);
}

static void
SetWindowIcon(SDL_Window *Window, game_memory *Memory, SDL_Renderer *Renderer)
{
    game_surface *Surface = GetSurface(Memory, "grid_cell.png", Renderer);
    Assert(Surface);
    
    SDL_SetWindowIcon(Window, Surface);
}

static void
SDLUpdateWindow(SDL_Renderer *Renderer)
{
    SDL_RenderPresent(Renderer);
    SDL_RenderClear(Renderer);
}

#undef main //NOTE(Max): Because SDL_main doesn't work on some windows versions 
int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) == -1)
    {
        printf("Error in SDL_Init: %s", SDL_GetError());
        return 1;
    }
    
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
    {
        printf("Error in Mix_OpenAudio: %s", Mix_GetError());
        
    }
    int flags=MIX_INIT_OGG|MIX_INIT_MOD;
    int initted=Mix_Init(flags);
    if(initted&flags != flags) {
        printf("Mix_Init: Failed to init required ogg and mod support!\n");
        printf("Mix_Init: %s\n", Mix_GetError());
        // handle error
    }
    
    
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
    
    SDL_DisplayMode Display = {};
    SDL_GetDesktopDisplayMode(0, &Display);
    printf("Display.w: %d\n", Display.w);
    printf("Display.h: %d\n", Display.h);
    
    SDL_Window* Window = SDL_CreateWindow("Tetroman",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          Display.w, Display.h,
                                          //SDL_WINDOW_FULLSCREEN);
                                          SDL_WINDOW_RESIZABLE|SDL_WINDOW_HIDDEN);
    
    if(Window)
    {
        //SDL_HideWindow(Window);
        
        int driver_index = 0;
        {
            //static const char* want_renderer = "software";
            //static const char* want_renderer = "opengl";
            static const char* want_renderer = "direct3d";
            
            int numdrivers = SDL_GetNumRenderDrivers();
            SDL_RendererInfo info;
            for(driver_index=0;driver_index<numdrivers;driver_index++) {
                SDL_GetRenderDriverInfo(driver_index,&info);
                if(strcmp(info.name, want_renderer) == 0) { break; };
            }
        }
        
        printf("scale_quality: %d\n", SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"));
        printf("vsync: %d\n", SDL_SetHint(SDL_HINT_RENDER_VSYNC, "2"));
        SDL_Renderer* Renderer = SDL_CreateRenderer(Window, driver_index, 
                                                    SDL_RENDERER_TARGETTEXTURE|SDL_RENDERER_ACCELERATED);
        
        SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
        //SDL_RenderSetLogicalSize(Renderer, 1920, 1080);
        
        if(Renderer)
        {
            bool IsRunning = true;
            
            window_dimension Dimension = {Display.w, Display.h};
            
            //SetWindowFullscreen(Window, false);
            //Dimension = SDLGetWindowDimension(Window);
            Dimension = SetWindowResolution(Window, game_resolution::QFULLHD);
            
            game_offscreen_buffer Buffer = {};
            Buffer.Renderer      = Renderer;
            Buffer.ScreenWidth   = Dimension.Width;
            Buffer.ScreenHeight  = Dimension.Height;
            Buffer.Width         = VIRTUAL_GAME_WIDTH;
            Buffer.Height        = VIRTUAL_GAME_HEIGHT;
            
            {
                v2 Viewport = CalculateViewport(Buffer.ScreenWidth, Buffer.ScreenHeight);
                
                Buffer.ViewportWidth  = Viewport.w;
                Buffer.ViewportHeight = Viewport.h;
            }
            
            u32 RefreshRate = 60;
            s32 DisplayIndex = 0, ModeIndex = 0;
            SDL_DisplayMode DisplayMode = {};
            if (SDL_GetDisplayMode(DisplayIndex, ModeIndex, &DisplayMode) == 0)
            {
                RefreshRate = DisplayMode.refresh_rate;
            }
            
            r32 dtForFrame = 1.0f / (r32)RefreshRate;
            
#if ASSET_BUILD
            // NOTE: This is for packaging data to the disk
            SDLAssetBuildBinaryFile();
            printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n");
            printf("BUILDED!!!\n");
            printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n");
#endif
            game_memory Memory = {};
            // TODO(msokolov): these needs to be revised
            Memory.AssetStorageSize     = Megabytes(100);
            Memory.LevelStorageSize     = Megabytes(15);
            Memory.PermanentStorageSize = Megabytes(10);
            Memory.TransientStorageSize = Megabytes(1);
            Memory.SettingsStorageSize  = Kilobytes(1);
            
            u64 TotalStorageSize = Memory.PermanentStorageSize
                + Memory.TransientStorageSize
                + Memory.AssetStorageSize
                + Memory.LevelStorageSize
                + Memory.SettingsStorageSize;
            
            void *MemoryStorage = malloc(TotalStorageSize);
            if (MemoryStorage)
            {
                memset(MemoryStorage, 0, TotalStorageSize);
                
                Memory.AssetStorage     = MemoryStorage;
                Memory.LevelStorage     = ((u8*)Memory.AssetStorage + Memory.AssetStorageSize);
                Memory.PermanentStorage = ((u8*)Memory.LevelStorage + Memory.LevelStorageSize);
                Memory.TransientStorage = ((u8*)Memory.PermanentStorage + Memory.PermanentStorageSize);
                Memory.SettingsStorage  = ((u8*)Memory.TransientStorage + Memory.TransientStorageSize);
                
                r32 TimeElapsed = 0.0f;
                r32 PreviousTimeTick = SDL_GetTicks();
                
                // TODO(msokolov): check if files exist and are opened
                ReadBinaryFile("package1.bin", Memory.AssetStorage, Memory.AssetStorageSize);
                ReadBinaryFile("package2.bin", Memory.LevelStorage, Memory.LevelStorageSize);
                ReadBinaryFile("package0.bin", Memory.SettingsStorage, Memory.SettingsStorageSize);
                
                player_data *PlayerData = (player_data *) Memory.SettingsStorage;
                game_settings *Settings = &PlayerData->Settings;
                
                game_settings PrevSettings = {};
                PrevSettings.MusicIsOn = Settings->MusicIsOn;
                PrevSettings.SoundIsOn = Settings->SoundIsOn;
                
                SDL_ShowWindow(Window);
                
                v2 OldMousePos = {};
                while (IsRunning)
                {
                    PreviousTimeTick = SDL_GetTicks();
                    
                    game_input Input = {};
                    Input.TimeElapsed = TimeElapsed;
                    Input.dtForFrame = dtForFrame;
                    
                    SDL_Event Event = {};
                    if(SDLHandleEvent(&Event, &Input, Window, &Buffer))
                    {
                        IsRunning = false;
                    }
                    
                    {
                        SDL_GetMouseState(&Input.MouseX, &Input.MouseY);
                        
                        v2 ActualScreenCenter = V2(Buffer.ScreenWidth * 0.5f, Buffer.ScreenHeight * 0.5f);
                        v2 ViewportCenter = V2(Buffer.ViewportWidth * 0.5f, Buffer.ViewportHeight * 0.5f);
                        
                        {
                            v2 NewMouse = V2((r32)Input.MouseX - (ActualScreenCenter.x - ViewportCenter.x),
                                             (r32)Input.MouseY - (ActualScreenCenter.y - ViewportCenter.y));
                            NewMouse.x = NewMouse.x / Buffer.ViewportWidth;
                            NewMouse.y = NewMouse.y / Buffer.ViewportHeight;
                            
                            NewMouse.x = NewMouse.x * Buffer.Width;
                            NewMouse.y = NewMouse.y * Buffer.Height;
                            
                            Input.MouseX = roundf(NewMouse.x);
                            Input.MouseY = roundf(NewMouse.y);
                        }
                        
                        Input.MouseRelX = Input.MouseX - OldMousePos.x;
                        Input.MouseRelY = Input.MouseY - OldMousePos.y;
                    }
                    
                    game_return_values GameResult = GameUpdateAndRender(&Memory, &Input, &Buffer);
                    
                    if(GameResult.ShouldQuit)
                    {	
                        IsRunning = false;
                    }
                    
                    if (GameResult.SettingsChanged)
                    {
                        if (PrevSettings.MusicIsOn != Settings->MusicIsOn)
                        {
                            // NOTE(msokolov): Music will be changed here
                            if (!Settings->MusicIsOn) {
                                Mix_PauseMusic();
                            }
                            else 
                                Mix_ResumeMusic();
                            
                            PrevSettings.MusicIsOn = Settings->MusicIsOn;
                        }
                        if (PrevSettings.SoundIsOn != Settings->SoundIsOn)
                        {
                            // NOTE(msokolov): Sound will be changed here
                            PrevSettings.SoundIsOn = Settings->SoundIsOn;
                            
                            if (!Settings->SoundIsOn) 
                                Mix_Volume(1, 0);
                            else
                                Mix_Volume(1, MIX_MAX_VOLUME);
                        }
                        
                        WriteBinaryFile("package0.bin", Memory.SettingsStorage, Memory.SettingsStorageSize);
                    }
                    
                    SDLUpdateWindow(Renderer);
                    
                    OldMousePos = V2(Input.MouseX, Input.MouseY);
                    
                    r32 CurrentTimeTick = SDL_GetTicks();
                    TimeElapsed += (CurrentTimeTick - PreviousTimeTick) / 1000.0f;
                    //printf("TimeElapsed: %f\n", TimeElapsed);
                }
            }
        }
    }
    
    return 0;
}
