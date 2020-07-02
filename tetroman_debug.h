/* date = June 29th 2020 7:24 pm */

#ifndef TETROMAN_DEBUG_H
#define TETROMAN_DEBUG_H

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


#endif //TETROMAN_DEBUG_H
