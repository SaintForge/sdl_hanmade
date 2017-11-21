/* game.h --- 
 * 
 * Filename: game.h
 * Author: Sierra
 * Created: Пн окт  9 14:15:31 2017 (+0300)
 * Last-Updated: Пт окт 27 10:34:58 2017 (+0300)
 *           By: Sierra
 */

#if !defined(GAME_H)

void LogErrorLine(const char* Message, int Line)
{
    fprintf(stderr, "Assert fail in %s: %d\n",Message, Line);
}

#define Assert(Expression) if(!(Expression)) { LogErrorLine( __FILE__, __LINE__); *(int *)0 = 0;  }

struct game_offscreen_buffer
{
    int Width;
    int Height;
    SDL_Texture *Memory;
    SDL_Renderer *Renderer;
};

struct game_button_state
{
    bool IsDown;
    bool WasDown;
};

struct game_input
{
    bool WasPressed;
    bool MouseMotion;
    union
    {
        game_button_state Buttons[7];
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
        };
    };
    
    s32 MouseX, MouseY;
    s32 MouseRelX, MouseRelY;
};

struct game_memory
{
    level_entity LevelEntity;
    level_editor *LevelEditor;
    
    void *Assets;
    u64 AssetsSpace;
    
    void *LevelMemory;
    u64 LevelSpace;
    
    game_music *Music;
    game_font *Font;
    
    bool IsInitialized;
    bool AssetsInitialized;
    bool LevelMemoryInitialized;
};

static bool GameUpdateAndRender(game_memory *Memory, game_input *Input,
                                game_offscreen_buffer *Buffer);

#define GAME_H
#endif
