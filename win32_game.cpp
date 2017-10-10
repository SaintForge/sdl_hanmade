// win32_platform.cpp ---
// 
// Filename: win32_platform.cpp
// Author: Sierra
// Created: Пн окт  9 12:00:49 2017 (+0300)
// Last-Updated: Вт окт 10 17:04:34 2017 (+0300)
//           By: Sierra
//

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdint.h>

typedef int8_t   s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#include "win32_game.h"
#include "game.cpp"

static void
SDLChangeBufferColor(sdl_offscreen_buffer *Buffer, u8 Red, u8 Green, u8 Blue, u8 Alpha)
{
		 if(Buffer->Memory)
		 {
					u32 *Pixel = (u32*)Buffer->Memory;
					u32 PixelAmount = Buffer->Width * Buffer->Height;
					for (u32 i = 0;
							 i < PixelAmount; ++i)
					{
							 *Pixel++ = ((Red << 24) | (Green << 16) | (Blue << 8) | (Alpha));
					}
		 }
}

static void
SDLCreateTexture(sdl_offscreen_buffer *Buffer, SDL_Renderer* Renderer, int Width, int Height)
{
		 int BytesPerPixel = 4;
		 Buffer->Width  = Width;
		 Buffer->Height = Height;
		 Buffer->Pitch  = Width * BytesPerPixel;
		 
		 Buffer->Memory = (u32*)calloc(Width * Height, BytesPerPixel);
		 if(Buffer->Memory)
		 {
					Buffer->Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888,
																							SDL_TEXTUREACCESS_STREAMING,
																							Width, Height);
					if(Buffer->Texture)
					{
					}
					else
					{
							 printf("Failed to create SDL_Texture!\n", SDL_GetError());
					}
		 }
		 else
		 {
					printf("Failed to malloc Buffer->Memory!\n");
		 }
}



window_dimension SDLGetWindowDimension(SDL_Window* Window)
{
		 window_dimension WindowQuad = {};
		 SDL_GetWindowSize(Window, &WindowQuad.Width, &WindowQuad.Height);

		 return (WindowQuad);
}


static void
SDLProcessKeyPress(game_button_state *NewState, bool IsDown, bool WasDown)
{
		 NewState->IsDown  = IsDown;
		 NewState->WasDown = WasDown;
}

bool HandleEvent(SDL_Event *Event, game_input *Input)
{
		 bool ShouldQuit = false;

		 switch(Event->type)
		 {
					case SDL_QUIT:
					{
							 printf("SDL_QUIT\n");
							 ShouldQuit = true;
					} break;

					case SDL_KEYDOWN:
					case SDL_KEYUP:
					{
							 SDL_Keycode KeyCode = Event->key.keysym.sym;
							 bool IsDown = (Event->key.state == SDL_PRESSED);
							 bool WasDown = false;

							 if (Event->key.state == SDL_RELEASED)
							 {
										WasDown = true;
							 }
							 else if (Event->key.repeat != 0)
							 {
										WasDown = true;
							 }

							 if(Event->key.repeat == 0)
							 {
										Input->WasPressed = true;
										
										if(KeyCode == SDLK_w)
										{
												 SDLProcessKeyPress(&Input->Up, IsDown, WasDown);
										}
										if(KeyCode == SDLK_s)
										{
												 SDLProcessKeyPress(&Input->Down, IsDown, WasDown);
										}
										if(KeyCode == SDLK_a)
										{
												 SDLProcessKeyPress(&Input->Left, IsDown, WasDown);
										}
										if(KeyCode == SDLK_d)
										{
												 SDLProcessKeyPress(&Input->Right, IsDown, WasDown);
										}
										if(KeyCode == SDLK_ESCAPE)
										{
												 SDLProcessKeyPress(&Input->Escape, IsDown, WasDown);
										}

							 }										 
            
					} break;
		 }

		 return (ShouldQuit);
}


static void
SDLUpdateWindow(SDL_Window* Window, SDL_Renderer *Renderer, sdl_offscreen_buffer *Buffer)
{
		 SDL_UpdateTexture(Buffer->Texture, 0, Buffer->Memory, Buffer->Pitch);
		 SDL_RenderCopy(Renderer, Buffer->Texture, 0, 0);
		 SDL_RenderPresent(Renderer);
}


int main(int argc, char **argv)
{
		 SDL_Init(SDL_INIT_VIDEO);

		 SDL_Window *Window = SDL_CreateWindow("This is window", SDL_WINDOWPOS_CENTERED,
																					 SDL_WINDOWPOS_CENTERED, 1024, 768,
																					 SDL_WINDOW_ALLOW_HIGHDPI);
		 if(Window)
		 {
					SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1,
																											SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
					SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
					
					if(Renderer)
					{
							 bool IsRunning = true;
							 window_dimension Dimension = SDLGetWindowDimension(Window);
							 
							 sdl_offscreen_buffer BackBuffer = {};
							 SDLCreateTexture(&BackBuffer, Renderer, Dimension.Width, Dimension.Height);
							 SDLChangeBufferColor(&BackBuffer, 0, 0, 0, 255);
							 SDLUpdateWindow(Window, Renderer, &BackBuffer);

							 game_input Input = {};
							 
							 game_memory Memory = {};

							 int ImageWidth, ImageHeight, BytesPerPixel;
							 Memory.Storage = (u32*)stbi_load("sample.png", &ImageWidth, &ImageHeight, &BytesPerPixel, 4);
							 
							 if(Memory.Storage)
							 {
										Memory.StorageSize += (ImageWidth + ImageHeight) * BytesPerPixel;
							 }
							 else
							 {
										printf("Failed to load sample.png!\n");
							 }

							 while(IsRunning)
							 {
										SDL_Event Event;
										while(SDL_PollEvent(&Event))
										{
												 if(HandleEvent(&Event, &Input))
												 {
															IsRunning = false;
												 }
										}

										game_offscreen_buffer Buffer = {};
										Buffer.Memory = BackBuffer.Memory;
										Buffer.Width  = BackBuffer.Width;
										Buffer.Height = BackBuffer.Height;
										Buffer.Pitch  = BackBuffer.Pitch;

										
										if(GameUpdateAndRender(&Memory, &Input, &Buffer))
										{
												 IsRunning = false;
										}

										SDLUpdateWindow(Window, Renderer, &BackBuffer);
							 }

					}
					else
					{
							 printf("Failed to create SDL_Renderer!\n", SDL_GetError());
					}
					
		 }
		 else
		 {
					printf("Failed to create SDL_Window!\n", SDL_GetError());
		 }

		 return 0;
}
