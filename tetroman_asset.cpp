/* ========================================= */
//     $File: tetroman_asset.cpp
//     $Date: June 16th 2017 10:08 am 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */


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
    
    if (Surface->format->palette)
    {
        for (s32 Index = 0;
             Index < Surface->format->palette->ncolors;
             ++Index)
        {
            BitmapHeader.Palette[Index] = Surface->format->palette->colors[Index];
        }
        
    }
    
    
    u32 ByteAmount = BitmapHeader.Height * BitmapHeader.Pitch;
    
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

static game_surface*
GetSurface(game_memory *Memory, const char* FileName, SDL_Renderer *Renderer)
{
    game_surface *Result = NULL;
    
    binary_header *BinaryHeader = (binary_header*)Memory->AssetStorage;
    u32 ByteOffset = sizeof(binary_header) + BinaryHeader->BitmapSizeInBytes;
    
    asset_header *AssetHeader = GetAssetHeader(Memory, AssetType_Bitmap, FileName, ByteOffset);
    if(AssetHeader)
    {
        asset_bitmap *Bitmap = &AssetHeader->Bitmap;
        asset_bitmap_header *Header = &Bitmap->Header;
        
        Bitmap->Data = (void*)AssetHeader;
        Bitmap->Data = ((u8*)Bitmap->Data) + sizeof(asset_header);
        Result = SDL_CreateRGBSurfaceFrom(Bitmap->Data,
                                          Header->Width, Header->Height,
                                          Header->BitsPerPixel, Header->Pitch,
                                          Header->Rmask, Header->Gmask,
                                          Header->Bmask, Header->Amask);
        Assert(Result);
        
        
        if (Header->BitsPerPixel == 8)
        {
            SDL_SetPaletteColors(Result->format->palette, Header->Palette, 0, 256);
        }
    }
    
    return(Result);
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
        
        
        if (Header->BitsPerPixel == 8)
        {
            SDL_SetPaletteColors(Surface->format->palette, Header->Palette, 0, 256);
        }
        
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

static void
SDLAssetBuildBinaryFile()
{
    SDL_RWops *BinaryFile = SDL_RWFromFile("package1.bin", "wb");
    
    binary_header BinaryHeader = {};
    SDL_RWwrite(BinaryFile, &BinaryHeader, sizeof(binary_header), 1);
    
    /* Bitmap loading */
    BinaryHeader.BitmapSizeInBytes = 0;
    
    SDLWriteBitmapToFile(BinaryFile, "test_animation.png");
    SDLWriteBitmapToFile(BinaryFile, "test_animation2.png");
    SDLWriteBitmapToFile(BinaryFile, "light.png");
    SDLWriteBitmapToFile(BinaryFile, "I_new.png");
    
    
    SDLWriteBitmapToFile(BinaryFile, "level_background.png");
    SDLWriteBitmapToFile(BinaryFile, "level_background2.png");
    SDLWriteBitmapToFile(BinaryFile, "level_background3.png");
    SDLWriteBitmapToFile(BinaryFile, "vertical_border.png");
    SDLWriteBitmapToFile(BinaryFile, "horizontal_border.png");
    SDLWriteBitmapToFile(BinaryFile, "horizontal_border_2.png");
    
    /* new */
    SDLWriteBitmapToFile(BinaryFile, "corner_left_top.png");
    SDLWriteBitmapToFile(BinaryFile, "corner_left_bottom.png");
    SDLWriteBitmapToFile(BinaryFile, "corner_right_top.png");
    SDLWriteBitmapToFile(BinaryFile, "corner_right_bottom.png");
    
    SDLWriteBitmapToFile(BinaryFile, "corner_menu_1.png");
    SDLWriteBitmapToFile(BinaryFile, "corner_menu_2.png");
    SDLWriteBitmapToFile(BinaryFile, "corner_menu_3.png");
    SDLWriteBitmapToFile(BinaryFile, "corner_menu_4.png");
    
    SDLWriteBitmapToFile(BinaryFile, "square_frame_locked.png");
    SDLWriteBitmapToFile(BinaryFile, "square_frame_unlocked.png");
    
    SDLWriteBitmapToFile(BinaryFile, "green_bar.png");
    SDLWriteBitmapToFile(BinaryFile, "blue_bar.png");
    SDLWriteBitmapToFile(BinaryFile, "orange_bar.png");
    SDLWriteBitmapToFile(BinaryFile, "red_bar.png");
    
    SDLWriteBitmapToFile(BinaryFile, "gear_new.png");
    SDLWriteBitmapToFile(BinaryFile, "gear_new_shadow.png");
    
    
    SDLWriteBitmapToFile(BinaryFile, "grid_cell_new.png");
    SDLWriteBitmapToFile(BinaryFile, "grid_cell_new2.png");
    SDLWriteBitmapToFile(BinaryFile, "border_vertical.png");
    
    SDLWriteBitmapToFile(BinaryFile, "grid_cell.png");
    SDLWriteBitmapToFile(BinaryFile, "grid_cell_1.png");
    SDLWriteBitmapToFile(BinaryFile, "grid_cell_2.png");
    
    SDLWriteBitmapToFile(BinaryFile, "frame1.png");
    SDLWriteBitmapToFile(BinaryFile, "frame2.png");
    SDLWriteBitmapToFile(BinaryFile, "frame3.png");
    SDLWriteBitmapToFile(BinaryFile, "frame4.png");
    
    SDLWriteBitmapToFile(BinaryFile, "i_d_new.png");
    SDLWriteBitmapToFile(BinaryFile, "i_d_new_shadow.png");
    
    SDLWriteBitmapToFile(BinaryFile, "o_d_new.png");
    SDLWriteBitmapToFile(BinaryFile, "o_d_new_shadow.png");
    
    SDLWriteBitmapToFile(BinaryFile, "l_d_new.png");
    SDLWriteBitmapToFile(BinaryFile, "l_d_new_shadow.png");
    
    SDLWriteBitmapToFile(BinaryFile, "j_d_new.png");
    SDLWriteBitmapToFile(BinaryFile, "j_d_new_shadow.png");
    
    SDLWriteBitmapToFile(BinaryFile, "s_d_new.png");
    SDLWriteBitmapToFile(BinaryFile, "s_d_new_shadow.png");
    
    SDLWriteBitmapToFile(BinaryFile, "z_d_new.png");
    SDLWriteBitmapToFile(BinaryFile, "z_d_new_shadow.png");
    
    SDLWriteBitmapToFile(BinaryFile, "t_d_new.png");
    SDLWriteBitmapToFile(BinaryFile, "t_d_new_shadow.png");
    
    SDLWriteBitmapToFile(BinaryFile, "left_arrow.png");
    SDLWriteBitmapToFile(BinaryFile, "right_arrow.png");
    
    /* Audio loading */
    BinaryHeader.AudioSizeInBytes = SDL_RWtell(BinaryFile) - sizeof(binary_header);
    
    SDLWriteSoundToFile(BinaryFile, "figure_pick.wav");
    SDLWriteSoundToFile(BinaryFile, "figure_stick.wav");
    SDLWriteSoundToFile(BinaryFile, "figure_rotate.wav");
    
    //SDLWriteMusicToFile(BinaryFile, "amb_ending_water.ogg");
    
    SDL_RWseek(BinaryFile, 0, RW_SEEK_SET);
    
    SDL_RWwrite(BinaryFile, &BinaryHeader, sizeof(binary_header), 1);
    
    SDL_RWclose(BinaryFile);
}

