// game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

#include "game_math.h"
#include "game.h"
#include "assets.h"
#include "menu.h"

#include "assets.cpp"
#include "menu.cpp"

static void
GameRenderBitmapToBuffer(game_offscreen_buffer *Buffer, game_texture *&Texture, game_rect *Quad)
{
    Assert(Texture);
    SDL_RenderCopy(Buffer->Renderer, Texture, 0, Quad);
}

static void 
FigureEntityHighlightFigureArea(figure_entity *FigureEntity, 
                                game_offscreen_buffer *Buffer,
                                SDL_Color Color, u32 Thickness)
{
    game_rect AreaQuad = FigureEntity->FigureArea;
    
    u8 r, g, b;
    SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, 0);
    SDL_SetRenderDrawColor(Buffer->Renderer, Color.r, Color.g, Color.b, FigureEntity->AreaAlpha);
    
    for(u32 i = 0; i < Thickness; i ++)
    {
        SDL_RenderDrawRect(Buffer->Renderer, &AreaQuad);
        
        AreaQuad.x += 1;
        AreaQuad.y += 1;
        AreaQuad.w -= 2;
        AreaQuad.h -= 2;
    }
    
    SDL_SetRenderDrawColor(Buffer->Renderer, r, g, b, 255);
}

static u32
GameResizeActiveBlock(u32 GridAreaWidth, 
                      u32 GridAreaHeight, 
                      u32 RowAmount, 
                      u32 ColumnAmount)
{
    u32 ResultBlockSize = 0;
    
    u32 DefaultBlockWidth  = GridAreaWidth  / (ColumnAmount + 1);
    u32 DefaultBlockHeight = GridAreaHeight / (RowAmount + 1);
    u32 DefaultBlockSize   = DefaultBlockWidth < DefaultBlockHeight ? DefaultBlockWidth : DefaultBlockHeight;
    
    ResultBlockSize = DefaultBlockSize;
    ResultBlockSize = ResultBlockSize - (ResultBlockSize % 2);
    
    return(ResultBlockSize);
}

static u32
GameResizeInActiveBlock(u32 FigureAreaWidth, 
                        u32 FigureAreaHeight, 
                        u32 DefaultBlocksInRow,
                        u32 DefaultBlocksInCol,
                        u32 BlocksInRow)
{
    u32 ResultBlockSize  = 0;
    
    u32 DefaultBlockWidth  = FigureAreaWidth / DefaultBlocksInRow;
    u32 DefaultBlockHeight = FigureAreaHeight / DefaultBlocksInCol;
    u32 DefaultBlockSize   = DefaultBlockWidth < DefaultBlockHeight ? DefaultBlockWidth : DefaultBlockHeight;
    u32 ActualBlockSize    = FigureAreaWidth / BlocksInRow;
    
    ResultBlockSize = ActualBlockSize < DefaultBlockSize ? ActualBlockSize : DefaultBlockSize;
    ResultBlockSize = ResultBlockSize - (ResultBlockSize % 2);
    
    return(ResultBlockSize);
}

#if 0
static void
GameCopyImageToBuffer(game_bitmap* GameBitmap, u32 X, u32 Y,
                      game_offscreen_buffer *Buffer)
{
    u8 BytesPerPixel = GameBitmap->BytesPerPixel;
    
    u8 *RowBuffer = (u8*)Buffer->Memory + (Y * GameBitmap->Pitch);
    u8 *RowTarget = (u8*)GameBitmap->Pixels;
    
    for (u32 y = 0; y < GameBitmap->Height; ++y)
    {
        u32 *PixelBuffer = (u32*)RowBuffer + X;
        u32 *PixelTarget = (u32*)RowTarget;
        
        for (u32 x = 0; x < GameBitmap->Width; ++x)
        {
            u8 Alpha = (*PixelTarget >> 24) & 0xFF;
            u8 Blue  = (*PixelTarget >> 16) & 0xFF;
            u8 Green = (*PixelTarget >> 8)  & 0xFF;
            u8 Red   = (*PixelTarget & 0xFF);
            
            *PixelBuffer = ((Red << 24) | (Green << 16) | (Blue << 8) | (Alpha));
            
            *PixelBuffer++;
            *PixelTarget++;
        }
        
        RowBuffer += Buffer->Pitch;
        RowTarget += GameBitmap->Width * BytesPerPixel;
    }
    
}
#endif

inline static bool
IsFigureUnitInsideRect(figure_unit *Unit, game_rect *AreaQuad)
{
    for (u32 i = 0; i < 4; ++i)
    {
        if(IsPointInsideRect(Unit->Shell[i].x, Unit->Shell[i].y, AreaQuad))
        {
            return true;
        }
    }
    
    return false;
}

static void
FigureEntityHighOrderFigure(figure_entity *FigureEntity, u32 Index)
{
    u32 StartIndex   = 0;
    u32 ActiveIndex  = FigureEntity->FigureActive;
    u32 FigureAmount = FigureEntity->FigureAmount;
    u32 *FigureOrder = FigureEntity->FigureOrder;
    
    if(ActiveIndex != FigureOrder[FigureAmount - 1])
    {
        for (u32 i = 0; i < FigureAmount; i++)
        {
            if(FigureOrder[i] == Index)
            {
                StartIndex= i;
            }
        }
        
        for (u32 i = StartIndex; i < FigureAmount - 1; ++i)
        {
            FigureOrder[i] = FigureOrder[i+1];
        }
        
        FigureOrder[FigureAmount-1] = Index;
    }
    
}

static void 
FigureEntityLowPriority(figure_entity *FigureEntity, u32 Index)
{
    u32 StartIndex   = 0;
    u32 FigureAmount = FigureEntity->FigureAmount;
    u32 *FigureOrder = FigureEntity->FigureOrder;
    
    if(FigureOrder[0] == Index)
    {
        return;
    }
    
    for (u32 i = 0; i < FigureAmount; i++)
    {
        if(FigureOrder[i] == Index)
        {
            StartIndex = i;
            break;
        }
    }
    
    
    for (u32  i = StartIndex; i > 0; i--)
    {
        FigureOrder[i] = FigureOrder[i-1];
    }
    
    FigureOrder[0] = Index;
}

static void
FigureUnitResizeBy(figure_unit *Entity, r32 ScaleFactor)
{
    game_rect *Rectangle = &Entity->AreaQuad;
    game_point OldCenter = {};
    game_point NewCenter = {};
    
    s32 OffsetX = 0;
    s32 OffsetY = 0;
    s32 OldX = Entity->AreaQuad.x;
    s32 OldY = Entity->AreaQuad.y;
    
    OldCenter.x = Entity->AreaQuad.x + (Entity->AreaQuad.w / 2);
    OldCenter.y = Entity->AreaQuad.y + (Entity->AreaQuad.h / 2);
    
    Rectangle->w = roundf(Rectangle->w * ScaleFactor);
    Rectangle->h = roundf(Rectangle->h * ScaleFactor);
    
    NewCenter.x = Entity->AreaQuad.x + (Entity->AreaQuad.w / 2);
    NewCenter.y = Entity->AreaQuad.y + (Entity->AreaQuad.h / 2);
    
    Rectangle->x += (OldCenter.x - NewCenter.x);
    Rectangle->y += (OldCenter.y - NewCenter.y);
    
    OffsetX = roundf((Entity->Center.x - OldX) * ScaleFactor);
    OffsetY = roundf((Entity->Center.y - OldY) * ScaleFactor);
    Entity->Center.x = OldX + OffsetX;
    Entity->Center.y = OldY + OffsetY;
    Entity->Center.x += (OldCenter.x - NewCenter.x);
    Entity->Center.y += (OldCenter.y - NewCenter.y);
    
    for (u32 i = 0; i < 4; ++i)
    {
        OffsetX = roundf((Entity->Shell[i].x - OldX) * ScaleFactor);
        OffsetY = roundf((Entity->Shell[i].y - OldY) * ScaleFactor);
        Entity->Shell[i].x = OldX + OffsetX;
        Entity->Shell[i].y = OldY + OffsetY;
        Entity->Shell[i].x += (OldCenter.x - NewCenter.x);
        Entity->Shell[i].y += (OldCenter.y - NewCenter.y);
    }
    
}

static void
FigureUnitDeleteFigure(figure_entity *FigureEntity, s32 Index)
{
    u32 FigureAmount = FigureEntity->FigureAmount;
    
    if(FigureAmount == 0) return;
    if(Index < 0 || Index >= FigureAmount) return;
    
    FreeTexture(FigureEntity->FigureUnit[Index].Texture);
    
    for(u32 i = Index; i < FigureAmount; ++i)
    {
        FigureEntity->FigureUnit[i] = FigureEntity->FigureUnit[i+1];
    }
    
    FigureEntity->FigureAmount -= 1;
}

static void
FigureUnitRotateShellBy(figure_unit *Entity, float Angle)
{
    if((s32)Entity->Angle == 0)
    {
        Entity->Angle = 0;
    }
    
    for (u32 i = 0; i < 4; ++i)
    {
        float Radians = Angle * (M_PI/180.0f);
        float Cos = cos(Radians);
        float Sin = sin(Radians);
        
        float X = Entity->Center.x + (Entity->Shell[i].x - Entity->Center.x) * Cos
            - (Entity->Shell[i].y - Entity->Center.y) * Sin;
        float Y = Entity->Center.y + (Entity->Shell[i].x - Entity->Center.x) * Sin
            + (Entity->Shell[i].y - Entity->Center.y) * Cos;
        
        Entity->Shell[i].x = roundf(X);
        Entity->Shell[i].y = roundf(Y);
    }
}

static void
FigureUnitInitFigure(figure_unit *FigureUnit, 
                     figure_form Form, 
                     figure_type Type,
                     r32 Angle,
                     game_memory *Memory, 
                     game_offscreen_buffer *Buffer)
{
    u32 ActiveBlockSize   = Memory->LevelEntity.Configuration.ActiveBlockSize;
    u32 InActiveBlockSize = Memory->LevelEntity.Configuration.InActiveBlockSize;
    
    FigureUnit->IsIdle       = true;
    FigureUnit->IsStick      = false;
    FigureUnit->IsEnlarged   = false;
    FigureUnit->Angle        = Angle;
    FigureUnit->DefaultAngle = 0.0f;
    FigureUnit->Form         = Form;
    FigureUnit->Type         = Type;
    FigureUnit->Flip         = SDL_FLIP_NONE;
    
    u32 RowAmount         = 0;
    u32 ColumnAmount      = 0;
    r32 CenterOffset      = 0.5f;
    
    vector<vector<s32>> matrix(2);
    for(u32 i = 0; i < 2; ++i)
    {
        matrix[i].resize(4);
    }
    
    game_texture *Texture = 0;
    
    switch(Form)
    {
        case I_figure:
        {
            matrix = 
            { 
                {1, 1, 1, 1},
                {0, 0, 0, 0} 
            };
            RowAmount    = 4;
            ColumnAmount = 1;
            if (Type == stone) Texture = GetTexture(Memory, "i_s.png", Buffer->Renderer);
            else if (Type == mirror) Texture = GetTexture(Memory, "i_m.png", Buffer->Renderer);
            else if (Type == classic) Texture = GetTexture(Memory, "i_d.png", Buffer->Renderer);
        } break;
        
        case O_figure:
        {
            matrix = 
            { 
                { 1, 1 },
                { 1, 1 } 
            };
            RowAmount    = 2;
            ColumnAmount = 2;
            if (Type == stone) Texture = GetTexture(Memory, "o_s.png", Buffer->Renderer);
            else if (Type == mirror) Texture = GetTexture(Memory, "o_m.png", Buffer->Renderer);
            else if (Type == classic) Texture = GetTexture(Memory, "o_d.png", Buffer->Renderer);
        }break;
        
        case Z_figure:
        {
            matrix =
            {
                {1, 1, 0}, 
                {0, 1, 1} 
            };
            RowAmount    = 3;
            ColumnAmount = 2;
            if (Type == stone) Texture = GetTexture(Memory, "z_s.png", Buffer->Renderer);
            else if (Type == mirror) Texture = GetTexture(Memory, "z_m.png", Buffer->Renderer);
            else if (Type == classic) Texture = GetTexture(Memory, "z_d.png", Buffer->Renderer);
        }break;
        
        case S_figure:
        {
            matrix =
            { 
                {0, 1, 1}, 
                {1, 1, 0} 
            };
            RowAmount    = 3;
            ColumnAmount = 2;
            if (Type == stone) Texture = GetTexture(Memory, "s_s.png", Buffer->Renderer);
            else if (Type == mirror) Texture = GetTexture(Memory, "s_m.png", Buffer->Renderer);
            else if (Type == classic) Texture = GetTexture(Memory, "s_d.png", Buffer->Renderer);
        }break;
        
        case T_figure:
        {
            matrix = 
            { 
                {0, 1, 0},
                {1, 1, 1}
            };
            CenterOffset = 0.75f;
            RowAmount    = 3;
            ColumnAmount = 2;
            if (Type == stone) Texture = GetTexture(Memory, "t_s.png", Buffer->Renderer);
            else if (Type == mirror) Texture = GetTexture(Memory, "t_m.png", Buffer->Renderer);
            else if (Type == classic) Texture = GetTexture(Memory, "t_d.png", Buffer->Renderer);
        }break;
        
        case L_figure:
        {
            matrix = 
            { 
                {0, 0, 1},
                {1, 1, 1}
            };
            CenterOffset = 0.75f;
            RowAmount    = 3;
            ColumnAmount = 2;
            if (Type == stone) Texture = GetTexture(Memory, "l_s.png", Buffer->Renderer);
            else if (Type == mirror) Texture = GetTexture(Memory, "l_m.png", Buffer->Renderer);
            else if (Type == classic) Texture = GetTexture(Memory, "l_d.png", Buffer->Renderer);
        }break;
        
        case J_figure:
        {
            matrix = 
            {
                {1, 0, 0},
                {1, 1, 1} 
            };
            CenterOffset = 0.75f;
            RowAmount    = 3;
            ColumnAmount = 2;
            if (Type == stone) Texture = GetTexture(Memory, "j_s.png", Buffer->Renderer);
            else if (Type == mirror) Texture = GetTexture(Memory, "j_m.png", Buffer->Renderer);
            else if (Type == classic) Texture = GetTexture(Memory, "j_d.png", Buffer->Renderer);
        }break;
    }
    
    FigureUnit->Texture = Texture;
    
    FigureUnit->AreaQuad.x = 0;
    FigureUnit->AreaQuad.y = 0;
    FigureUnit->AreaQuad.w = RowAmount * InActiveBlockSize;
    FigureUnit->AreaQuad.h = ColumnAmount * InActiveBlockSize;
    FigureUnit->Center.x   = FigureUnit->AreaQuad.x + (FigureUnit->AreaQuad.w / 2);
    FigureUnit->Center.y   = FigureUnit->AreaQuad.y + (FigureUnit->AreaQuad.h) * CenterOffset;
    FigureUnit->DefaultCenter = FigureUnit->Center;
    FigureUnit->DefaultAngle  = Angle;
    
    u32 ShellIndex = 0;
    u32 HalfBlock  = InActiveBlockSize >> 1;
    
    for(u32 i = 0; i < 2; i++)
    {
        for(u32 j = 0; j < 4; j++)
        {
            if(matrix[i][j] == 1)
            {
                FigureUnit->Shell[ShellIndex].x = FigureUnit->AreaQuad.x + (j * InActiveBlockSize) + HalfBlock;
                
                FigureUnit->Shell[ShellIndex].y = FigureUnit->AreaQuad.y + (i * InActiveBlockSize) + HalfBlock;
                
                FigureUnit->DefaultShell[ShellIndex] = FigureUnit->Shell[ShellIndex];
                
                ShellIndex++;
            }
        }
    }
    
    FigureUnitRotateShellBy(FigureUnit, FigureUnit->Angle);
}

static void
FigureUnitAddNewFigure(figure_entity *FigureEntity, 
                       figure_form Form, figure_type Type, r32 Angle, 
                       game_memory *Memory, game_offscreen_buffer *Buffer)
{
    if(FigureEntity->FigureAmount >= FigureEntity->FigureAmountReserved) return;
    
    u32 Index = FigureEntity->FigureAmount;
    FigureEntity->FigureAmount += 1;
    
    FigureUnitInitFigure(&FigureEntity->FigureUnit[Index], Form, Type, Angle, Memory, Buffer);
}

