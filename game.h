/* game.h --- 
 * 
 * Filename: game.h
 * Author: Sierra
 * Created: Пн окт  9 14:15:31 2017 (+0300)
 * Last-Updated: Пт окт 27 10:34:58 2017 (+0300)
 *           By: Sierra
 */

#if !defined(GAME_H)

void LogErrorLine(const char* Message, int Line)
{
     fprintf(stderr, "Assert fail in %s: %d\n",Message, Line);
}

#define Assert(Expression) if(!(Expression)) { LogErrorLine( __FILE__, __LINE__); *(int *)0 = 0;  }

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
     bool MouseMotion;
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

enum figure_form
{
     O_figure, I_figure, L_figure, J_figure,
     Z_figure, S_figure, T_figure
};

enum figure_type
{
     classic, stone, mirror
};

struct figure_unit
{
     figure_unit *Next;
     
     bool IsStick;
     bool IsEnlarged; // not sure if we need it
     bool IsIdle;


     u32 Index;     
     r32 Angle;
     r32 DefaultAngle;
     SDL_RendererFlip Flip;
     
     game_point Center;
     game_point DefaultCenter;
     game_point Shell[4];
     game_point DefaultShell[4];
     game_rect AreaQuad;
     
     figure_form Form;
     figure_type Type;
     
     game_texture *Texture;
};

struct figure_entity
{
     figure_unit *HeadFigure;
     figure_unit *GrabbedFigure;
     
     u32 FigureAmount;
     game_rect FigureArea;

     bool IsGrabbed;
     bool IsRotating;
     bool IsFlipping;

     u8 Alpha;
     u8 FadeInSum;
     u8 FadeOutSum;
     r32 RotationSum;
};

struct sticked_unit
{
     u32 Index;
     u32 Row[4];
     u32 Col[4];
};

struct grid_entity
{
     u8 **UnitField;
     
     u32 RowAmount;
     u32 ColumnAmount;
     u32 BlockSize;

     u32 StickUnitsAmount;
     sticked_unit *StickUnits;

     bool BlockIsGrabbed;
     bool BeginAnimationStart; // this is for tiny little animation at the beginning of a level
     
     game_rect GridArea;
     game_texture *NormalSquareTexture;
     game_texture *VerticalSquareTexture;
     game_texture *HorizontlaSquareTexture;
};

struct game_state
{
     grid_entity   *GridEntity;
     figure_entity *FigureEntity;

     u32 ActiveBlockSize;
     u32 InActiveBlockSize;
};

struct game_memory
{
     bool IsInitialized;
     game_state State;

     game_music *Music;
     game_sound *Sound;

     void *Assets;
     u64 AssetsSpace;
     bool AssetsInitialized;
};

static bool GameUpdateAndRender(game_memory *Memory, game_input *Input,
                                game_offscreen_buffer *Buffer);

#define GAME_H
#endif
