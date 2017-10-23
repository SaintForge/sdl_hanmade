// game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Вс окт 22 10:22:41 2017 (+0300)
//           By: Sierra
//


#include "game.h"
#include "asset_game.h"

static void
DEBUGRenderQuad(game_offscreen_buffer *Buffer, game_rect *AreaQuad, SDL_Color color)
{
     u8 r, g, b;
     SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, 0);
     
     SDL_SetRenderDrawColor(Buffer->Renderer, color.r, color.g, color.b, 255);
     SDL_RenderDrawRect(Buffer->Renderer, AreaQuad);
     SDL_SetRenderDrawColor(Buffer->Renderer, r, g, b, 255);
}

static void
GameRenderBitmapToBuffer(game_offscreen_buffer *Buffer, game_texture *Texture, game_rect *Quad)
{
     // SDL_SetRenderTarget(Buffer->Renderer, Buffer->Memory);
     SDL_RenderCopy(Buffer->Renderer, Texture, 0, Quad);
}

#if 0
static void
GameCopyImageToBuffer(game_bitmap* GameBitmap, u32 X, u32 Y,
                      game_offscreen_buffer *Buffer)
{
     u8 BytesPerPixel = GameBitmap->BytesPerPixel;

     u8 *RowBuffer = (u8*)Buffer->Memory + (Y * GameBitmap->Pitch);
     u8 *RowTarget = (u8*)GameBitmap->Pixels;
		 
     for (u32 y = 0; y < GameBitmap->Height; ++y)
     {
          u32 *PixelBuffer = (u32*)RowBuffer + X;
          u32 *PixelTarget = (u32*)RowTarget;
					
          for (u32 x = 0; x < GameBitmap->Width; ++x)
          {
               u8 Alpha = (*PixelTarget >> 24) & 0xFF;
               u8 Blue  = (*PixelTarget >> 16) & 0xFF;
               u8 Green = (*PixelTarget >> 8)  & 0xFF;
               u8 Red   = (*PixelTarget & 0xFF);
							 
               *PixelBuffer = ((Red << 24) | (Green << 16) | (Blue << 8) | (Alpha));

               *PixelBuffer++;
               *PixelTarget++;
          }
					
          RowBuffer += Buffer->Pitch;
          RowTarget += GameBitmap->Width * BytesPerPixel;
     }
     printf("GameCopyImageToBuffer\n");
}
#endif

static figure_entity*
CreateNewFigureEntity(game_offscreen_buffer *Buffer,
                      game_memory *Memory, char* AssetName, u32 X, u32 Y,
                      figure_entity_form Form, figure_entity_type Type,	u32 BlockSize)
{
     figure_entity *Figure = (figure_entity *)malloc(sizeof(figure_entity));
     Assert(Figure);

     int Index           = 0;
     int RowAmount       = 0;
     int ColumnAmount    = 0;
     float CenterOffset  = 0.5f;
		 
     std::vector<std::vector<int>> matrix(2);
     for (int i = 0; i < 2; i++)
     {
          matrix[i].resize(4);
     }

     switch(Form)
     {
          case I_figure:
          {
               matrix = { {1, 1, 1, 1}, {0, 0, 0, 0} };
               RowAmount = 4;
               ColumnAmount = 1;
          } break;
	  
          case O_figure:
          {
               matrix = { { 1, 1 }, { 1, 1 } };
               RowAmount = 2;
               ColumnAmount = 2;
          }break;
	  
          case Z_figure:
          {
               matrix = { {1, 1, 0}, {0, 1, 1} };
               RowAmount = 3;
               ColumnAmount = 2;
          }break;
	  
          case S_figure:
          {
               matrix = { {0, 1, 1}, {1, 1, 0} };
               RowAmount = 3;
               ColumnAmount = 2;
          }break;
	  
          case T_figure:
          {
               matrix = { {0, 1, 0}, {1, 1, 1} };
               CenterOffset = 0.75f;
               RowAmount = 3;
               ColumnAmount = 2;
          }break;
	  
          case L_figure:
          {
               matrix = { {0, 0, 1},{1, 1, 1} };
               CenterOffset = 0.75f;
               RowAmount = 3;
               ColumnAmount = 2;
          }break;
	  
          case J_figure:
          {
               matrix = { {1, 0, 0}, {1, 1, 1} };
               CenterOffset = 0.75f;
               RowAmount = 3;
               ColumnAmount = 2;
          }break;
     }

     Figure->AreaQuad.x = X;
     Figure->AreaQuad.y = Y;
     Figure->AreaQuad.w = RowAmount*BlockSize;
     Figure->AreaQuad.h = ColumnAmount*BlockSize;

     Figure->Center.x = Figure->AreaQuad.x + (Figure->AreaQuad.w / 2);
     Figure->Center.y = Figure->AreaQuad.y + (((float)Figure->AreaQuad.h) * CenterOffset);
     Figure->DefaultCenter = Figure->Center;
     
     for (u32 i = 0; i < 2; i++)
     {
          for (u32 j = 0; j < 4; j++)
          {
               if(matrix[i][j] == 1)
               {
                    Figure->Shell[Index].x = Figure->AreaQuad.x + (j * BlockSize) + (BlockSize / 2);
                    Figure->Shell[Index].y = Figure->AreaQuad.y + (i * BlockSize) + (BlockSize / 2);
                    Figure->DefaultShell[Index] = Figure->Shell[Index];
                    Index++;
               }
          }
     }

     Figure->IsIdle = true;
     Figure->IsIdle = false;
     Figure->Angle  = 0.0f;
     Figure->DefaultAnlge = 0.0f;
     Figure->Form = Form;
     Figure->Type = Type;
     Figure->Texture = GetTexture(Memory, AssetName, Buffer->Renderer);

     return (Figure);
}

