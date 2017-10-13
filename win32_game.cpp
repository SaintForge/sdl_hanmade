// win32_platform.cpp ---
// 
// Filename: win32_platform.cpp
// Author: Sierra
// Created: Пн окт  9 12:00:49 2017 (+0300)
// Last-Updated: Пт окт 13 16:42:50 2017 (+0300)
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
SDLLoadGameSound(SDL_RWops *&BinaryFile, game_sound *&Sound)
{
		 Sound = Mix_LoadWAV_RW(BinaryFile, 0);
}

static void
SDLLoadGameBitmap(SDL_RWops *&BinaryFile, SDL_Renderer *&Renderer, game_bitmap * GameBitmap)
{
		 SDL_Surface *TempSurface = 0;
		 sdl_memory_bitmap BitmapInfo = {};
		 
		 SDL_RWread(BinaryFile, &BitmapInfo, sizeof(sdl_memory_bitmap), 1);

		 void *Pixels = malloc(BitmapInfo.BytesPerPixel * BitmapInfo.Width * BitmapInfo.Height);
		 Assert(Pixels);

		 SDL_RWread(BinaryFile, Pixels, BitmapInfo.BytesPerPixel * BitmapInfo.Width *
								BitmapInfo.Height, 1);

		 TempSurface =
					SDL_CreateRGBSurfaceFrom(Pixels, BitmapInfo.Width, BitmapInfo.Height, BitmapInfo.BitsPerPixel, BitmapInfo.Pitch,
																	 BitmapInfo.Rmask, BitmapInfo.Gmask, BitmapInfo.Bmask, BitmapInfo.Amask);
		 Assert(TempSurface);

		 GameBitmap->Width   = TempSurface->w;
		 GameBitmap->Height  = TempSurface->h;
		 GameBitmap->Texture = SDL_CreateTextureFromSurface(Renderer, TempSurface);
		 Assert(GameBitmap->Texture);

		 free(Pixels);
		 SDL_FreeSurface(TempSurface);
}

#if ASSET_BUILD

static u32
SDLSizeOfFile(SDL_RWops *&BinaryFile)
{
		 u32 DefaultOffset = SDL_RWtell(BinaryFile);
		 SDL_RWseek(BinaryFile, 0, RW_SEEK_END);
		 
		 u32 ByteSize = SDL_RWtell(BinaryFile);
		 SDL_RWseek(BinaryFile, DefaultOffset, RW_SEEK_SET);

		 return (ByteSize);
}

static void
SDLWriteGameSoundToFile(SDL_RWops *&BinaryTarget, char* SourceFile)
{
		 u64 ByteSize  = 0;
		 void *Samples = 0;
		 SDL_RWops *BinarySource = 0;

		 BinarySource = SDL_RWFromFile(SourceFile, "rb");
		 Assert(BinarySource);
		 
		 ByteSize = SDLSizeOfFile(BinarySource);
		 printf("ByteSize = %d\n",ByteSize);

		 Samples = malloc(ByteSize);
		 Assert(Samples);
		 SDL_RWread(BinarySource, Samples, ByteSize, 1);
		 SDL_RWclose(BinarySource);
		 
		 SDL_RWwrite(BinaryTarget, Samples, ByteSize, 1);
		 free(Samples);
}

static void
SDLWriteGameBitmapToFile(SDL_RWops *&BinaryFile, char * FileName)
{
		 SDL_Surface *Surface = IMG_Load(FileName);
		 Assert(Surface);

		 sdl_memory_bitmap BitmapInfo = {};
		 BitmapInfo.Width         = Surface->w;
		 BitmapInfo.Height        = Surface->h;
		 BitmapInfo.Pitch         = Surface->pitch;
		 BitmapInfo.Rmask         = Surface->format->Rmask;
		 BitmapInfo.Gmask         = Surface->format->Gmask;
		 BitmapInfo.Bmask         = Surface->format->Bmask;
		 BitmapInfo.Amask         = Surface->format->Amask;
		 BitmapInfo.BytesPerPixel = Surface->format->BytesPerPixel;
		 BitmapInfo.BitsPerPixel  = Surface->format->BitsPerPixel;
		 
		 printf("Wrote bitmap_info: %d\n",SDL_RWwrite(BinaryFile, &BitmapInfo,	sizeof(sdl_memory_bitmap), 1));
		 
		 printf("Wrote bitmap: %d\n",SDL_RWwrite(BinaryFile, Surface->pixels, BitmapInfo.BytesPerPixel * BitmapInfo.Width * BitmapInfo.Height, 1));

		 SDL_FreeSurface(Surface);
}
#endif

int main(int argc, char **argv)
{
		 SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
		 Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

		 SDL_DisplayMode Display = {};
		 SDL_GetDesktopDisplayMode(0, &Display);
		 
		 SDL_Window *Window = SDL_CreateWindow("This is window", SDL_WINDOWPOS_CENTERED,
																					 SDL_WINDOWPOS_CENTERED, Display.w, Display.h,
																					 SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_FULLSCREEN);
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

							 SDL_RWops *BinaryFile = 0;
#if ASSET_BUILD
							 printf("Writing\n");
							 BinaryFile = SDL_RWFromFile("package.bin", "wb");
							 
							 SDLWriteGameBitmapToFile(BinaryFile, "grid_cell.png");
							 SDLWriteGameBitmapToFile(BinaryFile, "grid_cell.png");

							 SDLWriteGameSoundToFile(BinaryFile, "focus.wav");
							 SDLWriteGameSoundToFile(BinaryFile, "cannon_fire.wav");
							 SDLWriteGameSoundToFile(BinaryFile, "amb_ending_water.ogg");
							 // SDLWriteGameSoundToFile(BinaryFile, "amb_ending_water.ogg");
							 // SDLWriteGameSoundToFile(BinaryFile, "amb_ending_water.ogg");
							 
							 SDL_RWclose(BinaryFile);
#endif
							 game_memory Memory = {};

							 BinaryFile = SDL_RWFromFile("package.bin", "rb");
							 
							 SDLLoadGameBitmap(BinaryFile, Renderer, &Memory.SpriteOne);
							 SDLLoadGameBitmap(BinaryFile, Renderer, &Memory.SpriteTwo);
							 
							 SDLLoadGameSound(BinaryFile, Memory.SoundOne);
							 SDLLoadGameSound(BinaryFile, Memory.SoundTwo);

							 u32 TicksStart = SDL_GetTicks();
							 
							 // Mix_Chunk *chunk  = 0;
							 // Mix_Chunk *chunk2 = 0;
							 SDLLoadGameSound(BinaryFile, Memory.MusicOne);
							 // SDLLoadGameSound(BinaryFile, chunk);
							 // SDLLoadGameSound(BinaryFile, chunk2);

							 // Assert(chunk);

							 u32 TicksEnd = SDL_GetTicks();
							 u32 TimeElapsedMs = TicksEnd - TicksStart;
							 printf("Time Elapsed (ms) = %u\n",TimeElapsedMs);

							 // SDL_RWclose(BinaryFile);


							 
							 game_input Input = {};

							 Mix_PlayChannel(-1, Memory.SoundOne, 0);
							 Mix_PlayChannel(-1, Memory.SoundTwo, 0);
							 Mix_PlayChannel(-1, Memory.MusicOne, -1);

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
