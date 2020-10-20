/* ========================================= */
//     $File: tetroman.h
//     $Date: October 9th 2017 07:32 pm 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */

#if !defined(GAME_H)

#include "tetroman_math.h"
#include "tetroman_intrinsics.h"

#include "tetroman_playground.h"
#include "tetroman_asset.h"
#include "tetroman_editor.h"
#include "tetroman_render_group.h"
#include "tetroman_menu.h"

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
    MENU,
    PLAYGROUND
};

struct game_state
{
    memory_group MemoryGroup;
    
    game_mode CurrentMode;
    
    playground Playground;
    playground_data *PlaygroundData;
    playground_config Configuration;
    
    // TODO(msokolov): Should PlaygroundIndex be in the playground structure??
    u32 PlaygroundIndex;
    
    playground_menu PlaygroundMenu;
    game_font *Font;
    game_font *MenuTimerFont;
    game_font *TimerFont;
    
    game_settings Settings;
    
    /* NOTE(msokolov): Non-release thing only */
#if DEBUG_BUILD
    playground_editor *PlaygroundEditor;
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
