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
        
        SDL_RWops* rw = SDL_RWFromMem(Audio->Data, AssetHeader->AssetSize);
        Sound = Mix_LoadWAV_RW(rw, 0);
        
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
ReadBinaryFile(const char *FileName, void *Storage, u64 StorageSize)
{
    SDL_RWops *BinaryFile = SDL_RWFromFile(FileName, "rb");
    if (BinaryFile)
    {
        SDL_RWread(BinaryFile, Storage, StorageSize, 1);
        SDL_RWclose(BinaryFile);
    }
    else
    {
        printf("ReadBinaryFile failed for %s: %s\n", FileName, SDL_GetError());
    }
}

static void
WriteBinaryFile(const char *FileName, void *Storage, u64 StorageSize)
{
    SDL_RWops *BinaryFile = SDL_RWFromFile(FileName, "wb");
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
    
    SDLWriteBitmapToFile(BinaryFile, "background_.png");
    SDLWriteBitmapToFile(BinaryFile, "background_dimmed.png");
    SDLWriteBitmapToFile(BinaryFile, "level_background.png");
    SDLWriteBitmapToFile(BinaryFile, "level_background2.png");
    SDLWriteBitmapToFile(BinaryFile, "level_background3.png");
    SDLWriteBitmapToFile(BinaryFile, "vertical_border.png");
    SDLWriteBitmapToFile(BinaryFile, "horizontal_border.png");
    SDLWriteBitmapToFile(BinaryFile, "horizontal_border_2.png");
    
    SDLWriteBitmapToFile(BinaryFile, "next_level_indicator.png");
    SDLWriteBitmapToFile(BinaryFile, "corner_left_top1.png");
    SDLWriteBitmapToFile(BinaryFile, "corner_left_top_shadow.png");
    SDLWriteBitmapToFile(BinaryFile, "corner_left_bottom.png");
    SDLWriteBitmapToFile(BinaryFile, "corner_right_top.png");
    SDLWriteBitmapToFile(BinaryFile, "corner_right_bottom.png");
    
    SDLWriteBitmapToFile(BinaryFile, "corner_menu_left_top.png");
    SDLWriteBitmapToFile(BinaryFile, "corner_menu_left_top_shadow.png");
    
    SDLWriteBitmapToFile(BinaryFile, "corner_menu_left_bottom.png");
    SDLWriteBitmapToFile(BinaryFile, "corner_menu_right_top.png");
    SDLWriteBitmapToFile(BinaryFile, "corner_menu_right_bottom.png");
    
    SDLWriteBitmapToFile(BinaryFile, "square_frame_locked.png");
    SDLWriteBitmapToFile(BinaryFile, "square_frame_unlocked.png");
    SDLWriteBitmapToFile(BinaryFile, "square_frame_unlocked_next.png");
    
    SDLWriteBitmapToFile(BinaryFile, "green_bar.png");
    SDLWriteBitmapToFile(BinaryFile, "blue_bar.png");
    SDLWriteBitmapToFile(BinaryFile, "orange_bar.png");
    SDLWriteBitmapToFile(BinaryFile, "red_bar.png");
    
    SDLWriteBitmapToFile(BinaryFile, "gear_new.png");
    SDLWriteBitmapToFile(BinaryFile, "gear_new_shadow.png");
    SDLWriteBitmapToFile(BinaryFile, "level_indicator_empty.png");
    SDLWriteBitmapToFile(BinaryFile, "level_indicator_filled.png");
    SDLWriteBitmapToFile(BinaryFile, "option_progress_bar.png");
    
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
    
    SDLWriteBitmapToFile(BinaryFile, "i_green.png");
    SDLWriteBitmapToFile(BinaryFile, "i_blue.png");
    SDLWriteBitmapToFile(BinaryFile, "i_orange.png");
    SDLWriteBitmapToFile(BinaryFile, "i_red.png");
    SDLWriteBitmapToFile(BinaryFile, "i_outline.png");
    SDLWriteBitmapToFile(BinaryFile, "i_shadow.png");
    SDLWriteBitmapToFile(BinaryFile, "i_ground.png");
    
    SDLWriteBitmapToFile(BinaryFile, "o_green.png");
    SDLWriteBitmapToFile(BinaryFile, "o_blue.png");
    SDLWriteBitmapToFile(BinaryFile, "o_orange.png");
    SDLWriteBitmapToFile(BinaryFile, "o_red.png");
    SDLWriteBitmapToFile(BinaryFile, "o_outline.png");
    SDLWriteBitmapToFile(BinaryFile, "o_shadow.png");
    SDLWriteBitmapToFile(BinaryFile, "o_ground.png");
    
    SDLWriteBitmapToFile(BinaryFile, "l_green.png");
    SDLWriteBitmapToFile(BinaryFile, "l_blue.png");
    SDLWriteBitmapToFile(BinaryFile, "l_orange.png");
    SDLWriteBitmapToFile(BinaryFile, "l_red.png");
    SDLWriteBitmapToFile(BinaryFile, "l_outline.png");
    SDLWriteBitmapToFile(BinaryFile, "l_shadow.png");
    SDLWriteBitmapToFile(BinaryFile, "l_ground.png");
    
    SDLWriteBitmapToFile(BinaryFile, "j_green.png");
    SDLWriteBitmapToFile(BinaryFile, "j_blue.png");
    SDLWriteBitmapToFile(BinaryFile, "j_orange.png");
    SDLWriteBitmapToFile(BinaryFile, "j_red.png");
    SDLWriteBitmapToFile(BinaryFile, "j_outline.png");
    SDLWriteBitmapToFile(BinaryFile, "j_shadow.png");
    SDLWriteBitmapToFile(BinaryFile, "j_ground.png");
    
    SDLWriteBitmapToFile(BinaryFile, "s_green.png");
    SDLWriteBitmapToFile(BinaryFile, "s_blue.png");
    SDLWriteBitmapToFile(BinaryFile, "s_orange.png");
    SDLWriteBitmapToFile(BinaryFile, "s_red.png");
    SDLWriteBitmapToFile(BinaryFile, "s_outline.png");
    SDLWriteBitmapToFile(BinaryFile, "s_shadow.png");
    SDLWriteBitmapToFile(BinaryFile, "s_ground.png");
    
    SDLWriteBitmapToFile(BinaryFile, "z_green.png");
    SDLWriteBitmapToFile(BinaryFile, "z_blue.png");
    SDLWriteBitmapToFile(BinaryFile, "z_orange.png");
    SDLWriteBitmapToFile(BinaryFile, "z_red.png");
    SDLWriteBitmapToFile(BinaryFile, "z_outline.png");
    SDLWriteBitmapToFile(BinaryFile, "z_shadow.png");
    SDLWriteBitmapToFile(BinaryFile, "z_ground.png");
    
    SDLWriteBitmapToFile(BinaryFile, "t_green.png");
    SDLWriteBitmapToFile(BinaryFile, "t_blue.png");
    SDLWriteBitmapToFile(BinaryFile, "t_orange.png");
    SDLWriteBitmapToFile(BinaryFile, "t_red.png");
    SDLWriteBitmapToFile(BinaryFile, "t_outline.png");
    SDLWriteBitmapToFile(BinaryFile, "t_shadow.png");
    SDLWriteBitmapToFile(BinaryFile, "t_ground.png");
    
    SDLWriteBitmapToFile(BinaryFile, "left_arrow.png");
    SDLWriteBitmapToFile(BinaryFile, "right_arrow.png");
    
    /* Audio loading */
    BinaryHeader.AudioSizeInBytes = SDL_RWtell(BinaryFile) - sizeof(binary_header);
    
    SDLWriteSoundToFile(BinaryFile, "complete_sound2.wav");
    SDLWriteSoundToFile(BinaryFile, "figure_pick.wav");
    SDLWriteSoundToFile(BinaryFile, "figure_stick.wav");
    SDLWriteSoundToFile(BinaryFile, "figure_rotate.wav");
    SDLWriteSoundToFile(BinaryFile, "figure_drop.wav");
    SDLWriteSoundToFile(BinaryFile, "piano_key.wav");
    SDLWriteSoundToFile(BinaryFile, "string_pick1.wav");
    SDLWriteSoundToFile(BinaryFile, "string_pick2.wav");
    SDLWriteSoundToFile(BinaryFile, "figure_rotation.wav");
    SDLWriteSoundToFile(BinaryFile, "lego_snap1.wav");
    SDLWriteSoundToFile(BinaryFile, "lego_snap2.wav");
    SDLWriteSoundToFile(BinaryFile, "lego_snap3.wav");
    SDLWriteSoundToFile(BinaryFile, "lego_snap4.wav");
    SDLWriteSoundToFile(BinaryFile, "gear_sound.wav");
    SDLWriteSoundToFile(BinaryFile, "menu_slider_down.wav");
    SDLWriteSoundToFile(BinaryFile, "menu_slider_up.wav");
    
    SDLWriteMusicToFile(BinaryFile, "Jami Saber - Maenam.ogg");
    SDLWriteMusicToFile(BinaryFile, "music_1.ogg");
    SDLWriteMusicToFile(BinaryFile, "music_2.ogg");
    SDLWriteMusicToFile(BinaryFile, "music_3.ogg");
    SDLWriteMusicToFile(BinaryFile, "music_4.ogg");
    SDLWriteMusicToFile(BinaryFile, "music_5.ogg");
    SDLWriteMusicToFile(BinaryFile, "music_6.ogg");
    
    SDL_RWseek(BinaryFile, 0, RW_SEEK_SET);
    
    SDL_RWwrite(BinaryFile, &BinaryHeader, sizeof(binary_header), 1);
    
    SDL_RWclose(BinaryFile);
}

