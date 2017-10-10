/* game.h --- 
 * 
 * Filename: game.h
 * Author: Sierra
 * Created: Пн окт  9 14:15:31 2017 (+0300)
 * Last-Updated: Вт окт 10 14:09:43 2017 (+0300)
 *           By: Sierra
 */

#if !defined(GAME_H)

struct game_offscreen_buffer
{
		 u32 *Memory;
		 int Width;
		 int Height;
		 int Pitch;
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

struct game_memory
{
		 bool IsInitialized;
		 u32 StorageSize;
		 u32 *Storage;
};

static bool GameUpdateAndRender(game_memory *Memory,
																game_input *Input,
																game_offscreen_buffer *Buffer);

#define GAME_H
#endif
