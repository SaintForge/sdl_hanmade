/* game.h --- 
 * 
 * Filename: game.h
 * Author: Sierra
 * Created: Пн окт  9 14:15:31 2017 (+0300)
 * Last-Updated: Пт окт 27 10:34:58 2017 (+0300)
 *           By: Sierra
 */

#if !defined(GAME_H)

#include "tetroman_math.h"
#include "tetroman_asset.h"
#include "tetroman_entity.h"

#include "tetroman_editor.h"

struct memory_area
{
    memory_index Size;
    u8 *Base;
    memory_index Used;
};

struct game_state
{
    memory_area MemoryArea;
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
GameMakeTextureFromString(game_texture *&Texture, 
                          const char* Text, 
                          game_rect *TextureQuad, 
                          game_font *&Font, 
                          game_color Color, 
                          game_offscreen_buffer *Buffer)
{
    if(!Text || !Font) return;
    
    if(Texture)
    {
        SDL_DestroyTexture(Texture);
    }
    
    game_surface *Surface = TTF_RenderUTF8_Blended(Font, Text, Color);
    Assert(Surface);
    
    if(TextureQuad)
    {
        TextureQuad->w = Surface->w;
        TextureQuad->h = Surface->h;
    }
    
    Texture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    Assert(Texture);
    
    SDL_FreeSurface(Surface);
}

static void 
GameRenderBitmapToBuffer(game_offscreen_buffer *Buffer, game_texture *&Texture, game_rect *Quad)
{
    Assert(Texture);
    //if (!Texture) return;
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
