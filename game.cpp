// game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 20 16:28:29 2017 (+0300)
//           By: Sierra
//


#include "game.h"
#include "asset_game.h"
#include <vector>

static void
DEBUGRenderQuad(game_offscreen_buffer *Buffer, game_rect *AreaQuad, SDL_Color color)
{
		 SDL_SetRenderTarget(Buffer->Renderer, Buffer->Memory);
		 SDL_SetRenderDrawColor(Buffer->Renderer, color.r, color.g, color.b, 255);
		 SDL_RenderDrawRect(Buffer->Renderer, AreaQuad);
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
FigureEntityMove(figure_entity *Entity, float XShift, float YShift)
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
		 real32 OldWidth  = Entity->AreaQuad.w;
		 real32 OldHeight = Entity->AreaQuad.h;

		 u32 RowBlocks    = (Entity->AreaQuad.w) / (BlockSize);
		 u32 ColumnBlocks = (Entity->AreaQuad.h) / (BlockSize);

		 BlockSize += ScaleRatio;
		 Entity->AreaQuad.w = RowBlocks * BlockSize;
		 Entity->AreaQuad.h = ColumnBlocks *BlockSize;

		 float WidthRatio  = (Entity->Center.x - Entity->AreaQuad.w) / OldWidth;
		 float HeightRatio = (Entity->Center.x - Entity->AreaQuad.h) / OldHeight;
		 
		 float NewPointX = Round((WidthRatio * Entity->AreaQuad.w) + Entity->AreaQuad.x);
		 float NewPointY = Round((HeightRatio * Entity->AreaQuad.h) + Entity->AreaQuad.y);
		 Entity->Center.x = NewPointX;
		 Entity->Center.y = NewPointY;

		 for (u32 i = 0; i < 4; ++i)
		 {
					WidthRatio  = (Entity->Shell[i].x - Entity->AreaQuad.w) / OldWidth;
					HeightRatio = (Entity->Shell[i].y - Entity->AreaQuad.h) / OldHeight;

					NewPointX = Round((WidthRatio * Entity->AreaQuad.w) + Entity->AreaQuad.w);
					NewPointX = Round((HeightRatio * Entity->AreaQuad.h) + Entity->AreaQuad.h);

					Entity->Shell[i].x = NewPointX;
					Entity->Shell[i].y = NewPointY;
		 }
		 
}

static bool
GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
		 bool ShouldQuit = false;
		 u32 BlockSize = 40;

		 static bool Grabbed = false;
		 static s32 GrabIndex = 0;
		 
		 if(!Memory->IsInitialized)
		 {
					Memory->State = (game_state*)malloc(sizeof(game_state));
					Assert(Memory->State);

					game_state *State = Memory->State;
					State->Figure[0] =
							 CreateNewFigureEntity(Buffer, Memory, "i_d.png", 0, 0, I_figure, classic, BlockSize);
					
					State->Figure[1] =
							 CreateNewFigureEntity(Buffer, Memory, "o_d.png", 100, 100, O_figure, classic, BlockSize);

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
							 printf("Left mouse \n");
							 if(!Grabbed)
							 {
										for (u32 i = 0; i < 2; ++i)
										{
												 if(IsPointInsideRect(Input->MouseX, Input->MouseY, &State->Figure[i]->AreaQuad))
												 {
															GrabIndex = i;
															Grabbed = true;
															FigureEntityScaleBlock(State->Figure[i], BlockSize, 20);
												 }		 
										}
							 }
							 else
							 {
										if(GrabIndex != -1)
										{
												 FigureEntityScaleBlock(State->Figure[GrabIndex], BlockSize, -20);
												 GrabIndex = -1;
												 Grabbed = false;
										}
							 }
					}

					Input->WasPressed = false;
		 }

		 if(Grabbed)
		 {
					FigureEntityMoveTo(State->Figure[GrabIndex], Input->MouseX, Input->MouseY);
		 }

		 GameRenderBitmapToBuffer(Buffer, Memory->State->Figure[0]->Texture, &Memory->State->Figure[0]->AreaQuad);
		 GameRenderBitmapToBuffer(Buffer, Memory->State->Figure[1]->Texture, &Memory->State->Figure[1]->AreaQuad);

		 return(ShouldQuit);
}

