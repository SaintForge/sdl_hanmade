
static s32
CalculateGridBlockSize(s32 RowAmount, s32 ColumnAmount, 
                       r32 GridWidth, r32 GridHeight)

{
    s32 Result = 0;
    
    Result = GridWidth < GridHeight ? GridWidth : GridHeight;
    Result = roundf((r32)Result / 9.0f);
    
    if(Result % 2)
    {
        Result = Result + 1;
    }
    
    s32 MinRowSize = Result;
    s32 MinColumnSize = Result;
    
    if((Result * (RowAmount + 1)) > GridHeight)
    {
        s32 SizeLeft = Result * (RowAmount+1);
        SizeLeft = SizeLeft - GridHeight;
        SizeLeft = roundf((r32)SizeLeft / (r32)RowAmount);
        
        if(SizeLeft % 2 == 0)
        {
            SizeLeft += 1;
        }
        
        MinRowSize = MinRowSize - SizeLeft;
    }
    if((Result * (ColumnAmount+1)) > GridWidth)
    {
        s32 SizeLeft = Result * (ColumnAmount+1);
        SizeLeft = SizeLeft - GridWidth;
        SizeLeft = roundf((r32)SizeLeft / (r32)ColumnAmount);
        
        if(SizeLeft % 2 == 0)
        {
            SizeLeft += 1;
        }
        
        MinColumnSize = MinColumnSize - SizeLeft;
    }
    
    Result = MinRowSize < MinColumnSize ? MinRowSize : MinColumnSize;
    
    return(Result);
}

