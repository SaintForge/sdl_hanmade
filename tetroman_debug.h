/* ========================================= */
//     $File: tetroman_debug.cpp
//     $Date: June 29th 2020 10:32 pm 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */

#ifndef TETROMAN_DEBUG_H
#define TETROMAN_DEBUG_H



void PrintMatrix(u32 Matrix[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
            printf("%d\t", Matrix[i][j]);
        
        printf("\n");
    }
    printf("\n");
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

inline static void
DEBUGRenderQuad(game_offscreen_buffer *Buffer, rectangle2 Rectangle, v4 Color)
{
    
    game_rect SDLRect;
    SDLRect.x = roundf(Rectangle.Min.x);
    SDLRect.y = roundf(Rectangle.Min.y);
    SDLRect.w = roundf(Rectangle.Max.x - Rectangle.Min.x);
    SDLRect.h = roundf(Rectangle.Max.y - Rectangle.Min.y);
    
    u8 r, g, b, a;
    SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, &a);
    
    SDL_SetRenderDrawColor(Buffer->Renderer, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderDrawRect(Buffer->Renderer, &SDLRect);
    SDL_SetRenderDrawColor(Buffer->Renderer, r, g, b, a);
}

inline static void
DEBUGRenderQuadFill(game_offscreen_buffer *Buffer, rectangle2 Rectangle, v4 Color)
{
    
    game_rect SDLRect;
    SDLRect.x = roundf(Rectangle.Min.x);
    SDLRect.y = roundf(Rectangle.Min.y);
    SDLRect.w = roundf(Rectangle.Max.x - Rectangle.Min.x);
    SDLRect.h = roundf(Rectangle.Max.y - Rectangle.Min.y);
    
    u8 r, g, b, a;
    SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, &a);
    
    SDL_SetRenderDrawColor(Buffer->Renderer, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderFillRect(Buffer->Renderer, &SDLRect);
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
DEBUGRenderQuadFill(game_offscreen_buffer *Buffer, game_texture *Texture, rectangle2 Rectangle, v4 Color) {
    
    SDL_SetRenderTarget(Buffer->Renderer, Texture);
    
    
    game_rect SDLRect;
    SDLRect.x = roundf(Rectangle.Min.x);
    SDLRect.y = roundf(Rectangle.Min.y);
    SDLRect.w = roundf(Rectangle.Max.x - Rectangle.Min.x);
    SDLRect.h = roundf(Rectangle.Max.y - Rectangle.Min.y);
    
    u8 r, g, b, a;
    SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, &a);
    
    SDL_SetRenderDrawColor(Buffer->Renderer, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderFillRect(Buffer->Renderer, &SDLRect);
    SDL_SetRenderTarget(Buffer->Renderer, NULL);
    
    SDL_SetRenderDrawColor(Buffer->Renderer, r, g, b, a);
}

#endif //TETROMAN_DEBUG_H
