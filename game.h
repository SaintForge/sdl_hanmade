/* game.h --- 
 * 
 * Filename: game.h
 * Author: Sierra
 * Created: Пн окт  9 14:15:31 2017 (+0300)
 * Last-Updated: Ср окт 18 10:02:25 2017 (+0300)
 *           By: Sierra
 */

#if !defined(GAME_H)

/* TODO(Max): Check if we need these headers here!!! */
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <stdio.h>

void LogErrorLine(const char* Message, int Line)
{
		 fprintf(stderr, "Assert fail in %s: %d\n",Message, Line);
}

#define Assert(Expression) if(!(Expression)) { LogErrorLine( __FILE__, __LINE__); *(int *)0 = 0;  } 

typedef SDL_Rect    game_rect;
typedef SDL_Point   game_point;
typedef SDL_Texture game_texture;
typedef SDL_Surface game_surface;
typedef Mix_Chunk   game_sound;
typedef Mix_Music   game_music;

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

struct game_memory
{
		 bool IsInitialized;

		 game_texture *GridCell;
		 
		 game_texture *SpriteI_D;
		 game_texture *SpriteI_M;
		 game_texture *SpriteI_S;

		 game_texture *SpriteO_D;
		 game_texture *SpriteO_M;
		 game_texture *SpriteO_S;

		 game_sound *SoundOne;
		 game_sound *SoundTwo;

		 game_music *MusicOne;
};



static bool GameUpdateAndRender(game_memory *Memory, game_input *Input,
																game_offscreen_buffer *Buffer);

#define GAME_H
#endif
