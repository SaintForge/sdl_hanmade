/* game.h --- 
 * 
 * Filename: game.h
 * Author: Sierra
 * Created: Пн окт  9 14:15:31 2017 (+0300)
 * Last-Updated: Чт окт 12 17:22:21 2017 (+0300)
 *           By: Sierra
 */

#if !defined(GAME_H)

#include <SDL2/SDL.h>

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

typedef SDL_Rect game_rect;

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
					};
		 };
};

struct game_bitmap
{
		 u32 Width;
		 u32 Height;
		 SDL_Texture *Texture;
};

struct game_memory
{
		 bool IsInitialized;

		 game_bitmap SpriteOne; //NOTE(Max): Maybe change it to SDL_Texture???
		 /* game_bitmap SpriteTwo; */
};



static bool GameUpdateAndRender(game_memory *Memory,
																game_input *Input,
																game_offscreen_buffer *Buffer);

#define GAME_H
#endif
