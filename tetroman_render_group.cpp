

inline render_group*
AllocateRenderGroup(memory_group *Space, u64 MaxPushBufferSize, s32 Width, s32 Height)
{
    render_group *Result = PushStruct(Space, render_group);
    
    Result->PushBufferBase = (u8*)PushSize(Space, MaxPushBufferSize);
    Result->MaxPushBufferSize = MaxPushBufferSize;
    Result->PushBufferSize    = 0;
    
    Result->Width = Width;
    Result->Height = Height;
    
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


inline static void
Clear(render_group *Group, v4 Color)
{
    render_entry_clear *Piece = PushRenderElement(Group, render_entry_clear);
    if (Piece)
    {
        Piece->Color = Color;
    }
}

inline static void
PushRectangle(render_group *Group, rectangle2 Rectangle, v4 Color)
{
    render_entry_rectangle *Piece = PushRenderElement(Group, render_entry_rectangle);
    if (Piece)
    {
        Piece->Rectangle = Rectangle;
        Piece->Color = Color;
    }
}

inline static void
PushRectangleOutline(render_group *Group, rectangle2 Rectangle, v4 Color)
{
    render_entry_rectangle_outline *Piece = PushRenderElement(Group, render_entry_rectangle_outline);
    if (Piece)
    {
        Piece->Rectangle = Rectangle;
        Piece->Color = Color;
    }
}

inline static void
PushBitmap(render_group *Group, game_texture* Texture, rectangle2 Rectangle)
{
    render_entry_texture *Piece = PushRenderElement(Group, render_entry_texture);
    if(Piece)
    {
        Piece->Texture        = Texture;
        Piece->Rectangle      = Rectangle;
        Piece->ClipRectangle  = {};
        Piece->Angle          = 0;
        Piece->RelativeCenter = {0, 0};
        Piece->Flip           = SDL_FLIP_NONE;
    }
}

inline static void
PushBitmapEx(render_group *Group, game_texture *Texture, rectangle2 Rectangle, r32 Angle, v2 RelativeCenter, figure_flip Flip)
{
    render_entry_texture *Piece = PushRenderElement(Group, render_entry_texture);
    if(Piece)
    {
        Piece->Texture        = Texture;
        Piece->Rectangle      = Rectangle;
        Piece->ClipRectangle  = {};
        Piece->Angle          = Angle;
        Piece->RelativeCenter = RelativeCenter;
        Piece->Flip           = Flip;
    }
}

static void
DrawEntryTexture(game_offscreen_buffer *Buffer, render_entry_texture *Entry)
{
    game_texture *Texture = Entry->Texture;
    
    game_point Center;
    Center.x = roundf(Entry->RelativeCenter.x);
    Center.y = roundf(Entry->RelativeCenter.y);
    
    game_rect Rectangle;
    Rectangle.x = roundf(Entry->Rectangle.Min.x);
    Rectangle.y = roundf(Entry->Rectangle.Min.y);
    Rectangle.w = roundf(Entry->Rectangle.Max.x - Entry->Rectangle.Min.x);
    Rectangle.h = roundf(Entry->Rectangle.Max.y - Entry->Rectangle.Min.y);
    
    v2 ClipDim = GetDim(Entry->ClipRectangle);
    if (ClipDim.w > 0 || ClipDim.h > 0)
    {
        game_rect ClipRectangle = {};
        ClipRectangle.x = Entry->ClipRectangle.Min.x;
        ClipRectangle.y = Entry->ClipRectangle.Min.y;
        ClipRectangle.w = ClipDim.w;
        ClipRectangle.h = ClipDim.h;
        
        SDL_RenderCopyEx(Buffer->Renderer, Texture, &ClipRectangle, &Rectangle, Entry->Angle, &Center, Entry->Flip);
    }
    else 
    {
        SDL_RenderCopyEx(Buffer->Renderer, Texture, 0, &Rectangle, Entry->Angle, &Center, Entry->Flip);
    }
    
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
                
                game_rect Rectangle = {0, 0, Buffer->Width, Buffer->Height};
                DEBUGRenderQuadFill(Buffer, &Rectangle, {(u8)Entry->Color.r,(u8)Entry->Color.g, (u8)Entry->Color.b}, (u8)Entry->Color.a);
                
                BaseAddress += sizeof(*Entry);
                
            } break;
            
            case RenderGroupEntryType_render_entry_rectangle:
            {
                render_entry_rectangle *Entry = (render_entry_rectangle*) Data;
                
                DEBUGRenderQuadFill(Buffer, Entry->Rectangle, Entry->Color);
                
                BaseAddress += sizeof(*Entry);
            } break;
            
            case RenderGroupEntryType_render_entry_rectangle_outline:
            {
                render_entry_rectangle *Entry = (render_entry_rectangle*) Data;
                
                DEBUGRenderQuad(Buffer, Entry->Rectangle, Entry->Color);
                
                BaseAddress += sizeof(*Entry);
            } break;
            
            case RenderGroupEntryType_render_entry_texture: 
            {
                render_entry_texture *Entry = (render_entry_texture*) Data;
                
                DrawEntryTexture(Buffer, Entry);
                
                BaseAddress += sizeof(*Entry);
            } break;
        }
    }
}