static void
GridEntityUpdateStickUnits(grid_entity *GridEntity, u32 FigureAmount)
{
    if(GridEntity->StickUnits)
    {
        free(GridEntity->StickUnits);
    }
    
    GridEntity->StickUnitsAmount = FigureAmount;
    
    GridEntity->StickUnits = (sticked_unit *) malloc(sizeof(sticked_unit) * FigureAmount);
    Assert(GridEntity->StickUnits);
    
    for(u32 i = 0; i < FigureAmount; ++i)
    {
        GridEntity->StickUnits[i].Index = -1;
        GridEntity->StickUnits[i].IsSticked = false;
    }
}

static game_rect
FigureUnitGetArea(figure_unit *Unit)
{
    game_rect Area = {Unit->Shell[0].x, Unit->Shell[0].y, -500, -500};
    u32 OffsetX = 0;
    u32 OffsetY = 0;
    bool ZeroArea = false;
    
    for (u32 i = 0; i < 4; ++i)
    {
        if(Area.x >= Unit->Shell[i].x) Area.x = Unit->Shell[i].x;
        if(Area.y >= Unit->Shell[i].y) Area.y = Unit->Shell[i].y;
        if(Area.w <= Unit->Shell[i].x) Area.w = Unit->Shell[i].x;
        if(Area.h <= Unit->Shell[i].y) Area.h = Unit->Shell[i].y;
        
    }
    
    Area.w -= Area.x;
    Area.h -= Area.y;
    
    if(Area.w == 0 || Area.h == 0)
    {
        ZeroArea = true;
    }
    
    if(Area.w >= Unit->AreaQuad.h)
    {
        Area.h = Unit->AreaQuad.h;
        Area.w = Unit->AreaQuad.w;
        OffsetX = (Area.y + Area.h) - (Area.y + Area.h / 2);
    }
    else
    {
        Area.h = Unit->AreaQuad.w;
        Area.w = Unit->AreaQuad.h;
        OffsetX = (Area.x + Area.w) - (Area.x + Area.w / 2);
    }
    
    if(!ZeroArea)
    {
        OffsetX /= 2;
    }
    
    Area.x -= OffsetX;
    Area.y -= OffsetX;
    
    return(Area);
}



static void
FigureUnitFlipHorizontally(figure_unit *Unit)
{
    u32 NewX = 0;
    u32 NewY = 0;
    u32 NewCenterX = 0;
    u32 NewCenterY = 0;
    game_rect AreaQuad = FigureUnitGetArea(Unit);
    
    if(Unit->Flip != SDL_FLIP_HORIZONTAL)
    {
        Unit->Flip = SDL_FLIP_HORIZONTAL;
    }
    else
    {
        Unit->Flip = SDL_FLIP_NONE;
    }
    
    if(AreaQuad.w > AreaQuad.h)
    {
        for (u32 i = 0; i < 4; ++i)
        {
            NewX = (AreaQuad.x + AreaQuad.w) - (Unit->Shell[i].x - AreaQuad.x);
            Unit->Shell[i].x = NewX;
        }
        
        NewCenterX = (AreaQuad.x + AreaQuad.w) - (Unit->Center.x - AreaQuad.x);
        Unit->Center.x = NewCenterX;
    }
    else
    {
        for (u32 i = 0; i < 4; ++i)
        {
            NewY = (AreaQuad.y + AreaQuad.h) - (Unit->Shell[i].y - AreaQuad.y);
            Unit->Shell[i].y = NewY;
        }
        
        NewCenterY = (AreaQuad.y + AreaQuad.h) - (Unit->Center.y - AreaQuad.y);
        Unit->Center.y = NewCenterY;
    }
}


static void
FigureUnitMove(figure_unit *Entity, s32 XShift, s32 YShift)
{
    int XOffset = Entity->AreaQuad.x - Entity->Center.x;
    int YOffset = Entity->AreaQuad.y - Entity->Center.y;
    
    Entity->Center.x += XShift;
    Entity->Center.y += YShift;
    
    Entity->AreaQuad.x = Entity->Center.x + XOffset;
    Entity->AreaQuad.y = Entity->Center.y + YOffset;
    
    for (u32 i = 0; i < 4; ++i)
    {
        Entity->Shell[i].x += XShift;
        Entity->Shell[i].y += YShift;
    }
}

static void
FigureUnitMoveTo(figure_unit *Entity, s32 NewPointX, s32 NewPointY)
{
    s32 XShift = NewPointX - Entity->Center.x;
    s32 YShift = NewPointY - Entity->Center.y;
    FigureUnitMove(Entity, XShift, YShift);
}

static void
DestroyFigureEntity(figure_unit *Entity)
{
    if(Entity)
    {
        FreeTexture(Entity->Texture);
        free(Entity);
    }
}

static void
FigureUnitRenderBitmap(game_offscreen_buffer *Buffer, figure_unit *Entity)
{
    game_point Center;
    Center.x = Entity->Center.x - Entity->AreaQuad.x;
    Center.y = Entity->Center.y - Entity->AreaQuad.y;
    
    SDL_RenderCopyEx(Buffer->Renderer, Entity->Texture,
                     0, &Entity->AreaQuad, Entity->Angle, &Center, Entity->Flip);
}


static void
FigureUnitSetToDefaultArea(figure_unit* Unit, r32 BlockRatio)
{
    s32 ShiftX = 0;
    s32 ShiftY = 0;
    
    if(Unit->IsEnlarged)
    {
        r32 AngleDt      = 0;
        r32 Angle        = Unit->Angle;
        r32 DefaultAngle = Unit->DefaultAngle;
        
        if(Unit->Angle != Unit->DefaultAngle)
        {
            AngleDt = DefaultAngle - Angle;
            FigureUnitRotateShellBy(Unit, AngleDt);
            Unit->Angle = DefaultAngle;
        }
        
        FigureUnitResizeBy(Unit, BlockRatio);
        
        Unit->IsEnlarged = false;
    }
}

static void
FigureUnitDefineDefaultArea(figure_unit *Unit, s32 X, s32 Y)
{
    game_rect AreaQuad = FigureUnitGetArea(Unit);
    s32 ShiftX = X - AreaQuad.x;
    s32 ShiftY = Y - AreaQuad.y;
    
    FigureUnitMove(Unit, ShiftX, ShiftY);
    
    for (u32 i = 0; i < 4; ++i)
    {
        Unit->DefaultShell[i] = Unit->Shell[i];
    }
    
    Unit->DefaultCenter = Unit->Center;
    Unit->DefaultAngle  = Unit->Angle;
}

static void
FigureUnitMoveToDefaultArea(figure_unit *FigureUnit, u32 ActiveBlockSize)
{
    game_point Center        = FigureUnit->Center;
    game_point DefaultCenter = FigureUnit->DefaultCenter;
    
    r32 OffsetX   = 0.0f;
    r32 OffsetY   = 0.0f;
    r32 Magnitude = 0.0f;
    r32 MaxSpeed  = 0.0f;
    
    OffsetX = DefaultCenter.x - Center.x;
    OffsetY = DefaultCenter.y - Center.y;
    
    Magnitude = sqrt(OffsetX*OffsetX + OffsetY*OffsetY);
    MaxSpeed = Magnitude - ActiveBlockSize;
    if(Magnitude > (MaxSpeed))
    {
        if(Magnitude != 0) 
        {
            OffsetX /= Magnitude;
            OffsetY /= Magnitude;
        }
        
        OffsetX *= MaxSpeed;
        OffsetY *= MaxSpeed;
        
        OffsetX = roundf(OffsetX);
        OffsetY = roundf(OffsetY);
        FigureUnitMove(FigureUnit, OffsetX, OffsetY);
    }
}

static void
GridEntityAddMovingBlock(grid_entity *GridEntity,
                         u32 RowNumber, u32 ColNumber, 
                         bool IsVertical, bool MoveSwitch, 
                         u32 ActiveBlockSize)
{
    if(GridEntity->MovingBlocksAmount >= GridEntity->MovingBlocksAmountReserved) return;
    if((RowNumber >= GridEntity->RowAmount) || RowNumber < 0) return;
    if((ColNumber >= GridEntity->ColumnAmount) || ColNumber < 0) return;
    
    u32 Index = GridEntity->MovingBlocksAmount;
    
    GridEntity->MovingBlocks[Index].AreaQuad.w = ActiveBlockSize;
    GridEntity->MovingBlocks[Index].AreaQuad.h = ActiveBlockSize;
    GridEntity->MovingBlocks[Index].AreaQuad.x = GridEntity->GridArea.x + (ColNumber * ActiveBlockSize);
    GridEntity->MovingBlocks[Index].AreaQuad.y = GridEntity->GridArea.y + (RowNumber * ActiveBlockSize);
    
    GridEntity->MovingBlocks[Index].IsMoving   = false;
    GridEntity->MovingBlocks[Index].MoveSwitch = MoveSwitch;
    GridEntity->MovingBlocks[Index].IsVertical = IsVertical;
    GridEntity->MovingBlocks[Index].RowNumber  = RowNumber;
    GridEntity->MovingBlocks[Index].ColNumber  = ColNumber;
    
    GridEntity->UnitField[RowNumber][ColNumber] = IsVertical ? 3 : 2;
    
    GridEntity->MovingBlocksAmount += 1;
}

static void
GridEntityMoveBlockHorizontally(grid_entity *GridEntity, moving_block *MovingBlock)
{
    s32 NewColNumber = 0;
    u32 RowNumber = MovingBlock->RowNumber;
    u32 ColNumber = MovingBlock->ColNumber;
    u32 ActiveBlockSize = MovingBlock->AreaQuad.w;
    
    NewColNumber = MovingBlock->MoveSwitch
        ? NewColNumber = ColNumber + 1
        : NewColNumber = ColNumber - 1;
    
    if(NewColNumber < 0 || NewColNumber >= GridEntity->ColumnAmount) return;
    if(GridEntity->UnitField[RowNumber][NewColNumber] != 0) return;
    
    GridEntity->UnitField[RowNumber][ColNumber]    = 0;
    GridEntity->UnitField[RowNumber][NewColNumber] = 2;
    
    MovingBlock->ColNumber = NewColNumber;
    
    MovingBlock->MoveSwitch = MovingBlock->MoveSwitch == false ? true : false;
    MovingBlock->IsMoving = true;
}

static void
GridEntityMoveBlockVertically(grid_entity *GridEntity, moving_block *MovingBlock)
{
    s32 NewRowNumber = 0;
    u32 RowNumber = MovingBlock->RowNumber;
    u32 ColNumber = MovingBlock->ColNumber;
    u32 ActiveBlockSize = MovingBlock->AreaQuad.w;
    
    NewRowNumber = MovingBlock->MoveSwitch
        ? NewRowNumber = RowNumber + 1
        : NewRowNumber = RowNumber - 1;
    
    if(NewRowNumber < 0 || NewRowNumber >= GridEntity->RowAmount) return;
    if(GridEntity->UnitField[NewRowNumber][ColNumber] != 0) return;
    
    GridEntity->UnitField[RowNumber][ColNumber]    = 0;
    GridEntity->UnitField[NewRowNumber][ColNumber] = 3;
    
    MovingBlock->RowNumber = NewRowNumber;
    
    MovingBlock->MoveSwitch = MovingBlock->MoveSwitch == false ? true : false;
    MovingBlock->IsMoving = true;
}



static void
RestartLevelEntity(level_entity *LevelEntity)
{
    grid_entity   *&GridEntity   = LevelEntity->GridEntity;
    figure_entity *&FigureEntity = LevelEntity->FigureEntity;
    
    
    if(FigureEntity->IsRotating || FigureEntity->IsFlipping) return;
    
    r32 BlockRatio        = 0;
    u32 RowAmount         = 0;
    u32 ColumnAmount      = 0;
    u32 FigureAmount      = FigureEntity->FigureAmount;
    r32 ActiveBlockSize   = LevelEntity->Configuration.ActiveBlockSize;
    r32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
    
    FigureEntity->IsRestarting = true;
    
    for(u32 i = 0; i < FigureAmount; ++i){
        if(!FigureEntity->FigureUnit[i].IsIdle){
            
            FigureEntity->FigureUnit[i].IsStick = false;
            BlockRatio = InActiveBlockSize / ActiveBlockSize;
            FigureUnitSetToDefaultArea(&FigureEntity->FigureUnit[i], BlockRatio);
            FigureUnitMoveToDefaultArea(&FigureEntity->FigureUnit[i], ActiveBlockSize);
            
            printf("BlockRatio = %f\n" ,BlockRatio);
        }
    }
    
    for(u32 i = 0; i < GridEntity->StickUnitsAmount; ++i)
    {
        s32 Index = GridEntity->StickUnits[i].Index;
        
        if(GridEntity->StickUnits[i].IsSticked && Index >= 0)
        {
            s32 RowIndex = 0;
            s32 ColIndex = 0;
            
            for(u32 l = 0; l < 4; l++)
            {
                RowIndex = GridEntity->StickUnits[i].Row[l];
                ColIndex = GridEntity->StickUnits[i].Col[l];
                GridEntity->UnitField[RowIndex][ColIndex] = 0;
            }
            
            GridEntity->StickUnits[i].Index = -1;
        }
    }
}

