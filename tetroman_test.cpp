
struct memory_group {
    memory_index size;
    u8 *base;
    memory_index used;
};

#define PushStruct(Arena, type) (type *)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, Count, type) (type *)PushSize_(Arena, (Count)*sizeof(type))
#define PushSize(Arena, Size) PushSize_(Arena, Size)
inline void *
PushSize_(memory_group *area, memory_index size)
{
    Assert((area->used + size) <= area->size);
    void *result = area->base + area->used;
    area->used += size;
    
    return(result);
}

inline void
initialize_memory_space(memory_group *area, memory_index size, void *base)
{
    area->size = size;
    area->base = (u8 *)base;
    area->used = 0;
}


enum render_group_entry_type
{
    RenderGroupEntryType_render_entry_clear,
    RenderGroupEntryType_render_entry_bitmap,
    RenderGroupEntryType_render_entry_rectangle,
};


struct render_group
{
    u32 MaxPushBufferSize;
    u32 PushBufferSize;
    u8 *PushBufferBase;
};

struct loaded_bitmap
{
    u32 Width;
    u32 Height;
    u32 Pitch;
    void *Memory;
};

struct render_group_entry_header
{
    render_group_entry_type Type;
};

struct render_entry_bitmap
{
    loaded_bitmap *Bitmap;
    v2 Position;
    
    r32 Angle;
    v2 RelativeCenter;
    
    // TODO(msokolov): change figure_flip to something else
    figure_flip Flip;
};

struct render_entry_rectangle
{
    rectangle2 Rectangle;
    v4 Color;
};

struct render_entry_clear
{
    v4 Color;
};

inline render_group*
AllocateRenderGroup(memory_group *Space, u32 MaxPushBufferSize)
{
    render_group *Result = PushStruct(Space, render_group);
    if (Result) 
    {
        Result->MaxPushBufferSize = MaxPushBufferSize;
        Result->PushBufferSize    = 0;
        Result->PushBufferBase    = (u8*)PushSize(Space, MaxPushBufferSize);
    }
    
    return (Result);
}

#define PushRenderElement(Group, type) (type *)PushRenderElement_(Group, sizeof(type), RenderGroupEntryType_##type)

inline void *
PushRenderElement_(render_group *Group, u32 Size, render_group_entry_type Type)
{
    void *Result = 0;
    
    Size += sizeof(render_group_entry_header);
    
    if((Group->PushBufferSize + Size) < Group->MaxPushBufferSize)
    {
        render_group_entry_header *Header = (render_group_entry_header *)(Group->PushBufferBase + Group->PushBufferSize);
        Header->Type = Type;
        Result = (u8 *)Header + sizeof(*Header);
        Group->PushBufferSize += Size;
    }
    else
    {
        //InvalidCodePath;
    }
    
    return(Result);
}

inline void
PushBitmap(render_group *Group, loaded_bitmap *Bitmap, v2 Position, r32 Angle, v2 RelativeCenter, figure_flip Flip)
{
    render_entry_bitmap *Piece = PushRenderElement(Group, render_entry_bitmap);
    if(Piece)
    {
        Piece->Bitmap         = Bitmap;
        Piece->Position       = Position;
        Piece->Angle          = Angle;
        Piece->RelativeCenter = RelativeCenter;
        Piece->Flip           = Flip;
    }
}

inline void
PushRect(render_group *Group, rectangle2 Rectangle, v4 Color) 
{
    render_entry_rectangle *Piece = PushRenderElement(Group, render_entry_rectangle);
    if (Piece) 
    {
        Piece->Rectangle = Rectangle;
        Piece->Color     = Color;
    }
}

inline void
Clear(render_group *Group, v4 Color)
{
    render_entry_clear *Piece = PushRenderElement(Group, render_entry_clear);
    if (Piece)
    {
        Piece->Color = Color;
    }
}

struct grid_test 
{
    u32 row_amount;
    u32 column_amount;
    
    s32 *unit_field;
    u32 unit_amount;
    u32 unit_amount_reserved;
    
    loaded_bitmap DefaultUnitBitmap;
    loaded_bitmap MovingBlockBitmap;
};

struct unit_test 
{
    s32 pos_xen;
    s32 pos_y;
    
    u32 shape;
};

struct figure_test
{
    u32 figure_amount;
    u32 figure_amount_reserved;
    unit_test *units;
    
    loaded_bitmap figure_bitmap[5];
};

struct config_test
{
    s32 figure_size;
    s32 block_size;
};

struct level_test 
{
    memory_group MemoryGroup;
    
    figure_test *figures;
    grid_test *grid;
    
    config_test configuration;
};

struct memory_test 
{
    u64 permanent_storage_size;
    void *permanent_storage;
    
    u64 transient_storage_size;
    void *transient_storage;
};

static memory_test
init_memory() {
    
    memory_test result = {};
    result.permanent_storage_size = 10 * 1024 * 1024;
    result.transient_storage_size = 1  * 1024 * 1024;
    
    u64 total_storage_size = result.permanent_storage_size + result.transient_storage_size;
    void *memory_block = malloc(total_storage_size);
    if (memory_block) {
        printf("success\n");
        
        result.permanent_storage = memory_block;
        result.transient_storage = ((u8 *) result.permanent_storage + result.permanent_storage_size);
    }
    
    return (result);
}


