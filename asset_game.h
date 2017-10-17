/* assert_game.h --- 
 * 
 * Filename: assert_game.h
 * Author: Sierra
 * Created: Пн окт 16 16:29:29 2017 (+0300)
 * Last-Updated: Вт окт 17 09:03:38 2017 (+0300)
 *           By: Sierra
 */

#if !defined(ASSERT_GAME_H)

struct thread_data
{
		 SDL_Renderer *Renderer;
		 game_memory *Memory;

		 s64 ByteAmount;
		 bool IsInitialized;
};


#define ASSERT_GAME_H
#endif