static void
GameUpdateEvent(game_input *Input, level_entity *GameState,
                r32 ActiveBlockSize, r32 DefaultBlockSize,
                u32 ScreenWidth, u32 ScreenHeight)
{
    if(!GameState->LevelStarted) return;
    
    grid_entity   *&GridEntity   = GameState->GridEntity;
    figure_entity *&FigureEntity = GameState->FigureEntity;
    figure_unit   *FigureUnit    = FigureEntity->FigureUnit;
    
    u32 Size        = FigureEntity->FigureAmount;
    s32 MouseX      = Input->MouseX;
    s32 MouseY      = Input->MouseY;
    r32 BlockRatio  = 0;
    u32 ActiveIndex = FigureEntity->FigureActive;
    
    //NOTE(Max): This is for the switch between editor and the game
    if(Input->Keyboard.BackQuote.EndedDown)
    {
        if(!GameState->LevelPaused)
        {
            GameState->LevelPaused = true;
            RestartLevelEntity(GameState);
        }
        else
        {
            GameState->LevelPaused = false;
        }
    }
    
    if(GameState->LevelPaused)
    {
        return;
    }
    
    if(Input->MouseButtons[0].EndedDown)
    {
        if(!FigureEntity->IsGrabbed)
        {
            for (s32 i = Size-1; i >= 0; --i)
            {
                ActiveIndex = FigureEntity->FigureOrder[i];
                game_rect AreaQuad = FigureUnitGetArea(&FigureUnit[ActiveIndex]);
                if(IsPointInsideRect(MouseX, MouseY, &AreaQuad))
                {
                    game_rect ShellQuad = {0};
                    ShellQuad.w = FigureUnit[ActiveIndex].IsEnlarged ? ActiveBlockSize : DefaultBlockSize;
                    ShellQuad.h = FigureUnit[ActiveIndex].IsEnlarged ? ActiveBlockSize : DefaultBlockSize;
                    for(u32 j = 0; j < 4; ++j)
                    {
                        ShellQuad.x = FigureUnit[ActiveIndex].Shell[j].x - (ShellQuad.w / 2);
                        ShellQuad.y = FigureUnit[ActiveIndex].Shell[j].y - (ShellQuad.h / 2);
                        if(IsPointInsideRect(MouseX, MouseY, &ShellQuad))
                        {
                            FigureEntity->IsGrabbed = true;
                            
                            if(!FigureUnit[ActiveIndex].IsEnlarged)
                            {
                                BlockRatio = ActiveBlockSize / DefaultBlockSize;
                                FigureUnit[ActiveIndex].IsIdle = false;
                                FigureUnit[ActiveIndex].IsEnlarged = true;
                                FigureUnitResizeBy(&FigureUnit[ActiveIndex], BlockRatio);
                            }
                            
                            FigureEntity->FigureActive = ActiveIndex;
                            FigureEntityHighOrderFigure(FigureEntity, ActiveIndex);
                            SDL_ShowCursor(SDL_DISABLE);
                            //DEBUGPrintEntityOrder(Group);
                            
                            return;
                        }
                    }
                }
            }
            
            if(IsPointInsideRect(MouseX, MouseY, &GridEntity->GridArea))
            {
                for(u32 i = 0; i < GridEntity->MovingBlocksAmount; i++)
                {
                    if(IsPointInsideRect(MouseX, MouseY, &GridEntity->MovingBlocks[i].AreaQuad))
                    {
                        if(!GridEntity->MovingBlocks[i].IsVertical) 
                        {
                            GridEntityMoveBlockHorizontally(GridEntity, &GridEntity->MovingBlocks[i]);
                            printf("index of moving block = %d\n", i);
                        }
                        else
                        {
                            GridEntityMoveBlockVertically(GridEntity, &GridEntity->MovingBlocks[i]);
                        }
                        
                        return;
                    }
                }
            }
        }
        else
        {
            if(!FigureEntity->IsRotating && !FigureEntity->IsFlipping)
            {
                game_rect ScreenArea = {0};
                ScreenArea.w = ScreenWidth;
                ScreenArea.h = ScreenHeight;
                
                if(IsFigureUnitInsideRect(&FigureUnit[ActiveIndex], &FigureEntity->FigureArea) || 
                   (!IsPointInsideRect(FigureUnit[ActiveIndex].Center.x,FigureUnit[ActiveIndex].Center.y, &ScreenArea)))
                {
                    FigureUnit[ActiveIndex].IsIdle = true;
                    BlockRatio = DefaultBlockSize / ActiveBlockSize;
                    FigureUnitSetToDefaultArea(&FigureUnit[ActiveIndex], BlockRatio);
                    
                    FigureEntity->IsReturning = true;
                    FigureEntity->ReturnIndex = ActiveIndex;
                    
                    FigureUnitMoveToDefaultArea(&FigureUnit[ActiveIndex], ActiveBlockSize);
                }
                
                SDL_ShowCursor(SDL_ENABLE);
                FigureEntity->IsGrabbed = false;
                FigureEntity->FigureActive = -1;
            }
        }
    }
    if(Input->MouseButtons[1].EndedDown)
    {
        if(FigureEntity->IsGrabbed)
        {
            figure_type Type = FigureUnit[ActiveIndex].Type;
            switch(Type)
            {
                case classic:
                {
                    if (!FigureEntity->IsRotating)
                    {
                        FigureEntity->IsRotating = true;
                        FigureUnitRotateShellBy(&FigureUnit[ActiveIndex], 90);
                    }
                } break;
                case mirror:
                {
                    if (!FigureEntity->IsFlipping)
                    {
                        
                        SDL_SetTextureBlendMode(FigureUnit[ActiveIndex].Texture, SDL_BLENDMODE_BLEND);
                        
                        FigureEntity->IsFlipping = true;
                        FigureEntity->FigureAlpha= 255;
                        FigureEntity->FadeInSum  = 255;
                        FigureEntity->FadeOutSum = 0;
                    }
                } break;
                
                case stone:
                {
                    
                } break;
            }
        }
    } 
    if(FigureEntity->IsGrabbed)
    {
        s32 x = Input->MouseRelX;
        s32 y = Input->MouseRelY;
        FigureUnitMove(&FigureUnit[ActiveIndex], x, y);
        
    }
}


static void
FigureEntityAlignHorizontally(figure_entity* Entity, u32 BlockSize)
{
    u32 Size = Entity->FigureAmount;
    u32 RowSize1 = 0;
    u32 RowSize2 = 0;
    u32 FigureIntervalX = BlockSize / 4;
    u32 FigureIntervalY = BlockSize / 6;
    u32 FigureWidth     = 0;
    u32 FigureHeight    = 0;
    
    game_rect AreaQuad    = {};
    game_rect DefaultZone = Entity->FigureArea;
    
    for (u32 i = 0; i < Size; ++i)
    {
        AreaQuad = FigureUnitGetArea(&Entity->FigureUnit[i]);
        i % 2 == 0
            ? RowSize1 += AreaQuad.w + FigureIntervalX
            : RowSize2 += AreaQuad.w + FigureIntervalX;
    }
    
    u32 PitchY          = 0;
    s32 NewPositionX    = 0;
    s32 NewPositionY    = 0;
    u32 CurrentRowSize1 = 0;
    u32 CurrentRowSize2 = 0;
    u32 FigureBoxHeight = 0;
    
    for (u32 i = 0; i < Size; ++i)
    {
        PitchY = i % 2;
        AreaQuad = FigureUnitGetArea(&Entity->FigureUnit[i]);
        
        FigureBoxHeight = BlockSize * 4;
        NewPositionY = DefaultZone.y + (FigureBoxHeight * PitchY);
        NewPositionY += (FigureBoxHeight / 2 ) - (AreaQuad.h / 2);
        NewPositionY += FigureIntervalY * PitchY;
        
        if(i % 2 == 0)
        {
            NewPositionX = (DefaultZone.x + (DefaultZone.w / 2) - RowSize1 / 2);
            NewPositionX += CurrentRowSize1;
            
            CurrentRowSize1 += AreaQuad.w + FigureIntervalX;
        }
        else
        {
            NewPositionX = (DefaultZone.x + (DefaultZone.w / 2)) - (RowSize2 / 2);
            NewPositionX += CurrentRowSize2;
            
            CurrentRowSize2 += AreaQuad.w + FigureIntervalX;
        }
        
        FigureUnitDefineDefaultArea(&Entity->FigureUnit[i], NewPositionX, NewPositionY);
    }
}

inline static bool
Change1DUnitPerSec(r32 *Unit, r32 MaxValue, r32 ChangePerSec, r32 TimeElapsed)
{
    if(!Unit) return false;
    
    bool IsFinished = true;
    r32 UnitValue = *Unit;
    
    if(MaxValue > 0)
    {
        
        if(UnitValue < MaxValue)
        {
            UnitValue += TimeElapsed * ChangePerSec;
            UnitValue = roundf(UnitValue);
            
            if(UnitValue >= MaxValue) UnitValue = MaxValue;
            
            *Unit = roundf(UnitValue);
            IsFinished = false;
        }
    }
    else
    {
        
        if(UnitValue > MaxValue)
        {
            UnitValue -= TimeElapsed * ChangePerSec;
            UnitValue = roundf(UnitValue);
            
            if(UnitValue <= MaxValue) UnitValue = MaxValue;
            
            *Unit = roundf(UnitValue);
            IsFinished = false;
        }
    }
    
    return(IsFinished);
}

inline static vector2
Move2DPointPerSec(game_point *p1, game_point *p2, r32 MaxVelocity, r32 TimeElapsed)
{
    r32 Distance;
    vector2 Velocity;
    
    Velocity.x= p2->x - p1->x;
    Velocity.y= p2->y - p1->y;
    
    Distance = sqrt(Velocity.x * Velocity.x + Velocity.y * Velocity.y);
    if(Distance > MaxVelocity)
    {
        Velocity.x = Velocity.x / Distance;
        Velocity.y = Velocity.y / Distance;
        
        Velocity.x = Velocity.x * MaxVelocity;
        Velocity.y = Velocity.y * MaxVelocity;
    }
    
    Velocity.x = roundf(Velocity.x);
    Velocity.y = roundf(Velocity.y);
    
    return(Velocity);
}

static void
RescaleGameField(game_offscreen_buffer *Buffer,
                 u32 RowAmount, u32 ColumnAmount, u32 FigureAmount,
                 u32 DefaultBlocksInRow, u32 DefaultBlocksInCol,
                 level_entity *LevelEntity)
{
    u32 InActiveBlockSize  = 0;
    u32 ActiveBlockSize    = 0;
    
    u32 FigureAreaWidth  = Buffer->Width;
    u32 FigureAreaHeight = Buffer->Height * 0.4f;
    
    u32 BlocksInRow = (FigureAmount / 2.0) + 0.5f;
    BlocksInRow     = (BlocksInRow * 2) + 2;
    
    InActiveBlockSize = GameResizeInActiveBlock(FigureAreaWidth, FigureAreaHeight,
                                                DefaultBlocksInRow, DefaultBlocksInCol,BlocksInRow);
    FigureAreaHeight = InActiveBlockSize * DefaultBlocksInCol;
    
    u32 GridAreaWidth  = Buffer->Width;
    u32 GridAreaHeight = Buffer->Height - FigureAreaHeight;
    
    ActiveBlockSize = GameResizeActiveBlock(GridAreaWidth, GridAreaHeight, RowAmount, ColumnAmount);
    
    LevelEntity->Configuration.ActiveBlockSize   = ActiveBlockSize;
    LevelEntity->Configuration.InActiveBlockSize = InActiveBlockSize;
}

static void
LevelEntityUpdateLevelNumber(level_entity *LevelEntity, game_memory *Memory, game_offscreen_buffer *Buffer)
{
    //TODO(Max): Elaborate on where exactly should I draw level number
    
    char LevelNumberString[3] = {0};
    sprintf(LevelNumberString, "%d", LevelEntity->LevelNumber);
    
    game_surface *Surface = TTF_RenderUTF8_Blended(Memory->LevelNumberFont, LevelNumberString, {255, 255, 255});
    Assert(Surface);
    
    LevelEntity->LevelNumberQuad.w = Surface->w;
    LevelEntity->LevelNumberQuad.h = Surface->h;
    LevelEntity->LevelNumberQuad.x = (Buffer->Width - Surface->w) - (Surface->w /2);
    LevelEntity->LevelNumberQuad.y = 0;
    
    LevelEntity->LevelNumberShadowQuad = LevelEntity->LevelNumberQuad;
    LevelEntity->LevelNumberShadowQuad.x += 3;
    LevelEntity->LevelNumberShadowQuad.y += 3;
    
    LevelEntity->LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    Assert(LevelEntity->LevelNumberTexture);
    
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(Memory->LevelNumberFont, LevelNumberString, {0, 0, 0});
    Assert(Surface);
    
    LevelEntity->LevelNumberShadowTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    Assert(LevelEntity->LevelNumberShadowTexture);
    
    SDL_FreeSurface(Surface);
    
}

static void
LevelEntityUpdateLevelEntityFromMemory(level_entity *LevelEntity, 
                                       s32 Index, bool IsStarted,
                                       game_memory *Memory, game_offscreen_buffer *Buffer)
{
    /* level_entity deallocating memory */
    
    if(LevelEntity->LevelNumberTexture)
    {
        SDL_DestroyTexture(LevelEntity->LevelNumberTexture);
        LevelEntity->LevelNumberTexture = NULL;
    }
    
    if(LevelEntity->LevelNumberShadowTexture)
    {
        SDL_DestroyTexture(LevelEntity->LevelNumberShadowTexture);
        LevelEntity->LevelNumberTexture = NULL;
    }
    
    if(LevelEntity->FigureEntity->FigureOrder)
    {
        free(LevelEntity->FigureEntity->FigureOrder);
        LevelEntity->FigureEntity->FigureOrder = 0;
    }
    
    for(u32 i = 0; i < LevelEntity->FigureEntity->FigureAmount; ++i)
    {
        if(LevelEntity->FigureEntity->FigureUnit[i].Texture)
        {
            FreeTexture(LevelEntity->FigureEntity->FigureUnit[i].Texture);
            LevelEntity->FigureEntity->FigureUnit[i].Texture = 0;
        }
    }
    
    if(LevelEntity->FigureEntity->FigureUnit)
    {
        free(LevelEntity->FigureEntity->FigureUnit);
        LevelEntity->FigureEntity->FigureUnit = 0;
    }
    
    if(LevelEntity->GridEntity->UnitSize)
    {
        for(u32 i = 0; i < LevelEntity->GridEntity->RowAmount; ++i)
        {
            free(LevelEntity->GridEntity->UnitSize[i]);
        }
        
        free(LevelEntity->GridEntity->UnitSize);
        LevelEntity->GridEntity->UnitSize = 0;
    }
    
    if(LevelEntity->GridEntity->UnitField)
    {
        for(u32 i = 0; i < LevelEntity->GridEntity->RowAmount; ++i)
        {
            free(LevelEntity->GridEntity->UnitField[i]);
        }
        
        free(LevelEntity->GridEntity->UnitField);
        LevelEntity->GridEntity->UnitField = 0;
    }
    
    if(LevelEntity->GridEntity->StickUnits)
    {
        free(LevelEntity->GridEntity->StickUnits);
        LevelEntity->GridEntity->StickUnits = 0;
    }
    
    if(LevelEntity->GridEntity->MovingBlocks)
    {
        free(LevelEntity->GridEntity->MovingBlocks);
        LevelEntity->GridEntity->MovingBlocks = 0;
    }
    
    
    u32 RowAmount    = Memory->LevelMemory[Index].RowAmount;
    u32 ColumnAmount = Memory->LevelMemory[Index].ColumnAmount;
    
    LevelEntity->Configuration.PixelsDrawn  = 0;
    LevelEntity->Configuration.PixelsToDraw = 0;
    LevelEntity->Configuration.StartUpTimeElapsed  = 0.0f;
    LevelEntity->Configuration.StartUpTimeToFinish = 1.0f;
    
    LevelEntity->LevelNumber   = Memory->LevelMemory[Index].LevelNumber;
    LevelEntity->LevelStarted  = IsStarted;
    LevelEntity->LevelFinished = false;
    
    RescaleGameField(Buffer, RowAmount, ColumnAmount,
                     LevelEntity->FigureEntity->FigureAmountReserved, LevelEntity->Configuration.DefaultBlocksInRow, LevelEntity->Configuration.DefaultBlocksInCol, LevelEntity);
    
    u32 ActiveBlockSize   = LevelEntity->Configuration.ActiveBlockSize;
    u32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
    
    LevelEntity->Configuration.GridScalePerSec  = (ActiveBlockSize * ((RowAmount + ColumnAmount) - 1)) / (LevelEntity->Configuration.StartUpTimeToFinish * 0.5f);
    LevelEntity->Configuration.StartAlphaPerSec = 255.0f / (LevelEntity->Configuration.StartUpTimeToFinish * 0.5f);
    
    /*
    
    figure_entity initialization
    
    */
    
    LevelEntity->FigureEntity->FigureAlpha = 0;
    
    LevelEntity->FigureEntity->FigureArea.w = Buffer->Width;
    LevelEntity->FigureEntity->FigureArea.h = InActiveBlockSize * LevelEntity->Configuration.DefaultBlocksInCol;
    LevelEntity->FigureEntity->FigureArea.y = Buffer->Height - (LevelEntity->FigureEntity->FigureArea.h);
    LevelEntity->FigureEntity->FigureArea.x = 0;
    
    LevelEntity->FigureEntity->FigureAmount = 0;
    LevelEntity->FigureEntity->FigureUnit = (figure_unit *) malloc(sizeof(figure_unit) * LevelEntity->FigureEntity->FigureAmountReserved);
    Assert(LevelEntity->FigureEntity->FigureUnit);
    
    for(u32 i = 0; i < Memory->LevelMemory[Index].FigureAmount; ++i)
    {
        FigureUnitAddNewFigure(LevelEntity->FigureEntity, Memory->LevelMemory[Index].Figures[i].Form, Memory->LevelMemory[Index].Figures[i].Type, Memory->LevelMemory[Index].Figures[i].Angle, Memory, Buffer);
    }
    
    LevelEntity->FigureEntity->FigureOrder = (u32 *) malloc(sizeof(u32) * LevelEntity->FigureEntity->FigureAmountReserved);
    Assert(LevelEntity->FigureEntity->FigureOrder);
    
    for(u32 i = 0; 
        i < LevelEntity->FigureEntity->FigureAmountReserved;
        ++i)
    {
        LevelEntity->FigureEntity->FigureOrder[i] = i;
    }
    
    /*
    
    grid_entity initialization
    
    */
    
    LevelEntity->GridEntity->RowAmount    = RowAmount;
    LevelEntity->GridEntity->ColumnAmount = ColumnAmount;
    
    LevelEntity->GridEntity->GridArea.w   = ActiveBlockSize * ColumnAmount;
    LevelEntity->GridEntity->GridArea.h   = ActiveBlockSize * RowAmount;
    LevelEntity->GridEntity->GridArea.x   = (Buffer->Width / 2) - (LevelEntity->GridEntity->GridArea.w / 2);
    LevelEntity->GridEntity->GridArea.y   = (Buffer->Height - LevelEntity->FigureEntity->FigureArea.h) / 2 - (LevelEntity->GridEntity->GridArea.h / 2);
    
    LevelEntity->GridEntity->UnitField = (s32 **) malloc(sizeof(s32 *) * RowAmount);
    Assert(LevelEntity->GridEntity->UnitField);
    
    u32 UnitIndex = 0;
    for(u32 i = 0; i < RowAmount; ++i)
    {
        LevelEntity->GridEntity->UnitField[i] = (s32 *) malloc(sizeof(s32) * ColumnAmount);
        Assert(LevelEntity->GridEntity->UnitField[i]);
        for(u32 j = 0; j < ColumnAmount; ++j)
        {
            LevelEntity->GridEntity->UnitField[i][j] = Memory->LevelMemory[Index].UnitField[UnitIndex];
            
            UnitIndex += 1;
        }
    }
    
    if(!IsStarted)
    {
        LevelEntity->GridEntity->UnitSize = (r32 **) malloc(RowAmount * sizeof(r32 *));
        Assert(LevelEntity->GridEntity->UnitSize);
        
        for(u32 i = 0; i < RowAmount; ++i)
        {
            LevelEntity->GridEntity->UnitSize[i] = (r32 *) malloc(sizeof(r32) * ColumnAmount);
            Assert(LevelEntity->GridEntity->UnitSize[i]);
            for(u32 j = 0; j < ColumnAmount; ++j)
            {
                LevelEntity->GridEntity->UnitSize[i][j] = 0;
            }
        }
    }
    
    FigureEntityAlignHorizontally(LevelEntity->FigureEntity, InActiveBlockSize);
    
    LevelEntity->GridEntity->StickUnitsAmount = LevelEntity->FigureEntity->FigureAmount;
    LevelEntity->GridEntity->StickUnits = (sticked_unit *) malloc(sizeof(sticked_unit) * LevelEntity->FigureEntity->FigureAmount);
    Assert(LevelEntity->GridEntity->StickUnits);
    for(u32 i = 0; i < LevelEntity->FigureEntity->FigureAmount; ++i)
    {
        LevelEntity->GridEntity->StickUnits[i].Index = -1;
        LevelEntity->GridEntity->StickUnits[i].IsSticked = false;
    }
    
    //
    // Moving Blocks
    //
    
    LevelEntity->GridEntity->MovingBlocksAmount = 0;
    LevelEntity->GridEntity->MovingBlocks = (moving_block *) malloc(sizeof(moving_block) * LevelEntity->GridEntity->MovingBlocksAmountReserved);
    Assert(LevelEntity->GridEntity->MovingBlocks);
    
    for(u32 i = 0; i < Memory->LevelMemory[Index].MovingBlocksAmount; ++i)
    {
        u32 RowNumber   = Memory->LevelMemory[Index].MovingBlocks[i].RowNumber;
        u32 ColNumber   = Memory->LevelMemory[Index].MovingBlocks[i].ColNumber;
        bool IsVertical = Memory->LevelMemory[Index].MovingBlocks[i].IsVertical;
        bool MoveSwitch = Memory->LevelMemory[Index].MovingBlocks[i].MoveSwitch;
        
        GridEntityAddMovingBlock(LevelEntity->GridEntity, RowNumber, ColNumber, IsVertical, MoveSwitch, ActiveBlockSize);
    }
    
    //
    // level number texture loading
    //
    
    LevelEntityUpdateLevelNumber(LevelEntity, Memory, Buffer);
}

