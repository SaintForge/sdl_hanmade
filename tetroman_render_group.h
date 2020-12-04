/* ========================================= */
//     $File: tetroman_render_group.h
//     $Date: June 28th 2020 10:32 am 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */


#ifndef TETROMAN_RENDER_GROUP_H
#define TETROMAN_RENDER_GROUP_H

struct render_entry_clear
{
    v4 Color;
};

struct render_entry_clear_screen
{
    v4 Color;
};

struct render_entry_rectangle
{
    game_texture *Target;
    rectangle2 Rectangle;
    v4 Color;
};

struct render_entry_rectangle_outline
{
    game_texture *Target;
    rectangle2 Rectangle;
    v4 Color;
};

struct render_entry_texture
{
    game_texture *Texture;
    game_texture *Texture2;
    
    rectangle2 Rectangle;
    rectangle2 ClipRectangle;
    
    r32 Angle;
    v2 RelativeCenter;
    
    // TODO(msokolov): change figure_flip to something else
    figure_flip Flip;
};

enum render_group_entry_type
{
    RenderGroupEntryType_render_entry_clear,
    RenderGroupEntryType_render_entry_clear_screen,
    RenderGroupEntryType_render_entry_texture,
    RenderGroupEntryType_render_entry_rectangle,
    RenderGroupEntryType_render_entry_rectangle_outline,
};

struct render_group_entry_header
{
    render_group_entry_type Type;
};

struct render_group
{
    u32 MaxPushBufferSize;
    u32 PushBufferSize;
    u8 *PushBufferBase;
    
    u32 Width;
    u32 Height;
};


#endif //TETROMAN_RENDER_GROUP_H
