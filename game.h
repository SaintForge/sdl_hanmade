/* game.h --- 
 * 
 * Filename: game.h
 * Author: Sierra
 * Created: Пн окт  9 14:15:31 2017 (+0300)
 * Last-Updated: Пт окт 27 10:34:58 2017 (+0300)
 *           By: Sierra
 */

#if !defined(GAME_H)

#include "entity.h"

#define Assert(Expression) if(!(Expression)) { LogErrorLine( __FILE__, __LINE__); *(int *)0 = 0;  }

void LogErrorLine(const char* Message, int Line)
{
    fprintf(stderr, "Assert fail in %s: %d\n",Message, Line);
}

struct game_offscreen_buffer
{
    int Width;
    int Height;
    SDL_Texture *Memory;
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
        game_button_state Buttons[22];
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
        };
    };
};

struct game_input
{
    s32 MouseX, MouseY;
    s32 MouseRelX, MouseRelY;
    game_button_state MouseButtons[2];
    
    game_keyboard_input Keyboard;
};


struct game_memory
{
    level_entity LevelEntity;
    level_editor *LevelEditor;
    
    menu_entity *MenuEntity;
    
    void *Assets;
    u32 AssetsSpaceAmount;
    
    level_memory* LevelMemory;
    u32 CurrentLevelIndex;
    u32 LevelMemoryAmount;
    u32 LevelMemoryReserved;
    
    // TODO(Max): This should not be here!!!
    game_music *Music;
    game_font *LevelNumberFont;
    
    bool IsInitialized;
    bool AssetsInitialized;
    bool LevelMemoryInitialized;
    bool ToggleMenu;
};

static bool 
GameUpdateAndRender(game_memory *Memory, game_input *Input,
                    game_offscreen_buffer *Buffer);
static void 
GameRenderBitmapToBuffer(game_offscreen_buffer *Buffer, game_texture *&Texture, game_rect *Quad);

static void 
LevelEntityUpdateLevelEntityFromMemory(level_entity *LevelEntity, s32 Index, bool IsStarted,
                                       game_memory *Memory, game_offscreen_buffer *Buffer);

static void
LevelEditorChangeGridCounters(level_editor *LevelEditor, 
                              u32 NewRowAmount, u32 NewColumnAmount, 
                              game_offscreen_buffer *Buffer);

static void 
LevelEditorUpdateLevelStats(level_editor *LevelEditor, 
                            s32 LevelNumber, s32 LevelIndex, game_offscreen_buffer *Buffer);

static void
DEBUGPrintArray1D(u32 *Array, u32 Size)
{
    for (u32 i = 0; i < Size; ++i)
    {
        printf("%d ", Array[i]);
    }
    
    printf("\n");
}

static void
DEBUGPrintArray2D(s32 **Array, u32 RowAmount, u32 ColumnAmount)
{
    for (u32 i = 0; i < RowAmount; ++i) {
        for (u32 j = 0; j < ColumnAmount; ++j) {
            printf("%d ", Array[i][j]);
        }
        printf("\n");
    }
}


static void
DEBUGRenderLine(game_offscreen_buffer *Buffer, 
                s32 x1, s32 y1, s32 x2, s32 y2,
                SDL_Color color, u8 Alpha)
{
    u8 r, g, b, a;
    SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, &a);
    
    SDL_SetRenderDrawColor(Buffer->Renderer, color.r, color.g, color.b, Alpha);
    SDL_RenderDrawLine(Buffer->Renderer, x1, y1, x2, y2);
    SDL_SetRenderDrawColor(Buffer->Renderer, r, g, b, a);
}

static void
DEBUGRenderQuad(game_offscreen_buffer *Buffer, game_rect *AreaQuad, SDL_Color color, u8 Alpha)
{
    u8 r, g, b, a;
    SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, &a);
    
    SDL_SetRenderDrawColor(Buffer->Renderer, color.r, color.g, color.b, Alpha);
    SDL_RenderDrawRect(Buffer->Renderer, AreaQuad);
    SDL_SetRenderDrawColor(Buffer->Renderer, r, g, b, a);
}

static void
DEBUGRenderQuadFill(game_offscreen_buffer *Buffer, game_rect *AreaQuad, SDL_Color color, u8 Alpha)
{
    u8 r, g, b, a;
    SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, &a);
    
    SDL_SetRenderDrawColor(Buffer->Renderer, color.r, color.g, color.b, Alpha);
    SDL_RenderFillRect(Buffer->Renderer, AreaQuad);
    SDL_SetRenderDrawColor(Buffer->Renderer, r, g, b, a);
}

static void
DEBUGRenderFigureShell(game_offscreen_buffer *Buffer, figure_unit *Entity, u32 BlockSize, SDL_Color color, u8 Alpha)
{
    u8 r, g, b, a;
    SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(Buffer->Renderer, color.r, color.g, color.b, Alpha);
    
    game_rect Rect = {};
    
    for (u32 i = 0; i < 4; ++i)
    {
        Rect.w = BlockSize;
        Rect.h = BlockSize;
        Rect.x = Entity->Shell[i].x - (Rect.w / 2);
        Rect.y = Entity->Shell[i].y - (Rect.h / 2);
        
        SDL_RenderFillRect(Buffer->Renderer, &Rect);
    }
    
    //SDL_SetRenderDrawColor(Buffer->Renderer, 255, 255, 255, 255);
    Rect.x = Entity->Center.x - (Rect.w / 2);
    Rect.y = Entity->Center.y - (Rect.h / 2);
    //SDL_RenderDrawRect(Buffer->Renderer, &Rect);
    
    SDL_SetRenderDrawColor(Buffer->Renderer, r, g, b, a);
}

#define GAME_H
#endif