static void
LevelEntityUpdateStartUpAnimation(level_entity *LevelEntity,
                                  game_offscreen_buffer *Buffer, r32 TimeElapsed)

{
    if(LevelEntity->LevelStarted) 
    {
        return;
    }
    
    r32 PixelsDrawn = LevelEntity->Configuration.PixelsDrawn;
    r32 PixelsToDraw = LevelEntity->Configuration.PixelsToDraw;
    s32 MaximumBlockSize = LevelEntity->Configuration.ActiveBlockSize;
    r32 StartAlphaPerSec = LevelEntity->Configuration.StartAlphaPerSec;
    r32 StartUpTimeElapsed = LevelEntity->Configuration.StartUpTimeElapsed;
    r32 StartUpTimeToFinish = LevelEntity->Configuration.StartUpTimeToFinish;
    
    grid_entity   *&GridEntity   = LevelEntity->GridEntity;
    figure_entity *&FigureEntity = LevelEntity->FigureEntity;
    
    s32 RowAmount = GridEntity->RowAmount;
    s32 ColAmount = GridEntity->ColumnAmount;
    s32 FigureAmount = FigureEntity->FigureAmount;
    s32 GridAreaX = GridEntity->GridArea.x;
    s32 GridAreaY = GridEntity->GridArea.y;
    
    s32 AmountOfPixels = (MaximumBlockSize * (RowAmount + ColAmount - 1));
    s32 PixelScalePerSec = (AmountOfPixels / (StartUpTimeToFinish * 0.5f));
    
    //todo: change it to 1d array!!!
    //r32 **UnitSize  = GridEntity->UnitSize;
    //s32 **UnitField = GridEntity->UnitField;
    
    bool IsGridReady    = true;
    bool IsFiguresReady = true;
    bool IsLevelReady   = true;
    
    StartUpTimeElapsed += TimeElapsed;
    
    /* 
    
    Grid Start up animation
    
    */
    for(s32 Line = 1; Line <= ((RowAmount + ColAmount) - 1); ++Line)
    {
        bool ShouldBreak = true;
        
        s32 StartColumn  = Max2(0, Line - RowAmount);
        s32 Count        = Min3(Line, (ColAmount - StartColumn), RowAmount);
        
        for(u32 i = 0; i < Count; ++i)
        {
            u32 RowIndex = Min2(RowAmount, Line) - i - 1;
            u32 ColIndex = StartColumn + i;
            
            r32 *CellSize = &GridEntity->UnitSize[RowIndex][ColIndex];
            s32 *CellField = &GridEntity->UnitField[RowIndex][ColIndex];
            if(*CellSize < MaximumBlockSize)
            {
                r32 ScaleDt = TimeElapsed * PixelScalePerSec;
                *CellSize += ScaleDt;
                
                if(*CellSize >= MaximumBlockSize)
                {
                    *CellSize = MaximumBlockSize;
                    
                    if(i == (Count - 1))
                    {
                        PixelsDrawn += MaximumBlockSize;
                        PixelsToDraw = (((StartUpTimeElapsed) - TimeElapsed) * PixelScalePerSec);
                        
                        if(PixelsToDraw > AmountOfPixels)
                        {
                            PixelsToDraw = AmountOfPixels;
                        }
                        
                        if(PixelsDrawn < PixelsToDraw)
                        {
                            r32 PixelsYetToDraw = PixelsToDraw - PixelsDrawn;
                            
                            s32 LineCount = 0;
                            s32 LineAmount = (PixelsYetToDraw / (r32) MaximumBlockSize);
                            
                            for(s32 NextLine = Line + 1;
                                NextLine <= ((RowAmount + ColAmount) - 1) && 
                                (LineCount < LineAmount);
                                ++NextLine)
                            {
                                s32 NextStartColumn = Max2(0, NextLine - RowAmount);
                                s32 NextCount = Min3(NextLine, (ColAmount - NextStartColumn), RowAmount);
                                
                                for(s32 j = 0; j < NextCount; ++j)
                                {
                                    s32 NextRowIndex = Min2(RowAmount, NextLine) - j - 1;
                                    s32 NextColIndex = NextStartColumn + j;
                                    
                                    //s32 NextIndex = (NextRowIndex * ColAmount) + NextColIndex;
                                    GridEntity->UnitSize[NextRowIndex][NextColIndex] += PixelsYetToDraw;
                                    if(GridEntity->UnitSize[NextRowIndex][NextColIndex] >= MaximumBlockSize)
                                    {
                                        GridEntity->UnitSize[NextRowIndex][NextColIndex] = MaximumBlockSize;
                                        
                                        if((j == NextCount - 1) && PixelsYetToDraw >= MaximumBlockSize)
                                        {
                                            PixelsYetToDraw -= MaximumBlockSize;
                                            PixelsDrawn += MaximumBlockSize;
                                        }
                                    }
                                }
                                
                                LineCount++;
                            }
                        }
                    }
                }
            }
            else
            {
                if(ShouldBreak)
                {
                    ShouldBreak = false;
                }
            }
            
            s32 StartY = GridAreaY + (MaximumBlockSize * RowIndex) + (MaximumBlockSize / 2);
            s32 StartX = GridAreaX + (MaximumBlockSize * ColIndex) + (MaximumBlockSize / 2);
            
            game_rect AreaQuad = {};
            AreaQuad.w = roundf(*CellSize);
            AreaQuad.h = roundf(*CellSize);
            AreaQuad.x = StartX - (*CellSize / 2);
            AreaQuad.y = StartY - (*CellSize / 2);
            
            if(*CellField == 0)
            {
                GameRenderBitmapToBuffer(Buffer, GridEntity->NormalSquareTexture,     &AreaQuad);
            }
            else if(*CellField == 2)
            {
                GameRenderBitmapToBuffer(Buffer, GridEntity->HorizontlaSquareTexture, &AreaQuad);
            }
            else if(*CellField == 3)
            {
                GameRenderBitmapToBuffer(Buffer, GridEntity->VerticalSquareTexture,   &AreaQuad);
            }
        }
        
        if(ShouldBreak)
        {
            break;
        }
        
    }
    
    for(s32 i = 0; i < RowAmount; ++i)
    {
        for(s32 j = 0; j < ColAmount; ++j)
        {
            if(GridEntity->UnitSize[i][j] < MaximumBlockSize)
            {
                IsGridReady = false;
            }
        }
    }
    
    if(IsGridReady)
    {
        FigureEntity->FigureAlpha += TimeElapsed * StartAlphaPerSec;
        if(FigureEntity->FigureAlpha < 255)
        {
            IsFiguresReady = false;
        }
        else
        {
            FigureEntity->FigureAlpha = 255.0f;
        }
        
        for(s32 i = 0; i < FigureAmount; ++i)
        {
            SDL_SetTextureAlphaMod(FigureEntity->FigureUnit[i].Texture, FigureEntity->FigureAlpha);
            FigureUnitRenderBitmap(Buffer, &FigureEntity->FigureUnit[i]);
        }
    }
    
    IsLevelReady = IsGridReady && IsFiguresReady;// && IsFigureReady;
    
    if(IsLevelReady)
    {
        printf("TimeElapsed = %f\n", StartUpTimeElapsed);
        printf("PixelsToDraw = %f\n", PixelsToDraw);
        printf("PixelsDrawn  = %f\n", PixelsDrawn);
        printf("FigureEntity->FigureAlpha = %f\n", FigureEntity->FigureAlpha);
    }
    
    LevelEntity->Configuration.StartUpTimeElapsed = StartUpTimeElapsed;
    LevelEntity->Configuration.PixelsDrawn  = PixelsDrawn;
    LevelEntity->Configuration.PixelsToDraw = PixelsToDraw;
    LevelEntity->LevelStarted = IsLevelReady;
}

