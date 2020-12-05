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
    
    r32 WidthRatio = 0.00052083333f;
    r32 HeightRatio = 0.00092592592;
    v2 SizeRatio = V2(WidthRatio, HeightRatio);
    
    v2 NDC_Min = V2(SizeRatio * Rectangle.Min);
    v2 NDC_Max = V2(SizeRatio * Rectangle.Max);
    
    rectangle2 ResultRectangle = {};
    ResultRectangle.Min = V2(NDC_Min.x * Buffer->ScreenWidth, NDC_Min.y * Buffer->ScreenHeight);
    ResultRectangle.Max = V2(NDC_Max.x * Buffer->ScreenWidth, NDC_Max.y * Buffer->ScreenHeight);
    
    game_rect SDLRectangle;
    SDLRectangle.x = roundf(ResultRectangle.Min.x);
    SDLRectangle.y = roundf(ResultRectangle.Min.y);
    SDLRectangle.w = roundf(ResultRectangle.Max.x - ResultRectangle.Min.x);
    SDLRectangle.h = roundf(ResultRectangle.Max.y - ResultRectangle.Min.y);
    
    u8 r, g, b, a;
    SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, &a);
    
    SDL_SetRenderDrawColor(Buffer->Renderer, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderDrawRect(Buffer->Renderer, &SDLRectangle);
    SDL_SetRenderDrawColor(Buffer->Renderer, r, g, b, a);
}

inline static void
DEBUGRenderQuadFill(game_offscreen_buffer *Buffer, rectangle2 Rectangle, v4 Color)
{
    v2 SizeRatio = V2(Buffer->WidthRatio, Buffer->HeightRatio);
    
    v2 ActualScreenCenter = V2(Buffer->ScreenWidth * 0.5f, Buffer->ScreenHeight * 0.5f);
    v2 ViewportCenter = V2(Buffer->ViewportWidth * 0.5f, Buffer->ViewportHeight * 0.5f);
    
    v2 NDC_Min = V2(SizeRatio * Rectangle.Min);
    v2 NDC_Max = V2(SizeRatio * Rectangle.Max);
    
    rectangle2 ResultRectangle = {};
    ResultRectangle.Min = V2(NDC_Min.x * Buffer->ViewportWidth, NDC_Min.y * Buffer->ViewportHeight) + (ActualScreenCenter - ViewportCenter);
    ResultRectangle.Max = V2(NDC_Max.x * Buffer->ViewportWidth, NDC_Max.y * Buffer->ViewportHeight) + (ActualScreenCenter - ViewportCenter);
    
    game_rect SDLRectangle;
    SDLRectangle.x = roundf(ResultRectangle.Min.x);
    SDLRectangle.y = roundf(ResultRectangle.Min.y);
    SDLRectangle.w = roundf(ResultRectangle.Max.x - ResultRectangle.Min.x);
    SDLRectangle.h = roundf(ResultRectangle.Max.y - ResultRectangle.Min.y);
    
    u8 r, g, b, a;
    SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, &a);
    
    SDL_SetRenderDrawColor(Buffer->Renderer, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderFillRect(Buffer->Renderer, &SDLRectangle);
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
    
    r32 WidthRatio = 0.00052083333f;
    r32 HeightRatio = 0.00092592592;
    v2 SizeRatio = V2(WidthRatio, HeightRatio);
    
    v2 NDC_Min = V2(SizeRatio * Rectangle.Min);
    v2 NDC_Max = V2(SizeRatio * Rectangle.Max);
    
    rectangle2 ResultRectangle = {};
    ResultRectangle.Min = V2(NDC_Min.x * Buffer->ScreenWidth, NDC_Min.y * Buffer->ScreenHeight);
    ResultRectangle.Max = V2(NDC_Max.x * Buffer->ScreenWidth, NDC_Max.y * Buffer->ScreenHeight);
    
    game_rect SDLRectangle;
    SDLRectangle.x = roundf(ResultRectangle.Min.x);
    SDLRectangle.y = roundf(ResultRectangle.Min.y);
    SDLRectangle.w = roundf(ResultRectangle.Max.x - ResultRectangle.Min.x);
    SDLRectangle.h = roundf(ResultRectangle.Max.y - ResultRectangle.Min.y);
    
    u8 r, g, b, a;
    SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, &a);
    
    SDL_SetRenderDrawColor(Buffer->Renderer, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderFillRect(Buffer->Renderer, &SDLRectangle);
    SDL_SetRenderTarget(Buffer->Renderer, NULL);
    
    SDL_SetRenderDrawColor(Buffer->Renderer, r, g, b, a);
}

#endif //TETROMAN_DEBUG_H
