// menu_game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

#include "menu_game.h"

static void
MenuUpdateAndRender(game_offscreen_buffer *Buffer, game_memory *Memory, game_input *Input)
{
    game_rect ScreenQuad = {0, 0, Buffer->Width, Buffer->Height };
    DEBUGRenderQuadFill(Buffer, &ScreenQuad, {0, 0, 0}, 0);
    
    for(u32 i = 0; i < Memory->MenuEntity->ButtonsAmount; ++i)
    {
        GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->BackTexture, &Memory->MenuEntity->Buttons[i].ButtonQuad);
        GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->Buttons[i].LevelNumberTexture,
                                 &Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad);
    }
    }


