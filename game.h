/* game.h --- 
 * 
 * Filename: game.h
 * Author: Sierra
 * Created: Пн окт  9 14:15:31 2017 (+0300)
 * Last-Updated: Пт окт 20 16:18:51 2017 (+0300)
 *           By: Sierra
 */

#if !defined(GAME_H)

void LogErrorLine(const char* Message, int Line)
{
		 fprintf(stderr, "Assert fail in %s: %d\n",Message, Line);
}

#define Round(x) ((x)>=0?(float)((x)+0.5):(float)((x)-0.5))
#define Assert(Expression) if(!(Expression)) { LogErrorLine( __FILE__, __LINE__); *(int *)0 = 0;  }

enum figure_entity_form
{
		 O_figure, I_figure, L_figure, J_figure,
		 Z_figure, S_figure, T_figure
};

enum figure_entity_type
{
		 classic, stone, mirror
};

struct figure_entity
{
		 bool IsIdle;
		 bool IsStick;
		 float Angle;
		 float DefaultAnlge;
		 game_point Center;
		 game_point DefaultCenter;
		 game_point Shell[4];
		 game_point DefaultShell[4];
		 game_rect AreaQuad;
		 figure_entity_form Form;
		 figure_entity_type Type;
		 game_texture *Texture;
};


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

		 s32 MouseX, MouseY;
		 s32 MouseRelX, MouseRelY;
};

struct game_state
{
		 figure_entity *Figure[2];
};

struct game_memory
{
		 bool IsInitialized;
		 game_state *State;

		 void *Assets;
		 u32 AssetsSpace;
};



static bool GameUpdateAndRender(game_memory *Memory, game_input *Input,
																game_offscreen_buffer *Buffer);

#define GAME_H
#endif
