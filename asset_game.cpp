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
    if(!BinaryFile) return 0;
    
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
SDLReadEntireAssetFile(char* FileName, game_memory *&Memory)
{
    SDL_RWops *BinaryFile = SDL_RWFromFile(FileName, "rb");
    Memory->AssetsSpaceAmount = SDLSizeOfSDL_RWops(BinaryFile);
    // printf("filesize = %llu\n", Memory->AssetsSpace);
    
    Memory->Assets = malloc(Memory->AssetsSpaceAmount);
    Assert(Memory->Assets);
    
    SDL_RWread(BinaryFile, Memory->Assets, Memory->AssetsSpaceAmount, 1);
    SDL_RWclose(BinaryFile);
}

static void
SDLReadEntireLevelFile(char* FileName, game_memory *&Memory)
{
    
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
    
    while(TotalByteSize < Memory->AssetsSpaceAmount)
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
    SDLReadEntireAssetFile("package1.bin", Memory);
    
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

static void
ConvertLevelMemoryFromRaw(game_memory *Memory, void *RawMemory, u32 RawMemorySpace)
{
    
    u32 TotalBytesRead = 0;
    level_memory *LevelMemory = NULL;
    
    while(TotalBytesRead < RawMemorySpace)
    {
        u32 Index = Memory->LevelMemoryAmount;
        u32 BytesToSkip = 0;
        
        LevelMemory = (level_memory*)RawMemory;
        Memory->LevelMemory[Index].LevelNumber        = LevelMemory->LevelNumber;
        Memory->LevelMemory[Index].RowAmount          = LevelMemory->RowAmount;
        Memory->LevelMemory[Index].ColumnAmount       = LevelMemory->ColumnAmount;
        Memory->LevelMemory[Index].MovingBlocksAmount = LevelMemory->MovingBlocksAmount;
        Memory->LevelMemory[Index].FigureAmount       = LevelMemory->FigureAmount;
        
        BytesToSkip += sizeof(level_memory);
        
        printf("LevelMemory->RowAmount = %d\n", LevelMemory->RowAmount);
        
        if(LevelMemory->RowAmount > 0 && LevelMemory->ColumnAmount > 0)
        {
            s32 **UnitField = ((s32**)(((u8*)LevelMemory) + BytesToSkip));
            
            Memory->LevelMemory[Index].UnitField = (s32 **)malloc(LevelMemory->RowAmount * sizeof(s32*));
            Assert(Memory->LevelMemory[Index].UnitField);
            for(u32 i = 0; i < LevelMemory->RowAmount; ++i)
            {
                Memory->LevelMemory[Index].UnitField[i] = (s32*)malloc(sizeof(s32) * LevelMemory->ColumnAmount);
                Assert(Memory->LevelMemory[Index].UnitField[i]);
                for(u32 j = 0; j < LevelMemory->ColumnAmount; ++j)
                {
                    Memory->LevelMemory[Index].UnitField[i][j] = UnitField[i][j];
                }
            }
            
            BytesToSkip += LevelMemory->RowAmount * LevelMemory->ColumnAmount * sizeof(s32);
            }
        
        if(LevelMemory->MovingBlocksAmount > 0)
            {
                moving_block_memory *MovingBlocks = ((moving_block_memory*)(((u8*)LevelMemory) + BytesToSkip));
                
                Memory->LevelMemory[Index].MovingBlocks = (moving_block_memory*) malloc(sizeof(moving_block_memory) * LevelMemory->MovingBlocksAmount);
                Assert(Memory->LevelMemory[Index].MovingBlocks);
                
                for(u32 i = 0; i < LevelMemory->MovingBlocksAmount; ++i)
                {
                    Memory->LevelMemory[Index].MovingBlocks[i].RowNumber = MovingBlocks[i].RowNumber;
                    Memory->LevelMemory[Index].MovingBlocks[i].ColNumber = MovingBlocks[i].ColNumber;
                    Memory->LevelMemory[Index].MovingBlocks[i].IsVertical = MovingBlocks[i].IsVertical;
                    Memory->LevelMemory[Index].MovingBlocks[i].MoveSwitch = MovingBlocks[i].MoveSwitch;
                }
                
                BytesToSkip += LevelMemory->MovingBlocksAmount * sizeof(moving_block_memory);
                }
                
                if(LevelMemory->FigureAmount > 0)
                {
                     figure_memory *FigureMemory = ((figure_memory*)(((u8*)LevelMemory) + BytesToSkip));
                    
                    Memory->LevelMemory[Index].Figures = (figure_memory*) malloc(sizeof(figure_memory) * LevelMemory->FigureAmount);
                    Assert(Memory->LevelMemory[Index].Figures);
                    
                    for(u32 i = 0; i < LevelMemory->FigureAmount; ++i)
                    {
                        Memory->LevelMemory[Index].Figures[i].Angle = FigureMemory->Angle;
                        Memory->LevelMemory[Index].Figures[i].Flip  = FigureMemory->Flip;
                        Memory->LevelMemory[Index].Figures[i].Form  = FigureMemory->Form;
                        Memory->LevelMemory[Index].Figures[i].Type  = FigureMemory->Type;
                    }
                    
                    BytesToSkip += LevelMemory->FigureAmount * sizeof(figure_memory);
                }
                
                Memory->LevelMemoryAmount += 1;
                TotalBytesRead += BytesToSkip;
                LevelMemory = ((level_memory*)(((u8*)LevelMemory) + BytesToSkip));
        }
}

static void
LoadLevelMemoryFromFile(game_memory *Memory)
{
    Memory->LevelMemoryAmount = 0;
    Memory->LevelMemoryReserved = 100;
    Memory->LevelMemory = (level_memory *)calloc(sizeof(level_memory), Memory->LevelMemoryReserved);
    Assert(Memory->LevelMemory);
    
    SDL_RWops *BinaryFile = SDL_RWFromFile("package2.bin", "rb");
    u32 ByteAmount = SDLSizeOfSDL_RWops(BinaryFile);
    if(!(BinaryFile) || ByteAmount == 0)
    {
        printf("Failed to find package2.bin or it is empty!\n");
        }
        else
    {
        u32 RawMemorySpace = SDLSizeOfSDL_RWops(BinaryFile);
        void *RawMemory = malloc(RawMemorySpace);
        Assert(RawMemory);
        
        SDL_RWwrite(BinaryFile, RawMemory, RawMemorySpace, 1);
        
        ConvertLevelMemoryFromRaw(Memory, RawMemory, RawMemorySpace);
        
        SDL_RWclose(BinaryFile);
    }
}

static void
SaveLevelMemoryToFile(game_memory *Memory)
{
    SDL_RWops *BinaryFile = SDL_RWFromFile("package2.bin", "wb");
    
    for(u32 i = 0; i < Memory->LevelMemoryAmount; ++i)
    {
        printf("kek1\n");
        printf("Memory->LevelMemory[0].RowAmount = %d\n", Memory->LevelMemory[i].RowAmount);
        printf("Memory->LevelMemory[0].ColumnAmount = %d\n", Memory->LevelMemory[i].ColumnAmount);
        SDL_RWwrite(BinaryFile, &Memory->LevelMemory[i], sizeof(level_memory), 1);
        
        printf("kek2\n");
        SDL_RWwrite(BinaryFile, Memory->LevelMemory[i].UnitField, sizeof(s32), Memory->LevelMemory[i].RowAmount * Memory->LevelMemory[i].ColumnAmount);
        printf("kek3\n");
        SDL_RWwrite(BinaryFile, Memory->LevelMemory[i].MovingBlocks, sizeof(moving_block_memory), Memory->LevelMemory[i].MovingBlocksAmount);
        printf("kek4\n");
        SDL_RWwrite(BinaryFile, Memory->LevelMemory[i].Figures, sizeof(figure_memory), Memory->LevelMemory[i].FigureAmount);
    }
    
    SDL_RWclose(BinaryFile);
}

static void 
SaveLevelEntityToMemory(game_memory *Memory, level_entity* LevelEntity, u32 Index)
{
    if(Index < 0 || Index >= Memory->LevelMemoryAmount) return;
    
    printf("SaveLevelEntityToMemory\n");
    
    Memory->LevelMemory[Index].LevelNumber      = LevelEntity->LevelNumber;
    
    if(Memory->LevelMemory[Index].RowAmount > 0)
    {
        printf("Memory->LevelMemory[Index].RowAmount = %d\n", Memory->LevelMemory[Index].RowAmount);
        for(u32 i = 0; i < Memory->LevelMemory[Index].RowAmount; ++i)
        {
            free(Memory->LevelMemory[Index].UnitField[i]);
        }
        
        free(Memory->LevelMemory[Index].UnitField);
    }
    
    Memory->LevelMemory[Index].RowAmount        = LevelEntity->GridEntity->RowAmount;
    Memory->LevelMemory[Index].ColumnAmount     = LevelEntity->GridEntity->ColumnAmount;
    
    Memory->LevelMemory[Index].UnitField = (s32**)malloc(Memory->LevelMemory[Index].RowAmount * sizeof(s32*));
    
    for(u32 i = 0; i < Memory->LevelMemory[Index].RowAmount; ++i)
    {
        Memory->LevelMemory[Index].UnitField[i] = (s32*)malloc(sizeof(s32) * Memory->LevelMemory[Index].ColumnAmount);
        
        for(u32 j = 0; j < Memory->LevelMemory[Index].ColumnAmount;++j)
        {
            Memory->LevelMemory[Index].UnitField[i][j] = LevelEntity->GridEntity->UnitField[i][j];
        }
    }
    
    printf("sas1\n");
    
    if(Memory->LevelMemory[Index].MovingBlocksAmount > 0)
    {
    free(Memory->LevelMemory[Index].MovingBlocks);
    }
    
    Memory->LevelMemory[Index].MovingBlocksAmount = LevelEntity->GridEntity->MovingBlocksAmount;
    Memory->LevelMemory[Index].MovingBlocks = (moving_block_memory*)malloc(sizeof(moving_block_memory) * Memory->LevelMemory[Index].MovingBlocksAmount);
    
    for(u32 i = 0; i < Memory->LevelMemory[Index].MovingBlocksAmount; ++i)
    {
        Memory->LevelMemory[Index].MovingBlocks[i].RowNumber = LevelEntity->GridEntity->MovingBlocks[i].RowNumber;
        Memory->LevelMemory[Index].MovingBlocks[i].ColNumber = LevelEntity->GridEntity->MovingBlocks[i].ColNumber;
        Memory->LevelMemory[Index].MovingBlocks[i].IsVertical = LevelEntity->GridEntity->MovingBlocks[i].IsVertical;
        Memory->LevelMemory[Index].MovingBlocks[i].MoveSwitch = LevelEntity->GridEntity->MovingBlocks[i].MoveSwitch;
    }
    
    printf("sas2\n");
    
    if(Memory->LevelMemory[Index].FigureAmount > 0)
    {
    free(Memory->LevelMemory[Index].Figures);
    }
    
    Memory->LevelMemory[Index].FigureAmount = LevelEntity->FigureEntity->FigureAmount;
    Memory->LevelMemory[Index].Figures = (figure_memory*)malloc(sizeof(figure_memory) * Memory->LevelMemory[Index].FigureAmount);
    
    for(u32 i = 0; i < Memory->LevelMemory[Index].FigureAmount; ++i)
    {
        Memory->LevelMemory[Index].Figures[i].Angle = LevelEntity->FigureEntity->FigureUnit[i].Angle;
        Memory->LevelMemory[Index].Figures[i].Flip = LevelEntity->FigureEntity->FigureUnit[i].Flip;
        Memory->LevelMemory[Index].Figures[i].Form = LevelEntity->FigureEntity->FigureUnit[i].Form;
        Memory->LevelMemory[Index].Figures[i].Type = LevelEntity->FigureEntity->FigureUnit[i].Type;
    }
    
    printf("sas3\n");
    
    SaveLevelMemoryToFile(Memory);
    }

static void
        LoadLevelEntityFromMemory(game_memory *Memory, level_entity *LevelEntity, u32 LevelNumber)
{
    
    }