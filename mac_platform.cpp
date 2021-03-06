#include "mac_platform.h"
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
    
    //if(NewState->EndedDown != IsDown)
    if (!NewState->EndedDown)
    {
        NewState->EndedDown = IsDown;
    }
    //if(NewState->EndedUp != WasDown)
    if (!NewState->EndedUp)
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
                        printf("left mouse click\n");
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
    
    SDL_DisplayMode Display = {};
    SDL_GetDesktopDisplayMode(0, &Display);
    
    u32 RefreshRate = 60;
    s32 DisplayIndex = 0, ModeIndex = 0;
    if (SDL_GetDisplayMode(DisplayIndex, ModeIndex, &Display) == 0)
    {
        RefreshRate = Display.refresh_rate;
    }
    
    r32 dtForFrame = 1.0f / (r32)RefreshRate;
    
    SDL_Window* Window = SDL_CreateWindow("This is window",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          Display.w, Display.h,
                                          SDL_WINDOW_RESIZABLE);
    
    // NOTE(msokolov): this ideally should be either hardcoded always in the code
    // or be taken from the user's monitor resolution
    SDL_SetWindowSize(Window, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    
    if(Window)
    {
        SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
        
        SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1,
                                                    SDL_RENDERER_TARGETTEXTURE|SDL_RENDERER_ACCELERATED);
        
        SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderSetLogicalSize(Renderer, LOGICAL_GAME_WIDTH, LOGICAL_GAME_HEIGHT);
        
        
        if(Renderer)
        {
            bool IsRunning = true;
            window_dimension Dimension = SDLGetWindowDimension(Window);
            
            sdl_offscreen_buffer BackBuffer = {};
            BackBuffer.Width  = WINDOW_WIDTH;
            BackBuffer.Height = WINDOW_HEIGHT;
            
            game_offscreen_buffer Buffer = {};
            Buffer.Renderer = Renderer;
            Buffer.Width    = BackBuffer.Width;
            Buffer.Height   = BackBuffer.Height;
            Buffer.Width    = LOGICAL_GAME_WIDTH;
            Buffer.Height   = LOGICAL_GAME_HEIGHT;
            
            printf("Ready!\n");
            
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
            u64 TotalStorageSize = Memory.PermanentStorageSize + Memory.TransientStorageSize + Memory.AssetStorageSize + Memory.LevelStorageSize;
            
            void *MemoryStorage = malloc(TotalStorageSize);
            if (MemoryStorage)
            {
                memset(MemoryStorage, 0, TotalStorageSize);
                
                Memory.AssetStorage     = MemoryStorage;
                Memory.LevelStorage     = ((u8*)Memory.AssetStorage + Memory.AssetStorageSize);
                Memory.PermanentStorage = ((u8*)Memory.LevelStorage + Memory.LevelStorageSize);
                Memory.TransientStorage = ((u8*)Memory.PermanentStorage + Memory.PermanentStorageSize);
                
                r32 TimeElapsed = 0.0f;
                r32 PreviousTimeTick = SDL_GetTicks();
                
                s32 OldMouseX = 0;
                s32 OldMouseY = 0;
                
                ReadAssetFromFile("package1.bin", Memory.AssetStorage, Memory.AssetStorageSize);
                ReadLevelFromFile("package2.bin", Memory.LevelStorage, Memory.LevelStorageSize);
                
                while (IsRunning)
                {
                    PreviousTimeTick = SDL_GetTicks();
                    
                    game_input Input = {};
                    Input.MouseX = OldMouseX;
                    Input.MouseY = OldMouseY;
                    Input.TimeElapsedMs = TimeElapsed;
                    Input.dtForFrame = dtForFrame;
                    
                    SDL_Event Event = {};
                    if(SDLHandleEvent(&Event, &Input))
                    {
                        IsRunning = false;
                    }
                    
                    if(GameUpdateAndRender(&Memory, &Input, &Buffer))
                    {
                        IsRunning = false;
                    }
                    
                    SDLUpdateWindow(Window, Renderer, &BackBuffer);
                    
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
