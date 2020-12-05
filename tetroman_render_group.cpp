/* ========================================= */
//     $File: tetroman_render_group.cpp
//     $Date: October 10th 2017 10:32 am 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */


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
ClearScreen(render_group *Group, v4 Color)
{
    render_entry_clear_screen *Piece = PushRenderElement(Group, render_entry_clear_screen);
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
        Piece->Color  = Color;
        Piece->Target = {};
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
        Piece->Target = {};
    }
}

inline static void
PushFillRectOnBitmap(render_group *Group, game_texture *Texture, rectangle2 Rectangle, v4 Color) {
    render_entry_rectangle *Piece = PushRenderElement(Group, render_entry_rectangle);
    if (Piece)
    {
        Piece->Target = Texture; 
        Piece->Rectangle = Rectangle;
        Piece->Color = Color;
    }
}

inline static void
PushRectOnBitmap(render_group *Group, game_texture *Texture, rectangle2 Rectangle, v4 Color) {
    render_entry_rectangle_outline *Piece = PushRenderElement(Group, render_entry_rectangle_outline);
    if (Piece)
    {
        Piece->Target = Texture; 
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
        Piece->Texture2       = {};
        Piece->Rectangle      = Rectangle;
        Piece->ClipRectangle  = {};
        Piece->Angle          = 0;
        Piece->RelativeCenter = V2(0.0f, 0.0f);
        Piece->Flip           = SDL_FLIP_NONE;
        Piece->IsFont         = false;
    }
}

inline static void
PushBitmap(render_group *Group, game_texture* Texture, rectangle2 Rectangle, rectangle2 ClipRectangle)
{
    render_entry_texture *Piece = PushRenderElement(Group, render_entry_texture);
    if(Piece)
    {
        Piece->Texture        = Texture;
        Piece->Texture2       = {};
        Piece->Rectangle      = Rectangle;
        Piece->ClipRectangle  = ClipRectangle;
        Piece->Angle          = 0;
        Piece->RelativeCenter = V2(0, 0.0f);
        Piece->Flip           = SDL_FLIP_NONE;
        Piece->IsFont         = false;
    }
}

inline static void
PushBitmapEx(render_group *Group, game_texture *Texture, rectangle2 Rectangle, r32 Angle, v2 RelativeCenter, figure_flip Flip)
{
    render_entry_texture *Piece = PushRenderElement(Group, render_entry_texture);
    if(Piece)
    {
        Piece->Texture        = Texture;
        Piece->Texture2       = {};
        Piece->Rectangle      = Rectangle;
        Piece->ClipRectangle  = {};
        Piece->Angle          = Angle;
        Piece->RelativeCenter = RelativeCenter;
        Piece->Flip           = Flip;
        Piece->IsFont         = false;
    }
}

inline static void
PushBitmapOnBitmap(render_group *Group, game_texture *Texture1, game_texture *Texture2, rectangle2 Rectangle) {
    render_entry_texture *Piece = PushRenderElement(Group, render_entry_texture);
    if(Piece)
    {
        Piece->Texture        = Texture1;
        Piece->Texture2       = Texture2;
        Piece->Rectangle      = Rectangle;
        Piece->ClipRectangle  = {};
        Piece->Angle          = 0.0f;
        Piece->RelativeCenter = V2(0.0f, 0.0f);
        Piece->Flip           = SDL_FLIP_NONE;
        Piece->IsFont         = false;
    }
}

inline static void
PushFontBitmapOnBitmap(render_group *Group, game_texture *Texture1, game_texture *Texture2, rectangle2 CenterRectangle) {
    render_entry_texture *Piece = PushRenderElement(Group, render_entry_texture);
    if(Piece)
    {
        Piece->Texture        = Texture1;
        Piece->Texture2       = Texture2;
        Piece->Rectangle      = CenterRectangle;
        Piece->ClipRectangle  = {};
        Piece->Angle          = 0.0f;
        Piece->RelativeCenter = V2(0.0f, 0.0f);
        Piece->Flip           = SDL_FLIP_NONE;
        Piece->IsFont         = true;
    }
}

inline static void
PushFontBitmap(render_group *Group, game_texture* Texture, rectangle2 CenterRectangle) {
    
    render_entry_texture *Piece = PushRenderElement(Group, render_entry_texture);
    if(Piece)
    {
        Piece->Texture        = Texture;
        Piece->Texture2       = {};
        Piece->Rectangle      = CenterRectangle;
        Piece->ClipRectangle  = {};
        Piece->Angle          = 0;
        Piece->RelativeCenter = V2(0.0f, 0.0f);
        Piece->Flip           = SDL_FLIP_NONE;
        Piece->IsFont         = true;
    }
}

