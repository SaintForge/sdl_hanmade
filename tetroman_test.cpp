

struct memory_space {
    memory_index size;
    u8 *base;
    memory_index used;
};


#define PushStruct(Arena, type) (type *)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, Count, type) (type *)PushSize_(Arena, (Count)*sizeof(type))
#define PushSize(Arena, Size) PushSize_(Arena, Size)
inline void *
PushSize_(memory_space *area, memory_index size)
{
    Assert((area->used + size) <= area->size);
    void *result = area->base + area->used;
    area->used += size;
    
    return(result);
}

inline void
initialize_memory_space(memory_space *area, memory_index size, void *base)
{
    area->size = size;
    area->base = (u8 *)base;
    area->used = 0;
}

struct grid_test 
{
    u32 row_amount;
    u32 column_amount;
    
    s32 *unit_field;
    u32 unit_amount;
    u32 unit_amount_reserved;
};

struct unit_test 
{
    s32 pos_x;
    s32 pos_y;
    
    u32 shape;
};

struct figure_test
{
    u32 figure_amount;
    u32 figure_amount_reserved;
    unit_test *units;
};

struct config_test
{
    s32 figure_size;
    s32 block_size;
};

struct level_test 
{
    memory_space space;
    
    figure_test *figures;
    grid_test *grid;
    
    config_test configuration;
};

struct memory_test {
    
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
    
    initialize_memory_space(&level->space, memory->transient_storage_size - sizeof(level_test), (u8*)memory->transient_storage + sizeof(level_test));
    
    level->figures = PushStruct(&level->space, figure_test);
    level->figures->figure_amount = 0;
    level->figures->figure_amount_reserved = figure_amount_reserved;
    level->figures->units = PushArray(&level->space, figure_amount_reserved, unit_test);
    
    level->grid = PushStruct(&level->space, grid_test);
    level->grid->row_amount = 5;
    level->grid->column_amount = 4;
    level->grid->unit_amount = level->grid->row_amount * level->grid->column_amount;
    level->grid->unit_amount_reserved = unit_amount_reserved;
    level->grid->unit_field = PushArray(&level->space, unit_amount_reserved, s32);
}

static void
read_test_data(memory_test *memory) {
    
    level_test level = {};
}