static asset_header*
GetAssetHeaderFromStorage(void *AssetStorage, u32 AssetStorageSize, asset_type AssetType, const char* AssetName, u32 Offset)
{
    u8 *mem = (u8*)AssetStorage + Offset;
    asset_header *AssetHeader = (asset_header*)mem;
    u32 TotalByteSize = 0;
    
    while(TotalByteSize < AssetStorageSize)
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

static loaded_bitmap
GetBitmapAssetFromStorage(void *AssetStorage, u32 AssetStorageSize, const char* FileName)
{
    loaded_bitmap Result = {};
    
    binary_header *BinaryHeader = (binary_header*)AssetStorage;
    
    u32 ByteOffset = sizeof(binary_header) + BinaryHeader->BitmapSizeInBytes;
    asset_header *AssetHeader = GetAssetHeaderFromStorage(AssetStorage, AssetStorageSize, AssetType_Bitmap, FileName, ByteOffset);
    
    if(AssetHeader)
    {
        asset_bitmap *Bitmap = &AssetHeader->Bitmap;
        asset_bitmap_header *Header = &Bitmap->Header;
        
        Bitmap->Data = (void*)AssetHeader;
        Bitmap->Data = ((u8*)Bitmap->Data) + sizeof(asset_header);
        
        Result.Width = Header->Width;
        Result.Height = Header->Height;
        Result.Pitch = Header->Pitch;
        Result.Memory = Bitmap->Data;
    }
    
    return (Result);
}

static void
LoadAllBitmapsFromMemory(game_memory *Memory, level_test *Level) 
{
    Level->figures->figure_bitmap[0] = GetBitmapAssetFromStorage(Memory->AssetStorage, Memory->AssetsSpaceAmount, "i_d.png");
    Level->figures->figure_bitmap[1] = GetBitmapAssetFromStorage(Memory->AssetStorage, Memory->AssetsSpaceAmount, "o_d.png");
    Level->figures->figure_bitmap[2] = GetBitmapAssetFromStorage(Memory->AssetStorage, Memory->AssetsSpaceAmount, "l_d.png");
    Level->figures->figure_bitmap[3] = GetBitmapAssetFromStorage(Memory->AssetStorage, Memory->AssetsSpaceAmount, "j_d.png");
    Level->figures->figure_bitmap[4] = GetBitmapAssetFromStorage(Memory->AssetStorage, Memory->AssetsSpaceAmount, "s_d.png");
} 

static void
RenderGroupToOutput(render_group *RenderGroup, game_offscreen_buffer *Buffer)
{
    for (u32 BaseAddress = 0; 
         BaseAddress < RenderGroup->PushBufferSize;
         )
    {
        render_group_entry_header *Header = (render_group_entry_header*) (RenderGroup->PushBufferBase + BaseAddress);
        
        BaseAddress += sizeof(*Header);
        
        void *Data = (u8*)Header + sizeof(*Header);
        switch(Header->Type) 
        {
            case RenderGroupEntryType_render_entry_clear: 
            {
                render_entry_clear *Entry = (render_entry_clear*) Data;
                
                game_rect Rectangle = {0, 0, 800, 600};
                DEBUGRenderQuadFill(Buffer, &Rectangle, {(u8)Entry->Color.r,(u8)Entry->Color.g, (u8)Entry->Color.b}, (u8)Entry->Color.a);
                
                BaseAddress += sizeof(*Entry);
                
            } break;
            
            case RenderGroupEntryType_render_entry_rectangle: 
            {
                render_entry_rectangle *Entry = (render_entry_rectangle*) Data;
                
                s32 RectangleX = Entry->Rectangle.Min.x;
                s32 RectangleY = Entry->Rectangle.Min.y;
                s32 RectangleW = Entry->Rectangle.Max.x - RectangleX;
                s32 RectangleH = Entry->Rectangle.Max.y - RectangleY;
                
                game_rect Rectangle = {RectangleX, RectangleY, RectangleW, RectangleH};
                DEBUGRenderQuadFill(Buffer, &Rectangle, {(u8)Entry->Color.r,(u8)Entry->Color.g, (u8)Entry->Color.b}, (u8)Entry->Color.a);
                
                BaseAddress += sizeof(*Entry);
                
            } break;
            
            case RenderGroupEntryType_render_entry_bitmap: 
            {
                
            } break;
        }
    }
}

static void
write_test_data(memory_test *memory) {
    
    u32 figure_amount_reserved     = 20;
    u32 unit_amount_reserved       = 100;
    u32 block_amount_reserved      = 10;
    
    level_test *level = (level_test*) memory->transient_storage;
    
    config_test configuration = {};
    configuration.figure_size = 20;
    configuration.block_size  = 10;
    
    level->configuration = configuration;
    
    initialize_memory_space(&level->MemoryGroup, memory->transient_storage_size - sizeof(level_test), (u8*)memory->transient_storage + sizeof(level_test));
    
    level->figures = PushStruct(&level->MemoryGroup, figure_test);
    level->figures->figure_amount = 0;
    level->figures->figure_amount_reserved = figure_amount_reserved;
    level->figures->units = PushArray(&level->MemoryGroup, figure_amount_reserved, unit_test);
    
    
    level->grid = PushStruct(&level->MemoryGroup, grid_test);
    level->grid->row_amount = 5;
    level->grid->column_amount = 4;
    level->grid->unit_amount = level->grid->row_amount * level->grid->column_amount;
    level->grid->unit_amount_reserved = unit_amount_reserved;
    level->grid->unit_field = PushArray(&level->MemoryGroup, unit_amount_reserved, s32);
}

static void
read_test_data(memory_test *memory) {
    
    level_test level = {};
}
