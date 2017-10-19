/* win32_game.h --- 
 * 
 * Filename: win32_game.h
 * Author: 
 * Created: Ср окт 18 20:58:16 2017 (+0400)
 * Last-Updated: Чт окт 19 19:41:15 2017 (+0400)
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


