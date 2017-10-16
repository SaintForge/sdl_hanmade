/* asset_game.cpp --- 
 * 
 * Filename: asset_game.h
 * Author: Sierra
 * Created: Пн окт 16 10:08:17 2017 (+0300)
 * Last-Updated: Пн окт 16 16:43:35 2017 (+0300)
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
		 printf("ByteSize = %d\n",ByteSize);

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
		 
		 printf("Wrote bitmap_info: %d\n",SDL_RWwrite(BinaryFile, &BitmapInfo,	sizeof(sdl_memory_bitmap), 1));
		 
		 printf("Wrote bitmap: %d\n",SDL_RWwrite(BinaryFile, Surface->pixels, BitmapInfo.BytesPerPixel * BitmapInfo.Width * BitmapInfo.Height, 1));

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

static void
SDLLoadGameBitmap(SDL_RWops *&BinaryFile, SDL_Renderer *&Renderer, game_texture *&GameBitmap,
		 s64 *ByteCount)
{
		 SDL_Surface *TempSurface = 0;
		 sdl_memory_bitmap BitmapInfo = {};

		 u64 ByteOffset = SDL_RWtell(BinaryFile);

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

static void
SDLAssetBuildBinaryFile()
{
		 SDL_RWops *BinaryFile = SDL_RWFromFile("package.bin", "wb");
							 
		 SDLWriteGameBitmapToFile(BinaryFile, grid_cell);
		 SDLWriteGameBitmapToFile(BinaryFile, SpriteO_D);

		 SDLWriteGameSoundToFile(BinaryFile, focus);
		 SDLWriteGameSoundToFile(BinaryFile, cannon_fire);
		 SDLWriteGameSoundToFile(BinaryFile, amb_ending_water);
		 
		 SDL_RWclose(BinaryFile);
}

static int
SDLAssetLoadBinaryFile(void *Data)
{
		 printf("loading assets...\n");
		 SDL_RWops *BinaryFile = SDL_RWFromFile("package.bin", "rb");

		 thread_data *ThreadData = (thread_data*)Data;

		 SDL_Renderer *Renderer = ThreadData->Renderer;
		 game_memory *Memory    = ThreadData->Memory;
		 s64 *ByteOffset        = &ThreadData->ByteAmount;

		 SDLLoadGameBitmap(BinaryFile, Renderer, Memory->GridCell, &ThreadData->ByteAmount);
		 SDLLoadGameBitmap(BinaryFile, Renderer, Memory->SpriteO_D, &ThreadData->ByteAmount);

		 SDLLoadGameSound(BinaryFile, Memory->SoundOne, &ThreadData->ByteAmount);
		 SDLLoadGameSound(BinaryFile, Memory->SoundTwo, &ThreadData->ByteAmount);
		 SDLLoadGameSound(BinaryFile, Memory->MusicOne, &ThreadData->ByteAmount);

		 SDL_RWclose(BinaryFile);

		 ThreadData->IsInitialized = true;
		 printf("loaded assets\n");

		 return (1);
}
