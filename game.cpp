// game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Чт окт 19 17:33:16 2017 (+0300)
//           By: Sierra
//


#include "game.h"
#include "asset_game.h"
#include <vector>

static void
GameRenderBitmapToBuffer(game_offscreen_buffer *Buffer, game_texture *Texture, game_rect *Quad)
{
		 SDL_SetRenderTarget(Buffer->Renderer, Buffer->Memory);
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
CreateNewFigureEntity(figure_entity_form Form, figure_entity_type Type,
											u32 BlockSize, game_texture *&Texture)
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

     Figure->AreaQuad.x = 0;
     Figure->AreaQuad.y = 0;
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
		 Figure->Texture = Texture;

		 return (Figure);
}


static bool
GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
		 bool ShouldQuit = false;
		 u32 BlockSize = 40;
		 
		 game_rect rect = {};
		 rect.w = 100;
		 rect.h = 100;

		 game_rect rect2 = {};
		 rect2.x = 300;
		 rect2.y = 400;
		 rect2.w = 80;
		 rect2.h = 20;

		 // SDL_QueryTexture(Memory->SpriteI_D, 0, 0, &rect2.w, &rect2.h);

		 if(!Memory->IsInitialized)
		 {
					// Figure = CreateNewFigureEntity(I_figure, classic, BlockSize, Memory->SpriteI_D);
					// Assert(Figure->Texture);

					Memory->IsInitialized = true;
					printf("memory init!\n");

		 }
		 
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

					Input->WasPressed = false;
		 }

		 
		 return (ShouldQuit);
}