static void
LevelEntityUpdateAndRender(game_offscreen_buffer *Buffer, game_memory *Memory,  level_entity *LevelEntity, r32 TimeElapsed)
{
    grid_entity   *&GridEntity   = LevelEntity->GridEntity;
    figure_entity *&FigureEntity = LevelEntity->FigureEntity;
    figure_unit   *FigureUnit    = FigureEntity->FigureUnit;
    
    u32 ActiveBlockSize   = LevelEntity->Configuration.ActiveBlockSize;
    u32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
    
    game_rect AreaQuad = {};
    r32 MaxVel       = ActiveBlockSize / 6;
    s32 RowAmount    = GridEntity->RowAmount;
    s32 ColumnAmount = GridEntity->ColumnAmount;
    u32 FigureAmount = FigureEntity->FigureAmount;
    s32 ActiveIndex  = FigureEntity->FigureActive;
    
    bool ToggleHighlight = false;
    
    s32 StartX = 0;
    s32 StartY = 0;
    
    if(!LevelEntity->LevelStarted)
    {
        LevelEntityUpdateStartUpAnimation(LevelEntity,
                                          Buffer, TimeElapsed);
        if(LevelEntity->LevelStarted)
        {
            for(u32 i = 0; i < RowAmount; ++i)
            {
                free(GridEntity->UnitSize[i]);
            }
            
            free(GridEntity->UnitSize);
            GridEntity->UnitSize = 0;
            
            LevelEntity->Configuration.StartUpTimeElapsed = 0;
            LevelEntity->Configuration.PixelsDrawn  = 0;
            LevelEntity->Configuration.PixelsToDraw = 0;
        }
        
        return;
    }
    
    
    //
    // GridEntity Update and Rendering
    //
    for (u32 Index = 0; Index < FigureAmount; ++Index)
    {
        u32 FigureIndex = FigureEntity->FigureOrder[Index];
        bool IsIdle     = FigureUnit[FigureIndex].IsIdle;
        bool IsSticked  = FigureUnit[FigureIndex].IsStick;
        bool IsAttached = FigureIndex == ActiveIndex;
        
        if(IsIdle)
        {
            continue;
        }
        
        if(IsSticked && IsAttached)
        {
            // Unstick from the grid
            u32 StickAmount = GridEntity->StickUnitsAmount;
            u32 RowIndex = 0;
            u32 ColIndex = 0;
            for (u32 i = 0; i < StickAmount; ++i)
            {
                if(GridEntity->StickUnits[i].Index == FigureIndex)
                {
                    GridEntity->StickUnits[i].Index = -1;
                    for (u32 j = 0; j < 4; ++j)
                    {
                        RowIndex = GridEntity->StickUnits[i].Row[j];
                        ColIndex = GridEntity->StickUnits[i].Col[j];
                        GridEntity->UnitField[RowIndex][ColIndex] = 0;
                    }
                    
                    break;
                }
            }
            
            FigureUnit[FigureIndex].IsStick = false;
        }
        else if(!IsSticked && !IsAttached)
        {
            if(!FigureEntity->IsRotating && !FigureEntity->IsFlipping)
            {
                // Check if we can stick it!
                u32 Count   = 0;
                r32 OffsetX = 0;
                r32 OffsetY = 0;
                u32 RowIndex[4] = {0};
                u32 ColIndex[4] = {0};
                game_rect Rect = {0, 0, (s32)ActiveBlockSize, (s32)ActiveBlockSize};
                
                for (u32 i = 0 ; i < RowAmount && Count != 4; ++i)
                {
                    for (u32 j = 0; j < ColumnAmount && Count != 4; ++j)
                    {
                        Rect.x = GridEntity->GridArea.x + (j*ActiveBlockSize);
                        Rect.y = GridEntity->GridArea.y + (i*ActiveBlockSize);
                        
                        for (u32 l = 0; l < 4; ++l)
                        {
                            if(IsPointInsideRect(FigureUnit[FigureIndex].Shell[l].x,
                                                 FigureUnit[FigureIndex].Shell[l].y,
                                                 &Rect))
                            {
                                if(Count == 0)
                                {
                                    OffsetX = Rect.x + (Rect.w / 2) - FigureUnit[FigureIndex].Shell[l].x;
                                    OffsetY = Rect.y + (Rect.h / 2) - FigureUnit[FigureIndex].Shell[l].y;
                                }
                                
                                RowIndex[l] = i;
                                ColIndex[l] = j;
                                Count = Count + 1;
                                break;
                            }    
                        }
                    }
                }
                
                
                if(Count == 4)
                {
                    bool IsFree = true;
                    bool IsFull = false;
                    
                    for (u32 i = 0; i < 4; ++i)
                    {
                        if(GridEntity->UnitField[RowIndex[i]][ColIndex[i]] > 0)
                        {
                            IsFree = false;
                        }
                    }
                    
                    if(IsFree)
                    {
                        FigureUnit[FigureIndex].IsStick = true;
                        
                        u32 StickSize = GridEntity->StickUnitsAmount;
                        game_point FigureCenter = FigureUnit[FigureIndex].Center;
                        for (u32 i = 0; i < StickSize; ++i)
                        {
                            if(GridEntity->StickUnits[i].Index == -1)
                            {
                                GridEntity->StickUnits[i].Index     = FigureIndex;
                                GridEntity->StickUnits[i].Center.x  = FigureCenter.x + OffsetX;
                                GridEntity->StickUnits[i].Center.y  = FigureCenter.y + OffsetY;
                                GridEntity->StickUnits[i].IsSticked = false;
                                
                                for (u32 j = 0; j < 4; ++j)
                                {
                                    GridEntity->StickUnits[i].Row[j] = RowIndex[j];
                                    GridEntity->StickUnits[i].Col[j] = ColIndex[j];
                                }
                                
                                break;
                            }
                        }
                        
                        
                        FigureEntityLowPriority(FigureEntity, FigureIndex);
                        
                        for (u32 i = 0; i < RowAmount; ++i)
                        {
                            for (u32 j = 0; j < ColumnAmount; ++j)
                            {
                                if(GridEntity->UnitField[i][j] == 1)
                                {
                                    IsFull = true;
                                }
                            }
                        }
                        
                        
                        
                        if(IsFull == true)
                        {
                            // the grid is full and level is complete 
                        }
                    }
                }
            }
        }
    }
    
    //
    // Stick units
    //
    
    bool IsAllSticked = GridEntity->StickUnitsAmount > 0;
    
    for(u32 i = 0; i < GridEntity->StickUnitsAmount; ++i)
    {
        bool IsSticked = GridEntity->StickUnits[i].IsSticked;
        
        if(!IsSticked)
        {
            IsAllSticked = false;
        }
        
        s32 Index = GridEntity->StickUnits[i].Index;
        
        if(!IsSticked && Index >= 0)
        {
            game_point *FigureCenter = &FigureUnit[Index].Center;
            game_point *TargetCenter = &GridEntity->StickUnits[i].Center;
            
            vector2 Velocity = Move2DPointPerSec(FigureCenter, TargetCenter, MaxVel, TimeElapsed);
            
            FigureUnitMove(&FigureUnit[Index], Velocity.x, Velocity.y);
            
            if((FigureCenter->x == TargetCenter->x) && (FigureCenter->y == TargetCenter->y))
            {
                GridEntity->StickUnits[i].IsSticked = true;
                
                u32 RowIndex = 0;
                u32 ColIndex = 0;
                
                for(u32 j = 0; j < 4; j++)
                {
                    RowIndex = GridEntity->StickUnits[i].Row[j];
                    ColIndex = GridEntity->StickUnits[i].Col[j];
                    GridEntity->UnitField[RowIndex][ColIndex] = 1;
                }
            }
        }
    }
    
    if(IsAllSticked)
    {
        Memory->CurrentLevelIndex = Memory->CurrentLevelIndex < Memory->LevelMemoryAmount ? Memory->CurrentLevelIndex + 1 : Memory->CurrentLevelIndex;
        
        LevelEntityUpdateLevelEntityFromMemory(LevelEntity, Memory->CurrentLevelIndex, false, Memory, Buffer);
        
        LevelEditorChangeGridCounters(Memory->LevelEditor, 
                                      LevelEntity->GridEntity->RowAmount, LevelEntity->GridEntity->ColumnAmount, 
                                      Buffer);
        
        // Level is completed.
        return;
        
    }
    
    AreaQuad.w = ActiveBlockSize;
    AreaQuad.h = ActiveBlockSize;
    
    for (u32 i = 0; i < RowAmount; ++i)
    {
        StartY = GridEntity->GridArea.y + (ActiveBlockSize * i) + (ActiveBlockSize / 2);
        for (u32 j = 0; j < ColumnAmount; ++j)
        {
            StartX = GridEntity->GridArea.x + (ActiveBlockSize * j) + (ActiveBlockSize / 2);
            
            AreaQuad.x = StartX - (AreaQuad.w / 2);
            AreaQuad.y = StartY - (AreaQuad.h / 2);
            
            u32 GridUnit = GridEntity->UnitField[i][j];
            if(GridUnit == 0 || GridUnit == 2 || GridUnit == 3)
            {
                GameRenderBitmapToBuffer(Buffer, GridEntity->NormalSquareTexture, &AreaQuad);
            }
        }
    }
    
    //
    // MovingBlocks Update and Rendering
    //
    
    for(u32 i = 0; i < GridEntity->MovingBlocksAmount; ++i)
    {
        if(GridEntity->MovingBlocks[i].IsMoving)
        {
            s32 RowNumber = GridEntity->MovingBlocks[i].RowNumber;
            s32 ColNumber = GridEntity->MovingBlocks[i].ColNumber;
            
            game_point Center = { 
                GridEntity->MovingBlocks[i].AreaQuad.x, 
                GridEntity->MovingBlocks[i].AreaQuad.y 
            };
            
            game_point TargetCenter;
            TargetCenter.x = GridEntity->GridArea.x + (ColNumber * ActiveBlockSize);
            TargetCenter.y = GridEntity->GridArea.y + (RowNumber * ActiveBlockSize);
            
            vector2 Velocity = Move2DPointPerSec(&Center, &TargetCenter, MaxVel, TimeElapsed);
            
            GridEntity->MovingBlocks[i].AreaQuad.x += Velocity.x;
            GridEntity->MovingBlocks[i].AreaQuad.y += Velocity.y;
            
            if((Center.x == TargetCenter.x) && (Center.y == TargetCenter.y))
            {
                GridEntity->MovingBlocks[i].IsMoving = false;
                GridEntity->MovingBlocks[i].AreaQuad.x
                    = GridEntity->GridArea.x + (ColNumber * ActiveBlockSize);
                GridEntity->MovingBlocks[i].AreaQuad.y
                    = GridEntity->GridArea.y + (RowNumber * ActiveBlockSize);
                GridEntity->UnitField[RowNumber][ColNumber] = 1;
            }
        }
        
        if(GridEntity->MovingBlocks[i].IsVertical)
        {
            GameRenderBitmapToBuffer(Buffer, GridEntity->VerticalSquareTexture,   &GridEntity->MovingBlocks[i].AreaQuad);
        }
        else
        {
            GameRenderBitmapToBuffer(Buffer, GridEntity->HorizontlaSquareTexture, &GridEntity->MovingBlocks[i].AreaQuad);
        }
    }
    
    //
    // FigureEntity Update and Rendering
    //
    
    //
    // Figure Area Highlight
    //
    
    if(FigureEntity->IsRestarting)
    {
        bool AllFiguresReturned = true;
        for(u32 i = 0; i < FigureAmount; ++i)
        {
            game_point *Center        = &FigureUnit[i].Center;
            game_point *TargetCenter  = &FigureUnit[i].DefaultCenter;
            if(!FigureUnit[i].IsIdle)
            {
                AllFiguresReturned = false;
                
                vector2 Velocity =  Move2DPointPerSec(Center,TargetCenter, MaxVel, TimeElapsed);
                FigureUnitMove(&FigureUnit[i], Velocity.x, Velocity.y);
                
                if((Center->x == TargetCenter->x) && (Center->y == TargetCenter->y))
                {
                    FigureEntity->FigureUnit[i].IsIdle = true;
                }
            }
        }
        
        if(AllFiguresReturned)
        {
            FigureEntity->IsRestarting = false;
            printf("Restarted!\n");
        }
    }
    
    bool ShouldHighlight = false;
    
    if(ActiveIndex >= 0)
    {
        game_rect ScreenArea = {0};
        ScreenArea.w = Buffer->Width;
        ScreenArea.h = Buffer->Height;
        
        ShouldHighlight = IsFigureUnitInsideRect(&FigureUnit[ActiveIndex], &FigureEntity->FigureArea) || !(IsPointInsideRect(FigureUnit[ActiveIndex].Center.x,  FigureUnit[ActiveIndex].Center.y, &ScreenArea));
        
    }
    
    if(ShouldHighlight) 
    {
        Change1DUnitPerSec(&FigureEntity->AreaAlpha, 255, LevelEntity->Configuration.FlippingAlphaPerSec, TimeElapsed);
    }
    else
    {
        
        Change1DUnitPerSec(&FigureEntity->AreaAlpha, 0, LevelEntity->Configuration.FlippingAlphaPerSec, TimeElapsed);
    }
    
    
    if(FigureEntity->AreaAlpha != 0) ToggleHighlight = true;
    
    if(ToggleHighlight) FigureEntityHighlightFigureArea(FigureEntity, Buffer, {255, 255, 255}, InActiveBlockSize / 6);
    
    //
    // Figure returning to the idle zone
    //
    
    if(FigureEntity->IsReturning)
    {
        u32 ReturnIndex = FigureEntity->ReturnIndex;
        game_point *Center        = &FigureUnit[ReturnIndex].Center;
        game_point *TargetCenter  = &FigureUnit[ReturnIndex].DefaultCenter;
        
        vector2 Velocity =  Move2DPointPerSec(Center,TargetCenter, MaxVel, TimeElapsed);
        
        FigureUnitMove(&FigureUnit[ReturnIndex], Velocity.x, Velocity.y);
        
        if((Center->x == TargetCenter->x) && (Center->y == TargetCenter->y))
        {
            FigureEntity->IsReturning = false;
            FigureEntity->ReturnIndex = -1;
        }
    }
    
    //
    // Rotation Animation
    //
    
    if(FigureEntity->IsRotating)
    {
        // TODO(max): Maybe put these values in level_entity ???
        r32 AngleDt = TimeElapsed * LevelEntity->Configuration.RotationVel;
        if(FigureEntity->RotationSum < 90.0f && !(FigureEntity->RotationSum + AngleDt >= 90.0f))
        {
            FigureEntity->FigureUnit[ActiveIndex].Angle += AngleDt;
            FigureEntity->RotationSum += AngleDt;
        }
        else
        {
            FigureEntity->FigureUnit[ActiveIndex].Angle += 90.0f - FigureEntity->RotationSum;
            FigureEntity->RotationSum = 0;
            FigureEntity->IsRotating = false;
        }
    }
    
    //
    // Flipping Animation
    //
    
    if(FigureEntity->IsFlipping)
    {
        if(FigureEntity->FadeInSum > 0)
        {
            if(Change1DUnitPerSec(&FigureEntity->FigureAlpha, 0, LevelEntity->Configuration.FlippingAlphaPerSec, TimeElapsed))
            {
                FigureEntity->FigureAlpha = 0;
                FigureEntity->FadeInSum   = 0;
                FigureUnitFlipHorizontally(&FigureUnit[ActiveIndex]);
            }
        }
        else if(FigureEntity->FadeOutSum < 255)
        {
            if(Change1DUnitPerSec(&FigureEntity->FigureAlpha, 255, LevelEntity->Configuration.FlippingAlphaPerSec, TimeElapsed))
            {
                FigureEntity->FigureAlpha = 255;
                FigureEntity->FadeOutSum  = 255;
                FigureEntity->IsFlipping  = false;
            }
        }
        
        SDL_SetTextureAlphaMod(FigureUnit[ActiveIndex].Texture, FigureEntity->FigureAlpha);
    }
    
    //
    // Figure Rendering
    //
    
    for(u32 i = 0; i < FigureAmount; ++i)
    {
        u32 Index = FigureEntity->FigureOrder[i];
        
        FigureUnitRenderBitmap(Buffer, &FigureUnit[Index]);
        //DEBUGRenderQuad(Buffer, &FigureUnit[Index].AreaQuad, {255, 0, 0}, 255);
        
        //u32 BlockSize = FigureUnit[Index].IsEnlarged ? ActiveBlockSize : InActiveBlockSize;
        //DEBUGRenderFigureShell(Buffer, &FigureUnit[Index], 10, {255, 255, 0}, 255);
    }
    
    GameRenderBitmapToBuffer(Buffer, LevelEntity->LevelNumberShadowTexture, &LevelEntity->LevelNumberShadowQuad);
    GameRenderBitmapToBuffer(Buffer, LevelEntity->LevelNumberTexture,   &LevelEntity->LevelNumberQuad);
    
}

