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

struct render_entry_texture
{
    game_texture *Texture;
    game_rect Rectangle;
    
    r32 Angle;
    v2 RelativeCenter;
    
    // TODO(msokolov): change figure_flip to something else
    figure_flip Flip;
};

enum render_group_entry_type
{
    RenderGroupEntryType_render_entry_clear,
    RenderGroupEntryType_render_entry_texture,
    RenderGroupEntryType_render_entry_rectangle,
    RenderGroupEntryType_render_entry_rectangle_outline
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
};


#endif //TETROMAN_RENDER_GROUP_H
