/* asset_game.cpp --- 
 * 
 * Filename: asset_game.h
 * Author: Sierra
 * Created: Пн окт 16 10:08:17 2017 (+0300)
 * Last-Updated: Ср окт 18 17:42:02 2017 (+0300)
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

#if 0
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

		 asset_bitmap BitmapInfo = {};
		 BitmapInfo.Width         = Surface->w;
		 BitmapInfo.Height        = Surface->h;
		 BitmapInfo.Pitch         = Surface->pitch;
		 BitmapInfo.Rmask         = Surface->format->Rmask;
		 BitmapInfo.Gmask         = Surface->format->Gmask;
		 BitmapInfo.Bmask         = Surface->format->Bmask;
		 BitmapInfo.Amask         = Surface->format->Amask;
		 BitmapInfo.BytesPerPixel = Surface->format->BytesPerPixel;
		 BitmapInfo.BitsPerPixel  = Surface->format->BitsPerPixel;
		 
		 SDL_RWwrite(BinaryFile, &BitmapInfo,	sizeof(asset_memory_bitmap), 1);
		 
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
		 Assert(Sound);

		 Offset = Offset + ByteSize + sizeof(u64);
		 SDL_RWseek(BinaryFile, Offset, RW_SEEK_SET);

		 *ByteCount = Offset;
}


static void
SDLLoadGameBitmap(SDL_RWops *&BinaryFile, SDL_Renderer *&Renderer, game_texture *&GameBitmap,
									s64 *ByteCount)
{
		 SDL_Surface *TempSurface = 0;
		 asset_memory_bitmap BitmapInfo = {};

		 SDL_RWread(BinaryFile, &BitmapInfo, sizeof(asset_memory_bitmap), 1);

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
SDLLoadGameMusicFromMemory(void *&Memory, game_music *&Music, s64* ByteCount)
{
		 u64 *ByteSize = (u64*)Memory;
		 Memory = ((u8*) Memory) + sizeof(u64);
		 *ByteCount += sizeof(u64);
		 
		 SDL_RWops *rw = SDL_RWFromConstMem(Memory, *ByteSize);
		 Music  = Mix_LoadMUS_RW(rw, 0);
		 Assert(Music);
		 SDL_RWclose(rw);
		 
		 Memory = ((u8*) Memory) + *ByteSize;
		 *ByteCount += *ByteSize;
}

static void
SDLLoadGameSoundFromMemory(void *&Memory, game_sound *&Sound, s64* ByteCount)
{
		 u64 *ByteSize = (u64*)Memory;
		 Memory = ((u8*) Memory) + sizeof(u64);
		 *ByteCount += sizeof(u64);

		 u8* SoundBuffer = (u8*)Memory;
		 Sound = Mix_QuickLoad_WAV(SoundBuffer);
		 Assert(Sound);

		 Memory = ((u8*) Memory) + *ByteSize;
		 *ByteCount += *ByteSize;
}

static void
SDLLoadBitmapFromMemory(void *&Memory, game_texture *& Texture, s64 *ByteOffset,
												SDL_Renderer *&Renderer)
{
		 Assert(Memory);
		 SDL_Surface *TempSurface = 0;
		 asset_memory_bitmap *BitmapInfo = (asset_memory_bitmap*)Memory;

		 u64 BytePerSurface =
					BitmapInfo->BytesPerPixel * BitmapInfo->Width * BitmapInfo->Height;
		 
		 Memory = ((u8*) Memory) + sizeof(asset_memory_bitmap);
		 *ByteOffset += sizeof(asset_memory_bitmap);

		 TempSurface =
					SDL_CreateRGBSurfaceFrom(Memory, BitmapInfo->Width, BitmapInfo->Height, BitmapInfo->BitsPerPixel, BitmapInfo->Pitch,
																	 BitmapInfo->Rmask, BitmapInfo->Gmask, BitmapInfo->Bmask, BitmapInfo->Amask);
		 Assert(TempSurface);

		 Memory = ((u8*) Memory) + BytePerSurface;
		 *ByteOffset += BytePerSurface;

		 Texture = SDL_CreateTextureFromSurface(Renderer, TempSurface);
		 Assert(Texture);

		 SDL_FreeSurface(TempSurface);
}
#endif
static void
SDLReadEntireFile(char* FileName, void *&Memory)
{
		 SDL_RWops *BinaryFile = SDL_RWFromFile(FileName, "rb");
		 u64 ByteAmount = SDLSizeOfSDL_RWops(BinaryFile);

		 Memory = malloc(ByteAmount);
		 Assert(Memory);

		 SDL_RWread(BinaryFile, Memory, ByteAmount, 1);

		 SDL_RWclose(BinaryFile);
}

static u32
SDLGetBitmapHeader(asset_bitmap_header *BitmapHeader, SDL_Surface *&Surface)
{
		 BitmapHeader->Width   = Surface->w;
		 BitmapHeader->Height  = Surface->h;
		 BitmapHeader->Pitch   = Surface->pitch;
		 BitmapHeader->Rmask   = Surface->format->Rmask;
		 BitmapHeader->Gmask   = Surface->format->Gmask;
		 BitmapHeader->Bmask   = Surface->format->Bmask;
		 BitmapHeader->Amask   = Surface->format->Amask;
		 BitmapHeader->BytesPerPixel = Surface->format->BytesPerPixel;
		 BitmapHeader->BitsPerPixel  = Surface->format->BitsPerPixel;

		 u32 ByteAmount = BitmapHeader->Width *
					BitmapHeader->Height * BitmapHeader->BytesPerPixel;

		 return(ByteAmount);
}

static void
SDLWriteBitmapToFile(SDL_RWops *&BinaryFile, const char* FileName)
{
		 SDL_Surface *Surface = IMG_Load(FileName);
		 Assert(Surface);

		 asset_header AssetHeader  = {};
		 AssetHeader.AssetSize = SDLGetBitmapHeader(&AssetHeader.Bitmap.Header, Surface);
		 AssetHeader.AssetType = AssetType_Bitmap;

		 SDL_RWwrite(BinaryFile, &AssetHeader, sizeof(asset_header), 1);
		 SDL_RWwrite(BinaryFile, Surface->pixels, AssetHeader.AssetSize, 1);

		 SDL_FreeSurface(Surface);
}

static void
SDLWriteSoundToFile(SDL_RWops *&BinaryFile, char *FileName)
{
		 // TODO(max): try to read Mix_Chunk instead
		 SDL_RWops *SoundFile = SDL_RWFromFile(FileName, "rb");
		 Assert(SoundFile);

		 asset_header AssetHeader = {};
		 AssetHeader.AssetSize = SDLSizeOfSDL_RWops(SoundFile);
		 AssetHeader.AssetType = AssetType_Sound;
		 AssetHeader.Audio.Header.IsMusic = false;
		 
		 void *Memory = malloc(AssetHeader.AssetSize);
		 Assert(Memory);

		 SDL_RWread(SoundFile, Memory, AssetHeader.AssetSize, 1);
		 
		 SDL_RWwrite(BinaryFile, &AssetHeader, sizeof(asset_header), 1);
		 SDL_RWwrite(BinaryFile, Memory, AssetHeader.AssetSize, 1);

		 free(Memory);
		 SDL_RWclose(SoundFile);
}

static void
SDLWriteMusicToFile(SDL_RWops *&BinaryFile, char *FileName)
{
		 // TODO(max): try to read Mix_Music instead
		 SDL_RWops *SoundFile = SDL_RWFromFile(FileName, "rb");
		 Assert(SoundFile);

		 asset_header AssetHeader = {};
		 AssetHeader.AssetSize = SDLSizeOfSDL_RWops(SoundFile);
		 AssetHeader.AssetType = AssetType_Sound;
		 AssetHeader.Audio.Header.IsMusic = true;
		 
		 void *Memory = malloc(AssetHeader.AssetSize);
		 Assert(Memory);

		 SDL_RWread(SoundFile, Memory, AssetHeader.AssetSize, 1);
		 
		 SDL_RWwrite(BinaryFile, &AssetHeader, sizeof(asset_header), 1);
		 SDL_RWwrite(BinaryFile, Memory, AssetHeader.AssetSize, 1);

		 free(Memory);
		 SDL_RWclose(SoundFile);
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

		 // SDLLoadBitmapFromMemory(Memory, GameMemory->GridCell, ByteOffset, Renderer);
		 // SDLLoadBitmapFromMemory(Memory, GameMemory->SpriteI_D, ByteOffset, Renderer);
		 
		 // SDLLoadGameSoundFromMemory(Memory, GameMemory->SoundOne, ByteOffset);
		 // SDLLoadGameMusicFromMemory(Memory, GameMemory->MusicOne, ByteOffset);
		 
		 ThreadData->IsInitialized = true;
		 printf("%d total bytes read\n", *ByteOffset);

		 // free(Memory);
		 return (1);
}

static void
SDLAssetBuildBinaryFile()
{
		 SDL_RWops *BinaryFile = SDL_RWFromFile("package.bin", "wb");
		 
		 SDLWriteBitmapToFile(BinaryFile, grid_cell);
		 SDLWriteBitmapToFile(BinaryFile, SpriteI_D);
		 // SDLWriteGameBitmapToFile(BinaryFile, grid_cell);
		 // SDLWriteGameBitmapToFile(BinaryFile, SpriteI_D);

		 // SDLWriteGameSoundToFile(BinaryFile, focus);
		 // SDLWriteGameSoundToFile(BinaryFile, cannon_fire);
		 // SDLWriteGameSoundToFile(BinaryFile, amb_ending_water);

		 SDL_RWclose(BinaryFile);

}

