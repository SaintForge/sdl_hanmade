
#include "assets.h"
#include "entity.h"

static void
DEBUGRenderFigureShell(game_offscreen_buffer *Buffer, figure_unit *Entity, u32 BlockSize, SDL_Color color, u8 Alpha)
{
    u8 r, g, b, a;
    SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(Buffer->Renderer, color.r, color.g, color.b, Alpha);
    
    game_rect Rect = {};
    
    for (u32 i = 0; i < 4; ++i)
    {
        Rect.w = BlockSize;
        Rect.h = BlockSize;
        Rect.x = Entity->Shell[i].x - (Rect.w / 2);
        Rect.y = Entity->Shell[i].y - (Rect.h / 2);
        
        SDL_RenderFillRect(Buffer->Renderer, &Rect);
    }
    
    Rect.x = Entity->Center.x - (Rect.w / 2);
    Rect.y = Entity->Center.y - (Rect.h / 2);
    
    SDL_SetRenderDrawColor(Buffer->Renderer, r, g, b, a);
}

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
    level_entity *LevelEntity = (level_entity*)Memory->LocalMemoryStorage;
    
    u32 ActiveBlockSize   = LevelEntity->Configuration.ActiveBlockSize;
    u32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
    
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
    
    s32 RowAmount = GridEntity->RowAmount;
    s32 ColAmount = GridEntity->ColumnAmount;
    
    GridEntity->UnitField[(RowNumber * ColAmount) + ColNumber] = IsVertical ? 3 : 2;
    
    GridEntity->MovingBlocksAmount += 1;
}

static void
GridEntityMoveBlockHorizontally(grid_entity *GridEntity, moving_block *MovingBlock)
{
    s32 NewColNumber = 0;
    s32 RowAmount = GridEntity->RowAmount;
    s32 ColAmount = GridEntity->ColumnAmount;
    u32 RowNumber = MovingBlock->RowNumber;
    u32 ColNumber = MovingBlock->ColNumber;
    u32 ActiveBlockSize = MovingBlock->AreaQuad.w;
    s32 OldUnitIndex = (RowNumber * ColAmount) + ColNumber;
    
    NewColNumber = MovingBlock->MoveSwitch
        ? NewColNumber = ColNumber + 1
        : NewColNumber = ColNumber - 1;
    
    s32 NewUnitIndex = (RowNumber * ColAmount) + NewColNumber;
    if(NewColNumber < 0 || NewColNumber >= ColAmount) return;
    if(GridEntity->UnitField[NewUnitIndex] != 0) return;
    
    GridEntity->UnitField[OldUnitIndex] = 0;
    GridEntity->UnitField[NewUnitIndex] = 2;
    
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
    s32 RowAmount = GridEntity->RowAmount;
    s32 ColAmount = GridEntity->ColumnAmount;
    u32 ActiveBlockSize = MovingBlock->AreaQuad.w;
    s32 OldUnitIndex = (RowNumber * ColAmount) + ColNumber;
    
    NewRowNumber = MovingBlock->MoveSwitch
        ? NewRowNumber = RowNumber + 1
        : NewRowNumber = RowNumber - 1;
    
    s32 NewUnitIndex = (NewRowNumber * ColAmount) + ColNumber;
    if(NewRowNumber < 0 || NewRowNumber >= RowAmount) return;
    if(GridEntity->UnitField[NewUnitIndex] != 0) return;
    
    GridEntity->UnitField[OldUnitIndex]  = 0;
    GridEntity->UnitField[NewUnitIndex]  = 3;
    
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
    u32 RowAmount         = GridEntity->RowAmount;
    u32 ColumnAmount      = GridEntity->ColumnAmount;
    u32 FigureAmount      = FigureEntity->FigureAmount;
    r32 ActiveBlockSize   = LevelEntity->Configuration.ActiveBlockSize;
    r32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
    
    FigureEntity->IsRestarting = true;
    
    for(u32 i = 0; i < FigureAmount; ++i)
    {
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
                GridEntity->UnitField[(RowIndex * ColumnAmount) + ColIndex] = 0;
            }
            
            GridEntity->StickUnits[i].Index = -1;
        }
    }
}

