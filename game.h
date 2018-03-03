/* game.h --- 
 * 
 * Filename: game.h
 * Author: Sierra
 * Created: Пн окт  9 14:15:31 2017 (+0300)
 * Last-Updated: Пт окт 27 10:34:58 2017 (+0300)
 *           By: Sierra
 */

#if !defined(GAME_H)

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

struct game_memory
{
    u32 CurrentLevelIndex;
    u32 LevelMemoryAmount;
    u32 LevelMemoryReserved;
    
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

static u32
GameResizeActiveBlock(u32 GridAreaWidth, 
                      u32 GridAreaHeight, 
                      u32 RowAmount, 
                      u32 ColumnAmount)
{
    u32 ResultBlockSize = 0;
    
    u32 DefaultBlockWidth  = GridAreaWidth  / (ColumnAmount + 1);
    u32 DefaultBlockHeight = GridAreaHeight / (RowAmount + 1);
    u32 DefaultBlockSize   = DefaultBlockWidth < DefaultBlockHeight ? DefaultBlockWidth : DefaultBlockHeight;
    
    ResultBlockSize = DefaultBlockSize;
    ResultBlockSize = ResultBlockSize - (ResultBlockSize % 2);
    
    return(ResultBlockSize);
}

static u32
GameResizeInActiveBlock(u32 FigureAreaWidth, 
                        u32 FigureAreaHeight, 
                        u32 DefaultBlocksInRow,
                        u32 DefaultBlocksInCol,
                        u32 BlocksInRow)
{
    u32 ResultBlockSize  = 0;
    
    u32 DefaultBlockWidth  = FigureAreaWidth / DefaultBlocksInRow;
    u32 DefaultBlockHeight = FigureAreaHeight / DefaultBlocksInCol;
    u32 DefaultBlockSize   = DefaultBlockWidth < DefaultBlockHeight ? DefaultBlockWidth : DefaultBlockHeight;
    u32 ActualBlockSize    = FigureAreaWidth / BlocksInRow;
    
    ResultBlockSize = ActualBlockSize < DefaultBlockSize ? ActualBlockSize : DefaultBlockSize;
    ResultBlockSize = ResultBlockSize - (ResultBlockSize % 2);
    
    return(ResultBlockSize);
}

static void 
GameRenderBitmapToBuffer(game_offscreen_buffer *Buffer, game_texture *&Texture, game_rect *Quad)
{
    Assert(Texture);
    SDL_RenderCopy(Buffer->Renderer, Texture, 0, Quad);
}

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
DEBUGRenderMathRect(game_offscreen_buffer *Buffer, math_rect *Rectangle, u8 Red, u8 Green, u8 Blue, u8 Alpha)
{
    SDL_Rect AreaQuad = {};
    AreaQuad.x = Rectangle->Left;
    AreaQuad.y = Rectangle->Top;
    AreaQuad.w = Rectangle->Right - Rectangle->Left;
    AreaQuad.h = Rectangle->Bottom - Rectangle->Top;
    
    DEBUGRenderQuad(Buffer, &AreaQuad, {Red, Green, Blue}, Alpha);
}

static void
DEBUGRenderMathRectFill(game_offscreen_buffer *Buffer, math_rect *Rectangle, u8 Red, u8 Green, u8 Blue, u8 Alpha)

{
    
    SDL_Rect AreaQuad = {};
    AreaQuad.x = Rectangle->Left;
    AreaQuad.y = Rectangle->Top;
    AreaQuad.w = Rectangle->Right - Rectangle->Left;
    AreaQuad.h = Rectangle->Bottom - Rectangle->Top;
    
    DEBUGRenderQuadFill(Buffer, &AreaQuad, {Red, Green, Blue}, Alpha);
}

#define GAME_H
#endif
