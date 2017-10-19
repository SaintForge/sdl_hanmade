/* win32_game.h --- 
 * 
 * Filename: win32_game.h
 * Author: 
 * Created: Ср окт 18 20:58:16 2017 (+0400)
 * Last-Updated: Ср окт 18 20:59:52 2017 (+0400)
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

struct sdl_memory_bitmap
{
		 u32 Width;
		 u32 Height;
		 u32 Pitch;
		 u32 Rmask;
		 u32 Gmask;
		 u32 Bmask;
		 u32 Amask;

		 u8 BytesPerPixel;
		 u8 BitsPerPixel;
};

#define WIN32_GAME_H
#endif