static void
DestroyFigureEntity(figure_entity *Entity)
{
     if(Entity)
     {
          FreeTexture(Entity->Texture);
          free(Entity);
     }
}


inline static bool
IsPointInsideRect(s32 X, s32 Y, game_rect *Quad)
{
     if(!Quad) return false;

     if(X < Quad->x)                return false;
     else if(Y < Quad->y)           return false;
     else if(X > Quad->x + Quad->w) return false;
     else if(Y > Quad->y + Quad->h) return false;
     else                           return true;
}

static void
FigureEntityMove(figure_entity *Entity, s32 XShift, s32 YShift)
{
     int XOffset = Entity->AreaQuad.x - Entity->Center.x;
     int YOffset = Entity->AreaQuad.y - Entity->Center.y;

     Entity->Center.x += XShift;
     Entity->Center.y += YShift;

     Entity->AreaQuad.x = Entity->Center.x + XOffset;
     Entity->AreaQuad.y = Entity->Center.y + YOffset;

     for (u32 i = 0; i < 4; ++i)
     {
          Entity->Shell[i].x += XShift;
          Entity->Shell[i].y += YShift;
     }
}

static void
FigureEntityMoveTo(figure_entity *Entity, s32 NewPointX, s32 NewPointY)
{
     s32 XShift = NewPointX - Entity->Center.x;
     s32 YShift = NewPointY - Entity->Center.y;
     FigureEntityMove(Entity, XShift, YShift);
}

static void
FigureEntityScaleBlock(figure_entity *Entity, u32 BlockSize, s32 ScaleRatio)
{
     if(!Entity) return;

     u32 RowBlock  = 0;
     u32 ColBlock  = 0;
     r32 OldWidth  = 0;
     r32 OldHeight = 0;
     r32 WRatio    = 0;
     r32 HRatio    = 0;

     OldWidth  = Entity->AreaQuad.w;
     OldHeight = Entity->AreaQuad.h;

     RowBlock = (Entity->AreaQuad.w) / (BlockSize);
     ColBlock = (Entity->AreaQuad.h) / (BlockSize);

     BlockSize += ScaleRatio;
     Entity->AreaQuad.w = RowBlock * BlockSize;
     Entity->AreaQuad.h = ColBlock * BlockSize;

     WRatio = (Entity->Center.x - Entity->AreaQuad.x) / OldWidth;
     HRatio = (Entity->Center.y - Entity->AreaQuad.y) / OldHeight;
		 
     Entity->Center.x = roundf((WRatio * Entity->AreaQuad.w) + Entity->AreaQuad.x);
     Entity->Center.y = roundf((HRatio * Entity->AreaQuad.h) + Entity->AreaQuad.y);
     for (u32 i = 0; i < 4; ++i)
     {
          WRatio = (Entity->Shell[i].x - Entity->AreaQuad.x) / OldWidth;
          HRatio = (Entity->Shell[i].y - Entity->AreaQuad.y) / OldHeight;
          Entity->Shell[i].x = roundf((WRatio * Entity->AreaQuad.w) + Entity->AreaQuad.x);
          Entity->Shell[i].y = roundf((HRatio * Entity->AreaQuad.h) + Entity->AreaQuad.y);
     }
		 
}