static game_rect
FigureUnitGetArea(figure_unit *Unit)
{
    game_rect Area = {(s32)Unit->Shell[0].x, (s32)Unit->Shell[0].y, -500, -500};
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
    
    if(Area.w >= Unit->Size.h)
    {
        Area.h = Unit->Size.h;
        Area.w = Unit->Size.w;
        OffsetX = (Area.y + Area.h) - (Area.y + Area.h / 2);
    }
    else
    {
        Area.h = Unit->Size.w;
        Area.w = Unit->Size.h;
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

// TODO(Sierra): figure out the way to compute the size also by the row amount
static s32
CalculateFigureBlockSizeByWidth(s32 FigureAmount, s32 FigureAreaWidth)
{
    s32 Result = {};
    
    Result = roundf(((r32)FigureAreaWidth / 9.0f));
    
    if(Result % 2)
    {
        Result = Result + 1;
    }
    
    return(Result);
}

static s32 
CalculateFigureBlockSizeByHeight(s32 FigureAmount, s32 FigureAreaHeight)
{
    s32 Result = {};
    
    Result = roundf(((r32)FigureAreaHeight / 9.0f));
    
    if(Result % 2)
    {
        Result = Result + 1;
    }
    
    return(Result);
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
    //game_rect *Rectangle = &Entity->AreaQuad;
    game_point OldCenter = {};
    game_point NewCenter = {};
    
    s32 OffsetX = 0;
    s32 OffsetY = 0;
    s32 OldX = Entity->Position.x;
    s32 OldY = Entity->Position.y;
    
    OldCenter.x = Entity->Position.x + roundf((r32)Entity->Size.w / 2.0f);
    OldCenter.y = Entity->Position.y + roundf((r32)Entity->Size.h / 2.0f);
    
    Entity->Size.w = roundf((r32)Entity->Size.w * ScaleFactor);
    Entity->Size.h = roundf((r32)Entity->Size.h * ScaleFactor);
    
    NewCenter.x = Entity->Position.x + roundf((r32)Entity->Size.w / 2.0f);
    NewCenter.y = Entity->Position.y + roundf((r32)Entity->Size.h / 2.0f);
    
    Entity->Position.x += (OldCenter.x - NewCenter.x);
    Entity->Position.y += (OldCenter.y - NewCenter.y);
    
    for (u32 i = 0; i < 4; ++i)
    {
        s32 SomeOffsetX = Entity->Shell[i].x - OldX;
        s32 SomeOffsetY = Entity->Shell[i].y - OldY;
        
        OffsetX = roundf((r32)SomeOffsetX * ScaleFactor);
        OffsetY = roundf((r32)SomeOffsetY * ScaleFactor);
        Entity->Shell[i].x = OldX + OffsetX;
        Entity->Shell[i].y = OldY + OffsetY;
        Entity->Shell[i].x += (OldCenter.x - NewCenter.x);
        Entity->Shell[i].y += (OldCenter.y - NewCenter.y);
    }
    
}

static void
FigureUnitRotateShellBy(figure_unit *Entity, float Angle)
{
    if((s32)Entity->Angle == 0)
    {
        Entity->Angle = 0;
    }
    
    v2 Center = {};
    Center.x   = Entity->Position.x + (Entity->Size.w / 2);
    Center.y   = Entity->Position.y + (Entity->Size.h) * Entity->CenterOffset;
    
    for (u32 i = 0; i < 4; ++i)
    {
        float Radians = Angle * (M_PI / 180.0f);
        float Cos = cos(Radians);
        float Sin = sin(Radians);
        
        float X = Center.x + (Entity->Shell[i].x - Center.x) * Cos
            - (Entity->Shell[i].y - Center.y) * Sin;
        float Y = Center.y + (Entity->Shell[i].x - Center.x) * Sin
            + (Entity->Shell[i].y - Center.y) * Cos;
        
        Entity->Shell[i].x = roundf(X);
        Entity->Shell[i].y = roundf(Y);
    }
}


static void
FigureUnitInitFigure(figure_unit *FigureUnit, figure_form Form,
                     figure_type Type, u32 InActiveBlockSize)
{
    FigureUnit->IsIdle       = true;
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
    
    switch(Form)
    {
        case I_figure:
        {
            /* TODO(msokolov): Implement something like this instead of using std::vector
            v3 MapColor[] =
    {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
    };
            */
            
            matrix = 
            { 
                {1, 1, 1, 1},
                {0, 0, 0, 0} 
            };
            RowAmount    = 4;
            ColumnAmount = 1;
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
        }break;
    }
    
    FigureUnit->Position.x = 0;
    FigureUnit->Position.y = 0;
    FigureUnit->Size.w = RowAmount * InActiveBlockSize;
    FigureUnit->Size.h = ColumnAmount * InActiveBlockSize;
    FigureUnit->CenterOffset = CenterOffset;
    
    u32 ShellIndex = 0;
    u32 HalfBlock  = roundf((r32)InActiveBlockSize / 2.0f);
    
    for(u32 i = 0; i < 2; i++)
    {
        for(u32 j = 0; j < 4; j++)
        {
            if(matrix[i][j] == 1)
            {
                FigureUnit->Shell[ShellIndex].x = FigureUnit->Position.x + (j * InActiveBlockSize) + HalfBlock;
                FigureUnit->Shell[ShellIndex].y = FigureUnit->Position.y + (i * InActiveBlockSize) + HalfBlock;
                
                ShellIndex++;
            }
        }
    }
    
    FigureUnitRotateShellBy(FigureUnit, FigureUnit->Angle);
}

static void
FigureUnitAddNewFigure(figure_entity *FigureEntity, figure_form Form, figure_type Type, u32 InActiveBlockSize)

{
    if(FigureEntity->FigureAmount >= FIGURE_AMOUNT_MAXIMUM) return;
    
    u32 Index = FigureEntity->FigureAmount;
    FigureEntity->FigureAmount += 1;
    
    FigureUnitInitFigure(&FigureEntity->FigureUnit[Index], Form, Type, InActiveBlockSize);
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
        
        //NewCenterX = (AreaQuad.x + AreaQuad.w) - (Unit->Center.x - AreaQuad.x);
        //Unit->Center.x = NewCenterX;
    }
    else
    {
        for (u32 i = 0; i < 4; ++i)
        {
            NewY = (AreaQuad.y + AreaQuad.h) - (Unit->Shell[i].y - AreaQuad.y);
            Unit->Shell[i].y = NewY;
        }
        
        //NewCenterY = (AreaQuad.y + AreaQuad.h) - (Unit->Center.y - AreaQuad.y);
        //Unit->Center.y = NewCenterY;
    }
}

static void
FigureUnitMove_(figure_unit *Entity, v2 dt)
{
    Entity->Position += dt;
    
    for (u32 Index = 0;
         Index < FIGURE_BLOCKS_MAXIMUM;
         Index++)
    {
        Entity->Shell[Index] += dt;
    }
}

static void
FigureUnitMove(figure_unit *Entity, r32 XShift, r32 YShift)
{
    v2 Offset = {XShift, YShift};
    
    Entity->Position += Offset;
    
    for (u32 i = 0; i < 4; ++i)
    {
        Entity->Shell[i] += Offset;
    }
}

#if 0
static void
FigureUnitMoveTo(figure_unit *Entity, s32 NewPointX, s32 NewPointY)
{
    s32 XShift = NewPointX - Entity->Center.x;
    s32 YShift = NewPointY - Entity->Center.y;
    FigureUnitMove(Entity, XShift, YShift);
}
#endif


static game_rect 
ConstructFigureArea(figure_unit *FigureUnit, s32 FigureBlockSize)
{
    game_rect Result = {(s32)FigureUnit->Shell[0].x, (s32)FigureUnit->Shell[0].y, -99999, -9999};
    
    u32 OffsetX = 0;
    u32 OffsetY = 0;
    bool ZeroArea = false;
    
    for (u32 i = 0; i < 4; ++i)
    {
        if(Result.x >= FigureUnit->Shell[i].x) Result.x = FigureUnit->Shell[i].x;
        if(Result.y >= FigureUnit->Shell[i].y) Result.y = FigureUnit->Shell[i].y;
        if(Result.w <= FigureUnit->Shell[i].x) Result.w = FigureUnit->Shell[i].x;
        if(Result.h <= FigureUnit->Shell[i].y) Result.h = FigureUnit->Shell[i].y;
    }
    
    Result.w -= Result.x;
    Result.h -= Result.y;
    Result.w += roundf((r32)FigureBlockSize / 2.0f);
    Result.h += roundf((r32)FigureBlockSize / 2.0f);
    
    if(Result.w == 0 || Result.h == 0)
    {
        ZeroArea = true;
    }
    
    Result.x -= roundf((r32)FigureBlockSize / 2.0f);
    Result.y -= roundf((r32)FigureBlockSize / 2.0f);
    
    return(Result);
}

static game_texture *
PickFigureTexture(figure_form Form, figure_type Type, figure_entity *FigureEntity)
{
    game_texture *Result = NULL;
    
    switch(Form)
    {
        case O_figure:
        {
            switch(Type)
            {
                case classic:
                {
                    Result = FigureEntity->O_ClassicTexture;
                } break;
                
                case stone:
                {
                    Result = FigureEntity->O_StoneTexture;
                } break;
                
                case mirror:
                {
                    Result = FigureEntity->O_MirrorTexture;
                } break;
            }
        } break;
        
        case I_figure:
        {
            switch(Type)
            {
                case classic:
                {
                    Result = FigureEntity->I_ClassicTexture;
                } break;
                
                case stone:
                {
                    Result = FigureEntity->I_StoneTexture;
                } break;
                
                case mirror:
                {
                    Result = FigureEntity->I_MirrorTexture;
                } break;
            }
        } break;
        
        case L_figure:
        {
            switch(Type)
            {
                case classic:
                {
                    Result = FigureEntity->L_ClassicTexture;
                } break;
                
                case stone:
                {
                    Result = FigureEntity->L_StoneTexture;
                } break;
                
                case mirror:
                {
                    Result = FigureEntity->L_MirrorTexture;
                } break;
            }
        } break;
        
        case J_figure:
        {
            switch(Type)
            {
                case classic:
                {
                    Result = FigureEntity->J_ClassicTexture;
                } break;
                
                case stone:
                {
                    Result = FigureEntity->J_StoneTexture;
                } break;
                
                case mirror:
                {
                    Result = FigureEntity->J_MirrorTexture;
                } break;
            }
        } break;
        
        case Z_figure:
        {
            switch(Type)
            {
                case classic:
                {
                    Result = FigureEntity->Z_ClassicTexture;
                } break;
                
                case stone:
                {
                    Result = FigureEntity->Z_StoneTexture;
                } break;
                
                case mirror:
                {
                    Result = FigureEntity->Z_MirrorTexture;
                } break;
            }
        } break;
        
        case S_figure:
        {
            switch(Type)
            {
                
                case classic:
                {
                    Result = FigureEntity->S_ClassicTexture;
                } break;
                
                case stone:
                {
                    Result = FigureEntity->S_StoneTexture;
                } break;
                
                case mirror:
                {
                    Result = FigureEntity->S_MirrorTexture;
                } break;
            }
        } break;
        
        case T_figure:
        {
            switch(Type)
            {
                case classic:
                {
                    Result = FigureEntity->T_ClassicTexture;
                } break;
                
                case stone:
                {
                    Result = FigureEntity->T_StoneTexture;
                } break;
                
                case mirror:
                {
                    Result = FigureEntity->T_MirrorTexture;
                } break;
            } 
        } break;
    }
    
    
    return(Result);
}

static void
FigureUnitRenderBitmap(render_group *RenderGroup, figure_unit *Entity, game_texture *Texture)
{
    
}

static void
FigureUnitSetToDefaultArea(figure_unit* Unit, r32 BlockRatio)
{
    s32 ShiftX = 0;
    s32 ShiftY = 0;
    
    if(Unit->IsEnlarged)
    {
        r32 Angle     = Unit->Angle;
        r32 HomeAngle = Unit->HomeAngle;
        r32 AngleDifference      = 0;
        
        if(Unit->Angle != Unit->HomeAngle)
        {
            AngleDifference = HomeAngle - Angle;
            FigureUnitRotateShellBy(Unit, AngleDifference);
            Unit->Angle = HomeAngle;
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
    
    Unit->HomePosition = Unit->Position;
    Unit->HomeAngle  = Unit->Angle;
}

static void
FigureUnitMoveToDefaultArea(figure_unit *FigureUnit, u32 ActiveBlockSize)
{
    v2 Offset = FigureUnit->HomePosition - FigureUnit->Position;
    
    r32 Magnitude = 0.0f;
    r32 MaxSpeed  = 0.0f;
    
    Magnitude = sqrt(Offset.x*Offset.x + Offset.y*Offset.y);
    MaxSpeed = Magnitude - ActiveBlockSize;
    
    if(Magnitude > (MaxSpeed))
    {
        if(Magnitude != 0) 
        {
            // TODO(msokolov): add divide operation for vectors
            Offset.x /= Magnitude;
            Offset.y /= Magnitude;
        }
        
        Offset *= MaxSpeed;
        Offset.x = roundf(Offset.x);
        Offset.y = roundf(Offset.y);
        
        FigureUnitMove_(FigureUnit, Offset);
    }
}

static void
GridEntityAddMovingBlock(grid_entity *GridEntity,
                         u32 RowNumber, u32 ColNumber, 
                         bool IsVertical, bool MoveSwitch, 
                         u32 GridBlockSize)
{
    if(GridEntity->MovingBlocksAmount >= MOVING_BLOCKS_MAXIMUM) return;
    if((RowNumber >= GridEntity->RowAmount) || RowNumber < 0) return;
    if((ColNumber >= GridEntity->ColumnAmount) || ColNumber < 0) return;
    
    u32 Index = GridEntity->MovingBlocksAmount;
    
    s32 ActualGridWidth = GridBlockSize * GridEntity->ColumnAmount;
    s32 ActualGridHeight = GridBlockSize * GridEntity->RowAmount;
    
    game_rect GridArea = {};
    GridArea.w = ActualGridWidth;
    GridArea.h = ActualGridHeight;
    GridArea.x = GridEntity->GridArea.x + (GridEntity->GridArea.w / 2) - (ActualGridWidth / 2);
    GridArea.y = GridEntity->GridArea.y + (GridEntity->GridArea.h / 2) - (ActualGridHeight / 2);
    
    GridEntity->MovingBlocks[Index].AreaQuad.w = GridBlockSize;
    GridEntity->MovingBlocks[Index].AreaQuad.h = GridBlockSize;
    GridEntity->MovingBlocks[Index].AreaQuad.x = GridArea.x + (ColNumber * GridBlockSize);
    GridEntity->MovingBlocks[Index].AreaQuad.y = GridArea.y + (RowNumber * GridBlockSize);
    
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
RestartLevelEntity(playground *LevelEntity)
{
    grid_entity   *GridEntity   = &LevelEntity->GridEntity;
    figure_entity *FigureEntity = &LevelEntity->FigureEntity;
    
    if(FigureEntity->IsRotating || FigureEntity->IsFlipping) return;
    
    r32 BlockRatio        = 0;
    u32 RowAmount         = GridEntity->RowAmount;
    u32 ColumnAmount      = GridEntity->ColumnAmount;
    u32 FigureAmount      = FigureEntity->FigureAmount;
    r32 GridBlockSize     = LevelEntity->Configuration.GridBlockSize;
    r32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
    
    FigureEntity->IsRestarting = true;
    
    for(u32 i = 0; i < FigureAmount; ++i)
    {
        if(!FigureEntity->FigureUnit[i].IsIdle)
        {
            
            FigureEntity->FigureUnit[i].IsStick = false;
            BlockRatio = InActiveBlockSize / GridBlockSize;
            FigureUnitSetToDefaultArea(&FigureEntity->FigureUnit[i], BlockRatio);
            FigureUnitMoveToDefaultArea(&FigureEntity->FigureUnit[i], GridBlockSize);
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
GameUpdateEvent(game_input *Input, playground *LevelEntity, u32 ScreenWidth, u32 ScreenHeight)
{
    grid_entity   *GridEntity   = &LevelEntity->GridEntity;
    figure_entity *FigureEntity = &LevelEntity->FigureEntity;
    figure_unit   *FigureUnit    = FigureEntity->FigureUnit;
    
    u32 Size        = FigureEntity->FigureAmount;
    s32 MouseX      = Input->MouseX;
    s32 MouseY      = Input->MouseY;
    r32 BlockRatio  = 0;
    u32 ActiveIndex = FigureEntity->FigureActive;
    
    r32 GridBlockSize = LevelEntity->Configuration.GridBlockSize;
    r32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
    
    if(!LevelEntity->LevelPaused)
    {
        if(Input->MouseButtons[0].EndedDown)
        {
            if(!FigureEntity->IsGrabbed)
            {
                
                for (s32 i = Size - 1; i >= 0; --i)
                {
                    ActiveIndex = FigureEntity->FigureOrder[i];
                    game_rect AreaQuad = FigureUnitGetArea(&FigureUnit[ActiveIndex]);
                    if(IsPointInsideRect(MouseX, MouseY, &AreaQuad))
                    {
                        game_rect ShellQuad = {0};
                        ShellQuad.w = FigureUnit[ActiveIndex].IsEnlarged ? GridBlockSize : InActiveBlockSize;
                        ShellQuad.h = FigureUnit[ActiveIndex].IsEnlarged ? GridBlockSize : InActiveBlockSize;
                        for(u32 j = 0; j < 4; ++j)
                        {
                            ShellQuad.x = FigureUnit[ActiveIndex].Shell[j].x - (ShellQuad.w / 2);
                            ShellQuad.y = FigureUnit[ActiveIndex].Shell[j].y - (ShellQuad.h / 2);
                            if(IsPointInsideRect(MouseX, MouseY, &ShellQuad))
                            {
                                FigureEntity->IsGrabbed = true;
                                
                                if(!FigureUnit[ActiveIndex].IsEnlarged)
                                {
                                    BlockRatio = GridBlockSize / InActiveBlockSize;
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
                    
                    v2 Center = {};
                    Center.x   = FigureUnit[ActiveIndex].Position.x + (FigureUnit[ActiveIndex].Size.w / 2);
                    Center.y   = FigureUnit[ActiveIndex].Position.y + (FigureUnit[ActiveIndex].Size.h) * FigureUnit[ActiveIndex].CenterOffset;
                    
                    if(IsFigureUnitInsideRect(&FigureUnit[ActiveIndex], &FigureEntity->FigureArea) || 
                       (!IsPointInsideRect(Center.x,Center.y, &ScreenArea)))
                    {
                        FigureUnit[ActiveIndex].IsIdle = true;
                        BlockRatio = InActiveBlockSize / GridBlockSize;
                        FigureUnitSetToDefaultArea(&FigureUnit[ActiveIndex], BlockRatio);
                        
                        FigureEntity->IsReturning = true;
                        FigureEntity->ReturnIndex = ActiveIndex;
                        
                        FigureUnitMoveToDefaultArea(&FigureUnit[ActiveIndex], GridBlockSize);
                    }
                    
                    SDL_ShowCursor(SDL_ENABLE);
                    FigureEntity->IsGrabbed    = false;
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
                            //SDL_SetTextureBlendMode(FigureUnit[ActiveIndex].Texture, SDL_BLENDMODE_BLEND);
                            
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
            v2 dt = {(r32)Input->MouseRelX, (r32)Input->MouseRelY};
            r32 XShift = Input->MouseRelX;
            r32 YShift = Input->MouseRelY;
            
            FigureUnitMove_(&FigureUnit[ActiveIndex], dt);
            //FigureUnitMove(&FigureUnit[ActiveIndex], XShift, YShift);
        }
    }
}


static void
FigureEntityAlignFigures(figure_entity *Entity, u32 BlockSize)
{
    u32 Size = Entity->FigureAmount;
    u32 ColumnSize1 = 0;
    u32 ColumnSize2 = 0;
    u32 FigureIntervalX = roundf((r32)BlockSize / 4.0f);
    u32 FigureIntervalY = roundf((r32)BlockSize / 4.0f);
    
    game_rect AreaQuad    = {};
    game_rect DefaultZone = Entity->FigureArea;
    
    for (u32 i = 0; i < Size; ++i)
    {
        AreaQuad = FigureUnitGetArea(&Entity->FigureUnit[i]);
        i % 2 == 0
            ? ColumnSize1 += AreaQuad.h + FigureIntervalY
            : ColumnSize2 += AreaQuad.h + FigureIntervalY;
    }
    
    u32 PitchX           = 0;
    s32 NewPositionX     = 0;
    s32 NewPositionY     = 0;
    u32 CurrentColumnSize1  = 0;
    u32 CurrentColumnSize2  = 0;
    u32 FigureBoxWidth  = 0;
    s32 SpaceBetweenGrid = roundf((r32)BlockSize / 4.0f);
    
    for (u32 i = 0; i < Size; ++i)
    {
        PitchX = i % 2;
        AreaQuad = FigureUnitGetArea(&Entity->FigureUnit[i]);
        
        FigureBoxWidth = BlockSize * 4;
        NewPositionX  = DefaultZone.x + (FigureBoxWidth * PitchX);
        NewPositionX += SpaceBetweenGrid;
        NewPositionX += (FigureBoxWidth / 2 ) - (AreaQuad.w / 2);
        NewPositionX += FigureIntervalX * PitchX;
        
        if(i % 2 == 0)
        {
            NewPositionY = DefaultZone.y + FigureIntervalY;
            NewPositionY += CurrentColumnSize1;
            
            CurrentColumnSize1 += AreaQuad.h + FigureIntervalY;
        }
        else
        {
            NewPositionY = DefaultZone.y + FigureIntervalY;
            NewPositionY += CurrentColumnSize2;
            
            CurrentColumnSize2 += AreaQuad.h + FigureIntervalY;
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

static v2
Move2DPointPerSec(v2 p1, v2 p2, r32 MaxVelocity, r32 TimeElapsed)
{
    v2 Result = {};
    
    Result.x= p2.x - p1.x;
    Result.y= p2.y - p1.y;
    
    r32 Distance = sqrt(Result.x * Result.x + Result.y * Result.y);
    if(Distance > MaxVelocity)
    {
        Result.x = Result.x / Distance;
        Result.y = Result.y / Distance;
        
        Result.x = Result.x * MaxVelocity;
        Result.y = Result.y * MaxVelocity;
    }
    
    Result.x = roundf(Result.x);
    Result.y = roundf(Result.y);
    
    return(Result);
}
#if 0
static void
LevelEntityUpdateLevelNumber(playground *LevelEntity, game_memory *Memory, game_offscreen_buffer *Buffer)
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
LevelEntityFinishAnimationInit(playground *LevelEntity, game_memory *Memory, game_offscreen_buffer *Buffer)
{
    level_animation *AnimationData = &LevelEntity->AnimationData;
    
    s32 OldRowAmount = AnimationData->OldRowAmount;
    s32 OldColAmount = AnimationData->OldColAmount;
    
    grid_entity *GridEntity = LevelEntity->GridEntity;
    
    // drawing the grid texture
    s32 OriginalBlockWidth, OriginalBlockHeight;
    SDL_QueryTexture(GridEntity->NormalSquareTexture, 0, 0, &OriginalBlockWidth, &OriginalBlockHeight);
    
    s32 RowAmount    = GridEntity->RowAmount;
    s32 ColumnAmount = GridEntity->ColumnAmount;
    
    s32 FinishTextureWidth  = ColumnAmount * OriginalBlockWidth;
    s32 FinishTextureHeight = RowAmount * OriginalBlockHeight;
    
    // deleting animation data
    
    {
        if(AnimationData->FinishTexture)
        {
            printf("deleting animation data\n");
            s32 CommonAmount = OldRowAmount * OldColAmount;
            for(s32 i = 0; i < CommonAmount; ++i)
            {
                if(AnimationData->TileTexture[i].Texture)
                {
                    SDL_SetRenderTarget(Buffer->Renderer, AnimationData->TileTexture[i].Texture);
                    
                    // TODO(Sierra): This feels wrong and weird. Color can be also any texture
                    
                    SDL_SetRenderDrawColor(Buffer->Renderer, 42, 6, 21, 255);
                    SDL_RenderClear(Buffer->Renderer);
                    
                    FreeTexture(AnimationData->TileTexture[i].Texture);
                }
            }
            
            free(AnimationData->TileTexture);
            free(AnimationData->TileAngle);
            free(AnimationData->TileQuad);
            free(AnimationData->TileAlpha);
            free(AnimationData->TileOffset);
            
            SDL_SetRenderTarget(Buffer->Renderer, AnimationData->FinishTexture);
            SDL_RenderClear(Buffer->Renderer);
            
            FreeTexture(AnimationData->FinishTexture);
            
            SDL_SetRenderTarget(Buffer->Renderer, NULL);
            SDL_SetRenderDrawColor(Buffer->Renderer, 0, 0, 0, 255);
            
            printf("finished deletion\n");
        }
    }
    
    AnimationData->OldRowAmount = RowAmount;
    AnimationData->OldColAmount = ColumnAmount;
    
    game_texture *FinishTexture = SDL_CreateTexture(Buffer->Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, FinishTextureWidth, FinishTextureHeight);
    if(!FinishTexture)
    {
        printf("%s", SDL_GetError());
    }
    
    Assert(FinishTexture);
    
    SDL_SetTextureBlendMode(FinishTexture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(Buffer->Renderer, FinishTexture);
    
    for(s32 i = 0; i < RowAmount; ++i )
    {
        for(s32 j = 0; j < ColumnAmount; ++j)
        {
            game_rect AreaQuad; 
            AreaQuad.x = j * OriginalBlockWidth;
            AreaQuad.y = i * OriginalBlockHeight;
            AreaQuad.w = OriginalBlockWidth;
            AreaQuad.h = OriginalBlockHeight;
            
            s32 UnitIndex = (i * ColumnAmount) + j;
            
            s32 UnitField = GridEntity->UnitField[UnitIndex];
            if(UnitField == 0)
            {
                SDL_RenderCopy(Buffer->Renderer, GridEntity->NormalSquareTexture, NULL, &AreaQuad);
            }
        }
    }
    
    // drawing moving blocks
    for(s32 i = 0; i < GridEntity->MovingBlocksAmount; ++i)
    {
        s32 RowNumber = GridEntity->MovingBlocks[i].RowNumber;
        s32 ColNumber = GridEntity->MovingBlocks[i].ColNumber;
        
        game_rect AreaQuad = {};
        AreaQuad.w = OriginalBlockWidth;
        AreaQuad.h = OriginalBlockHeight;
        AreaQuad.x = ColNumber * OriginalBlockWidth;
        AreaQuad.y = RowNumber * OriginalBlockHeight;
        
        b32 IsVertical = GridEntity->MovingBlocks[i].IsVertical;
        if(IsVertical)
        {
            SDL_RenderCopy(Buffer->Renderer, GridEntity->VerticalSquareTexture, NULL, &AreaQuad);
        }
        else
        {
            SDL_RenderCopy(Buffer->Renderer, GridEntity->HorizontlaSquareTexture, NULL, &AreaQuad);
        }
        
    }
    
    // drawing figure onto the texture
    
    figure_entity *FigureEntity = LevelEntity->FigureEntity;
    figure_unit *FigureUnit = FigureEntity->FigureUnit;
    
    s32 GridBlockSize = LevelEntity->Configuration.GridBlockSize;
    r32 ActualGridWidth  = ColumnAmount * GridBlockSize;
    r32 ActualGridHeight = RowAmount * GridBlockSize;
    
    game_rect GridArea = {};
    GridArea.w = ActualGridWidth;
    GridArea.h = ActualGridHeight;
    GridArea.x = GridEntity->GridArea.x + (GridEntity->GridArea.w / 2) - (ActualGridWidth / 2);
    GridArea.y = GridEntity->GridArea.y + (GridEntity->GridArea.h / 2) - (ActualGridHeight / 2);
    
    s32 FigureAmount = FigureEntity->FigureAmount;
    for(s32 Index = 0; Index < FigureAmount; ++Index)
    {
        game_rect FigureArea = FigureUnitGetArea(&FigureUnit[Index]);
        
        s32 FigureRowAmount    = FigureUnit[Index].AreaQuad.h / GridBlockSize;
        s32 FigureColumnAmount = FigureUnit[Index].AreaQuad.w / GridBlockSize;
        
        s32 RowNumber = (FigureArea.y - GridArea.y) / (GridArea.h / RowAmount);
        s32 ColumnNumber = (FigureArea.x - GridArea.x) / (GridArea.w / ColumnAmount);
        
        game_rect AreaQuad = {};
        AreaQuad.w = FigureColumnAmount*OriginalBlockWidth;
        AreaQuad.h = FigureRowAmount*OriginalBlockHeight;
        AreaQuad.x = ColumnNumber * OriginalBlockWidth;
        AreaQuad.y = RowNumber * OriginalBlockHeight;
        
        r32 OffsetX = (r32)(FigureUnit[Index].AreaQuad.x - GridArea.x) / (r32)GridBlockSize;
        r32 OffsetY = (r32)(FigureUnit[Index].AreaQuad.y - GridArea.y) / (r32)GridBlockSize;
        OffsetX = OffsetX * (r32)OriginalBlockWidth;
        OffsetY = OffsetY * (r32)OriginalBlockHeight;
        
        AreaQuad.x = roundf(OffsetX);
        AreaQuad.y = roundf(OffsetY);
        
        r32 BlockRatio = (r32)OriginalBlockWidth / (r32)GridBlockSize;
        FigureUnitResizeBy(&FigureUnit[Index], BlockRatio);
        
        game_point Center;
        Center.x = FigureUnit[Index].Center.x - FigureUnit[Index].AreaQuad.x;
        Center.y = FigureUnit[Index].Center.y - FigureUnit[Index].AreaQuad.y;
        
        //SDL_RenderCopyEx(Buffer->Renderer, FigureUnit[Index].Texture, 0, &AreaQuad, FigureUnit[Index].Angle, &Center, FigureUnit[Index].Flip);
        
        BlockRatio = (r32)GridBlockSize / (r32)OriginalBlockWidth;
        FigureUnitResizeBy(&FigureUnit[Index], BlockRatio);
    }
    
    AnimationData->FinishQuad.w = FinishTextureWidth;
    AnimationData->FinishQuad.h = FinishTextureHeight;
    
    AnimationData->MaxTileDelaySec = 0.05f;
    
    s32 CommonAmount = RowAmount * ColumnAmount;
    
    AnimationData->TileTexture = (p_texture *) calloc(CommonAmount, sizeof(p_texture));
    Assert(AnimationData->TileTexture);
    
    AnimationData->TileAngle = (r32 *) calloc(CommonAmount, sizeof(r32));
    Assert(AnimationData->TileAngle);
    
    AnimationData->TileQuad = (game_rect *)calloc(CommonAmount, sizeof(game_rect));
    Assert(AnimationData->TileQuad);
    
    AnimationData->TileAlpha = (s32 *)calloc(CommonAmount, sizeof(s32));
    Assert(AnimationData->TileAlpha);
    
    AnimationData->TileOffset = (r32 *)calloc(CommonAmount, sizeof(r32));
    Assert(AnimationData->TileOffset);
    
    
    for(s32 i = 0; i < RowAmount; ++i)
    {
        for(s32 j = 0; j < ColumnAmount; ++j)
        {
            s32 Index = (i * ColumnAmount) + j;
            
            game_texture *TileTexture;
            
            TileTexture = SDL_CreateTexture(Buffer->Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, OriginalBlockWidth, OriginalBlockHeight);
            Assert(TileTexture);
            
            SDL_SetTextureBlendMode(TileTexture, SDL_BLENDMODE_BLEND);
            
            game_rect RectQuad;
            RectQuad.w = OriginalBlockWidth;
            RectQuad.h = OriginalBlockHeight;
            RectQuad.x = j * OriginalBlockWidth;
            RectQuad.y = i * OriginalBlockHeight;
            
            SDL_SetRenderTarget(Buffer->Renderer, TileTexture);
            SDL_RenderCopy(Buffer->Renderer, FinishTexture, &RectQuad, NULL);
            
            AnimationData->TileTexture[Index].Texture = TileTexture;
            
            AnimationData->TileQuad[Index].w = GridBlockSize;
            AnimationData->TileQuad[Index].h = GridBlockSize;
            AnimationData->TileQuad[Index].x = GridArea.x + (j * GridBlockSize);
            AnimationData->TileQuad[Index].y = GridArea.y + (i * GridBlockSize);
            
            AnimationData->TileAlpha[Index]  = 255;
            AnimationData->TileOffset[Index] = 0;
        }
    }
    
    AnimationData->FinishTexture = FinishTexture;
    
    SDL_SetRenderTarget(Buffer->Renderer, NULL);
    
    AnimationData->AlphaChannel = 0;
}


static void
LevelEntityFinishAnimation(playground *LevelEntity, game_memory *Memory, game_offscreen_buffer *Buffer, r32 TimeElapsedMs)
{
    level_animation *AnimationData = &LevelEntity->AnimationData;
    
    s32 RowAmount    = LevelEntity->GridEntity->RowAmount;
    s32 ColumnAmount = LevelEntity->GridEntity->ColumnAmount;
    
    s32 GridBlockSize    = LevelEntity->Configuration.GridBlockSize;
    r32 ActualGridWidth  = ColumnAmount * GridBlockSize;
    r32 ActualGridHeight = RowAmount * GridBlockSize;
    
    game_rect GridArea = {};
    GridArea.w = ActualGridWidth;
    GridArea.h = ActualGridHeight;
    GridArea.x = LevelEntity->GridEntity->GridArea.x + (LevelEntity->GridEntity->GridArea.w / 2) - (ActualGridWidth / 2);
    GridArea.y = LevelEntity->GridEntity->GridArea.y + (LevelEntity->GridEntity->GridArea.h / 2) - (ActualGridHeight / 2);
    
    game_rect ScreenArea = {};
    ScreenArea.w = Buffer->Width;
    ScreenArea.h = Buffer->Height;
    
    b32 AnimationFinished = true;
    
    r32 MaxTileDelaySec = AnimationData->MaxTileDelaySec;
    s32 TileAnglePerSec = AnimationData->TileAnglePerSec;
    s32 TilePixelPerSec = AnimationData->TilePixelPerSec;
    s32 TileAlphaPerSec = AnimationData->TileAlphaPerSec;
    
    for(s32 Line = 1; Line <= ((RowAmount + ColumnAmount) - 1); ++Line)
    {
        b32 ShouldBreak = true;
        
        s32 StartColumn = Max2(0, Line - RowAmount);
        s32 Count = Min3(Line, (ColumnAmount - StartColumn), RowAmount);
        
        for(s32 i = 0; i < Count; ++i)
        {
            s32 RowIndex = Min2(RowAmount, Line) - i - 1;
            s32 ColIndex = StartColumn + i;
            
            s32 Index = (RowIndex * ColumnAmount) + ColIndex;
            
            if(AnimationData->TileOffset[Index] < MaxTileDelaySec)
            {
                AnimationData->TileOffset[Index] += TimeElapsedMs;
            }
            else
            {
                if(ShouldBreak)
                {
                    ShouldBreak = false;
                }
            }
            
            if(AnimationData->TileAlpha[Index] > 0)
            {
                AnimationData->TileAlpha[Index] -= roundf(TileAlphaPerSec * TimeElapsedMs);
                
                u8 Alpha = 0;
                
                if(AnimationData->TileAlpha[Index] > 0)
                {
                    Alpha = AnimationData->TileAlpha[Index];
                }
                
                SDL_SetTextureAlphaMod(AnimationData->TileTexture[Index].Texture, Alpha);
            }
            
            AnimationData->TileAngle[Index] += TileAnglePerSec * TimeElapsedMs;
            
            AnimationData->TileQuad[Index].y += roundf(TilePixelPerSec * TimeElapsedMs);
            AnimationData->TileQuad[Index].x += roundf(TilePixelPerSec * TimeElapsedMs);
            AnimationData->TileQuad[Index].w -= roundf(TilePixelPerSec * TimeElapsedMs);
            AnimationData->TileQuad[Index].h -= roundf(TilePixelPerSec * TimeElapsedMs);
            
            if(AnimationData->TileQuad[Index].w < 0)
            {
                AnimationData->TileQuad[Index].w = 0;
                AnimationData->TileQuad[Index].h = 0;
            }
            
        }
        
        if(ShouldBreak)
        {
            AnimationFinished = false;
            break;
        }
    }
    
    if(AnimationFinished)
    {
        b32 ShouldStop = true;
        
        for(s32 i = 0; i < RowAmount; ++i)
        {
            for(s32 j = 0; j < ColumnAmount; ++j)
            {
                s32 Index = (i * ColumnAmount) + j;
                
                if(AnimationData->TileAlpha[Index] > 0)
                {
                    ShouldStop = false;
                    break;
                }
            }
        }
        
        if(AnimationData->AlphaChannel != 255)
        {
            ShouldStop = false;
        }
        
        AnimationData->AlphaChannel = AnimationData->AlphaChannel != 255
            ? (AnimationData->AlphaChannel + (255.0f * TimeElapsedMs)) : 255;
        
        if(ShouldStop)
        {
            LevelEntity->LevelFinished = false;
            
            return;
        }
    }
    
    for(s32 i = 0; i < RowAmount; ++i)
    {
        for(s32 j = 0; j < ColumnAmount; ++j)
        {
            s32 Index = (i * ColumnAmount) + j;
            
            SDL_RenderCopyEx(Buffer->Renderer, AnimationData->TileTexture[Index].Texture, NULL, &AnimationData->TileQuad[Index], AnimationData->TileAngle[Index], 0, SDL_FLIP_NONE);
        }
    }
    
    
    u8 Black = AnimationData->AlphaChannel <= 255 ? AnimationData->AlphaChannel : 255;
    DEBUGRenderQuadFill(Buffer, &ScreenArea, { 0, 0, 0 }, Black);
    
    AnimationData->AlphaChannel = Black;
}



static void
LevelEntityStartAnimationInit(playground *LevelEntity, game_offscreen_buffer *Buffer)
{
    level_animation *AnimationData = &LevelEntity->AnimationData;
    
    grid_entity *&GridEntity = LevelEntity->GridEntity;
    
    s32 RowAmount = GridEntity->RowAmount;
    s32 ColAmount = GridEntity->ColumnAmount;
    s32 GridBlockSize = LevelEntity->Configuration.GridBlockSize;
    
    s32 ActualGridWidth = ColAmount * GridBlockSize;
    s32 ActualGridHeight = RowAmount * GridBlockSize;
    
    s32 GridAreaX = GridEntity->GridArea.x + (GridEntity->GridArea.w / 2) - (ActualGridWidth / 2);
    s32 GridAreaY = GridEntity->GridArea.y + (GridEntity->GridArea.h / 2) - (ActualGridHeight / 2);
    
    s32 GridCenterX = GridAreaX + (ActualGridWidth / 2);
    s32 GridCenterY = GridAreaY + (ActualGridHeight / 2);
    
    s32 GeneralAmount = ColAmount * RowAmount;
    AnimationData->TilePos = (v3 *)calloc(GeneralAmount, sizeof(v3));
    Assert(AnimationData->TilePos);
    
    r32 TimeToCompleteSec = 0.5f;
    
    s32 BlockCenterX = GridCenterX - (GridBlockSize / 2);
    s32 BlockCenterY = GridCenterY - (GridBlockSize / 2);
    
    for(s32 i = 0; i < RowAmount; ++i)
    {
        for(s32 j = 0; j < ColAmount; ++j)
        {
            s32 Index = (i * ColAmount) + j;
            
            game_point TargetPosition = {};
            TargetPosition.x = GridAreaX + (j * GridBlockSize);
            TargetPosition.y = GridAreaY + (i * GridBlockSize);
            
            AnimationData->TilePos[Index].x = BlockCenterX;
            AnimationData->TilePos[Index].y = BlockCenterY;
            
            v2 TargetVector = {};
            TargetVector.x = TargetPosition.x - BlockCenterX;
            TargetVector.y = TargetPosition.y - BlockCenterY;
            
            r32 Distance =
                sqrt((TargetVector.x * TargetVector.x) + (TargetVector.y * TargetVector.y));
            
            // this is speed
            AnimationData->TilePos[Index].z = Distance / TimeToCompleteSec;
        }
    }
    
    AnimationData->TimeElapsed = 0.0f;
    AnimationData->TileAlphaChannel = 0;
    AnimationData->TileAlphaPerSec = roundf(255.0f / TimeToCompleteSec);
    AnimationData->TileRect.w = 0;
    AnimationData->TileRect.h = 0;
}

static void
LevelEntityUpdateStartAnimation(playground *LevelEntity, game_memory *Memory, game_offscreen_buffer *Buffer, r32 TimeElapsed)
{
    if(LevelEntity->LevelStarted)
    {
        return;
    }
    
    level_animation *AnimationData = &LevelEntity->AnimationData;
    
    grid_entity   *&GridEntity   = LevelEntity->GridEntity;
    figure_entity *&FigureEntity = LevelEntity->FigureEntity;
    
    s32 RowAmount = GridEntity->RowAmount;
    s32 ColAmount = GridEntity->ColumnAmount;
    s32 FigureAmount  = FigureEntity->FigureAmount;
    s32 GridBlockSize = LevelEntity->Configuration.GridBlockSize;
    
    s32 ActualGridWidth = ColAmount * GridBlockSize;
    s32 ActualGridHeight = RowAmount * GridBlockSize;
    
    s32 GridAreaX = GridEntity->GridArea.x + (GridEntity->GridArea.w / 2) - (ActualGridWidth / 2);
    s32 GridAreaY = GridEntity->GridArea.y + (GridEntity->GridArea.h / 2) - (ActualGridHeight / 2);
    
    s32 GridCenterX = GridAreaX + ActualGridWidth / 2;
    s32 GridCenterY = GridAreaY + ActualGridHeight / 2;
    
    b32 IsGridReady   = true;
    b32 IsFigureReady = true;
    b32 IsLevelReady  = true;
    
    s32 SpaceOverFrame = GridBlockSize / 4;
    s32 FrameSize = GridBlockSize / 2;
    
    if(AnimationData->TileRect.w < FrameSize)
    {
        IsGridReady = false;
        
        AnimationData->TileRect.w += 1;
        AnimationData->TileRect.h += 1;
        
        if(AnimationData->TileRect.w >= FrameSize)
        {
            AnimationData->TileRect.w = FrameSize;
            AnimationData->TileRect.h = FrameSize;
        }
        
        game_rect AreaQuad = {};
        AreaQuad.w = AnimationData->TileRect.w;
        AreaQuad.h = AnimationData->TileRect.h;
        
        // Top left corner
        AreaQuad.x = GridCenterX - (GridBlockSize / 2) - SpaceOverFrame;
        AreaQuad.y = GridCenterY - (GridBlockSize / 2) - SpaceOverFrame;
        GameRenderBitmapToBuffer(Buffer, GridEntity->TopLeftCornerFrame, &AreaQuad);
        
        // Top right corner
        AreaQuad.x = GridCenterX + (GridBlockSize / 2) + SpaceOverFrame - AnimationData->TileRect.w;
        AreaQuad.y = GridCenterY - (GridBlockSize / 2) - SpaceOverFrame;
        GameRenderBitmapToBuffer(Buffer, GridEntity->TopRightCornerFrame, &AreaQuad);
        
        // Down left corner
        AreaQuad.x = GridCenterX - (GridBlockSize / 2) - SpaceOverFrame;
        AreaQuad.y = GridCenterY + (GridBlockSize / 2) + SpaceOverFrame - AnimationData->TileRect.h;
        GameRenderBitmapToBuffer(Buffer, GridEntity->DownLeftCornerFrame, &AreaQuad);
        
        // Down right corner
        AreaQuad.x = GridCenterX + (GridBlockSize / 2) + SpaceOverFrame - AnimationData->TileRect.w;
        AreaQuad.y = GridCenterY + (GridBlockSize / 2) + SpaceOverFrame - AnimationData->TileRect.h;
        GameRenderBitmapToBuffer(Buffer, GridEntity->DownRightCornerFrame, &AreaQuad);
    }
    else
    {
        
    }
    
    if(!IsGridReady) return;
    
    if(AnimationData->TileAlphaChannel != 255)
    {
        AnimationData->TileAlphaChannel += roundf(AnimationData->TileAlphaPerSec * TimeElapsed);
        if(AnimationData->TileAlphaChannel > 255)
        {
            AnimationData->TileAlphaChannel = 255;
        }
        else
        {
            IsGridReady = false;
        }
        
        u8 AlphaChannel = AnimationData->TileAlphaChannel;
        
        s32 LastIndex = (RowAmount * ColAmount) - 1;
        
        game_rect AreaQuad = {};
        AreaQuad.w = GridBlockSize;
        AreaQuad.h = GridBlockSize;
        AreaQuad.x = roundf(AnimationData->TilePos[LastIndex].x); 
        AreaQuad.y = roundf(AnimationData->TilePos[LastIndex].y); 
        
        s32 UnitIndex = GridEntity->UnitField[LastIndex];
        
        if(UnitIndex == 0)
        {
            SDL_SetTextureAlphaMod(GridEntity->NormalSquareTexture, AlphaChannel);
            GameRenderBitmapToBuffer(Buffer, GridEntity->NormalSquareTexture, &AreaQuad);
        }
        else if(UnitIndex == 1)
        {
            SDL_SetTextureAlphaMod(GridEntity->VerticalSquareTexture, AlphaChannel);
            GameRenderBitmapToBuffer(Buffer, GridEntity->VerticalSquareTexture, &AreaQuad);
        }
        else if(UnitIndex == 2)
        {
            SDL_SetTextureAlphaMod(GridEntity->HorizontlaSquareTexture, AlphaChannel);
            GameRenderBitmapToBuffer(Buffer, GridEntity->HorizontlaSquareTexture, &AreaQuad);
        }
    }
    
    s32 MajorAmount = RowAmount > ColAmount ? RowAmount : ColAmount;
    s32 Iterations = roundf((r32)MajorAmount / 2);
    
    s32 RowIndex1 = roundf((r32)RowAmount / 2.0f) - 1;
    s32 RowIndex2 = RowAmount / 2;
    
    s32 ColIndex1 = roundf((r32)ColAmount / 2.0f) - 1;
    s32 ColIndex2 = ColAmount / 2;
    s32 OldColIndex1 = -1;
    s32 OldColIndex2 = -1;
    
    AnimationData->TimeElapsed += TimeElapsed;
    
    for(s32 Iter = 0; Iter < Iterations; ++Iter)
    {
        for(s32 i = RowIndex1; i <= RowIndex2; )
        {
            for(s32 j = ColIndex1; j <= ColIndex2; )
            {
                // drawing
                s32 Index = (i * ColAmount) + j;
                
                v2 TargetPosition = {};
                TargetPosition.x = GridAreaX + (j * GridBlockSize);
                TargetPosition.y = GridAreaY + (i * GridBlockSize);
                
                v2 CurrentPosition = {};
                CurrentPosition.x = AnimationData->TilePos[Index].x;
                CurrentPosition.y = AnimationData->TilePos[Index].y;
                
                v2 Velocity = {};
                Velocity.x = TargetPosition.x - CurrentPosition.x;
                Velocity.y = TargetPosition.y - CurrentPosition.y;
                
                r32 MaxVelocity = AnimationData->TilePos[Index].z;
                r32 MaxDistanceDelta = MaxVelocity * TimeElapsed;
                r32 Magnitude = sqrt(Velocity.x * Velocity.x + Velocity.y * Velocity.y);
                
                if(Magnitude <= MaxDistanceDelta || Magnitude == 0.0f)
                {
                    CurrentPosition.x = TargetPosition.x;
                    CurrentPosition.y = TargetPosition.y;
                }
                else
                {
                    v2 NormVector = {};
                    NormVector.x = Velocity.x / Magnitude;
                    NormVector.y = Velocity.y / Magnitude;
                    
                    NormVector.x *= MaxDistanceDelta;
                    NormVector.y *= MaxDistanceDelta;
                    
                    CurrentPosition.x += NormVector.x;
                    CurrentPosition.y += NormVector.y;
                }
                
                AnimationData->TilePos[Index].x = CurrentPosition.x;
                AnimationData->TilePos[Index].y = CurrentPosition.y;
                
                if(CurrentPosition.x != TargetPosition.x || CurrentPosition.y != TargetPosition.y)
                {
                    IsGridReady = false;
                }
                
                game_rect AreaQuad = {};
                AreaQuad.w = GridBlockSize;
                AreaQuad.h = GridBlockSize;
                AreaQuad.x = roundf(AnimationData->TilePos[Index].x); 
                AreaQuad.y = roundf(AnimationData->TilePos[Index].y); 
                
                s32 UnitIndex = GridEntity->UnitField[Index];
                
                if(UnitIndex == 0)
                {
                    GameRenderBitmapToBuffer(Buffer, GridEntity->NormalSquareTexture, &AreaQuad);
                }
                else if(UnitIndex == 1)
                {
                    GameRenderBitmapToBuffer(Buffer, GridEntity->VerticalSquareTexture, &AreaQuad);
                }
                else if(UnitIndex == 2)
                {
                    GameRenderBitmapToBuffer(Buffer, GridEntity->HorizontlaSquareTexture, &AreaQuad);
                }
                
                if(RowIndex1 == RowIndex2 && j != ColIndex2)
                {
                    j = ColIndex2;
                }
                else if(i == RowIndex1 || i == RowIndex2)
                {
                    j += 1;
                }
                else
                {
                    if(j != ColIndex2)
                    {
                        j = ColIndex2;
                    }
                    else
                    {
                        break;
                    }
                    
                }
            }
            
            if(OldColIndex1 == ColIndex1 && OldColIndex2 == ColIndex2 && i != RowIndex2)
            {
                i = RowIndex2;
            }
            else
            {
                i += 1;
            }
            
        }
        
        OldColIndex1 = ColIndex1;
        OldColIndex2 = ColIndex2;
        
        if(ColIndex1 > 0)
        {
            ColIndex1 -= 1;
        }
        
        if(ColIndex2 < ColAmount - 1)
        {
            ColIndex2 += 1;
        }
        
        if(RowIndex1 > 0)
        {
            RowIndex1 -= 1;
        }
        
        if(RowIndex2 < RowAmount - 1)
        {
            RowIndex2 += 1;
        }
    }
    
    s32 CurrentGridWidth = AnimationData->TilePos[ColAmount - 1].x - AnimationData->TilePos[0].x;
    
    s32 CurrentGridHeight = AnimationData->TilePos[((RowAmount - 1) * ColAmount) + ColAmount-1].y - AnimationData->TilePos[0].y;
    
    game_rect AreaQuad = {};
    AreaQuad.w = AnimationData->TileRect.w;
    AreaQuad.h = AnimationData->TileRect.h;
    
    // Top left corner
    AreaQuad.x = GridCenterX - (CurrentGridWidth / 2) - SpaceOverFrame - FrameSize;
    AreaQuad.y = GridCenterY - (CurrentGridHeight / 2) - SpaceOverFrame - FrameSize;
    GameRenderBitmapToBuffer(Buffer, GridEntity->TopLeftCornerFrame, &AreaQuad);
    
    // Top right corner
    AreaQuad.x = GridCenterX + (CurrentGridWidth / 2) + SpaceOverFrame;
    AreaQuad.y = GridCenterY - (CurrentGridHeight / 2) - SpaceOverFrame - FrameSize;
    GameRenderBitmapToBuffer(Buffer, GridEntity->TopRightCornerFrame, &AreaQuad);
    
    // Down left corner
    AreaQuad.x = GridCenterX - (CurrentGridWidth / 2) - SpaceOverFrame - FrameSize;
    AreaQuad.y = GridCenterY + (CurrentGridHeight / 2) + SpaceOverFrame;
    GameRenderBitmapToBuffer(Buffer, GridEntity->DownLeftCornerFrame, &AreaQuad);
    
    // Down right corner
    AreaQuad.x = GridCenterX + (CurrentGridWidth / 2) + SpaceOverFrame;
    AreaQuad.y = GridCenterY + (CurrentGridHeight / 2) + SpaceOverFrame;
    GameRenderBitmapToBuffer(Buffer, GridEntity->DownRightCornerFrame, &AreaQuad);
    
    b32 Result = IsGridReady && IsFigureReady && IsLevelReady;
    
    if(Result)
    {
        LevelEntity->LevelStarted = true;
        printf("TimeElapsed = %f\n", AnimationData->TimeElapsed);
        
        free(LevelEntity->AnimationData.TilePos);
        LevelEntity->AnimationData.TilePos = NULL;
        
        free(LevelEntity->AnimationData.TileAlpha);
        LevelEntity->AnimationData.TileAlpha = NULL;
        
        //LevelEntityStartAnimationInit(LevelEntity, Buffer);
    }
}

#endif 

static void
RenderFigureStructure(render_group *RenderGroup, figure_unit *Entity)
{
    v4 Color = {0, 255, 255, 255};
    
    game_rect Rectangle = {};
    Rectangle.x = (s32)Entity->Position.x;
    Rectangle.y = (s32)Entity->Position.y;
    Rectangle.w = (s32)Entity->Size.w;
    Rectangle.h = (s32)Entity->Size.h;
    
    
    PushRectOutline(RenderGroup, Rectangle, Color);
    
    for (u32 Index = 0;
         Index < FIGURE_BLOCKS_MAXIMUM;
         ++Index)
    {
        game_rect Rectangle = {};
        Rectangle.w = 20;
        Rectangle.h = 20;
        Rectangle.x = Entity->Shell[Index].x - (Rectangle.w / 2);
        Rectangle.y = Entity->Shell[Index].y - (Rectangle.h / 2);
        
        PushRect(RenderGroup, Rectangle, Color);
        
        v2 Center = {};
        Center.x   = Entity->Position.x + (Entity->Size.w / 2);
        Center.y   = Entity->Position.y + (Entity->Size.h) * Entity->CenterOffset;
        
        game_rect CenterRectangle = {};
        CenterRectangle.w = 20;
        CenterRectangle.h = 20;
        CenterRectangle.x = Center.x - (CenterRectangle.w / 2);
        CenterRectangle.y = Center.y - (CenterRectangle.h / 2);
        
        PushRect(RenderGroup, CenterRectangle, {255, 0, 0, 255});
    }
}

static playground_status
LevelEntityUpdateAndRender(playground *LevelEntity, render_group *RenderGroup, game_input *Input)
{
    playground_status Result = playground_status::LEVEL_RUNNING;
    
    grid_entity   *GridEntity   = &LevelEntity->GridEntity;
    figure_entity *FigureEntity = &LevelEntity->FigureEntity;
    figure_unit   *FigureUnit   = FigureEntity->FigureUnit;
    
    s32 GridBlockSize     = LevelEntity->Configuration.GridBlockSize;
    u32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
    
    game_rect AreaQuad = {};
    
    r32 TimeElapsed  = Input->TimeElapsedMs;
    r32 MaxVel       = GridBlockSize / 6;
    s32 RowAmount    = GridEntity->RowAmount;
    s32 ColumnAmount = GridEntity->ColumnAmount;
    u32 FigureAmount = FigureEntity->FigureAmount;
    s32 ActiveIndex  = FigureEntity->FigureActive;
    
    s32 ActualGridWidth  = ColumnAmount * GridBlockSize;
    s32 ActualGridHeight = RowAmount * GridBlockSize;
    
    game_rect GridArea = {};
    GridArea.w = ActualGridWidth;
    GridArea.h = ActualGridHeight;
    GridArea.x = GridEntity->GridArea.x + (GridEntity->GridArea.w / 2) - (ActualGridWidth / 2);
    GridArea.y = GridEntity->GridArea.y + (GridEntity->GridArea.h / 2) - (ActualGridHeight / 2);
    
    bool ToggleHighlight = false;
    
    s32 StartX = 0;
    s32 StartY = 0;
    
    /* game_input checking */
    GameUpdateEvent(Input, LevelEntity, RenderGroup->Width, RenderGroup->Height);
    
    Clear(RenderGroup, {42, 6, 21, 255});
    
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
                game_rect Rect = {0, 0, (s32)GridBlockSize, (s32)GridBlockSize};
                
                for (u32 i = 0 ; i < RowAmount && Count != 4; ++i)
                {
                    for (u32 j = 0; j < ColumnAmount && Count != 4; ++j)
                    {
                        Rect.x = GridArea.x + (j*GridBlockSize);
                        Rect.y = GridArea.y + (i*GridBlockSize);
                        
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
                        
                        v2 FigureCenter = {};
                        FigureCenter.x   = FigureUnit->Position.x + (FigureUnit->Size.w / 2);
                        FigureCenter.y   = FigureUnit->Position.y + (FigureUnit->Size.h) * FigureUnit->CenterOffset;
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
                            printf("Level is complete \n");
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
            //v2 FigureCenter = FigureUnit[Index].Center;
            v2 FigureCenter = {};
            FigureCenter.x   = FigureUnit->Position.x + (FigureUnit->Size.w / 2);
            FigureCenter.y   = FigureUnit->Position.y + (FigureUnit->Size.h) * FigureUnit->CenterOffset;
            
            v2 TargetCenter = GridEntity->StickUnits[i].Center;
            
            v2 dt = Move2DPointPerSec(FigureCenter, TargetCenter, FigureEntity->FigureVelocity, TimeElapsed);
            
            FigureUnitMove(&FigureUnit[Index], dt.x, dt.y);
            
            if((FigureCenter.x == TargetCenter.x) && (FigureCenter.y == TargetCenter.y))
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
        // TODO(msokolov): also check in there is any figures left??
        LevelEntity->LevelFinished = true;
        Result = playground_status::LEVEL_FINISHED;
        //LevelEntityFinishAnimationInit(LevelEntity, Memory, Buffer);
    }
    
    AreaQuad.w = GridBlockSize;
    AreaQuad.h = GridBlockSize;
    
    for (u32 Row = 0; Row < RowAmount; ++Row)
    {
        StartY = GridArea.y + (GridBlockSize * Row);
        
        for (u32 Col = 0; Col < ColumnAmount; ++Col)
        {
            StartX = GridArea.x + (GridBlockSize * Col);
            
            AreaQuad.x = StartX;
            AreaQuad.y = StartY;
            
            u32 GridUnit = GridEntity->UnitField[(Row * ColumnAmount) + Col];
            if(GridUnit == 0 || GridUnit == 2 || GridUnit == 3)
            {
                //GameRenderBitmapToBuffer(Buffer, GridEntity->NormalSquareTexture, &AreaQuad);
                PushBitmap(RenderGroup, GridEntity->NormalSquareTexture, AreaQuad);
            }
        }
    }
    
    s32 SpaceOverFrame = GridBlockSize / 4;
    s32 FrameSize = GridBlockSize / 2;
    AreaQuad.w = FrameSize;
    AreaQuad.h = FrameSize;
    
    // Top left corner
    AreaQuad.x = GridArea.x - SpaceOverFrame;
    AreaQuad.y = GridArea.y - SpaceOverFrame;
    PushBitmap(RenderGroup, GridEntity->TopLeftCornerFrame, AreaQuad);
    
    // Top right corner
    AreaQuad.x = GridArea.x + ActualGridWidth - SpaceOverFrame;
    AreaQuad.y = GridArea.y - SpaceOverFrame;
    PushBitmap(RenderGroup, GridEntity->TopRightCornerFrame, AreaQuad);
    
    // Down left corner
    AreaQuad.x = GridArea.x - SpaceOverFrame;
    AreaQuad.y = GridArea.y + ActualGridHeight - SpaceOverFrame;
    PushBitmap(RenderGroup, GridEntity->DownLeftCornerFrame, AreaQuad);
    
    // Down right corner
    AreaQuad.x = GridArea.x + ActualGridWidth - SpaceOverFrame;
    AreaQuad.y = GridArea.y + ActualGridHeight - SpaceOverFrame;
    PushBitmap(RenderGroup, GridEntity->DownRightCornerFrame, AreaQuad);
    
    /* MovingBlocks Update and Rendering */
    
    for(u32 i = 0; i < GridEntity->MovingBlocksAmount; ++i)
    {
        if(GridEntity->MovingBlocks[i].IsMoving)
        {
            s32 RowNumber = GridEntity->MovingBlocks[i].RowNumber;
            s32 ColNumber = GridEntity->MovingBlocks[i].ColNumber;
            
            v2 Center = { 
                (r32)GridEntity->MovingBlocks[i].AreaQuad.x, 
                (r32)GridEntity->MovingBlocks[i].AreaQuad.y 
            };
            
            v2 TargetCenter;
            TargetCenter.x = GridArea.x + (ColNumber * GridBlockSize);
            TargetCenter.y = GridArea.y + (RowNumber * GridBlockSize);
            
            v2 Velocity = Move2DPointPerSec(Center, TargetCenter, MaxVel, TimeElapsed);
            
            GridEntity->MovingBlocks[i].AreaQuad.x += Velocity.x;
            GridEntity->MovingBlocks[i].AreaQuad.y += Velocity.y;
            
            if((Center.x == TargetCenter.x) && (Center.y == TargetCenter.y))
            {
                GridEntity->MovingBlocks[i].IsMoving = false;
                GridEntity->MovingBlocks[i].AreaQuad.x
                    = GridArea.x + (ColNumber * GridBlockSize);
                GridEntity->MovingBlocks[i].AreaQuad.y
                    = GridArea.y + (RowNumber * GridBlockSize);
                GridEntity->UnitField[(RowNumber * ColumnAmount) + ColNumber] = 1;
            }
        }
        
        if(GridEntity->MovingBlocks[i].IsVertical)
        {
            PushBitmap(RenderGroup, GridEntity->VerticalSquareTexture, GridEntity->MovingBlocks[i].AreaQuad);
        }
        else
        {
            PushBitmap(RenderGroup, GridEntity->HorizontlaSquareTexture, GridEntity->MovingBlocks[i].AreaQuad);
        }
    }
    
    /* FigureEntity Update and Rendering */
    
    /* Figure Area Highlight */
    if(FigureEntity->IsRestarting)
    {
        bool AllFiguresReturned = true;
        for(u32 i = 0; i < FigureAmount; ++i)
        {
            //v2 Center        = FigureUnit[i].Center;
            
            //v2 Center = {};
            //Center.x   = FigureUnit[i].AreaQuad.x + (FigureUnit[i].AreaQuad.w / 2);
            //Center.y   = FigureUnit[i].AreaQuad.y + (FigureUnit[i].AreaQuad.h) * FigureUnit[i].CenterOffset;
            
            //v2 TargetCenter  = FigureUnit[i].DefaultCenter;
            v2 Position = {};
            Position.x = FigureUnit[i].Position.x;
            Position.y = FigureUnit[i].Position.y;
            
            v2 HomePosition = FigureUnit[i].HomePosition;
            
            if(!FigureUnit[i].IsIdle)
            {
                AllFiguresReturned = false;
                
                v2 Velocity =  Move2DPointPerSec(Position, HomePosition, MaxVel, TimeElapsed);
                FigureUnitMove(&FigureUnit[i], Velocity.x, Velocity.y);
                
                if(((s32)Position.x == (s32)HomePosition.x) && ((s32)Position.y == (s32)HomePosition.y))
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
        ScreenArea.w = RenderGroup->Width;
        ScreenArea.h = RenderGroup->Height;
        
        v2 Center = {};
        Center.x   = FigureUnit[ActiveIndex].Position.x + (FigureUnit[ActiveIndex].Position.w / 2);
        Center.y   = FigureUnit[ActiveIndex].Position.y + (FigureUnit[ActiveIndex].Size.h) * FigureUnit[ActiveIndex].CenterOffset;
        
        ShouldHighlight = IsFigureUnitInsideRect(&FigureUnit[ActiveIndex], &FigureEntity->FigureArea) || !(IsPointInsideRect(Center.x,  Center.y, &ScreenArea));
        
    }
    
    if(ShouldHighlight) 
    {
        Change1DUnitPerSec(&FigureEntity->AreaAlpha, 255, LevelEntity->Configuration.FlippingAlphaPerSec, TimeElapsed);
    }
    else
    {
        Change1DUnitPerSec(&FigureEntity->AreaAlpha, 0, LevelEntity->Configuration.FlippingAlphaPerSec, TimeElapsed);
    }
    
    
    if(FigureEntity->AreaAlpha != 0) 
    {
        ToggleHighlight = true;
    }
    
    if(ToggleHighlight)
    {
        v4 HightlightColor = {255, 255, 255, 255};
        game_rect HightlightRect = FigureEntity->FigureArea;
        
        u32 RectThickness = InActiveBlockSize / 6;
        for(u32 i = 0; i < RectThickness; i ++)
        {
            PushRectOutline(RenderGroup, HightlightRect, HightlightColor);
            
            HightlightRect.x += 1;
            HightlightRect.y += 1;
            HightlightRect.w -= 2;
            HightlightRect.h -= 2;
        }
    }
    
    /* Figure returning to the idle zone */
    
    if(FigureEntity->IsReturning)
    {
        u32 ReturnIndex = FigureEntity->ReturnIndex;
        
        v2 Position = {};
        Position.x = FigureUnit[ReturnIndex].Position.x;
        Position.y = FigureUnit[ReturnIndex].Position.y;
        
        v2 HomePosition = FigureUnit[ReturnIndex].HomePosition;
        v2 Velocity =  Move2DPointPerSec(Position, HomePosition, MaxVel, TimeElapsed);
        
        FigureUnitMove(&FigureUnit[ReturnIndex], Velocity.x, Velocity.y);
        
        if(((s32)Position.x == (s32)HomePosition.x) && ((s32)Position.y == (s32)HomePosition.y))
        {
            FigureEntity->IsReturning = false;
            FigureEntity->ReturnIndex = -1;
        }
    }
    
    /* Rotation Animation */
    
    if(FigureEntity->IsRotating)
    {
        // TODO(max): Maybe put these values in playground ???
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
        
        //SDL_SetTextureAlphaMod(FigureUnit[ActiveIndex].Texture, FigureEntity->FigureAlpha);
    }
    
    /* Figure Rendering */
    
    for(u32 i = 0; i < FigureAmount; ++i)
    {
        u32 Index = FigureEntity->FigureOrder[i];
        
        figure_unit *Entity = &FigureUnit[Index];
        
        v2 FigureCenter = {};
        FigureCenter.x   = Entity->Position.x + (Entity->Size.w / 2);
        FigureCenter.y   = Entity->Position.y + (Entity->Size.h) * Entity->CenterOffset;
        
        v2 Center;
        Center.x = FigureCenter.x - Entity->Position.x;
        Center.y = FigureCenter.y - Entity->Position.y;
        
        game_texture *Texture = PickFigureTexture(Entity->Form, Entity->Type, FigureEntity);
        
        game_rect Rectangle = {};
        Rectangle.x = Entity->Position.x;
        Rectangle.y = Entity->Position.y;
        Rectangle.w = Entity->Size.w;
        Rectangle.h = Entity->Size.h;
        
        PushBitmapEx(RenderGroup, Texture, Rectangle, Entity->Angle, Center, Entity->Flip);
        
        RenderFigureStructure(RenderGroup, Entity);
    }
    
    return (Result);
}

