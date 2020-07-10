/* game.h --- 
 * 
 * Filename: game.h
 * Author: Sierra
 * Created: Пн окт  9 14:15:31 2017 (+0300)
 * Last-Updated: Пт окт 27 10:34:58 2017 (+0300)
 *           By: Sierra
 */

#if !defined(GAME_H)

#include "tetroman_math.h"
#include "tetroman_intrinsics.h"

#include "tetroman_playground.h"
#include "tetroman_asset.h"
#include "tetroman_editor.h"
#include "tetroman_render_group.h"

#include "tetroman_debug.h"

struct memory_group
{
    memory_index Size;
    u8 *Base;
    memory_index Used;
};

inline void
InitializeMemoryGroup(memory_group *Group, memory_index Size, void *Base)
{
    Group->Size = Size;
    Group->Base = (u8 *)Base;
    Group->Used = 0;
}

#define PushStruct(Arena, type) (type *)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, Count, type) (type *)PushSize_(Arena, (Count)*sizeof(type))
#define PushSize(Arena, Size) PushSize_(Arena, Size)
inline void *
PushSize_(memory_group *Area, memory_index Size)
{
    Assert((Area->Used + Size) <= Area->Size);
    void *Result = Area->Base + Area->Used;
    Area->Used += Size;
    
    return(Result);
}

enum game_mode
{
    LEVEL, 
    LEVEL_MENU, 
    MAIN_MENU
};

struct game_state
{
    memory_group MemoryGroup;
    
    playground Playground;
    playground_data *PlaygroundData;
    playground_config Configuration;
    u32 PlaygroundIndex;
    
    menu_entity *MenuEntity;
    game_mode CurrentMode;
    
    /* NOTE(msokolov): Non-release thing only */
#if DEBUG_BUILD
    game_editor GameEditor;
    b32 EditorMode;
#endif
};

struct transient_state
{
    memory_group TransGroup;
    b32 IsInitialized;
};

#define GAME_H
#endif
