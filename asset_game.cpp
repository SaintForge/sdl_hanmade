/* asset_game.cpp --- 
 * 
 * Filename: asset_game.h
 * Author: Sierra
 * Created: Пн окт 16 10:08:17 2017 (+0300)
 * Last-Updated: Пт окт 27 10:15:58 2017 (+0300)
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
    return(ByteSize);
}

static u64
SDLSizeOfBinaryFile(char *FileName)
{
    SDL_RWops *File = SDL_RWFromFile(FileName, "rb");
    u64 ByteSize = SDLSizeOfSDL_RWops(File);
    return(ByteSize);
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
        SDL_CreateRGBSurfaceFrom(Memory, BitmapInfo->Width, BitmapInfo->Height,
                                 BitmapInfo->BitsPerPixel, BitmapInfo->Pitch,
                                 BitmapInfo->Rmask, BitmapInfo->Gmask,
                                 BitmapInfo->Bmask, BitmapInfo->Amask);
    Assert(TempSurface);
    
    Memory = ((u8*) Memory) + BytePerSurface;
    *ByteOffset += BytePerSurface;
    
    Texture = SDL_CreateTextureFromSurface(Renderer, TempSurface);
    Assert(Texture);
    
    SDL_FreeSurface(TempSurface);
}
#endif


static void
SDLReadEntireFile(char* FileName, game_memory *&Memory)
{
    SDL_RWops *BinaryFile = SDL_RWFromFile(FileName, "rb");
    Memory->AssetsSpace = SDLSizeOfSDL_RWops(BinaryFile);
    // printf("filesize = %llu\n", Memory->AssetsSpace);
    
    Memory->Assets = malloc(Memory->AssetsSpace);
    Assert(Memory->Assets);
    
    SDL_RWread(BinaryFile, Memory->Assets, Memory->AssetsSpace, 1);
    SDL_RWclose(BinaryFile);
}


static void
SDLWriteBitmapToFile(SDL_RWops *&BinaryFile, const char* FileName)
{
    char FullName[128];
    strcpy(FullName, SpritePath);
    strcat(FullName, FileName);
    
    SDL_Surface *Surface = IMG_Load(FullName);
    Assert(Surface);
    
    asset_bitmap_header BitmapHeader;
    BitmapHeader.Width   = Surface->w;
    BitmapHeader.Height  = Surface->h;
    BitmapHeader.Pitch   = Surface->pitch;
    BitmapHeader.Rmask   = Surface->format->Rmask;
    BitmapHeader.Gmask   = Surface->format->Gmask;
    BitmapHeader.Bmask   = Surface->format->Bmask;
    BitmapHeader.Amask   = Surface->format->Amask;
    BitmapHeader.BytesPerPixel = Surface->format->BytesPerPixel;
    BitmapHeader.BitsPerPixel  = Surface->format->BitsPerPixel;
    
    u32 ByteAmount = BitmapHeader.Width * BitmapHeader.Height * BitmapHeader.BytesPerPixel;
    
    asset_header AssetHeader = {};
    AssetHeader.AssetSize = ByteAmount;
    AssetHeader.AssetType = AssetType_Bitmap;
    strcpy(AssetHeader.AssetName, FileName);
    
    AssetHeader.Bitmap.Data = 0;
    AssetHeader.Bitmap.Header = BitmapHeader;
    // printf("AssetSize = %u\n", AssetHeader.AssetSize);
    
    SDL_RWwrite(BinaryFile, &AssetHeader, sizeof(asset_header), 1);
    SDL_RWwrite(BinaryFile, Surface->pixels, AssetHeader.AssetSize, 1);
    
    SDL_FreeSurface(Surface);
}

static void
SDLWriteSoundToFile(SDL_RWops *&BinaryFile, const char *FileName)
{
    char FullName[128];
    strcpy(FullName, SoundPath);
    strcat(FullName, FileName);
    
    SDL_RWops *SoundFile = SDL_RWFromFile(FullName, "rb");
    Assert(SoundFile);
    
    asset_header AssetHeader;
    AssetHeader.AssetSize = SDLSizeOfSDL_RWops(SoundFile);
    AssetHeader.AssetType = AssetType_Sound;
    strcpy(AssetHeader.AssetName, FileName);
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
SDLWriteMusicToFile(SDL_RWops *&BinaryFile, const char *FileName)
{
    char FullName[128];
    strcpy(FullName, SoundPath);
    strcat(FullName, FileName);
    
    SDL_RWops *MusicFile = SDL_RWFromFile(FullName, "rb");
    Assert(MusicFile);
    
    asset_header AssetHeader = {};
    AssetHeader.AssetSize = SDLSizeOfSDL_RWops(MusicFile);
    AssetHeader.AssetType = AssetType_Music;
    strcpy(AssetHeader.AssetName, FileName);
    AssetHeader.Audio.Header.IsMusic = true;
    
    void *Memory = malloc(AssetHeader.AssetSize);
    Assert(Memory);
    
    SDL_RWread(MusicFile, Memory, AssetHeader.AssetSize, 1);
    
    SDL_RWwrite(BinaryFile, &AssetHeader, sizeof(asset_header), 1);
    SDL_RWwrite(BinaryFile, Memory, AssetHeader.AssetSize, 1);
    
    free(Memory);
    SDL_RWclose(MusicFile);
}

static bool
IsAsset(asset_header*& AssetHeader, asset_type AssetType, char* AssetName)
{
    bool Result = false;
    
    if(AssetHeader->AssetType == AssetType)
    {
        if(strcmp(AssetHeader->AssetName, AssetName) == 0)
        {
            Result = true;
        }
    }
    
    return(Result);
}

static asset_header*
GetAssetHeader(game_memory *&Memory, asset_type AssetType, char* AssetName, u32 Offset)
{
    u8 *mem = (u8*)Memory->Assets + Offset;
    asset_header *AssetHeader = (asset_header*)mem;
    u32 TotalByteSize = 0;
    
    while(TotalByteSize < Memory->AssetsSpace)
    {
        if(IsAsset(AssetHeader, AssetType, AssetName))
        {
            return AssetHeader;
        }
        else
        {
            TotalByteSize += (sizeof(asset_header) + AssetHeader->AssetSize);
            AssetHeader = ((asset_header*)(((u8*)AssetHeader) + sizeof(asset_header) + AssetHeader->AssetSize));
        }
    }
    
    return(NULL);
}

static game_music*
GetMusic(game_memory *Memory, char* FileName)
{
    game_music *Music = NULL;
    
    binary_header *BinaryHeader = (binary_header*)Memory->Assets;
    u32 ByteOffset = sizeof(binary_header) + BinaryHeader->AudioSizeInBytes;
    
    asset_header *AssetHeader = GetAssetHeader(Memory, AssetType_Music, FileName, ByteOffset);
    if(AssetHeader)
    {
        asset_audio *Audio = &AssetHeader->Audio;
        asset_audio_header *Header = &Audio->Header;
        
        Audio->Data = (u8*)AssetHeader;
        Audio->Data = Audio->Data + sizeof(asset_header);
        
        void *MusicData = ((void*)Audio->Data);
        SDL_RWops *RWMusic = SDL_RWFromConstMem(MusicData, AssetHeader->AssetSize);
        Music = Mix_LoadMUS_RW(RWMusic, 1);
        Assert(Music);
    }
    
    return(Music);
}

static game_sound*
GetSound(game_memory *Memory, char* FileName)
{
    game_sound *Sound = NULL;
    
    binary_header *BinaryHeader = (binary_header*)Memory->Assets;
    u32 ByteOffset = sizeof(binary_header) + BinaryHeader->AudioSizeInBytes;
    
    asset_header *AssetHeader = GetAssetHeader(Memory, AssetType_Sound, FileName, ByteOffset);
    if(AssetHeader)
    {
        asset_audio *Audio = &AssetHeader->Audio;
        asset_audio_header *Header = &Audio->Header;
        
        Audio->Data = (u8*)AssetHeader;
        Audio->Data = Audio->Data + sizeof(asset_header);
        
        Sound = Mix_QuickLoad_WAV(Audio->Data);
        Assert(Sound);
    }
    
    return(Sound);
}

static game_texture*
GetTexture(game_memory *&Memory, char* FileName, SDL_Renderer *&Renderer)
{
    game_texture *Texture = NULL;
    
    binary_header *BinaryHeader = (binary_header*)Memory->Assets;
    u32 ByteOffset = sizeof(binary_header) + BinaryHeader->BitmapSizeInBytes;
    
    asset_header *AssetHeader = GetAssetHeader(Memory, AssetType_Bitmap, FileName, ByteOffset);
    if(AssetHeader)
    {
        asset_bitmap *Bitmap = &AssetHeader->Bitmap;
        asset_bitmap_header *Header = &Bitmap->Header;
        
        Bitmap->Data = (void*)AssetHeader;
        Bitmap->Data = ((u8*)Bitmap->Data) + sizeof(asset_header);
        game_surface *Surface =
            SDL_CreateRGBSurfaceFrom(Bitmap->Data,
                                     Header->Width, Header->Height,
                                     Header->BitsPerPixel, Header->Pitch,
                                     Header->Rmask, Header->Gmask,
                                     Header->Bmask, Header->Amask);
        Assert(Surface);
        
        Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
        Assert(Texture);
    }
    
    return(Texture);
}

static int
SDLAssetLoadBinaryFile(void *Data)
{
    game_memory *Memory = ((game_memory*)Data);
    SDLReadEntireFile("package1.bin", Memory);
    
    Memory->AssetsInitialized = true;
    
    return(1);
}

static void
SDLAssetBuildBinaryFile()
{
    SDL_RWops *BinaryFile = SDL_RWFromFile("package1.bin", "wb");
    
    binary_header BinaryHeader = {};
    SDL_RWwrite(BinaryFile, &BinaryHeader, sizeof(binary_header), 1);
    
    /* Bitmap loading */
    BinaryHeader.BitmapSizeInBytes = 0;
    
    SDLWriteBitmapToFile(BinaryFile, "grid_cell.png");
    SDLWriteBitmapToFile(BinaryFile, "grid_cell_1.png");
    SDLWriteBitmapToFile(BinaryFile, "grid_cell_2.png");
    
    SDLWriteBitmapToFile(BinaryFile, "i_d.png");
    SDLWriteBitmapToFile(BinaryFile, "i_m.png");
    SDLWriteBitmapToFile(BinaryFile, "i_s.png");
    
    SDLWriteBitmapToFile(BinaryFile, "o_d.png");
    SDLWriteBitmapToFile(BinaryFile, "o_m.png");
    SDLWriteBitmapToFile(BinaryFile, "o_s.png");
    
    SDLWriteBitmapToFile(BinaryFile, "l_d.png");
    SDLWriteBitmapToFile(BinaryFile, "l_m.png");
    SDLWriteBitmapToFile(BinaryFile, "l_s.png");
    
    SDLWriteBitmapToFile(BinaryFile, "j_d.png");
    SDLWriteBitmapToFile(BinaryFile, "j_m.png");
    SDLWriteBitmapToFile(BinaryFile, "j_s.png");
    
    SDLWriteBitmapToFile(BinaryFile, "s_d.png");
    SDLWriteBitmapToFile(BinaryFile, "s_m.png");
    SDLWriteBitmapToFile(BinaryFile, "s_s.png");
    
    SDLWriteBitmapToFile(BinaryFile, "z_d.png");
    SDLWriteBitmapToFile(BinaryFile, "z_m.png");
    SDLWriteBitmapToFile(BinaryFile, "z_s.png");
    
    SDLWriteBitmapToFile(BinaryFile, "t_d.png");
    SDLWriteBitmapToFile(BinaryFile, "t_m.png");
    SDLWriteBitmapToFile(BinaryFile, "t_s.png");
    
    /* Audio loading */
    BinaryHeader.AudioSizeInBytes = SDL_RWtell(BinaryFile) - sizeof(binary_header);
    
    SDLWriteSoundToFile(BinaryFile, "focus.wav");
    SDLWriteMusicToFile(BinaryFile, "amb_ending_water.ogg");
    
    SDL_RWseek(BinaryFile, 0, RW_SEEK_SET);
    
    SDL_RWwrite(BinaryFile, &BinaryHeader, sizeof(binary_header), 1);
    
    SDL_RWclose(BinaryFile);
}

