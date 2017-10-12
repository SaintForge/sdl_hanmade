// game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Чт окт 12 17:19:17 2017 (+0300)
//           By: Sierra
//

#include "game.h"

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

static void
GameRenderBitmapToBuffer(game_offscreen_buffer *Buffer, game_bitmap *Bitmap, game_rect *Quad)
{
		 SDL_SetRenderTarget(Buffer->Renderer, Buffer->Memory);
		 SDL_RenderCopy(Buffer->Renderer, Bitmap->Texture, 0, Quad);
}

static bool
GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
		 bool ShouldQuit = false;

		 static game_rect SpriteQuad1 = {};
		 // static game_rect SpriteQuad2 = {};

		 if(!Memory->IsInitialized)
		 {
					Assert(Memory->SpriteOne.Texture);
					// Assert(Memory->SpriteTwo.Texture);

					SpriteQuad1.w = Memory->SpriteOne.Width;
					SpriteQuad1.h = Memory->SpriteOne.Height;
					SpriteQuad1.x = 0; SpriteQuad1.y = 0;

					// SpriteQuad2.w = Memory->SpriteTwo.Width;
					// SpriteQuad2.h = Memory->SpriteTwo.Height;
					// SpriteQuad2.x = Buffer->Width - SpriteQuad2.w; SpriteQuad2.y = 0;

					Memory->IsInitialized = true;
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

		 GameRenderBitmapToBuffer(Buffer, &Memory->SpriteOne, &SpriteQuad1);



		 return (ShouldQuit);
}
