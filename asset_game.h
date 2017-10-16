/* assert_game.h --- 
 * 
 * Filename: assert_game.h
 * Author: Sierra
 * Created: Пн окт 16 16:29:29 2017 (+0300)
 * Last-Updated: Пн окт 16 16:40:49 2017 (+0300)
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

#if _WIN32

/* Bitmaps */
static const char* grid_cell = "..\\data\\grid_cell.png";

/* Sound */
static const char* focus = "..\\data\\focus.wav";
static const char* cannon_fire = "..\\data\\cannon_fire.wav";
static const char* amb_ending_water = "..\\data\\amb_ending_water.ogg";

#else

/* Bitmaps */
static const char* grid_cell = "../data/grid_cell.png";

static const char* SpriteI_D = "../data/sprites/i_d.png";
static const char* SpriteI_M = "../data/sprites/i_m.png";
static const char* SpriteI_S = "../data/sprites/i_s.png";

static const char* SpriteO_D = "../data/sprites/o_d.png";
static const char* SpriteO_M = "../data/sprites/o_m.png";
static const char* SpriteO_S = "../data/sprites/o_s.png";

/* Sound */
static const char* focus = "../data/focus.wav";
static const char* cannon_fire = "../data/cannon_fire.wav";
static const char* amb_ending_water = "../data/amb_ending_water.ogg";

#endif

#define ASSERT_GAME_H
#endif
