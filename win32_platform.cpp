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
        SDL_SetWindowFullscreen(Window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    else
        SDL_SetWindowFullscreen(Window, 0);
}

static void
SetWindowResolution(SDL_Window *Window, game_resolution Resolution)
{
    switch(Resolution)
    {
        case HD:
        {
            SDL_SetWindowSize(Window, 1280, 720);
            SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        } break;
        
        case FULLHD:
        {
            SDL_SetWindowSize(Window, 1920, 1080);
            SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        } break;
        case QFULLHD:
        {
            SDL_SetWindowSize(Window, 2560, 1440);
            SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        } break;
    }
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
    
    SDL_Window* Window = SDL_CreateWindow("Tetroman",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          Display.w, Display.h,
                                          SDL_WINDOW_RESIZABLE|SDL_WINDOW_HIDDEN);
    
    
    if(Window)
    {
        SDL_HideWindow(Window);
        
        
        SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        
        SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1,
                                                    SDL_RENDERER_TARGETTEXTURE|SDL_RENDERER_ACCELERATED);
        
        SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderSetLogicalSize(Renderer, VIRTUAL_GAME_WIDTH, VIRTUAL_GAME_HEIGHT);
        
        if(Renderer)
        {
            
            bool IsRunning = true;
            window_dimension Dimension = SDLGetWindowDimension(Window);
            
            sdl_offscreen_buffer BackBuffer = {};
            BackBuffer.Width  = WINDOW_WIDTH;
            BackBuffer.Height = WINDOW_HEIGHT;
            
            game_offscreen_buffer Buffer = {};
            Buffer.Renderer      = Renderer;
            Buffer.ScreenWidth   = Display.w;
            Buffer.ScreenHeight  = Display.h;
            Buffer.Width         = VIRTUAL_GAME_WIDTH;
            Buffer.Height        = VIRTUAL_GAME_HEIGHT;
            
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
            Memory.SettingsStorageSize  = Megabytes(1);
            
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
                
                s32 OldMouseX = 0;
                s32 OldMouseY = 0;
                
                // TODO(msokolov): check if files exist and are opened
                ReadBinaryFile("package1.bin", Memory.AssetStorage, Memory.AssetStorageSize);
                ReadBinaryFile("package2.bin", Memory.LevelStorage, Memory.LevelStorageSize);
                ReadBinaryFile("settings.bin", Memory.SettingsStorage, Memory.SettingsStorageSize);
                
                game_settings *Settings = (game_settings *) Memory.SettingsStorage;
                
                game_settings PrevSettings = {};
                PrevSettings.MusicIsOn = Settings->MusicIsOn;
                PrevSettings.SoundIsOn = Settings->SoundIsOn;
                
                SDL_ShowWindow(Window);
                SetWindowFullscreen(Window, false);
                SetWindowResolution(Window, game_resolution::FULLHD);
                SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                
                //game_music *gMusic = Mix_LoadMUS("..\\data\\sound\\music_test.ogg");
                //Mix_PlayMusic(gMusic, -1);
                
                while (IsRunning)
                {
                    PreviousTimeTick = SDL_GetTicks();
                    
                    game_input Input = {};
                    Input.MouseX = OldMouseX;
                    Input.MouseY = OldMouseY;
                    Input.TimeElapsed = TimeElapsed;
                    Input.dtForFrame = dtForFrame;
                    
                    SDL_Event Event = {};
                    if(SDLHandleEvent(&Event, &Input, Window, &Buffer))
                    {
                        IsRunning = false;
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
                        }
                        
                        WriteBinaryFile("settings.bin", Memory.SettingsStorage, Memory.SettingsStorageSize);
                    }
                    
                    
                    SDLUpdateWindow(Renderer);
                    
                    OldMouseX = Input.MouseX;
                    OldMouseY = Input.MouseY;
                    
                    r32 CurrentTimeTick = SDL_GetTicks();
                    TimeElapsed += (CurrentTimeTick - PreviousTimeTick) / 1000.0f;
                    
                    
                }
            }
        }
    }
    
    return 0;
}
