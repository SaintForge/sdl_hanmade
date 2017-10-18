/* linux_game.h --- 
 * 
 * Filename: linux_game.h
 * Author: Sierra
 * Created: Вт окт 10 10:26:51 2017 (+0300)
 * Last-Updated: Ср окт 18 12:34:46 2017 (+0300)
 *           By: Sierra
 */

#if !defined(LINUX_GAME_H)

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

#define LINUX_GAME_H
#endif


