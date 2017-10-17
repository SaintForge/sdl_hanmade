/* asset_game.cpp --- 
 * 
 * Filename: asset_game.h
 * Author: Sierra
 * Created: Пн окт 16 10:08:17 2017 (+0300)
 * Last-Updated: Вт окт 17 17:54:19 2017 (+0300)
 *           By: Sierra
 */

#include "asset_game.h"

static u64
SDLSizeOfSDL_RWops(SDL_RWops *&BinaryFile)
{
		 u32 DefaultOffset = SDL_RWtell(BinaryFile);
		 SDL_RWseek(BinaryFile, 0, RW_SEEK_END);
		 
		 u32 ByteSize = SDL_RWtell(BinaryFile);
		 SDL_RWseek(BinaryFile, DefaultOffset, RW_SEEK_SET);

		 return (ByteSize);
}

static u64
SDLSizeOfBinaryFile(char *FileName)
{
		 SDL_RWops *File = SDL_RWFromFile(FileName, "rb");
		 u64 ByteSize = SDLSizeOfSDL_RWops(File);

		 return (ByteSize);
}

static void
SDLWriteGameSoundToFile(SDL_RWops *&BinaryTarget, const char* SourceFile)
{
		 u64 ByteSize  = 0;
		 void *Samples = 0;
		 SDL_RWops *BinarySource = 0;

		 BinarySource = SDL_RWFromFile(SourceFile, "rb");
		 Assert(BinarySource);
		 
		 ByteSize = SDLSizeOfSDL_RWops(BinarySource);
		 // printf("ByteSize = %lu\n",ByteSize);

		 Samples = malloc(ByteSize);
		 Assert(Samples);
		 SDL_RWread(BinarySource, Samples, ByteSize, 1);
		 SDL_RWclose(BinarySource);

		 SDL_RWwrite(BinaryTarget, &ByteSize, sizeof(u64), 1);
		 SDL_RWwrite(BinaryTarget, Samples, ByteSize, 1);
		 free(Samples);
}

static void
SDLWriteGameBitmapToFile(SDL_RWops *&BinaryFile, const char * FileName)
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
		 
		 SDL_RWwrite(BinaryFile, &BitmapInfo,	sizeof(sdl_memory_bitmap), 1);
		 
		 SDL_RWwrite(BinaryFile, Surface->pixels,
								 BitmapInfo.BytesPerPixel * BitmapInfo.Width * BitmapInfo.Height, 1);

		 SDL_FreeSurface(Surface);
}

static void
SDLLoadGameSound(SDL_RWops *&BinaryFile, game_sound *&Sound, s64 *ByteCount)
{
		 // NOTE(Max): For some reason if you load big amount of bytes,
		 // Mix_LoadWAV_RW function sets your SDL_RWops cursor to zero

		 u64 Offset = SDL_RWtell(BinaryFile);
		 u64 ByteSize = 0;
		 SDL_RWread(BinaryFile, &ByteSize, sizeof(u64), 1);
		 
		 Sound = Mix_LoadWAV_RW(BinaryFile, 0);

		 Offset = Offset + ByteSize + sizeof(u64);
		 SDL_RWseek(BinaryFile, Offset, RW_SEEK_SET);

		 *ByteCount = Offset;
}

#if 0
static void
SDLLoadGameBitmap(SDL_RWops *&BinaryFile, SDL_Renderer *&Renderer, game_texture *&GameBitmap,
									s64 *ByteCount)
{
		 SDL_Surface *TempSurface = 0;
		 sdl_memory_bitmap BitmapInfo = {};

		 SDL_RWread(BinaryFile, &BitmapInfo, sizeof(sdl_memory_bitmap), 1);

		 void *Pixels = malloc(BitmapInfo.BytesPerPixel * BitmapInfo.Width * BitmapInfo.Height);
		 Assert(Pixels);

		 SDL_RWread(BinaryFile, Pixels,
								BitmapInfo.BytesPerPixel * BitmapInfo.Width * BitmapInfo.Height, 1);

		 TempSurface =
					SDL_CreateRGBSurfaceFrom(Pixels, BitmapInfo.Width, BitmapInfo.Height, BitmapInfo.BitsPerPixel, BitmapInfo.Pitch,
																	 BitmapInfo.Rmask, BitmapInfo.Gmask, BitmapInfo.Bmask, BitmapInfo.Amask);
		 Assert(TempSurface);

		 GameBitmap = SDL_CreateTextureFromSurface(Renderer, TempSurface);
		 Assert(GameBitmap);

		 free(Pixels);
		 SDL_FreeSurface(TempSurface);

		 *ByteCount = SDL_RWtell(BinaryFile);
}