static void
LevelEditorInit(level_entity *LevelEntity, game_memory *Memory, game_offscreen_buffer *Buffer)
{
    Memory->LevelEditor = (level_editor*)malloc(sizeof(level_editor));
    Assert(Memory->LevelEditor);
    
    *Memory->LevelEditor = {};
    
    Memory->LevelEditor->LevelNumberSelected = false;
    
    Memory->LevelEditor->ActiveButton.x = 0;
    Memory->LevelEditor->ActiveButton.y = 0;
    Memory->LevelEditor->ActiveButton.w = 0;
    Memory->LevelEditor->ActiveButton.h = 0;
    
    Memory->LevelEditor->SelectedFigure = 0;
    Memory->LevelEditor->ButtonPressed  = false;
    
    s32 ButtonSize   = LevelEntity->Configuration.InActiveBlockSize * 2;
    s32 FontSize     = LevelEntity->Configuration.InActiveBlockSize * 2;
    u32 RowAmount    = LevelEntity->GridEntity->RowAmount;
    u32 ColumnAmount = LevelEntity->GridEntity->ColumnAmount;
    
    char RowString[2] = {0};
    char ColString[2] = {0};
    sprintf(RowString, "%d", RowAmount);
    sprintf(ColString, "%d", ColumnAmount);
    
    level_editor *&LevelEditor = Memory->LevelEditor;
    LevelEditor->GridButtonLayer.w = ButtonSize;
    LevelEditor->GridButtonLayer.h = ButtonSize * 6;
    
    LevelEditor->GridButtonLayer.x = 0;
    LevelEditor->GridButtonLayer.y = 0;
    
    LevelEditor->Font = TTF_OpenFont("..\\data\\Karmina-Bold.otf", FontSize);
    Assert(LevelEditor->Font);
    
    LevelEditor->StatsFont = TTF_OpenFont("..\\data\\Karmina-Bold.otf", ButtonSize / 3);
    Assert(LevelEditor->StatsFont);
    
    game_surface *Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, "+", {0, 0, 0});
    LevelEditor->PlusTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->PlusTexture, 0, 0, &LevelEditor->GridButtonQuad[0].w, &LevelEditor->GridButtonQuad[0].h);
    SDL_QueryTexture(LevelEditor->PlusTexture, 0, 0, &LevelEditor->GridButtonQuad[3].w, &LevelEditor->GridButtonQuad[3].h);
    SDL_FreeSurface(Surface);
    
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, "-", {0, 0, 0});
    LevelEditor->MinusTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->MinusTexture, 0, 0, &LevelEditor->GridButtonQuad[2].w,
                     &LevelEditor->GridButtonQuad[2].h);
    SDL_QueryTexture(LevelEditor->MinusTexture, 0, 0, &LevelEditor->GridButtonQuad[5].w,
                     &LevelEditor->GridButtonQuad[5].h);
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, RowString, {0, 0, 0});
    LevelEditor->RowTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->RowTexture, 0, 0, &LevelEditor->GridButtonQuad[1].w,
                     &LevelEditor->GridButtonQuad[1].h);
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, ColString, {0, 0, 0});
    LevelEditor->ColumnTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->ColumnTexture, 0, 0, &LevelEditor->GridButtonQuad[4].w, &LevelEditor->GridButtonQuad[4].h);
    SDL_FreeSurface(Surface);
    
    
    game_rect UiQuad = 
    {
        LevelEditor->GridButtonLayer.x, LevelEditor->GridButtonLayer.y, 
        ButtonSize, ButtonSize
    };
    
    for(u32 i = 0; i < 6; i++)
    {
        LevelEditor->GridButtonQuad[i].x = (UiQuad.x + UiQuad.w / 2) - (LevelEditor->GridButtonQuad[i].w / 2);
        LevelEditor->GridButtonQuad[i].y = (UiQuad.y + UiQuad.h / 2) - (LevelEditor->GridButtonQuad[i].h / 2);
        LevelEditor->GridButton[i] = UiQuad;
        UiQuad.y += UiQuad.h;
    }
    
    LevelEditor->FigureButtonLayer.w = ButtonSize;
    LevelEditor->FigureButtonLayer.h = ButtonSize * 6;
    //LevelEditor->FigureButtonLayer.x = (Buffer->Width / 2) - (LevelEditor->FigureButtonLayer.w / 2);
    //LevelEditor->FigureButtonLayer.y = Buffer->Height - LevelEditor->FigureButtonLayer.h;
    
    LevelEditor->FigureButtonLayer.x = LevelEditor->GridButtonLayer.x + LevelEditor->GridButtonLayer.w;
    LevelEditor->FigureButtonLayer.y = LevelEditor->GridButtonLayer.y;
    
    LevelEditor->FigureButtonQuad[0].w = LevelEditor->GridButtonQuad[0].w;
    LevelEditor->FigureButtonQuad[0].h = LevelEditor->GridButtonQuad[0].h;
    LevelEditor->FigureButtonQuad[1].w = LevelEditor->GridButtonQuad[2].w;
    LevelEditor->FigureButtonQuad[1].h = LevelEditor->GridButtonQuad[2].h;
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, "R", {0, 0, 0});
    LevelEditor->RotateTexture  = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->RotateTexture, 0, 0, &LevelEditor->FigureButtonQuad[2].w, &LevelEditor->FigureButtonQuad[2].h);
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, "F", {0, 0, 0});
    LevelEditor->FlipTexture  = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->FlipTexture, 0, 0, &LevelEditor->FigureButtonQuad[3].w, &LevelEditor->FigureButtonQuad[3].h);
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, "<", {0, 0, 0});
    LevelEditor->FormTexture  = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->FormTexture, 0, 0, &LevelEditor->FigureButtonQuad[4].w, &LevelEditor->FigureButtonQuad[4].h);
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, ">", {0, 0, 0});
    LevelEditor->TypeTexture  = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->TypeTexture, 0, 0, &LevelEditor->FigureButtonQuad[5].w, &LevelEditor->FigureButtonQuad[5].h);
    SDL_FreeSurface(Surface);
    
    UiQuad.x = LevelEditor->FigureButtonLayer.x;
    UiQuad.y = LevelEditor->FigureButtonLayer.y;
    UiQuad.w = ButtonSize;
    UiQuad.h = ButtonSize;
    
    for(u32 i = 0; i < 6; i++)
    {
        LevelEditor->FigureButtonQuad[i].x = (UiQuad.x + UiQuad.w / 2) - (LevelEditor->FigureButtonQuad[i].w / 2);
        LevelEditor->FigureButtonQuad[i].y = (UiQuad.y + UiQuad.h / 2) - (LevelEditor->FigureButtonQuad[i].h / 2);
        LevelEditor->FigureButton[i] = UiQuad;
        UiQuad.y += UiQuad.h;
    }
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, "save", {255, 255, 255});
    LevelEditor->SaveTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->SaveTexture, 0, 0,
                     &LevelEditor->SaveButtonQuad.w, &LevelEditor->SaveButtonQuad.h);
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, "load", {255, 255, 255});
    LevelEditor->LoadTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->LoadTexture, 0, 0, &LevelEditor->LoadButtonQuad.w, &LevelEditor->LoadButtonQuad.h);
    SDL_FreeSurface(Surface);
    
    LevelEditor->SaveButtonLayer.w = LevelEditor->FigureButtonLayer.w * 2;
    LevelEditor->SaveButtonLayer.h = LevelEditor->FigureButtonLayer.w;
    LevelEditor->SaveButtonLayer.x = 0;
    LevelEditor->SaveButtonLayer.y = LevelEditor->FigureButtonLayer.y + LevelEditor->FigureButtonLayer.h;
    
    LevelEditor->SaveButtonQuad.x = LevelEditor->SaveButtonLayer.x + (LevelEditor->SaveButtonLayer.w / 2) - (LevelEditor->SaveButtonQuad.w / 2);
    LevelEditor->SaveButtonQuad.y = LevelEditor->SaveButtonLayer.y + (LevelEditor->SaveButtonLayer.h / 2) - (LevelEditor->SaveButtonQuad.h / 2);
    
    LevelEditor->LoadButtonLayer.w = LevelEditor->FigureButtonLayer.w * 2;
    LevelEditor->LoadButtonLayer.h = LevelEditor->FigureButtonLayer.w;
    LevelEditor->LoadButtonLayer.x = 0;
    LevelEditor->LoadButtonLayer.y = LevelEditor->SaveButtonLayer.y + (LevelEditor->LoadButtonLayer.h);
    
    LevelEditor->LoadButtonQuad.x = LevelEditor->LoadButtonLayer.x + (LevelEditor->LoadButtonLayer.w / 2) - (LevelEditor->LoadButtonQuad.w / 2);
    LevelEditor->LoadButtonQuad.y = LevelEditor->LoadButtonLayer.y + (LevelEditor->LoadButtonLayer.h / 2) - (LevelEditor->LoadButtonQuad.h / 2);
    
    
    Memory->LevelEditor->ActiveButton.x = LevelEditor->GridButtonLayer.x;
    Memory->LevelEditor->ActiveButton.y = -100;
    Memory->LevelEditor->ActiveButton.w = ButtonSize;
    Memory->LevelEditor->ActiveButton.h = ButtonSize;
    
    //
    // Next/Prev Level Buttons
    //
    
    Memory->LevelEditor->PrevLevelTexture = GetTexture(Memory, "left_arrow.png", Buffer->Renderer);
    Assert(Memory->LevelEditor->PrevLevelTexture);
    
    Memory->LevelEditor->PrevLevelQuad.w = ButtonSize;
    Memory->LevelEditor->PrevLevelQuad.h = ButtonSize;
    Memory->LevelEditor->PrevLevelQuad.x = Memory->LevelEditor->PrevLevelQuad.w;
    Memory->LevelEditor->PrevLevelQuad.y = Buffer->Height - (Memory->LevelEditor->PrevLevelQuad.h * 2);
    
    Memory->LevelEditor->NextLevelTexture = GetTexture(Memory, "right_arrow.png", Buffer->Renderer);
    Assert(Memory->LevelEditor->NextLevelTexture);
    
    Memory->LevelEditor->NextLevelQuad.w = ButtonSize;
    Memory->LevelEditor->NextLevelQuad.h = ButtonSize;
    Memory->LevelEditor->NextLevelQuad.x = Buffer->Width - (Memory->LevelEditor->NextLevelQuad.w * 2);
    Memory->LevelEditor->NextLevelQuad.y = Buffer->Height - (Memory->LevelEditor->NextLevelQuad.h * 2);
}

static void
GridEntityDeleteMovingBlock(grid_entity *GridEntity, u32 Index)
{
    u32 Amount = GridEntity->MovingBlocksAmount;
    
    if(Index < 0 || Index >= Amount) return;
    if(Amount <= 0) return;
    
    moving_block *MovingBlocks = GridEntity->MovingBlocks;
    
    for(u32 i = Index; i < Amount-1; ++i)
    {
        MovingBlocks[i].AreaQuad   = MovingBlocks[i+1].AreaQuad;
        MovingBlocks[i].RowNumber  = MovingBlocks[i+1].RowNumber;
        MovingBlocks[i].ColNumber  = MovingBlocks[i+1].ColNumber;
        MovingBlocks[i].IsVertical = MovingBlocks[i+1].IsVertical;
        MovingBlocks[i].IsMoving   = MovingBlocks[i+1].IsMoving;
        MovingBlocks[i].MoveSwitch = MovingBlocks[i+1].MoveSwitch;
    }
    
    GridEntity->MovingBlocksAmount -= 1;
}

static void
LevelEditorChangeGridCounters(level_editor *LevelEditor, 
                              u32 NewRowAmount, u32 NewColumnAmount,
                              game_offscreen_buffer *Buffer)
{
    game_surface *Surface = 0;
    
    char RowString[3]   = {};
    char ColString[3]   = {};
    
    sprintf(RowString, "%d", NewRowAmount);
    sprintf(ColString, "%d", NewColumnAmount);
    
    if(LevelEditor->RowTexture)
    {
        FreeTexture(LevelEditor->RowTexture);
    }
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, RowString, { 0, 0, 0 });
    
    LevelEditor->RowTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->RowTexture, 0, 0, &LevelEditor->GridButtonQuad[1].w, &LevelEditor->GridButtonQuad[1].h);
    
    LevelEditor->GridButtonQuad[1].x = LevelEditor->GridButton[1].x + (LevelEditor->GridButton[1].w /2) - (LevelEditor->GridButtonQuad[1].w / 2);
    LevelEditor->GridButtonQuad[1].y = LevelEditor->GridButton[1].y + (LevelEditor->GridButton[4].h /2) - (LevelEditor->GridButtonQuad[1].h / 2);
    
    SDL_FreeSurface(Surface);
    
    if(LevelEditor->ColumnTexture)
    {
        FreeTexture(LevelEditor->ColumnTexture);
    }
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, ColString, { 0, 0, 0 });
    LevelEditor->ColumnTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->ColumnTexture, 0, 0, &LevelEditor->GridButtonQuad[4].w, &LevelEditor->GridButtonQuad[4].h);
    
    LevelEditor->GridButtonQuad[4].x = LevelEditor->GridButton[4].x + (LevelEditor->GridButton[4].w /2) - (LevelEditor->GridButtonQuad[4].w / 2);
    LevelEditor->GridButtonQuad[4].y = LevelEditor->GridButton[4].y + (LevelEditor->GridButton[4].h /2) - (LevelEditor->GridButtonQuad[4].h / 2);
    
    SDL_FreeSurface(Surface);
}

static void 
LevelEditorUpdateLevelStats(level_editor *LevelEditor, 
                            s32 LevelNumber, s32 LevelIndex, game_offscreen_buffer *Buffer)
{
    game_surface *Surface = {};
    
    char LevelNumberString[3] = {};
    char LevelIndexString[3]  = {};
    
    sprintf(LevelNumberString, "%d", LevelNumber);
    sprintf(LevelIndexString, "%d", LevelIndex);
    
    //
    // level index texture init
    //
    {
        char TmpBuffer[128] = {};
        strcpy(TmpBuffer, "level index      = ");
        strcat(TmpBuffer, LevelIndexString);
        
        if(LevelEditor->LevelIndexTexture)
        {
            SDL_DestroyTexture(LevelEditor->LevelIndexTexture);
        }
        
        Surface = TTF_RenderUTF8_Blended(LevelEditor->StatsFont, TmpBuffer, {255, 255, 255});
        Assert(Surface);
        
        LevelEditor->LevelIndexQuad.w = Surface->w;
        LevelEditor->LevelIndexQuad.h = Surface->h;
        LevelEditor->LevelIndexQuad.x = 0;
        LevelEditor->LevelIndexQuad.y = (LevelEditor->LoadButtonQuad.y + LevelEditor->LoadButtonQuad.h) + (LevelEditor->LevelIndexQuad.h / 2);
        
        LevelEditor->LevelIndexTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
        Assert(LevelEditor->LevelIndexTexture);
        
        SDL_FreeSurface(Surface);
        
    }
    //
    // level number texture init
    //
    {
        char TmpBuffer[128] = {};
        strcpy(TmpBuffer, "level number = ");
        strcat(TmpBuffer, LevelNumberString);
        
        if(LevelEditor->LevelNumberTexture)
        {
            SDL_DestroyTexture(LevelEditor->LevelNumberTexture);
        }
        
        Surface = TTF_RenderUTF8_Blended(LevelEditor->StatsFont, TmpBuffer, {255, 255, 255});
        Assert(Surface);
        
        LevelEditor->LevelNumberQuad.w = Surface->w;
        LevelEditor->LevelNumberQuad.h = Surface->h;
        LevelEditor->LevelNumberQuad.x = 0;
        LevelEditor->LevelNumberQuad.y = LevelEditor->LevelIndexQuad.y + LevelEditor->LevelIndexQuad.h;
        
        LevelEditor->LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
        Assert(LevelEditor->LevelNumberTexture);
        
        SDL_FreeSurface(Surface);
    }
    
}

static void
GridEntityNewGrid(game_offscreen_buffer *Buffer, level_entity *LevelEntity, 
                  s32 NewRowAmount, s32 NewColumnAmount, level_editor *LevelEditor)
{
    if(NewRowAmount < 0 || NewColumnAmount < 0) return;
    
    grid_entity *&GridEntity = LevelEntity->GridEntity;
    
    s32 **UnitField = (s32**)malloc(sizeof(s32*) * NewRowAmount);
    Assert(UnitField);
    for(u32 i = 0; i < NewRowAmount; ++i){
        UnitField[i] = (s32*)malloc(sizeof(s32) * NewColumnAmount);
        Assert(UnitField[i]);
        for(u32 j = 0; j < NewColumnAmount; j ++){
            UnitField[i][j] = 0;
        }
    }
    
    u32 CurrentRowAmount = NewRowAmount < GridEntity->RowAmount ? NewRowAmount : GridEntity->RowAmount;
    u32 CurrentColumnAmount = NewColumnAmount < GridEntity->ColumnAmount ? NewColumnAmount : GridEntity->ColumnAmount;
    
    for(u32 i = 0; i < CurrentRowAmount; ++i){
        for(u32 j = 0; j < CurrentColumnAmount; ++j){
            UnitField[i][j] = GridEntity->UnitField[i][j];
        }
    }
    
    for(u32 i = 0; i < GridEntity->RowAmount; ++i){
        free(GridEntity->UnitField[i]);
    }
    
    free(GridEntity->UnitField);
    
    LevelEditorChangeGridCounters(LevelEditor, 
                                  NewRowAmount, NewColumnAmount, 
                                  Buffer);
    
    u32 DefaultBlocksInRow = 12;
    u32 DefaultBlocksInCol = 9;
    
    RescaleGameField(Buffer, NewRowAmount, NewColumnAmount,
                     LevelEntity->FigureEntity->FigureAmount, DefaultBlocksInRow, DefaultBlocksInCol, LevelEntity);
    
    GridEntity->UnitField    = UnitField;
    GridEntity->RowAmount    = NewRowAmount;
    GridEntity->ColumnAmount = NewColumnAmount;
    
    u32 ActiveBlockSize   = LevelEntity->Configuration.ActiveBlockSize;
    u32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
    
    LevelEntity->GridEntity->GridArea.w = ActiveBlockSize * NewColumnAmount;
    LevelEntity->GridEntity->GridArea.h = ActiveBlockSize * NewRowAmount;
    LevelEntity->GridEntity->GridArea.x = (Buffer->Width / 2) - (GridEntity->GridArea.w / 2);
    LevelEntity->GridEntity->GridArea.y = (Buffer->Height - LevelEntity->FigureEntity->FigureArea.h) / 2 - (GridEntity->GridArea.h / 2);
    
    for(u32 i = 0; i < LevelEntity->GridEntity->MovingBlocksAmount; ++i)
    {
        u32 RowNumber = LevelEntity->GridEntity->MovingBlocks[i].RowNumber;
        u32 ColNumber = LevelEntity->GridEntity->MovingBlocks[i].ColNumber;
        
        if(RowNumber >= NewRowAmount || ColNumber >= NewColumnAmount)
        {
            GridEntityDeleteMovingBlock(GridEntity, i);
        }
    }
    
    for(u32 i = 0; i < LevelEntity->GridEntity->MovingBlocksAmount; ++i)
    {
        u32 RowNumber = LevelEntity->GridEntity->MovingBlocks[i].RowNumber;
        u32 ColNumber = LevelEntity->GridEntity->MovingBlocks[i].ColNumber;
        
        GridEntity->MovingBlocks[i].AreaQuad.w = ActiveBlockSize;
        GridEntity->MovingBlocks[i].AreaQuad.h = ActiveBlockSize;
        GridEntity->MovingBlocks[i].AreaQuad.x = GridEntity->GridArea.x + (ColNumber * ActiveBlockSize);
        GridEntity->MovingBlocks[i].AreaQuad.y = GridEntity->GridArea.y + (RowNumber * ActiveBlockSize);
    }
    
}

static figure_form
LevelEditorGetNextFigureForm(figure_form CurrentForm)
{
    switch(CurrentForm)
    {
        case I_figure: return O_figure;
        case O_figure: return Z_figure;
        case Z_figure: return S_figure;
        case S_figure: return T_figure;
        case T_figure: return L_figure;
        case L_figure: return J_figure;
        case J_figure: return I_figure;
    }
    
    return O_figure;
}

