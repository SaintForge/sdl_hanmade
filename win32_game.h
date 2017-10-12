
/* win32_game.h --- 
 * 
 * Filename: win32_game.h
 * Author: Sierra
 * Created: Вт окт 10 10:26:51 2017 (+0300)
 * Last-Updated: Чт окт 12 16:04:08 2017 (+0300)
 *           By: Sierra
 */

#if !defined(WIN32_GAME_H)

struct sdl_offscreen_buffer
{
		 SDL_Texture *Texture;
		 u32 *Memory;
		 int Width;
		 int Height;
		 int Pitch;
};


struct window_dimension
{
		 int Width;
		 int Height;
};

#define WIN32_GAME_H
#endif