static void
DrawEntryTexture(game_offscreen_buffer *Buffer, render_entry_texture *Entry)
{
    game_texture *Texture = Entry->Texture;
    if (Entry->Texture2) {
        Texture = Entry->Texture2;
        SDL_SetRenderTarget(Buffer->Renderer, Entry->Texture);
    }
    
    game_point Center;
    Center.x = roundf(Entry->RelativeCenter.x);
    Center.y = roundf(Entry->RelativeCenter.y);
    
    v2 SizeRatio = V2(Buffer->WidthRatio, Buffer->HeightRatio);
    v2 ActualScreenCenter = V2(Buffer->ScreenWidth * 0.5f, Buffer->ScreenHeight * 0.5f);
    v2 ViewportCenter = V2(Buffer->ViewportWidth * 0.5f, Buffer->ViewportHeight * 0.5f);
    
    v2 NDC_Min = V2(SizeRatio * Entry->Rectangle.Min);
    v2 NDC_Max = V2(SizeRatio * Entry->Rectangle.Max);
    
    rectangle2 ResultRectangle = {};
    
    ResultRectangle.Min = V2(NDC_Min.x * Buffer->ViewportWidth, NDC_Min.y * Buffer->ViewportHeight); 
    ResultRectangle.Max = V2(NDC_Max.x * Buffer->ViewportWidth, NDC_Max.y * Buffer->ViewportHeight); 
    
    if (!Entry->Texture2) {
        ResultRectangle.Min += (ActualScreenCenter - ViewportCenter);
        ResultRectangle.Max += (ActualScreenCenter - ViewportCenter);
    }
    
    game_rect Rectangle = {};
    if (!Entry->IsFont) {
        Rectangle.x = roundf(ResultRectangle.Min.x);
        Rectangle.y = roundf(ResultRectangle.Min.y);
        Rectangle.w = roundf(ResultRectangle.Max.x - ResultRectangle.Min.x);
        Rectangle.h = roundf(ResultRectangle.Max.y - ResultRectangle.Min.y);
    }
    else {
        Rectangle.w = roundf(Entry->Rectangle.Max.x - Entry->Rectangle.Min.x);
        Rectangle.h = roundf(Entry->Rectangle.Max.y - Entry->Rectangle.Min.y);
        Rectangle.x = ResultRectangle.Min.x - ((r32)Rectangle.w * 0.5f);
        Rectangle.y = ResultRectangle.Min.y - ((r32)Rectangle.h * 0.5f);
    }
    
    v2 ClipDim = GetDim(Entry->ClipRectangle);
    if (ClipDim.w > 0.0f || ClipDim.h > 0.0f)
    {
        game_rect ClipRectangle = {};
        ClipRectangle.x = roundf(Entry->ClipRectangle.Min.x);
        ClipRectangle.y = roundf(Entry->ClipRectangle.Min.y);
        ClipRectangle.w = roundf(ClipDim.w);
        ClipRectangle.h = roundf(ClipDim.h);
        
        SDL_RenderCopyEx(Buffer->Renderer, Texture, &ClipRectangle, &Rectangle, Entry->Angle, &Center, Entry->Flip);
    }
    else 
    {
        SDL_RenderCopyEx(Buffer->Renderer, Texture, 0, &Rectangle, Entry->Angle, &Center, Entry->Flip);
    }
    
    if (Entry->Texture2) {
        SDL_SetRenderTarget(Buffer->Renderer, NULL);
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
                
                
                SDL_SetRenderDrawColor(Buffer->Renderer, Entry->Color.r, Entry->Color.g, Entry->Color.b, Entry->Color.a);
                
                game_rect Rectangle = {0, 0, Buffer->Width, Buffer->Height};
                SDL_RenderFillRect(Buffer->Renderer, &Rectangle);
                
                BaseAddress += sizeof(*Entry);
                
            } break;
            
            case RenderGroupEntryType_render_entry_clear_screen: 
            {
                render_entry_clear_screen *Entry = (render_entry_clear_screen*) Data;
                
                SDL_SetRenderDrawColor(Buffer->Renderer, Entry->Color.r, Entry->Color.g, Entry->Color.b, Entry->Color.a);
                
                BaseAddress += sizeof(*Entry);
                
            } break;
            
            case RenderGroupEntryType_render_entry_rectangle:
            {
                render_entry_rectangle *Entry = (render_entry_rectangle*) Data;
                if (Entry->Target)
                    DEBUGRenderQuadFill(Buffer, Entry->Target, Entry->Rectangle, Entry->Color);
                else 
                    DEBUGRenderQuadFill(Buffer, Entry->Rectangle, Entry->Color);
                
                BaseAddress += sizeof(*Entry);
            } break;
            
            case RenderGroupEntryType_render_entry_rectangle_outline:
            {
                render_entry_rectangle *Entry = (render_entry_rectangle*) Data;
                
                if (Entry->Target)
                    ;
                else
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