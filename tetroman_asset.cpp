/* asset_game.cpp --- 
 * 
 * Filename: asset_game.h
 * Author: Sierra
 * Created: Пн окт 16 10:08:17 2017 (+0300)
 * Last-Updated: Пт окт 27 10:15:58 2017 (+0300)
 *           By: Sierra
 */

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
SDLSizeOfBinaryFile(const char *FileName)
{
    SDL_RWops *File = SDL_RWFromFile(FileName, "rb");
    u64 ByteSize = SDLSizeOfSDL_RWops(File);
    return(ByteSize);
}

static void
ReadAssetFromFile(const char* FileName, void *AssetStorage, u64 AssetStorageSize)
{
    SDL_RWops *BinaryFile = SDL_RWFromFile(FileName, "rb");
    if (BinaryFile)
    {
        SDL_RWread(BinaryFile, AssetStorage, AssetStorageSize, 1);
        SDL_RWclose(BinaryFile);
    }
    else
    {
        printf("Failed to open %s\n", FileName);
    }
    
}

static void
SDLWriteBitmapToFile(SDL_RWops *&BinaryFile, const char* FileName)
{
    char FullName[128];
    strcpy(FullName, SpritePath);
    strcat(FullName, FileName);
    
    SDL_Surface *Surface = IMG_Load(FullName);
    printf("error:\n", IMG_GetError());
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
IsAsset(asset_header*& AssetHeader, asset_type AssetType, const char* AssetName)
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
GetAssetHeader(game_memory *&Memory, asset_type AssetType, const char* AssetName, u32 Offset)
{
    u8 *mem = (u8*)Memory->AssetStorage + Offset;
    asset_header *AssetHeader = (asset_header*)mem;
    u32 TotalByteSize = 0;
    
    while(TotalByteSize < Memory->AssetStorageSize)
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
    
    binary_header *BinaryHeader = (binary_header*)Memory->AssetStorage;
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
    
    binary_header *BinaryHeader = (binary_header*)Memory->AssetStorage;
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
GetTexture(game_memory *Memory, const char* FileName, SDL_Renderer *Renderer)
{
    game_texture *Result = NULL;
    
    binary_header *BinaryHeader = (binary_header*)Memory->AssetStorage;
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
        
        Result = SDL_CreateTextureFromSurface(Renderer, Surface);
        Assert(Result);
        
        SDL_FreeSurface(Surface);
    }
    
    return(Result);
}


static void
ReadLevelFromFile(const char *FileName, void *Storage, u64 StorageSize)
{
    SDL_RWops *BinaryFile = SDL_RWFromFile("package2.bin", "rb");
    if (BinaryFile)
    {
        SDL_RWread(BinaryFile, Storage, StorageSize, 1);
        SDL_RWclose(BinaryFile);
    }
    else
    {
        printf("ReadLevelPackageFile failed: %s\n", SDL_GetError());
    }
}

static void
WriteLevelsToFile(void *Storage, u64 StorageSize)
{
    SDL_RWops *BinaryFile = SDL_RWFromFile("package2.bin", "wb");
    if (BinaryFile)
    {
        SDL_RWwrite(BinaryFile, Storage, StorageSize, 1);
        SDL_RWclose(BinaryFile);
    }
    else
    {
        printf("WriteLevelPackageFile failed: %s\n", SDL_GetError());
    }
}

static playground_data
ReadPlaygroundData(playground_data *Playground, u32 Index)
{
    Assert(Index < PLAYGROUND_MAXIMUM);
    
    playground_data Result = {};
    Result.IsLocked = Playground[Index].IsLocked;
    Result.LevelNumber = Playground[Index].LevelNumber;
    Result.RowAmount = Playground[Index].RowAmount;
    Result.ColumnAmount = Playground[Index].ColumnAmount;
    Result.MovingBlocksAmount = Playground[Index].MovingBlocksAmount;
    Result.FigureAmount = Playground[Index].FigureAmount;
    
    for(u32 UnitIndex = 0; 
        UnitIndex < Result.RowAmount * Result.ColumnAmount;
        ++UnitIndex)
    {
        Result.UnitField[UnitIndex] = Playground[Index].UnitField[UnitIndex];
    }
    
    for(u32 BlockIndex = 0;
        BlockIndex < Result.MovingBlocksAmount;
        ++BlockIndex)
    {
        Result.MovingBlocks[BlockIndex] = Playground[Index].MovingBlocks[BlockIndex];
    }
    
    for(u32 FigureIndex = 0;
        FigureIndex < Result.FigureAmount;
        ++FigureIndex)
    {
        Result.Figures[FigureIndex] = Playground[Index].Figures[FigureIndex];
    }
    
    return (Result);
}

static void
WritePlaygroundData(playground_data *Playground, playground *Entity, u32 Index)
{
    Assert(Index < PLAYGROUND_MAXIMUM);
    
    Playground[Index].IsLocked = 0;
    Playground[Index].LevelNumber = Entity->LevelNumber;
    Playground[Index].RowAmount = Entity->GridEntity.RowAmount;
    Playground[Index].ColumnAmount = Entity->GridEntity.ColumnAmount;
    Playground[Index].MovingBlocksAmount = Entity->GridEntity.MovingBlocksAmount;
    Playground[Index].FigureAmount = Entity->FigureEntity.FigureAmount;
    
    s32 *UnitFieldSource = Entity->GridEntity.UnitField;
    s32 *UnitFieldTarget = Playground->UnitField;
    for (u32 Row = 0; 
         Row < Playground[Index].RowAmount;
         ++Row)
    {
        for (u32 Column = 0; 
             Column < Playground[Index].ColumnAmount;
             ++Column)
        {
            s32 UnitIndex = (Row * Playground[Index].ColumnAmount) + Column;
            UnitFieldTarget[UnitIndex] = UnitFieldSource[UnitIndex];
        }
    }
    
    moving_block_data *MovingBlocks = Playground[Index].MovingBlocks;
    
    for (u32 BlockIndex = 0;
         BlockIndex < Playground[Index].MovingBlocksAmount;
         ++BlockIndex)
    {
        MovingBlocks[BlockIndex].RowNumber = Entity->GridEntity.MovingBlocks[BlockIndex].RowNumber;
        MovingBlocks[BlockIndex].ColNumber = Entity->GridEntity.MovingBlocks[BlockIndex].ColNumber;
        MovingBlocks[BlockIndex].IsVertical = Entity->GridEntity.MovingBlocks[BlockIndex].IsVertical;
        MovingBlocks[BlockIndex].MoveSwitch = Entity->GridEntity.MovingBlocks[BlockIndex].MoveSwitch;
        
    }
    
    figure_data *Figures = Playground[Index].Figures;
    for (u32 FigureIndex = 0;
         FigureIndex < Playground[Index].FigureAmount;
         ++FigureIndex)
    {
        Figures[FigureIndex].Angle = Entity->FigureEntity.FigureUnit[FigureIndex].Angle;
        Figures[FigureIndex].Flip  = Entity->FigureEntity.FigureUnit[FigureIndex].Flip;
        Figures[FigureIndex].Form  = Entity->FigureEntity.FigureUnit[FigureIndex].Form;
        Figures[FigureIndex].Type  = Entity->FigureEntity.FigureUnit[FigureIndex].Type;
    }
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
    
    SDLWriteBitmapToFile(BinaryFile, "frame1.png");
    SDLWriteBitmapToFile(BinaryFile, "frame2.png");
    SDLWriteBitmapToFile(BinaryFile, "frame3.png");
    SDLWriteBitmapToFile(BinaryFile, "frame4.png");
    
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
    
    SDLWriteBitmapToFile(BinaryFile, "left_arrow.png");
    SDLWriteBitmapToFile(BinaryFile, "right_arrow.png");
    
    /* Audio loading */
    BinaryHeader.AudioSizeInBytes = SDL_RWtell(BinaryFile) - sizeof(binary_header);
    
    SDLWriteSoundToFile(BinaryFile, "focus.wav");
    SDLWriteSoundToFile(BinaryFile, "chunk.wav");
    SDLWriteMusicToFile(BinaryFile, "amb_ending_water.ogg");
    
    SDL_RWseek(BinaryFile, 0, RW_SEEK_SET);
    
    SDL_RWwrite(BinaryFile, &BinaryHeader, sizeof(binary_header), 1);
    
    SDL_RWclose(BinaryFile);
    printf("finish");
}