#endif

static void
SDLLoadGameSoundFromMemory(void *&Memory, game_sound *&Sound, s64* ByteCount)
{
		 u64 *ByteSize = (u64*)Memory;
		 printf("ByteSize = %d\n", *ByteSize);
		 Memory += sizeof(u64);
		 ByteCount += sizeof(u64);

		 u8* SoundBuffer = (u8*)Memory;
		 Sound = Mix_QuickLoad_RAW(SoundBuffer, *ByteSize);
		 Assert(Sound);

		 Memory += *ByteSize;
		 *ByteCount += *ByteSize;
}

static void
SDLLoadBitmapFromMemory(void *&Memory, game_texture *& Texture, s64 *ByteOffset,
												SDL_Renderer *&Renderer)
{
		 Assert(Memory);
		 SDL_Surface *TempSurface = 0;
		 sdl_memory_bitmap *BitmapInfo = (sdl_memory_bitmap*)Memory;

		 u64 BytePerSurface =
					BitmapInfo->BytesPerPixel * BitmapInfo->Width * BitmapInfo->Height;
		 
		 Memory += sizeof(sdl_memory_bitmap);
		 *ByteOffset += sizeof(sdl_memory_bitmap);

		 TempSurface =
					SDL_CreateRGBSurfaceFrom(Memory, BitmapInfo->Width, BitmapInfo->Height, BitmapInfo->BitsPerPixel, BitmapInfo->Pitch,
																	 BitmapInfo->Rmask, BitmapInfo->Gmask, BitmapInfo->Bmask, BitmapInfo->Amask);
		 Assert(TempSurface);

		 Memory += BytePerSurface;
		 *ByteOffset += BytePerSurface;

		 Texture = SDL_CreateTextureFromSurface(Renderer, TempSurface);
		 Assert(Texture);

		 SDL_FreeSurface(TempSurface);
}

static void
SDLReadEntireFile(char* FileName, void *&Memory)
{
		 SDL_RWops *BinaryFile = SDL_RWFromFile(FileName, "rb");
		 u64 ByteAmount = SDLSizeOfSDL_RWops(BinaryFile);
		 // printf("ByteAmount = %d\n", ByteAmount);

		 Memory = malloc(ByteAmount);
		 Assert(Memory);

		 SDL_RWread(BinaryFile, Memory, ByteAmount, 1);

		 SDL_RWclose(BinaryFile);
}


static void
SDLAssetBuildBinaryFile()
{
		 SDL_RWops *BinaryFile = SDL_RWFromFile("package.bin", "wb");

		 SDLWriteGameBitmapToFile(BinaryFile, grid_cell);
		 SDLWriteGameBitmapToFile(BinaryFile, SpriteI_D);

		 SDLWriteGameSoundToFile(BinaryFile, focus);
		 // SDLWriteGameSoundToFile(BinaryFile, cannon_fire);
		 SDLWriteGameSoundToFile(BinaryFile, amb_ending_water);
		 
		 SDL_RWclose(BinaryFile);
}

static int
SDLAssetLoadBinaryFile(void *Data)
{
		 thread_data  *ThreadData = (thread_data*)Data;
		 
		 void *Memory = 0;
		 SDLReadEntireFile("package.bin", Memory);
		 
		 s64 *ByteOffset          = &ThreadData->ByteAmount;
		 SDL_Renderer *Renderer   = ThreadData->Renderer;
		 game_memory  *GameMemory = ThreadData->Memory;

		 SDLLoadBitmapFromMemory(Memory, GameMemory->GridCell, ByteOffset, Renderer);
		 SDLLoadBitmapFromMemory(Memory, GameMemory->SpriteI_D, ByteOffset, Renderer);
		 
		 // SDLLoadGameBitmap(BinaryFile, Renderer, Memory->GridCell, &ThreadData->ByteAmount);
		 // SDLLoadGameBitmap(BinaryFile, Renderer, Memory->SpriteI_D, &ThreadData->ByteAmount);

		 SDLLoadGameSoundFromMemory(Memory, GameMemory->SoundOne, ByteOffset);
		 SDLLoadGameSoundFromMemory(Memory, GameMemory->MusicOne, ByteOffset);
		 // SDLLoadGameSound(BinaryFile, Memory->SoundTwo, &ThreadData->ByteAmount);
		 // SDLLoadGameSound(BinaryFile, Memory->MusicOne, &ThreadData->ByteAmount);

		 // SDL_RWclose(BinaryFile);

		 ThreadData->IsInitialized = true;
		 return (1);
}
