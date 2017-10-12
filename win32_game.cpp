// win32_platform.cpp ---
// 
// Filename: win32_platform.cpp
// Author: Sierra
// Created: Пн окт  9 12:00:49 2017 (+0300)
// Last-Updated: Чт окт 12 17:28:24 2017 (+0300)
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
																							SDL_TEXTUREACCESS_TARGET,
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
		 SDL_SetRenderTarget(Renderer, NULL);
		 SDL_RenderCopy(Renderer, Buffer->Texture, 0, 0);
		 SDL_RenderPresent(Renderer);
}


static void
SDLLoadGameBitmapFromSurface(SDL_Renderer *Renderer, game_bitmap *Bitmap, char *FileName)
{
		 SDL_Surface *Surface = IMG_Load(FileName);

		 Bitmap->Width = Surface->w;
		 Bitmap->Height = Surface->h;
		 Bitmap->Texture = SDL_CreateTextureFromSurface(Renderer, Surface);

		 SDL_FreeSurface(Surface);
}

static void
SDLLoadGameMemoryFromBinary(SDL_RWops *&BinaryFile, game_memory * GameMemory)
{
		 u8 BytesPerPixel = 4;
		 u32 Width = 100;
		 u32 Height = 100;
		 u32 SurfaceSize = Width * Height;

     // TODO(Max): Elaborate that!!!
		 SDL_Surface *Surface = 0;
		 
		 printf("Read: %d\n",SDL_RWread(BinaryFile, GameMemory->SpriteOne.Texture,
																		BytesPerPixel, SurfaceSize));
		 GameMemory->SpriteOne.Width = Width;
		 GameMemory->SpriteOne.Height = Height;
}

#if ASSET_BUILD
static void
SDLWriteGameAssetsToFile(SDL_RWops *&BinaryFile, char * FileName)
{
		 SDL_Surface *Surface = IMG_Load(FileName);
		 Assert(Surface);

		 u8 BytesPerPixel = Surface->format->BytesPerPixel;
		 u32 Width = Surface->w;
		 u32 Height = Surface->h;
		 u32 SurfaceSize = Width * Height;
		 printf("Bytes = %d\n",BytesPerPixel);
		 printf("Width = %d\n",Width);
		 printf("Height = %d\n",Height);
		 printf("SurfaceSize = %d\n",SurfaceSize);

		 printf("Wrote: %d\n",SDL_RWwrite(BinaryFile, Surface->pixels, BytesPerPixel, SurfaceSize));

		 SDL_FreeSurface(Surface);
}
#endif

int main(int argc, char **argv)
{
		 SDL_Init(SDL_INIT_VIDEO);

		 SDL_DisplayMode Display = {};
		 SDL_GetDesktopDisplayMode(0, &Display);
		 
		 SDL_Window *Window = SDL_CreateWindow("This is window", SDL_WINDOWPOS_CENTERED,
																					 SDL_WINDOWPOS_CENTERED, Display.w, Display.h,
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

							 SDL_RWops *BinaryFile = SDL_RWFromFile("package.bin", "wb");
							 Assert(BinaryFile);

							 SDLWriteGameAssetsToFile(BinaryFile, "grid_cell.png");
							 SDLWriteGameAssetsToFile(BinaryFile, "grid_cell.png");

							 SDL_RWclose(BinaryFile);

							 game_memory Memory = {};
							 BinaryFile = SDL_RWFromFile("package.bin", "rb");
							 Assert(BinaryFile);

							 SDLLoadGameMemoryFromBinary(BinaryFile, &Memory);

							 // SDLLoadGameBitmapFromSurface(Renderer, &Memory.SpriteOne, "sample.png");
							 
							 SDL_RWclose(BinaryFile);
							 
							 game_input Input = {};

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
										Buffer.Renderer = Renderer;
										Buffer.Memory   = BackBuffer.Texture;
										Buffer.Width    = BackBuffer.Width;
										Buffer.Height   = BackBuffer.Height;

										
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
