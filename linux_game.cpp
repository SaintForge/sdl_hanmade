// linux_platform.cpp ---
// 
// Filename: linux_platform.cpp
// Author: Sierra
// Created: Пн окт  9 12:00:49 2017 (+0300)
// Last-Updated: Чт окт 19 17:01:42 2017 (+0300)
//           By: Sierra
//

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <stdint.h>
#include <string>

typedef SDL_Rect    game_rect;
typedef SDL_Point   game_point;
typedef SDL_Texture game_texture;
typedef SDL_Surface game_surface;
typedef Mix_Chunk   game_sound;
typedef Mix_Music   game_music;

typedef int8_t   s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#include "linux_game.h"

static const char* SpritePath = "../data/sprites/";
static const char* SoundPath  = "../data/sound/";

#include "game.cpp"
#include "asset_game.cpp"

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
SDLCreateBufferTexture(sdl_offscreen_buffer *Buffer, SDL_Renderer* Renderer, int Width, int Height)
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
							 printf("Failed to create SDL_Texture!%s\n", SDL_GetError());
					}
		 }
		 else
		 {
					printf("Failed to malloc Buffer->Memory!\n");
		 }
}

static SDL_Texture*
SDLUploadTexture(SDL_Renderer *&Renderer, game_rect *Quad, char* FileName)
{
		 SDL_Surface *Surface = IMG_Load(FileName);
		 Quad->w = Surface->w;
		 Quad->h = Surface->h;
		 SDL_Texture *Texture = SDL_CreateTextureFromSurface(Renderer, Surface);

		 return (Texture);
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
SDLReloadFontTexture(TTF_Font *&Font, SDL_Texture *&Texture, SDL_Rect *Quad,
										 char* Text, SDL_Renderer*& Renderer)
{
		 SDL_Surface *Surface = 0;
		 
		 if(Texture)
		 {
					SDL_DestroyTexture(Texture);
		 }

		 Surface = TTF_RenderUTF8_Blended(Font, Text, {255,255,255});
		 Quad->w = Surface->w;
		 Quad->h = Surface->h;

		 Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
		 Assert(Texture);
		 SDL_FreeSurface(Surface);
}

int main(int argc, char **argv)
{
		 SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
		 
		 SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
		 Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
		 TTF_Init();

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
							 SDLCreateBufferTexture(&BackBuffer, Renderer, Dimension.Width, Dimension.Height);
							 SDLChangeBufferColor(&BackBuffer, 0, 0, 0, 255);
							 SDLUpdateWindow(Window, Renderer, &BackBuffer);

#if ASSET_BUILD
							 // NOTE: This is for packaging data to the disk
							 SDLAssetBuildBinaryFile();
							 printf("builded!\n");
#endif
							 game_memory Memory = {};

							 u64 TotalByteAmount = SDLSizeOfBinaryFile("package.bin");
							 printf("should read %d bytes\n", TotalByteAmount);

							 thread_data ThreadData = {};
							 ThreadData.Renderer      = Renderer;
							 ThreadData.Memory        = &Memory;
							 ThreadData.ByteAmount    = 0;
							 ThreadData.IsInitialized = false;
							 
							 SDL_Thread *AssetThread = SDL_CreateThread(SDLAssetLoadBinaryFile, "LoadingThread",
																													(void*)&ThreadData);
							 
							 game_rect LoadingBarQuad = {};
							 SDL_Texture *LoadingBarTexture =	SDLUploadTexture(Renderer, &LoadingBarQuad, "../data/sprites/button.png");

							 LoadingBarQuad.h = 10;
							 LoadingBarQuad.w = 0;
							 LoadingBarQuad.x = 0;
							 LoadingBarQuad.y = (Dimension.Height / 2) + (LoadingBarQuad.h / 2);

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

										if(ThreadData.IsInitialized)
										{
												 if(GameUpdateAndRender(&Memory, &Input, &Buffer))
												 {
															IsRunning = false;
												 }
										}

										LoadingBarQuad.w = (ThreadData.ByteAmount * Dimension.Width) / TotalByteAmount;
										
										// draw loading screen
										GameRenderBitmapToBuffer(&Buffer, LoadingBarTexture, &LoadingBarQuad);
										SDLUpdateWindow(Window, Renderer, &BackBuffer);
							 }

					}
					else
					{
							 printf("Failed to create SDL_Renderer! %s\n", SDL_GetError());
					}

		 }
		 else
		 {
					printf("Failed to create SDL_Window!\n", SDL_GetError());
		 }


		 return 0;
}
