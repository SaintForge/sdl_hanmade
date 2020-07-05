/* date = June 28th 2020 11:55 am */

#ifndef TETROMAN_RENDER_GROUP_H
#define TETROMAN_RENDER_GROUP_H

struct render_entry_clear
{
    v4 Color;
};

struct render_entry_rectangle
{
    game_rect Rectangle;
    v4 Color;
};

struct render_entry_rectangle_outline
{
    game_rect Rectangle;
    v4 Color;
};

struct render_entry_rectangle2
{
    rectangle2 Rectangle;
    v4 Color;
};

struct render_entry_rectangle2_outline
{
    rectangle2 Rectangle;
    v4 Color;
};

struct render_entry_texture
{
    game_texture *Texture;
    
    // TODO(msokolov): should get rid of game_rect
    game_rect Rectangle;
    rectangle2 Rectangle2;
    
    r32 Angle;
    v2 RelativeCenter;
    
    // TODO(msokolov): change figure_flip to something else
    figure_flip Flip;
};

struct render_entry_texture2
{
    game_texture *Texture;
    
    rectangle2 Rectangle;
    
    r32 Angle;
    v2 RelativeCenter;
    
    // TODO(msokolov): change figure_flip to something else
    figure_flip Flip;
};

enum render_group_entry_type
{
    RenderGroupEntryType_render_entry_clear,
    RenderGroupEntryType_render_entry_texture,
    RenderGroupEntryType_render_entry_texture2,
    RenderGroupEntryType_render_entry_rectangle,
    RenderGroupEntryType_render_entry_rectangle_outline,
    RenderGroupEntryType_render_entry_rectangle2,
    RenderGroupEntryType_render_entry_rectangle2_outline
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