static figure_type
LevelEditorGetNextFigureType(figure_type CurrentType)
{
    switch(CurrentType)
    {
        case classic: return stone;
        case stone:   return mirror;
        case mirror:  return classic;
    }
    
    return classic;
}

static void
GameMakeTextureFromString(game_texture *&Texture, 
                          char* Text, 
                          game_rect *TextureQuad, 
                          game_font *&Font, 
                          game_color Color, 
                          game_offscreen_buffer *Buffer)
{
    if(!Text || !Font) return;
    
    if(Texture)
    {
        SDL_DestroyTexture(Texture);
    }
    
    game_surface *Surface = TTF_RenderUTF8_Blended(Font, Text, Color);
    Assert(Surface);
    
    if(TextureQuad)
    {
        TextureQuad->w = Surface->w;
        TextureQuad->h = Surface->h;
    }
    
    Texture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    Assert(Texture);
    
    SDL_FreeSurface(Surface);
}

inline void
LevelEditorRenderButton(game_rect *ButtonQuad, game_rect *ImageQuad,
                        game_color ButtonColor, u8 Alpha, game_texture *ImageTexture,
                        s32 *Accumulator, s32 Offset,  game_offscreen_buffer *Buffer)
{
    DEBUGRenderQuad(Buffer, ButtonQuad, ButtonColor, Alpha);
    GameRenderBitmapToBuffer(Buffer, ImageTexture, ImageQuad);
    
    if(Accumulator)
    {
        *Accumulator += Offset;
    }
}

static void
LevelEditorUpdateAndRender(level_editor *LevelEditor, level_entity *LevelEntity, 
                           game_memory *Memory, game_offscreen_buffer *Buffer, game_input *Input)
{
    if(!LevelEntity->LevelPaused) return;
    
    
    game_rect GridArea   = LevelEntity->GridEntity->GridArea;
    game_rect FigureArea = LevelEntity->FigureEntity->FigureArea;
    
    u32 FigureAmount  = LevelEntity->FigureEntity->FigureAmount;
    u32 RowAmount     = LevelEntity->GridEntity->RowAmount;
    u32 ColAmount     = LevelEntity->GridEntity->ColumnAmount;
    s32 NewIndex      = LevelEditor->SelectedFigure;
    s32 CurrentLevelIndex = Memory->CurrentLevelIndex;
    
    if(LevelEditor->LevelNumberSelected)
    {
        char NextDigit  = '\n';
        s32  DigitIndex = LevelEditor->LevelNumberBufferIndex;
        
        if(Input->Keyboard.Zero.EndedDown)
        {
            NextDigit = '0';
        }
        else if(Input->Keyboard.One.EndedDown)
        {
            NextDigit = '1';
        }
        else if(Input->Keyboard.Two.EndedDown)
        {
            NextDigit = '2';
        }
        else if(Input->Keyboard.Three.EndedDown)
        {
            NextDigit = '3';
        }
        else if(Input->Keyboard.Four.EndedDown)
        {
            NextDigit = '4';
        }
        else if(Input->Keyboard.Five.EndedDown)
        {
            NextDigit = '5';
        }
        else if(Input->Keyboard.Six.EndedDown)
        {
            NextDigit = '6';
        }
        else if(Input->Keyboard.Seven.EndedDown)
        {
            NextDigit = '7';
        }
        else if(Input->Keyboard.Eight.EndedDown)
        {
            NextDigit = '8';
        }
        else if(Input->Keyboard.Nine.EndedDown)
        {
            NextDigit = '9';
        }
        
        if(DigitIndex <= 2 && NextDigit != '\n')
        {
            LevelEditor->LevelNumberBuffer[DigitIndex] = NextDigit;
            LevelEditor->LevelNumberBufferIndex = ++DigitIndex;
            
            GameMakeTextureFromString(LevelEntity->LevelNumberTexture, 
                                      LevelEditor->LevelNumberBuffer, 
                                      &LevelEntity->LevelNumberQuad, 
                                      Memory->LevelNumberFont, 
                                      {255, 255, 255}, 
                                      Buffer);
            
            GameMakeTextureFromString(LevelEntity->LevelNumberShadowTexture, 
                                      LevelEditor->LevelNumberBuffer, 
                                      &LevelEntity->LevelNumberShadowQuad, 
                                      Memory->LevelNumberFont, 
                                      {0, 0, 0}, 
                                      Buffer);
            
            LevelEntity->LevelNumberQuad.x = (Buffer->Width - LevelEntity->LevelNumberQuad.w) - (LevelEntity->LevelNumberQuad.w / 2);
            LevelEntity->LevelNumberQuad.y = 0;
            
            LevelEntity->LevelNumberShadowQuad.x = LevelEntity->LevelNumberQuad.x + 3;
            LevelEntity->LevelNumberShadowQuad.y = LevelEntity->LevelNumberQuad.y + 3;
            
            printf("LevelNumberBufferIndex = %d\n", LevelEditor->LevelNumberBufferIndex);
            
            printf("LevelNumberBuffer = %s\n", LevelEditor->LevelNumberBuffer);
        }
        
    }
    
    if(Input->Keyboard.Up.EndedDown)
    {
        NewIndex -= 1;
    }
    else if(Input->Keyboard.Down.EndedDown)
    {
        NewIndex += 1;
    }
    else if(Input->Keyboard.Left.EndedDown)
    {
        NewIndex -= 2;
    }
    else if(Input->Keyboard.Right.EndedDown)
    {
        NewIndex += 2;
    }
    else if(Input->Keyboard.Q_Button.EndedDown)
    {
        //s32 PrevLevelNumber = CurrentLevel - 1;
        s32 PrevLevelNumber = CurrentLevelIndex - 1;
        if(PrevLevelNumber >= 0)
        {
            Memory->CurrentLevelIndex = PrevLevelNumber;
            
            LevelEntityUpdateLevelEntityFromMemory(&Memory->LevelEntity, 
                                                   Memory->CurrentLevelIndex, true,
                                                   Memory, Buffer);
            
            LevelEditorChangeGridCounters(Memory->LevelEditor, 
                                          Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount,
                                          Buffer);
            
            LevelEditorUpdateLevelStats(Memory->LevelEditor, 
                                        Memory->LevelEntity.LevelNumber, Memory->CurrentLevelIndex, Buffer);
        }
    }
    else if(Input->Keyboard.E_Button.EndedDown)
    {
        //s32 NextLevelNumber = CurrentLevel + 1;
        s32 NextLevelNumber = CurrentLevelIndex + 1;
        if(NextLevelNumber < Memory->LevelMemoryAmount)
        {
            Memory->CurrentLevelIndex = NextLevelNumber;
            
            LevelEntityUpdateLevelEntityFromMemory(&Memory->LevelEntity, 
                                                   Memory->CurrentLevelIndex,true,
                                                   Memory, Buffer);
            
            LevelEditorChangeGridCounters(Memory->LevelEditor, 
                                          Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount,
                                          Buffer);
            
            LevelEditorUpdateLevelStats(Memory->LevelEditor, 
                                        Memory->LevelEntity.LevelNumber, Memory->CurrentLevelIndex, Buffer);
        }
    }
    else if(Input->Keyboard.Enter.EndedDown)
    {
        if(LevelEditor->LevelNumberSelected)
        {
            LevelEntity->LevelNumber = strtol(LevelEditor->LevelNumberBuffer, 0, 10);
            LevelEditor->LevelNumberSelected = false;
            MenuChangeButtonText(Memory->LevelNumberFont, LevelEditor->LevelNumberBuffer, Memory->MenuEntity, 
                                 &Memory->MenuEntity->Buttons[Memory->CurrentLevelIndex], {255, 255, 255}, Buffer);
        }
    }
    
    if(Input->MouseButtons[0].EndedDown)
    {
        LevelEditor->ButtonPressed = true;
        
        if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEntity->LevelNumberQuad))
        {
            LevelEditor->LevelNumberSelected = true;
            LevelEditor->LevelNumberBufferIndex = 0;
            LevelEditor->OldLevelNumber = LevelEntity->LevelNumber;
            
            LevelEditor->LevelNumberBuffer[0] = '\0';
            LevelEditor->LevelNumberBuffer[1] = '\0';
            LevelEditor->LevelNumberBuffer[2] = '\0';
            LevelEditor->LevelNumberBuffer[3] = '\0';
        }
        else
        {
            if(LevelEditor->LevelNumberSelected)
            {
                LevelEditor->LevelNumberSelected = false;
                
                char StringBuffer[4] = {};
                sprintf(StringBuffer, "%d", LevelEntity->LevelNumber);
                
                GameMakeTextureFromString(LevelEntity->LevelNumberTexture, 
                                          StringBuffer, 
                                          &LevelEntity->LevelNumberQuad, 
                                          Memory->LevelNumberFont, 
                                          {255, 255, 255}, 
                                          Buffer);
                
                GameMakeTextureFromString(LevelEntity->LevelNumberShadowTexture, 
                                          StringBuffer, 
                                          &LevelEntity->LevelNumberShadowQuad, 
                                          Memory->LevelNumberFont, 
                                          {0, 0, 0}, 
                                          Buffer);
                
                
                LevelEntity->LevelNumberQuad.x = (Buffer->Width - LevelEntity->LevelNumberQuad.w) - (LevelEntity->LevelNumberQuad.w / 2);
                LevelEntity->LevelNumberQuad.y = 0;
                
                LevelEntity->LevelNumberShadowQuad.x = LevelEntity->LevelNumberQuad.x + 3;
                LevelEntity->LevelNumberShadowQuad.y = LevelEntity->LevelNumberQuad.y + 3;
            }
        }
        
        if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->GridButtonLayer))
        {
            /* Grid layer*/
            
            /* Plus row */
            if (IsPointInsideRect(Input->MouseX, Input->MouseY,
                                  &LevelEditor->GridButton[0]))
            {
                printf("Plus row!\n");
                GridEntityNewGrid(Buffer, LevelEntity, RowAmount+1, ColAmount, LevelEditor);
                
                LevelEditor->ActiveButton = LevelEditor->GridButton[0];
            }
            /* Minus row */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->GridButton[2]))
            {
                printf("Minus row!\n");
                GridEntityNewGrid(Buffer, LevelEntity, RowAmount-1, ColAmount, LevelEditor);
                LevelEditor->ActiveButton = LevelEditor->GridButton[2];
            }
            /* Plus column */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->GridButton[3]))
            {
                printf("Plus column!\n");
                GridEntityNewGrid(Buffer, LevelEntity, RowAmount, ColAmount+1, LevelEditor);
                LevelEditor->ActiveButton = LevelEditor->GridButton[3];
            }
            /* Minus column */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->GridButton[5]))
            {
                printf("Minus column!\n");
                GridEntityNewGrid(Buffer, LevelEntity, RowAmount, ColAmount-1, LevelEditor);
                
                LevelEditor->ActiveButton = LevelEditor->GridButton[5];
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->FigureButtonLayer))
        {
            /* Figure Layer*/ 
            
            /* Add figure*/ 
            if (IsPointInsideRect(Input->MouseX, Input->MouseY,
                                  &LevelEditor->FigureButton[0]))
            {
                FigureUnitAddNewFigure(LevelEntity->FigureEntity, O_figure, classic, 0.0f, Memory, Buffer);
                FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                GridEntityUpdateStickUnits(LevelEntity->GridEntity, LevelEntity->FigureEntity->FigureAmount);
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[0];
            }
            /* Delete figure */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->FigureButton[1]))
            {
                FigureUnitDeleteFigure(LevelEntity->FigureEntity, LevelEntity->FigureEntity->FigureAmount - 1);
                FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                GridEntityUpdateStickUnits(LevelEntity->GridEntity, LevelEntity->FigureEntity->FigureAmount);
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[1];
            }
            /* Rotate figure */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->FigureButton[2]))
            {
                FigureUnitRotateShellBy(&LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure], 90);
                LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Angle += 90.0f;
                FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[2];
            }
            /* Flip figure */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->FigureButton[3]))
            {
                FigureUnitFlipHorizontally(&LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure]);
                FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[3];
            }
            /* Change figure form */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->FigureButton[4]))
            {
                if(LevelEditor->SelectedFigure >= 0)
                {
                    FreeTexture(LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Texture);
                    
                    figure_form Form = LevelEditorGetNextFigureForm(LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Form);
                    
                    figure_type Type = LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Type;
                    
                    FigureUnitInitFigure(&LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure], Form,Type, 0.0f,  Memory, Buffer);
                    
                    FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                }
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[4];
            }
            /* Change figure type */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->FigureButton[5]))
            {
                if(LevelEditor->SelectedFigure >= 0)
                {
                    FreeTexture(LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Texture);
                    
                    figure_type Type = LevelEditorGetNextFigureType(LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Type);
                    
                    figure_form Form = LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Form;
                    
                    FigureUnitInitFigure(&LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure], Form,Type, 0.0f, Memory, Buffer);
                    
                    FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                }
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[5];
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY,
                                  &LevelEditor->SaveButtonLayer))
        {
            printf("Save!\n");
            printf("LevelNumber = %d\n", LevelEntity->LevelNumber);
            
            SaveLevelToMemory(Memory, LevelEntity, Memory->CurrentLevelIndex);
            LevelEditor->ActiveButton = LevelEditor->SaveButtonLayer;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                  &LevelEditor->LoadButtonLayer))
        {
            LevelEntityUpdateLevelEntityFromMemory(LevelEntity, 
                                                   Memory->CurrentLevelIndex,
                                                   false, Memory, Buffer);
            LevelEditorChangeGridCounters(LevelEditor, 
                                          LevelEntity->GridEntity->RowAmount, LevelEntity->GridEntity->ColumnAmount, 
                                          Buffer);
            
            LevelEditor->ActiveButton = LevelEditor->LoadButtonLayer;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &FigureArea))
        {
            for(u32 i = 0; i < FigureAmount; ++i)
            {
                game_rect AreaQuad = FigureUnitGetArea(&LevelEntity->FigureEntity->FigureUnit[i]);
                if(IsPointInsideRect(Input->MouseX, Input->MouseY, &AreaQuad))
                {
                    NewIndex = i;
                }
            }
            
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->PrevLevelQuad))
            {
                s32 PrevLevelNumber = LevelEntity->LevelNumber - 1;
                if(PrevLevelNumber >= 0)
                {
                    LevelEntityUpdateLevelEntityFromMemory(&Memory->LevelEntity, 
                                                           PrevLevelNumber,
                                                           true, Memory, Buffer);
                    
                    
                    LevelEditorChangeGridCounters(Memory->LevelEditor, 
                                                  Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount, 
                                                  Buffer);
                }
                
                LevelEditor->ActiveButton = LevelEditor->PrevLevelQuad;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->NextLevelQuad))
            {
                s32 NextLevelNumber = LevelEntity->LevelNumber + 1;
                if(NextLevelNumber < Memory->LevelMemoryAmount)
                {
                    LevelEntityUpdateLevelEntityFromMemory(&Memory->LevelEntity, 
                                                           NextLevelNumber,
                                                           true, Memory, Buffer);
                    
                    LevelEditorChangeGridCounters(Memory->LevelEditor, 
                                                  Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount, Buffer);
                    
                }
                
                LevelEditor->ActiveButton = LevelEditor->NextLevelQuad;
            }
            
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &GridArea))
        {
            game_rect AreaQuad = { 0, 0, (s32)LevelEntity->Configuration.ActiveBlockSize, (s32)LevelEntity->Configuration.ActiveBlockSize };
            
            u32 StartX = 0;
            u32 StartY = 0;
            
            for(u32 i = 0; i < RowAmount; ++i)
            {
                StartY = GridArea.y + (LevelEntity->Configuration.ActiveBlockSize * i);
                
                for(u32 j = 0; j < ColAmount; ++j)
                {
                    StartX = GridArea.x + (LevelEntity->Configuration.ActiveBlockSize * j);
                    
                    AreaQuad.x = StartX;
                    AreaQuad.y = StartY;
                    
                    if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                         &AreaQuad))
                    {
                        u32 GridUnit = LevelEntity->GridEntity->UnitField[i][j];
                        if(GridUnit == 0)
                        {
                            LevelEntity->GridEntity->UnitField[i][j] = 1;
                        }
                        else
                        {
                            s32 Index = -1;
                            for(u32 m = 0; m < LevelEntity->GridEntity->MovingBlocksAmount; ++m)
                            {
                                u32 RowNumber = LevelEntity->GridEntity->MovingBlocks[m].RowNumber;
                                u32 ColNumber = LevelEntity->GridEntity->MovingBlocks[m].ColNumber;
                                
                                if((i == RowNumber) && (j == ColNumber))
                                {
                                    Index = m;
                                    break;
                                }
                            }
                            
                            if(Index >= 0)
                            {
                                
                                if(!LevelEntity->GridEntity->MovingBlocks[Index].MoveSwitch)
                                {
                                    LevelEntity->GridEntity->MovingBlocks[Index].MoveSwitch = true;
                                }
                                else if(!LevelEntity->GridEntity->MovingBlocks[Index].IsVertical)
                                {
                                    LevelEntity->GridEntity->MovingBlocks[Index].IsVertical = true;
                                    LevelEntity->GridEntity->MovingBlocks[Index].MoveSwitch = false;
                                }
                                else
                                {
                                    GridEntityDeleteMovingBlock(LevelEntity->GridEntity, Index);
                                    LevelEntity->GridEntity->UnitField[i][j] = 0;
                                }
                            }
                            else
                            {
                                GridEntityAddMovingBlock(LevelEntity->GridEntity, i, j, false, false, LevelEntity->Configuration.ActiveBlockSize);
                            }
                        }
                    }
                }
            }
        }
    }
    else if(Input->MouseButtons[0].EndedUp)
    {
        LevelEditor->ButtonPressed = false;
        LevelEditor->ActiveButton.y = -100;
    }
    
    if(NewIndex < 0)
    {
        NewIndex = LevelEntity->FigureEntity->FigureAmount - 1;
    }
    
    if(NewIndex >= LevelEntity->FigureEntity->FigureAmount)
    {
        NewIndex = 0;
    }
    
    LevelEditor->SelectedFigure = NewIndex;
    
    if(LevelEntity->LevelStarted)
    {
        for(u32 i = 0; i < LevelEntity->FigureEntity->FigureAmount; ++i)
        {
            DEBUGRenderFigureShell(Buffer, &LevelEntity->FigureEntity->FigureUnit[i], LevelEntity->Configuration.InActiveBlockSize / 4, {0, 0, 255}, 255);
            DEBUGRenderQuad(Buffer, &LevelEntity->FigureEntity->FigureUnit[i].AreaQuad, {255, 0, 0}, 255);
        }
        
        if(LevelEntity->FigureEntity->FigureAmount > 0)
        {
            DEBUGRenderFigureShell(Buffer, &LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure], LevelEntity->Configuration.InActiveBlockSize, {255, 255, 255}, 100);
        }
        
    }
    
    game_rect ButtonQuad = 
    {
        ButtonQuad.x = LevelEditor->GridButtonLayer.x,
        ButtonQuad.y = LevelEditor->GridButtonLayer.y,
        ButtonQuad.w = LevelEntity->Configuration.InActiveBlockSize * 2, 
        ButtonQuad.h = LevelEntity->Configuration.InActiveBlockSize * 2
    };
    
    
    /* Grid buttons rendering*/ 
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->GridButtonLayer, {0, 0, 255}, 100);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->GridButtonQuad[0], {0, 0, 0}, 255, LevelEditor->PlusTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->GridButtonQuad[1], {0, 0, 0}, 255, LevelEditor->RowTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->GridButtonQuad[2], {0, 0, 0}, 255, LevelEditor->MinusTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->GridButtonQuad[3], {0, 0, 0}, 255, LevelEditor->PlusTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->GridButtonQuad[4], {0, 0, 0}, 255, LevelEditor->ColumnTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->GridButtonQuad[5], {0, 0, 0}, 255, LevelEditor->MinusTexture, 0, 0, Buffer);
    
    ButtonQuad.x = LevelEditor->FigureButtonLayer.x;
    ButtonQuad.y = LevelEditor->FigureButtonLayer.y;
    
    
    /* Figure buttons rendering*/ 
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->FigureButtonLayer, {0, 255, 0}, 100);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->FigureButtonQuad[0], {0, 0, 0}, 255, LevelEditor->PlusTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->FigureButtonQuad[1], {0, 0, 0}, 255, LevelEditor->MinusTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->FigureButtonQuad[2], {0, 0, 0}, 255, LevelEditor->RotateTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->FigureButtonQuad[3], {0, 0, 0}, 255, LevelEditor->FlipTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->FigureButtonQuad[4], {0, 0, 0}, 255, LevelEditor->FormTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->FigureButtonQuad[5], {0, 0, 0}, 255, LevelEditor->TypeTexture, 0, 0, Buffer);
    
    
    /* Save button rendering*/ 
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->SaveButtonLayer, {255, 0, 0}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->SaveButtonLayer, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->SaveTexture, &LevelEditor->SaveButtonQuad);
    
    
    /* Load buttons rendering*/ 
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->LoadButtonLayer, {255, 0, 0}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->LoadButtonLayer, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->LoadTexture, &LevelEditor->LoadButtonQuad);
    //
    // Prev/Next level buttons rendering
    //
    
    if(CurrentLevelIndex > 0)
    {
        GameRenderBitmapToBuffer(Buffer, LevelEditor->PrevLevelTexture, &LevelEditor->PrevLevelQuad);
    }
    
    if(CurrentLevelIndex < (s32)Memory->LevelMemoryAmount-1)
    {
        GameRenderBitmapToBuffer(Buffer, LevelEditor->NextLevelTexture, &LevelEditor->NextLevelQuad);
    }
    
    if(LevelEditor->ButtonPressed)
    {
        DEBUGRenderQuadFill(Buffer, &LevelEditor->ActiveButton, {255, 0, 0}, 150);
    }
    
    GameRenderBitmapToBuffer(Buffer, LevelEditor->LevelIndexTexture, &LevelEditor->LevelIndexQuad);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->LevelNumberTexture, &LevelEditor->LevelNumberQuad);
    
    /* Level number is selected */ 
    if(LevelEditor->LevelNumberSelected)
    {
        DEBUGRenderQuadFill(Buffer, &LevelEntity->LevelNumberQuad, {255, 0, 0}, 150);
    }
    
}

