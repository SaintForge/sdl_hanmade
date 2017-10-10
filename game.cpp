// game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Вт окт 10 16:25:50 2017 (+0300)
//           By: Sierra
//

#include "game.h"

struct game_bitmap
{
		 u32 Width, Height, Pitch;
		 u8 BytesPerPixel;
		 void *Pixels;
};

static void
GameCopyImageToBuffer(game_bitmap* GameBitmap, u32 X, u32 Y,
											game_offscreen_buffer *Buffer)
{
		 u8 BytesPerPixel = 4;

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
}

static game_bitmap ImageSurface = {};

static bool
GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
		 bool ShouldQuit = false;

		 if(!Memory->IsInitialized)
		 {
					Memory->IsInitialized = true;
					
					ImageSurface.Width  = 512;
					ImageSurface.Height = 512;
					ImageSurface.BytesPerPixel = 4;
					ImageSurface.Pitch  = ImageSurface.BytesPerPixel * ImageSurface.Width;
					ImageSurface.Pixels = Memory->Storage;
		 }

		 GameCopyImageToBuffer(&ImageSurface, 0, 0, Buffer);

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