static void
GameUpdateEvent(game_input *Input, level_entity *LevelEntity, game_offscreen_buffer *Buffer)
{
    grid_entity   *&GridEntity   = LevelEntity->GridEntity;
    figure_entity *&FigureEntity = LevelEntity->FigureEntity;
    figure_unit   *FigureUnit    = FigureEntity->FigureUnit;
    
    s32 ScreenWidth  = Buffer->Width;
    s32 ScreenHeight = Buffer->Height;
    u32 Size        = FigureEntity->FigureAmount;
    s32 MouseX      = Input->MouseX;
    s32 MouseY      = Input->MouseY;
    r32 BlockRatio  = 0;
    u32 ActiveIndex = FigureEntity->FigureActive;
    
    r32 ActiveBlockSize   = LevelEntity->Configuration.ActiveBlockSize;
    r32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
    
    if(!LevelEntity->LevelPaused)
    {
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
                        ShellQuad.w = FigureUnit[ActiveIndex].IsEnlarged ? ActiveBlockSize : InActiveBlockSize;
                        ShellQuad.h = FigureUnit[ActiveIndex].IsEnlarged ? ActiveBlockSize : InActiveBlockSize;
                        for(u32 j = 0; j < 4; ++j)
                        {
                            ShellQuad.x = FigureUnit[ActiveIndex].Shell[j].x - (ShellQuad.w / 2);
                            ShellQuad.y = FigureUnit[ActiveIndex].Shell[j].y - (ShellQuad.h / 2);
                            if(IsPointInsideRect(MouseX, MouseY, &ShellQuad))
                            {
                                FigureEntity->IsGrabbed = true;
                                
                                if(!FigureUnit[ActiveIndex].IsEnlarged)
                                {
                                    BlockRatio = ActiveBlockSize / InActiveBlockSize;
                                    FigureUnit[ActiveIndex].IsIdle = false;
                                    FigureUnit[ActiveIndex].IsEnlarged = true;
                                    FigureUnitResizeBy(&FigureUnit[ActiveIndex], BlockRatio);
                                }
                                
                                FigureEntity->FigureActive = ActiveIndex;
                                FigureEntityHighOrderFigure(FigureEntity, ActiveIndex);
                                SDL_ShowCursor(SDL_DISABLE);
                                
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
                        BlockRatio = InActiveBlockSize / ActiveBlockSize;
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
LevelEntityUpdateLevelEntityFromMemory(game_memory *Memory, s32 Index, bool IsStarted,
                                       game_offscreen_buffer *Buffer)
{
    level_entity *LevelEntity = (level_entity *)Memory->LocalMemoryStorage;
    level_memory *LevelMemory = (level_memory *)Memory->GlobalMemoryStorage;
    
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
        free(LevelEntity->GridEntity->UnitSize);
        LevelEntity->GridEntity->UnitSize = 0;
    }
    
    if(LevelEntity->GridEntity->UnitField)
    {
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
    
    
    u32 RowAmount    = LevelMemory[Index].RowAmount;
    u32 ColumnAmount = LevelMemory[Index].ColumnAmount;
    
    LevelEntity->Configuration.PixelsDrawn  = 0;
    LevelEntity->Configuration.PixelsToDraw = 0;
    LevelEntity->Configuration.StartUpTimeElapsed  = 0.0f;
    LevelEntity->Configuration.StartUpTimeToFinish = 1.0f;
    
    LevelEntity->LevelNumber   = LevelMemory[Index].LevelNumber;
    LevelEntity->LevelStarted  = IsStarted;
    LevelEntity->LevelFinished = false;
    
    RescaleGameField(Buffer, RowAmount, ColumnAmount,
                     LevelEntity->FigureEntity->FigureAmountReserved, LevelEntity->Configuration.DefaultBlocksInRow, LevelEntity->Configuration.DefaultBlocksInCol, LevelEntity);
    
    u32 ActiveBlockSize   = LevelEntity->Configuration.ActiveBlockSize;
    u32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
    
    LevelEntity->Configuration.GridScalePerSec  = (ActiveBlockSize * ((RowAmount + ColumnAmount) - 1)) / (LevelEntity->Configuration.StartUpTimeToFinish * 0.5f);
    LevelEntity->Configuration.StartAlphaPerSec = 255.0f / (LevelEntity->Configuration.StartUpTimeToFinish * 0.5f);
    
    /* figure_entity initialization */
    
    LevelEntity->FigureEntity->FigureAlpha = 0;
    
    LevelEntity->FigureEntity->FigureArea.w = Buffer->Width;
    LevelEntity->FigureEntity->FigureArea.h = InActiveBlockSize * LevelEntity->Configuration.DefaultBlocksInCol;
    LevelEntity->FigureEntity->FigureArea.y = Buffer->Height - (LevelEntity->FigureEntity->FigureArea.h);
    LevelEntity->FigureEntity->FigureArea.x = 0;
    
    LevelEntity->FigureEntity->FigureAmount = 0;
    LevelEntity->FigureEntity->FigureUnit = (figure_unit *) malloc(sizeof(figure_unit) * LevelEntity->FigureEntity->FigureAmountReserved);
    Assert(LevelEntity->FigureEntity->FigureUnit);
    
    for(u32 i = 0; i < LevelMemory[Index].FigureAmount; ++i)
    {
        FigureUnitAddNewFigure(LevelEntity->FigureEntity, LevelMemory[Index].Figures[i].Form, LevelMemory[Index].Figures[i].Type, LevelMemory[Index].Figures[i].Angle, Memory, Buffer);
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
    
    LevelEntity->GridEntity->UnitField = (s32 *) calloc(RowAmount * ColumnAmount, sizeof(s32));
    Assert(LevelEntity->GridEntity->UnitField);
    
    for(s32 Row = 0; Row < RowAmount; ++Row)
    {
        for(s32 Col = 0; Col < ColumnAmount; ++Col)
        {
            s32 UnitIndex = (Row * ColumnAmount) + Col;
            LevelEntity->GridEntity->UnitField[UnitIndex] = LevelMemory[Index].UnitField[UnitIndex];
        }
    }
    
    if(!IsStarted)
    {
        LevelEntity->GridEntity->UnitSize = (r32 *) calloc(RowAmount * ColumnAmount, sizeof(r32));
        Assert(LevelEntity->GridEntity->UnitSize);
        
        for(s32 Row = 0; Row < RowAmount; ++Row)
        {
            for (s32 Col = 0; Col < ColumnAmount; ++Col)
            {
                LevelEntity->GridEntity->UnitSize[(Row * ColumnAmount) + Col] = 0;
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
    
    for(u32 i = 0; i < LevelMemory[Index].MovingBlocksAmount; ++i)
    {
        u32 RowNumber   = LevelMemory[Index].MovingBlocks[i].RowNumber;
        u32 ColNumber   = LevelMemory[Index].MovingBlocks[i].ColNumber;
        bool IsVertical = LevelMemory[Index].MovingBlocks[i].IsVertical;
        bool MoveSwitch = LevelMemory[Index].MovingBlocks[i].MoveSwitch;
        
        GridEntityAddMovingBlock(LevelEntity->GridEntity, RowNumber, ColNumber, IsVertical, MoveSwitch, ActiveBlockSize);
    }
    
    //
    // level number texture loading
    //
    
    LevelEntityUpdateLevelNumber(LevelEntity, Memory, Buffer);
}

static void
LevelEntityUpdateStartUpAnimation(level_entity *LevelEntity,
                                  game_memory *Memory,
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
    
    bool IsGridReady    = true;
    bool IsFiguresReady = true;
    bool IsLevelReady   = true;
    
    StartUpTimeElapsed += TimeElapsed;
    
    /* Grid Start up animation */
    
    for(s32 Line = 1; Line <= ((RowAmount + ColAmount) - 1); ++Line)
    {
        bool ShouldBreak = true;
        
        s32 StartColumn  = Max2(0, Line - RowAmount);
        s32 Count        = Min3(Line, (ColAmount - StartColumn), RowAmount);
        
        for(u32 i = 0; i < Count; ++i)
        {
            u32 RowIndex = Min2(RowAmount, Line) - i - 1;
            u32 ColIndex = StartColumn + i;
            s32 UnitIndex = (RowIndex * ColAmount) + ColIndex;
            
            r32 *CellSize = &GridEntity->UnitSize[UnitIndex];
            s32 *CellField = &GridEntity->UnitField[UnitIndex];
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
                                    s32 NextUnitIndex = (NextRowIndex * RowAmount) + NextColIndex;
                                    
                                    GridEntity->UnitSize[NextUnitIndex] += PixelsYetToDraw;
                                    if(GridEntity->UnitSize[NextUnitIndex] >= MaximumBlockSize)
                                    {
                                        GridEntity->UnitSize[NextUnitIndex] = MaximumBlockSize;
                                        //Mix_PlayChannel( -1, Memory->Sound, 0);
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
    
    for(s32 Row = 0; Row < RowAmount; ++Row)
    {
        for(s32 Col = 0; Col < ColAmount; ++Col)
        {
            if(GridEntity->UnitSize[(Row * ColAmount) + Col] < MaximumBlockSize)
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
    
    IsLevelReady = IsGridReady && IsFiguresReady;
    
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
LevelEntityUpdateAndRender(level_entity *LevelEntity, game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
    grid_entity   *&GridEntity   = LevelEntity->GridEntity;
    figure_entity *&FigureEntity = LevelEntity->FigureEntity;
    figure_unit   *FigureUnit    = FigureEntity->FigureUnit;
    
    u32 ActiveBlockSize   = LevelEntity->Configuration.ActiveBlockSize;
    u32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
    
    game_rect AreaQuad = {};
    r32 TimeElapsed  = Input->TimeElapsedMs;
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
        LevelEntityUpdateStartUpAnimation(LevelEntity, Memory,
                                          Buffer, TimeElapsed);
        if(LevelEntity->LevelStarted)
        {
            free(GridEntity->UnitSize);
            GridEntity->UnitSize = 0;
            
            LevelEntity->Configuration.StartUpTimeElapsed = 0;
            LevelEntity->Configuration.PixelsDrawn  = 0;
            LevelEntity->Configuration.PixelsToDraw = 0;
        }
        else
        {
            return;
        }
        
    }
    
    /* game_input checking */
    GameUpdateEvent(Input, LevelEntity, Buffer);
    
    
    /* GridEntity update and render */
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
            /* Unstick from the grid */
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
                        GridEntity->UnitField[(RowIndex * ColumnAmount) + ColIndex] = 0;
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
                /* Check if we can stick it! */
                
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
                        if(GridEntity->UnitField[(RowIndex[i] * ColumnAmount) + ColIndex[i]] > 0)
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
                        
                        for (u32 Row = 0; Row < RowAmount; ++Row)
                        {
                            for (u32 Col = 0; Col < ColumnAmount; ++Col)
                            {
                                if(GridEntity->UnitField[(Row * ColumnAmount) + Col] == 1)
                                {
                                    IsFull = true;
                                }
                            }
                        }
                        
                        
                        
                        if(IsFull == true)
                        {
                            /* the grid is full and level is complete */
                        }
                    }
                }
            }
        }
    }
    
    
    /* StickUnits */
    
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
                    GridEntity->UnitField[(RowIndex * ColumnAmount) + ColIndex] = 1;
                }
            }
        }
    }
    
    if(IsAllSticked)
    {
        /* Level is completed */
        
        s32 NextLevelIndex = Memory->CurrentLevelIndex + 1;
        if(NextLevelIndex < Memory->LevelMemoryAmount)
        {
            /* Switching to the next level*/
            
            //TODO(Max): Do we need that LevelFinished thing???
            LevelEntity->LevelFinished = true;
            Memory->CurrentLevelIndex = NextLevelIndex;
            LevelEntityUpdateLevelEntityFromMemory(Memory, NextLevelIndex, false, Buffer);
            
            return;
        }
        else
        {
            /* Staying at the same level */
        }
    }
    
    AreaQuad.w = ActiveBlockSize;
    AreaQuad.h = ActiveBlockSize;
    
    for (u32 Row = 0; Row < RowAmount; ++Row)
    {
        StartY = GridEntity->GridArea.y + (ActiveBlockSize * Row) + (ActiveBlockSize / 2);
        for (u32 Col = 0; Col < ColumnAmount; ++Col)
        {
            StartX = GridEntity->GridArea.x + (ActiveBlockSize * Col) + (ActiveBlockSize / 2);
            
            AreaQuad.x = StartX - (AreaQuad.w / 2);
            AreaQuad.y = StartY - (AreaQuad.h / 2);
            
            u32 GridUnit = GridEntity->UnitField[(Row * ColumnAmount) + Col];
            if(GridUnit == 0 || GridUnit == 2 || GridUnit == 3)
            {
                GameRenderBitmapToBuffer(Buffer, GridEntity->NormalSquareTexture, &AreaQuad);
            }
        }
    }
    
    /* MovingBlocks Update and Rendering */
    
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
                GridEntity->UnitField[(RowNumber * ColumnAmount) + ColNumber] = 1;
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
    
    /* FigureEntity Update and Rendering */
    
    /* Figure Area Highlight */
    
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
    
    /* Figure returning to the idle zone */
    
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
    
    /* Rotation Animation */
    
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
    
    /* Flipping Animation */
    
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
    
    /* Figure Rendering */
    
    for(u32 i = 0; i < FigureAmount; ++i)
    {
        u32 Index = FigureEntity->FigureOrder[i];
        
        FigureUnitRenderBitmap(Buffer, &FigureUnit[Index]);
    }
    
    GameRenderBitmapToBuffer(Buffer, LevelEntity->LevelNumberShadowTexture, &LevelEntity->LevelNumberShadowQuad);
    GameRenderBitmapToBuffer(Buffer, LevelEntity->LevelNumberTexture,   &LevelEntity->LevelNumberQuad);
    
}