static bool
GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
    bool ShouldQuit = false;
    
    level_entity  *LevelEntity     = &Memory->LevelEntity;
    grid_entity   *&GridEntity   = LevelEntity->GridEntity;
    figure_entity *&FigureEntity = LevelEntity->FigureEntity;
    
    if(!Memory->IsInitialized)
    {
        // Memory initialization
        Memory->ToggleMenu = false;
        Memory->CurrentLevelIndex = 0;
        printf("Memory->LevelMemoryAmount = %d\n", Memory->LevelMemoryAmount);
        
        Memory->LevelNumberFont = TTF_OpenFont("..\\data\\Karmina-Bold.otf", 50);
        Assert(Memory->LevelNumberFont);
        
        MenuInit(Memory->MenuEntity, Memory, Buffer);
        
        // temporal level_entity initialization
        
        LevelEntity->LevelNumber = 0;
        LevelEntity->Configuration.DefaultBlocksInRow = 12;
        LevelEntity->Configuration.DefaultBlocksInCol = 9;
        LevelEntity->LevelStarted  = false;
        LevelEntity->LevelFinished = false;
        
        u32 RowAmount           = 5;
        u32 ColumnAmount        = 5;
        u32 FigureAmountReserve = 20;
        u32 MovingBlocksAmountReserved  = 10;
        
        RescaleGameField(Buffer, RowAmount, ColumnAmount,
                         FigureAmountReserve, LevelEntity->Configuration.DefaultBlocksInRow, LevelEntity->Configuration.DefaultBlocksInCol, LevelEntity);
        
        u32 ActiveBlockSize   = LevelEntity->Configuration.ActiveBlockSize;
        u32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
        
        //
        // Figure initialization
        //
        FigureEntity = (figure_entity*)malloc(sizeof(figure_entity));
        Assert(FigureEntity);
        
        FigureEntity->FigureAmount         = 0;
        FigureEntity->FigureAmountReserved = FigureAmountReserve;
        
        FigureEntity->ReturnIndex   = -1;
        FigureEntity->FigureActive  = -1;
        
        FigureEntity->IsGrabbed     = false;
        FigureEntity->IsRotating    = false;
        FigureEntity->IsReturning   = false;
        FigureEntity->IsRestarting  = false;
        FigureEntity->IsFlipping    = false;
        
        FigureEntity->RotationSum   = 0;
        FigureEntity->AreaAlpha     = 0;
        FigureEntity->FigureAlpha   = 0;
        FigureEntity->FadeInSum     = 0;
        FigureEntity->FadeOutSum    = 0;
        
        FigureEntity->FigureArea.w  = Buffer->Width;
        FigureEntity->FigureArea.h  = InActiveBlockSize * LevelEntity->Configuration.DefaultBlocksInCol;
        FigureEntity->FigureArea.y  = Buffer->Height - (FigureEntity->FigureArea.h);
        FigureEntity->FigureArea.x  = 0;
        
        FigureEntity->FigureUnit = (figure_unit*)malloc(sizeof(figure_unit)*FigureEntity->FigureAmountReserved);
        Assert(FigureEntity->FigureUnit);
        
        FigureUnitAddNewFigure(FigureEntity, Z_figure, classic, 0.0f, Memory, Buffer);
        FigureUnitAddNewFigure(FigureEntity, I_figure, stone,   0.0f, Memory, Buffer);
        FigureUnitAddNewFigure(FigureEntity, J_figure, mirror,  0.0f, Memory, Buffer);
        
        FigureEntity->FigureOrder = (u32*)malloc(sizeof(u32) * FigureEntity->FigureAmountReserved);
        Assert(FigureEntity->FigureOrder);
        
        for(u32 i = 0; i < FigureEntity->FigureAmountReserved; ++i) FigureEntity->FigureOrder[i] = i;
        
        // Grid initialization
        GridEntity  = (grid_entity *) malloc(sizeof(grid_entity));
        Assert(GridEntity);
        
        GridEntity->RowAmount           = RowAmount;
        GridEntity->ColumnAmount        = ColumnAmount;
        GridEntity->StickUnitsAmount    = FigureEntity->FigureAmount;
        GridEntity->MovingBlocksAmount  = 0;
        GridEntity->MovingBlocksAmountReserved  = MovingBlocksAmountReserved;
        GridEntity->GridArea.w = ActiveBlockSize * ColumnAmount;
        GridEntity->GridArea.h = ActiveBlockSize * RowAmount;
        GridEntity->GridArea.x = (Buffer->Width / 2) - (GridEntity->GridArea.w / 2);
        GridEntity->GridArea.y = (Buffer->Height - FigureEntity->FigureArea.h) / 2 - (GridEntity->GridArea.h / 2);
        
        //
        // UnitField initialization
        //
        GridEntity->UnitField = (s32**)malloc(GridEntity->RowAmount * sizeof(s32*));
        Assert(GridEntity->UnitField);
        for (u32 i = 0; i < GridEntity->RowAmount; ++i)
        {
            GridEntity->UnitField[i] = (s32*)malloc(sizeof(s32) * GridEntity->ColumnAmount);
            Assert(GridEntity->UnitField[i]);
            for (u32 j = 0; j < GridEntity->ColumnAmount; ++j)
            {
                GridEntity->UnitField[i][j] = 0;
            }
        }
        
        GridEntity->UnitSize = (r32**)malloc(GridEntity->RowAmount * sizeof(r32*));
        Assert(GridEntity->UnitSize);
        for(u32 i = 0; i < GridEntity->RowAmount; ++i)
        {
            GridEntity->UnitSize[i] = (r32*)malloc(sizeof(r32) * GridEntity->ColumnAmount);
            Assert(GridEntity->UnitSize[i]);
            for(u32 j = 0; j < GridEntity->ColumnAmount; ++j)
            {
                GridEntity->UnitSize[i][j] = 0;
            }
        }
        
        LevelEntity->Configuration.StartUpTimeToFinish = 2.0f;
        /* Change values below to be time configured*/
        LevelEntity->Configuration.RotationVel         = 600.0f;
        LevelEntity->Configuration.StartAlphaPerSec    = 500.0f;
        LevelEntity->Configuration.FlippingAlphaPerSec = 1000.0f;
        
        LevelEntity->Configuration.GridScalePerSec     = (ActiveBlockSize * ((RowAmount + ColumnAmount) - 1)) / LevelEntity->Configuration.StartUpTimeToFinish;
        
        FigureEntityAlignHorizontally(FigureEntity, InActiveBlockSize);
        
        //
        // StickUnits initialization
        //
        GridEntity->StickUnits = (sticked_unit*)calloc(sizeof(sticked_unit), FigureEntity->FigureAmountReserved);
        Assert(GridEntity->StickUnits);
        for (u32 i = 0; i < FigureEntity->FigureAmountReserved; ++i)
        {
            GridEntity->StickUnits[i].Index     = -1;
            GridEntity->StickUnits[i].IsSticked = false;
        }
        
        //
        // MovingBlocks initialization
        //
        
        GridEntity->MovingBlocks = (moving_block*)malloc(sizeof(moving_block) * MovingBlocksAmountReserved);
        Assert(GridEntity->MovingBlocks);
        
        //GridEntityAddMovingBlock(GridEntity, 1, 3, false, true, ActiveBlockSize);
        //GridEntityAddMovingBlock(GridEntity, 3, 2, true,  false, ActiveBlockSize);
        
        //
        // GridEntity texture initialization
        //
        
        GridEntity->NormalSquareTexture     = GetTexture(Memory, "grid_cell.png", Buffer->Renderer);
        GridEntity->VerticalSquareTexture   = GetTexture(Memory, "o_s.png", Buffer->Renderer);
        GridEntity->HorizontlaSquareTexture = GetTexture(Memory, "o_m.png", Buffer->Renderer);
        
        LevelEntityUpdateLevelNumber(LevelEntity, Memory, Buffer);
        
        // Level Editor initialization
        LevelEditorInit(LevelEntity, Memory, Buffer);
        LevelEditorUpdateLevelStats(Memory->LevelEditor, LevelEntity->LevelNumber, Memory->CurrentLevelIndex, Buffer);
        
        //Memory->Music = GetMusic(Memory, "amb_ending_water.ogg");
        //Assert(Memory->Music);
        //Mix_PlayMusic(Memory->Music, -1);
        
        Memory->IsInitialized = true;
        printf("memory init!\n");
    }
    
    /*
        CurrentTimeTick  = SDL_GetTicks();
        
        r32 TimeElapsed = (CurrentTimeTick - PreviousTimeTick) / 1000.0f;
        PreviousTimeTick = CurrentTimeTick;
        */
    
    if(Input->Keyboard.Escape.EndedDown)
    {
        if (!Memory->ToggleMenu) Memory->ToggleMenu = true;
        else Memory->ToggleMenu = false;
    }
    
    if(Memory->ToggleMenu)
    {
        MenuUpdateAndRender(Buffer, Memory, Input);
    }
    else
    {
        GameUpdateEvent(Input, LevelEntity, LevelEntity->Configuration.ActiveBlockSize, LevelEntity->Configuration.InActiveBlockSize, Buffer->Width, Buffer->Height);
        
        //PrintArray2D(GridEntity->UnitField, GridEntity->RowAmount, GridEntity->ColumnAmount);
        
        game_rect ScreenArea = { 0, 0, Buffer->Width, Buffer->Height};
        DEBUGRenderQuadFill(Buffer, &ScreenArea, { 42, 6, 21 }, 255);
        
        LevelEntityUpdateAndRender(Buffer, Memory, LevelEntity, Input->TimeElapsedMs);
        LevelEditorUpdateAndRender(Memory->LevelEditor, LevelEntity, Memory, Buffer, Input);
        
        //printf("TimeElapsed = %f\n", TimeElapsed);
        //printf("LevelEntity->LevelNumber = %d\n", LevelEntity->LevelNumber);
        
        //PrintArray2D(LevelEntity->GridEntity->UnitField, LevelEntity->GridEntity->RowAmount, LevelEntity->GridEntity->ColumnAmount);
    }
    
    //printf("Memory->CurrentLevelIndex = %d\n", Memory->CurrentLevelIndex);
    
    
    
    return(ShouldQuit);
}