static void
FigureEntityRotateBy(figure_entity *Entity, float Angle)
{
     if((s32)Entity->Angle == 0)
     {
          Entity->Angle = 0;
     }

     for (u32 i = 0; i < 4; ++i)
     {
          float Radians = Angle * (M_PI/180.0f);
          float Cos = cos(Radians);
          float Sin = sin(Radians);

          float X = Entity->Center.x + (Entity->Shell[i].x - Entity->Center.x) * Cos
               - (Entity->Shell[i].y - Entity->Center.y) * Sin;
          float Y = Entity->Center.y + (Entity->Shell[i].x - Entity->Center.x) * Sin
               + (Entity->Shell[i].y - Entity->Center.y) * Cos;

          Entity->Shell[i].x = roundf(X);
          Entity->Shell[i].y = roundf(Y);
     }
}

static bool
GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
     bool ShouldQuit = false;
     u32 BlockSize = 40;

     static bool Grabbed = false;
     static s32 GrabIndex = 0;

     static s32 OffsetX = 0;
     static s32 OffsetY = 0;
		 
     if(!Memory->IsInitialized)
     {
          Memory->State = (game_state*)malloc(sizeof(game_state));
          Assert(Memory->State);

          game_state *State = Memory->State;
          State->Figure[0] =
               CreateNewFigureEntity(Buffer, Memory, "i_s.png", 0, 0, I_figure, classic, BlockSize);
					
          State->Figure[1] =
               CreateNewFigureEntity(Buffer, Memory, "o_d.png", 100, 100, O_figure, classic, BlockSize);

          // Memory->Music = GetMusic(Memory, "amb_ending_water.ogg");
          // Mix_PlayMusic(Memory->Music, -1);

          Memory->IsInitialized = true;
          printf("memory init!\n");

     }

     game_state *State = Memory->State;
		 
     if(Input->WasPressed)
     {
          if(Input->Up.IsDown)
          {
               printf("Up is down!\n");
          }
					
          if(Input->Escape.IsDown)
          {
               printf("Quiting!\n");
               ShouldQuit = true;
          }

          if(Input->LeftClick.IsDown)
          {
               if(!Grabbed)
               {
                    for (u32 i = 0; i < 2; ++i)
                    {
                         if(IsPointInsideRect(Input->MouseX, Input->MouseY, &State->Figure[i]->AreaQuad))
                         {
                              GrabIndex = i;
                              Grabbed = true;
                              
                              FigureEntityScaleBlock(State->Figure[i], BlockSize, 20);
                              FigureEntityMove(State->Figure[i], -10, -10);
                              
                              OffsetX = Input->MouseX - State->Figure[i]->Center.x;
                              OffsetY = Input->MouseY - State->Figure[i]->Center.y;
                              FigureEntityMove(State->Figure[i], OffsetX, OffsetY);
                              SDL_ShowCursor(SDL_DISABLE);
                              break;
                         }		 
                    }
               }
               else
               {
                    if(GrabIndex != -1)
                    {
                         FigureEntityScaleBlock(State->Figure[GrabIndex], BlockSize+10, -10);
                         FigureEntityMove(State->Figure[GrabIndex], 10, 10);
                         Grabbed = false;
                         SDL_ShowCursor(SDL_ENABLE);
                    }
               }
          }
          if(Input->RightClick.IsDown)
          {
               printf("Right click \n");
               
          }

          Input->WasPressed = false;
     }

     if(Grabbed)
     {
          FigureEntityMoveTo(State->Figure[GrabIndex], Input->MouseX - OffsetX, Input->MouseY - OffsetY);
     }

     DEBUGRenderQuad(Buffer, &State->Figure[0]->AreaQuad, {255, 0, 0});

     game_rect Rect = {};
     Rect.x = State->Figure[0]->Center.x - 2;
     Rect.y = State->Figure[0]->Center.y - 2;
     Rect.w = 4;
     Rect.h = 4;

     GameRenderBitmapToBuffer(Buffer, Memory->State->Figure[0]->Texture, &Memory->State->Figure[0]->AreaQuad);
     GameRenderBitmapToBuffer(Buffer, Memory->State->Figure[1]->Texture, &Memory->State->Figure[1]->AreaQuad);

     DEBUGRenderQuad(Buffer, &Rect, {255, 0, 0});

     for (u32 i = 0; i < 4; ++i)
     {
          game_rect ShellRect = {};
          ShellRect.x = State->Figure[0]->Shell[i].x - 2;
          ShellRect.y = State->Figure[0]->Shell[i].y - 2;
          ShellRect.w = 4;
          ShellRect.h = 4;
          
          DEBUGRenderQuad(Buffer, &ShellRect, {255, 0, 0});
     }


     return(ShouldQuit);
}

