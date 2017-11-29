// menu_game.h --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

#if !defined(MENU_GAME_H)

struct menu_button
{
    game_rect TextureQuad;
    game_texture *Texture;
};

struct game_menu
{
    u32 ButtonSizeWidth;
    u32 ButtonSizeHeight;
    u32 ButtonsAmount;
    
    game_texture *Texture;
    menu_button *Buttons;
    };

static void MenuUpdateAndRender(game_offscreen_buffer *Buffer, game_memory *Memory, game_input *Input);

#define MENU_GAME_H
#endif