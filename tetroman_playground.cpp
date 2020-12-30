/* ========================================= */
//     $File: tetroman_playground.cpp
//     $Date: October 18th 2017 08:52 pm 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */

static rectangle2
FigureUnitGetArea(figure_unit *Figure)
{
    rectangle2 Result = {};
    Result.Min.x = FLT_MAX;
    Result.Min.y = FLT_MAX;
    Result.Max.x = FLT_MIN;
    Result.Max.y = FLT_MIN;
    
    r32 BlockSize = Figure->IsEnlarged ? GRID_BLOCK_SIZE : IDLE_BLOCK_SIZE;
    
    for(s32 Index = 0;
        Index < FIGURE_BLOCKS_MAXIMUM;
        ++Index)
    {
        rectangle2 ShellRectangle;
        ShellRectangle.Min.x = Figure->Shell[Index].x - (BlockSize / 2.0f);
        ShellRectangle.Min.y = Figure->Shell[Index].y - (BlockSize / 2.0f);
        SetDim(&ShellRectangle, BlockSize, BlockSize);
        
        if (ShellRectangle.Min.x < Result.Min.x)
        {
            Result.Min.x = ShellRectangle.Min.x;
        }
        if (ShellRectangle.Min.y < Result.Min.y)
        {
            Result.Min.y = ShellRectangle.Min.y;
        }
        if (ShellRectangle.Max.x > Result.Max.x)
        {
            Result.Max.x = ShellRectangle.Max.x;
        }
        if (ShellRectangle.Max.y > Result.Max.y)
        {
            Result.Max.y = ShellRectangle.Max.y;
        }
    }
    
    return (Result);
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
        
        FigureOrder[FigureAmount - 1] = Index;
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
FigureUnitResizeBy(figure_unit *Figure, r32 Scale)
{
    v2 Center    = Figure->Position + (Figure->Size / 2.0f);
    v2 Size      = Figure->Size * Scale;
    v2 NewCenter = Figure->Position + (Size / 2.0f);
    
    v2 FinalOffset = (Center - NewCenter);
    
    for(u32 Index = 0;
        Index < FIGURE_BLOCKS_MAXIMUM;
        ++Index)
    {
        v2 ShellOffset = (Figure->Shell[Index] - Figure->Position) * Scale;
        Figure->Shell[Index] = Figure->Position + ShellOffset;
        Figure->Shell[Index] += FinalOffset;
    }
    
    Figure->Position = Figure->Position + FinalOffset;
    Figure->Size = Size;
}

static void
FigureUnitRotateShellBy(figure_unit *Entity, float Angle)
{
    if((s32)Entity->Angle == 0)
    {
        Entity->Angle = 0.0f;
    }
    
    v2 Center = {};
    Center.x  = Entity->Position.x + (Entity->Size.w / 2.0f);
    Center.y  = Entity->Position.y + (Entity->Size.h) * Entity->CenterOffset;
    
    for (u32 i = 0; i < FIGURE_BLOCKS_MAXIMUM; ++i)
    {
        r32 Radians = Angle * (M_PI / 180.0f);
        r32 Cos = cos(Radians);
        r32 Sin = sin(Radians);
        
        v2 Position = {};
        Position.x = Center.x + (Entity->Shell[i].x - Center.x) * Cos - (Entity->Shell[i].y - Center.y) * Sin;
        Position.y = Center.y + (Entity->Shell[i].x - Center.x) * Sin + (Entity->Shell[i].y - Center.y) * Cos;
        
        Entity->Shell[i] = Position;
    }
}

static void
FigureUnitInitFigure(figure_unit *FigureUnit, figure_form Form)

{
    FigureUnit->IsIdle = true;
    FigureUnit->Form   = Form;
    FigureUnit->Flip   = SDL_FLIP_NONE;
    
    u32 RowAmount    = 0;
    u32 ColumnAmount = 0;
    r32 CenterOffset = 0.5f;
    
    vector<vector<s32>> matrix(2);
    for(u32 i = 0; i < 2; ++i)
    {
        matrix[i].resize(4);
    }
    
    switch(Form)
    {
        case figure_form::I_figure:
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
        
        case figure_form::O_figure:
        {
            matrix = 
            { 
                { 1, 1 },
                { 1, 1 } 
            };
            
            RowAmount    = 2;
            ColumnAmount = 2;
        }break;
        
        case figure_form::Z_figure:
        {
            matrix =
            {
                {1, 1, 0}, 
                {0, 1, 1} 
            };
            
            RowAmount    = 3;
            ColumnAmount = 2;
        }break;
        
        case figure_form::S_figure:
        {
            matrix =
            { 
                {0, 1, 1}, 
                {1, 1, 0} 
            };
            
            RowAmount    = 3;
            ColumnAmount = 2;
        }break;
        
        case figure_form::T_figure:
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
        
        case figure_form::L_figure:
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
        
        case figure_form::J_figure:
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
    FigureUnit->Size.w = RowAmount * IDLE_BLOCK_SIZE;
    FigureUnit->Size.h = ColumnAmount * IDLE_BLOCK_SIZE;
    FigureUnit->CenterOffset = CenterOffset;
    
    u32 ShellIndex = 0;
    u32 HalfBlock  = roundf(IDLE_BLOCK_SIZE / 2.0f);
    
    for(u32 i = 0; i < 2; i++)
    {
        for(u32 j = 0; j < 4; j++)
        {
            if(matrix[i][j] == 1)
            {
                FigureUnit->Shell[ShellIndex].x = FigureUnit->Position.x + (j * IDLE_BLOCK_SIZE) + HalfBlock;
                FigureUnit->Shell[ShellIndex].y = FigureUnit->Position.y + (i * IDLE_BLOCK_SIZE) + HalfBlock;
                
                ShellIndex++;
            }
        }
    }
    
    FigureUnitRotateShellBy(FigureUnit, FigureUnit->Angle);
}

static void
FigureUnitAddNewFigure(figure_entity *FigureEntity, figure_form Form)
{
    Assert(FigureEntity->FigureAmount < FIGURE_AMOUNT_MAXIMUM);
    
    u32 Index = FigureEntity->FigureAmount;
    FigureEntity->FigureAmount += 1;
    
    FigureUnitInitFigure(&FigureEntity->FigureUnit[Index], Form);
}

static void
FigureUnitFlipHorizontally(figure_unit *Unit)
{
    u32 NewX = 0;
    u32 NewY = 0;
    u32 NewCenterX = 0;
    u32 NewCenterY = 0;
    
    rectangle2 AreaQuad = FigureUnitGetArea(Unit);
    v2 AreaQuadDimension = GetDim(AreaQuad);
    
    if(Unit->Flip != SDL_FLIP_HORIZONTAL)
    {
        Unit->Flip = SDL_FLIP_HORIZONTAL;
    }
    else
    {
        Unit->Flip = SDL_FLIP_NONE;
    }
    
    if(AreaQuadDimension.w > AreaQuadDimension.h)
    {
        for (u32 i = 0; i < FIGURE_BLOCKS_MAXIMUM; ++i)
        {
            NewX = (AreaQuad.Min.x + AreaQuadDimension.w) - (Unit->Shell[i].x - AreaQuad.Min.x);
            Unit->Shell[i].x = NewX;
        }
    }
    else
    {
        for (u32 i = 0; i < FIGURE_BLOCKS_MAXIMUM; ++i)
        {
            NewY = (AreaQuad.Min.y + AreaQuadDimension.h) - (Unit->Shell[i].y - AreaQuad.Min.y);
            Unit->Shell[i].y = NewY;
        }
    }
}

static inline v2
FigureUnitMove(figure_unit *Entity, v2 dt)
{
    Entity->Position += dt;
    
    for (u32 Index = 0;
         Index < FIGURE_BLOCKS_MAXIMUM;
         Index++)
    {
        Entity->Shell[Index] += dt;
    }
    
    return (Entity->Position);
}

static game_texture *
PickFigureTexture(figure_entity *FigureEntity, figure_form Form)
{
    game_texture *Result = NULL;
    
    u32 FormIndex = static_cast<u32> (Form);
    
    switch(FigureEntity->CurrentType)
    {
        case figure_type::Blue: {
            Result = FigureEntity->BlueTexture[FormIndex];
        } break;
        case figure_type::Green: {
            Result = FigureEntity->GreenTexture[FormIndex];
        } break;
        case figure_type::Orange: {
            Result = FigureEntity->OrangeTexture[FormIndex];
        } break;
        case figure_type::Red: {
            Result = FigureEntity->RedTexture[FormIndex];
        } break;
    }
    
    return(Result);
}


static game_texture*
PickFigureShadowTexture(figure_form Form, figure_entity *FigureEntity)
{
    game_texture *Result = NULL;
    
    u32 FormIndex = static_cast<u32>(Form);
    Result = FigureEntity->ShadowTexture[FormIndex];
    
    return(Result);
}

static game_texture*
PickFigureOutlineTexture(figure_form Form, figure_entity *FigureEntity)
{
    game_texture *Result = NULL;
    u32 FormIndex = static_cast<u32>(Form);
    Result = FigureEntity->OutlineTexture[FormIndex];
    
    return(Result);
}

static game_texture*
PickFigureGroundTexture(figure_form Form, figure_entity *FigureEntity)
{
    game_texture *Result = NULL;
    u32 FormIndex = static_cast<u32>(Form);
    Result = FigureEntity->GroundTexture[FormIndex];
    
    return(Result);
}


static void
FigureUnitSetToDefaultArea(figure_unit* Unit, r32 BlockRatio)
{
    if(Unit->IsEnlarged)
    {
        r32 Angle     = Unit->Angle;
        r32 HomeAngle = Unit->HomeAngle;
        r32 AngleDifference = 0;
        
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
FigureUnitDefineDefaultArea(figure_unit *Unit, v2 Position)
{
    rectangle2 AreaQuad = FigureUnitGetArea(Unit);
    
    v2 Offset = {};
    Offset.x = Position.x - AreaQuad.Min.x;
    Offset.y = Position.y - AreaQuad.Min.y;
    
    FigureUnitMove(Unit, Offset);
    
    Unit->HomePosition = Unit->Position;
    Unit->HomeAngle    = Unit->Angle;
}

static void
FigureUnitMoveToDefaultArea(figure_unit *FigureUnit, u32 ActiveBlockSize)
{
    v2 Offset = FigureUnit->HomePosition - FigureUnit->Position;
    
    r32 Magnitude = SquareRoot(Offset);
    r32 MaxSpeed  = abs(Magnitude - ActiveBlockSize);
    
    if(Magnitude > MaxSpeed)
    {
        Offset = Normalize(Offset);
        Offset *= MaxSpeed;
        
        FigureUnitMove(FigureUnit, Offset);
    }
}

static void
GridEntityMoveBlockHorizontally(grid_entity *GridEntity, moving_block *MovingBlock)
{
    s32 NewColNumber = 0;
    s32 RowAmount = GridEntity->RowAmount;
    s32 ColAmount = GridEntity->ColumnAmount;
    u32 RowNumber = MovingBlock->RowNumber;
    u32 ColNumber = MovingBlock->ColNumber;
    u32 ActiveBlockSize = MovingBlock->Area.Max.x - MovingBlock->Area.Min.x;
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
    u32 ActiveBlockSize = MovingBlock->Area.Max.x - MovingBlock->Area.Min.x;
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
    r32 GridBlockSize     = GRID_BLOCK_SIZE;
    r32 InActiveBlockSize = IDLE_BLOCK_SIZE;
    
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

static options_choice
PlaygroundUpdateEvents(game_input *Input, playground *LevelEntity, u32 ScreenWidth, u32 ScreenHeight)
{
    options_choice Result = options_choice::NONE_OPTION;
    
    b32 ToggleMenuOption  = LevelEntity->Options.ToggleMenu;
    grid_entity   *GridEntity   = &LevelEntity->GridEntity;
    figure_entity *FigureEntity = &LevelEntity->FigureEntity;
    figure_unit   *FigureUnit   = FigureEntity->FigureUnit;
    
    u32 Size   = FigureEntity->FigureAmount;
    
    v2 MousePos = {};
    MousePos.x = Input->MouseX;
    MousePos.y = Input->MouseY;
    
    r32 BlockRatio  = 0;
    u32 ActiveIndex = FigureEntity->FigureActive;
    
    r32 GridBlockSize = GRID_BLOCK_SIZE;
    r32 InActiveBlockSize = IDLE_BLOCK_SIZE;
    
    if(Input->Keyboard.Escape.EndedDown)
    {
        Result = options_choice::MAINMENU_OPTION;
        
        if (FigureEntity->IsGrabbed) {
            FigureEntity->IsGrabbed = false;
        }
        
        Mix_FadeOutMusic(128);
    }
    
    if (!FigureEntity->IsGrabbed && !LevelEntity->LevelFinished) {
        
        b32 IsInShell = false;
        for (s32 Index = Size - 1; Index >= 0; --Index) {
            s32 ActiveIndex = FigureEntity->FigureOrder[Index];
            
            r32 CurrentBlockSize = FigureUnit[ActiveIndex].IsEnlarged ? GridBlockSize : InActiveBlockSize;
            r32 HalfShellSize = CurrentBlockSize * 0.5f;
            
            
            for (int BlockIndex = 0;
                 BlockIndex < FIGURE_BLOCKS_MAXIMUM;
                 ++BlockIndex)
            {
                rectangle2 BlockRectangle = {};
                BlockRectangle.Min.x = FigureUnit[ActiveIndex].Shell[BlockIndex].x - HalfShellSize;
                BlockRectangle.Min.y = FigureUnit[ActiveIndex].Shell[BlockIndex].y - HalfShellSize;
                BlockRectangle.Max.x = FigureUnit[ActiveIndex].Shell[BlockIndex].x + HalfShellSize;
                BlockRectangle.Max.y = FigureUnit[ActiveIndex].Shell[BlockIndex].y + HalfShellSize;
                
                if (IsInRectangle(MousePos, BlockRectangle))
                {
                    IsInShell = true;
                    break;
                }
            }
            
            if (IsInShell) {
                FigureEntity->FigureOutline = ActiveIndex;
                break;
            }
        }
        if (!IsInShell) {
            FigureEntity->FigureOutline = -1;
        }
    }
    
    if(!LevelEntity->LevelPaused)
    {
        if(Input->MouseButtons[0].EndedDown)
        {
            rectangle2 GearRectangle = {};
            GearRectangle.Min.x = VIRTUAL_GAME_WIDTH  - 200.0f;
            GearRectangle.Min.y = VIRTUAL_GAME_HEIGHT - 200.0f;
            SetDim(&GearRectangle, 100.0f, 100.0f);
            
            if (IsInRectangle(MousePos, GearRectangle))
            {
                ToggleMenuOption = LevelEntity->Options.ToggleMenu ? false : true;
                LevelEntity->Options.Choice = options_choice::RESTART_OPTION;
                LevelEntity->GearIsRotating = true;
                
                Mix_PlayChannel(0, LevelEntity->Options.GearSound, 0);
            }
            else if(!FigureEntity->IsGrabbed)
            {
                if (!LevelEntity->Options.ToggleMenu) {
                    for (s32 i = Size - 1; i >= 0; --i)
                    {
                        ActiveIndex = FigureEntity->FigureOrder[i];
                        
                        r32 CurrentBlockSize = FigureUnit[ActiveIndex].IsEnlarged ? GridBlockSize : InActiveBlockSize;
                        r32 HalfShellSize = CurrentBlockSize / 2.0f;
                        
                        b32 IsInShell = false;
                        for (int BlockIndex = 0;
                             BlockIndex < FIGURE_BLOCKS_MAXIMUM;
                             ++BlockIndex)
                        {
                            rectangle2 BlockRectangle = {};
                            BlockRectangle.Min.x = FigureUnit[ActiveIndex].Shell[BlockIndex].x - HalfShellSize;
                            BlockRectangle.Min.y = FigureUnit[ActiveIndex].Shell[BlockIndex].y - HalfShellSize;
                            BlockRectangle.Max.x = FigureUnit[ActiveIndex].Shell[BlockIndex].x + HalfShellSize;
                            BlockRectangle.Max.y = FigureUnit[ActiveIndex].Shell[BlockIndex].y + HalfShellSize;
                            
                            if (IsInRectangle(MousePos, BlockRectangle))
                            {
                                IsInShell = true;
                                break;
                            }
                        }
                        
                        if (IsInShell)
                        {
                            FigureEntity->IsGrabbed = true;
                            FigureEntity->FigureOutline = -1;
                            
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
                            
                            Mix_PlayChannel(1, LevelEntity->PickSound, 0);
                            break;
                        }
                    }
                }
                
                if(IsInRectangle(MousePos, GridEntity->GridArea))
                {
                    for(u32 i = 0; i < GridEntity->MovingBlocksAmount; i++)
                    {
                        if (IsInRectangle(MousePos, GridEntity->MovingBlocks[i].Area))
                        {
                            if(!GridEntity->MovingBlocks[i].IsVertical) 
                            {
                                GridEntityMoveBlockHorizontally(GridEntity, &GridEntity->MovingBlocks[i]);
                            }
                            else
                            {
                                GridEntityMoveBlockVertically(GridEntity, &GridEntity->MovingBlocks[i]);
                            }
                            
                            //return ShouldExit;
                        }
                    }
                }
            }
            else
            {
                if(!FigureEntity->IsRotating && !FigureEntity->IsFlipping)
                {
                    rectangle2 ScreenArea;
                    ScreenArea.Min.x = 0;
                    ScreenArea.Min.y = 0;
                    ScreenArea.Max.x = ScreenWidth;
                    ScreenArea.Max.y = ScreenHeight;
                    
                    v2 Center = {};
                    Center.x   = FigureUnit[ActiveIndex].Position.x + (FigureUnit[ActiveIndex].Size.w / 2);
                    Center.y   = FigureUnit[ActiveIndex].Position.y + (FigureUnit[ActiveIndex].Size.h) * FigureUnit[ActiveIndex].CenterOffset;
                    
                    rectangle2 Rectangle = FigureEntity->FigureArea;
                    b32 HasToReturn = (IsInRectangle(FigureUnit[ActiveIndex].Shell, FIGURE_BLOCKS_MAXIMUM, FigureEntity->FigureArea)) || (!IsInRectangle(Center, ScreenArea));
                    
                    if (HasToReturn)
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
                    
                    Mix_PlayChannel(1, LevelEntity->DropSound, 0);
                }
            }
        }
        if(Input->MouseButtons[1].EndedDown)
        {
            if(FigureEntity->IsGrabbed)
            {
                if (!FigureEntity->IsRotating)
                {
                    FigureEntity->IsRotating = true;
                    FigureUnitRotateShellBy(&FigureUnit[ActiveIndex], 90);
                }
            }
        } 
        
        if(FigureEntity->IsGrabbed)
        {
            // TODO(msokolov): not very resolution independent 
            v2 dt = 
            {
                (r32) Input->MouseRelX, (r32) Input->MouseRelY
            };
            
            FigureUnitMove(&FigureUnit[ActiveIndex], dt);
        }
    }
    
    if (LevelEntity->Options.ToggleMenu)
    {
        v2 ButtonRectangleDim = LevelEntity->Options.ButtonDimension;
        v2 MenuPosition = LevelEntity->Options.MenuPosition;
        
        rectangle2 ButtonRectangle = {};
        rectangle2 TextRectangle = {};
        
        b32 ButtonWasPressed = false;
        
        for (u32 Index = 0;
             Index < 4;
             ++Index)
        {
            ButtonRectangle.Min.x = MenuPosition.x;
            ButtonRectangle.Min.y = MenuPosition.y + (ButtonRectangleDim.h * Index);
            SetDim(&ButtonRectangle, ButtonRectangleDim);
            
            v2 TextureDim   = QueryTextureDim(LevelEntity->Options.MenuTexture[Index]);
            TextRectangle.Min.x = ButtonRectangle.Min.x + (ButtonRectangleDim.w / 2.0f) - (TextureDim.w / 2.0f);
            TextRectangle.Min.y = ButtonRectangle.Min.y + (ButtonRectangleDim.h / 2.0f) - (TextureDim.h / 2.0f);
            SetDim(&TextRectangle, TextureDim);
            
            if (IsInRectangle(MousePos, ButtonRectangle))
            {
                options_choice NewChoice = (options_choice)Index;
                
                if (NewChoice != LevelEntity->Options.Choice)
                {
                    LevelEntity->Options.Choice = NewChoice;
                }
                
                if (Input->MouseButtons[0].EndedDown)
                {
                    if (IsInRectangle(MousePos, ButtonRectangle))
                    {
                        options_choice OptionChoice = (options_choice) Index;
                        switch(OptionChoice)
                        {
                            case RESTART_OPTION:
                            {
                                Result = options_choice::RESTART_OPTION;
                            } break;
                            case SETTINGS_OPTION:
                            {
                                Result = options_choice::SETTINGS_OPTION;
                            } break;
                            case MAINMENU_OPTION:
                            {
                                Result = options_choice::MAINMENU_OPTION;
                                Mix_FadeOutMusic(128);
                            } break;
                            case QUIT_OPTION:
                            {
                                Result = options_choice::QUIT_OPTION;
                                Mix_FadeOutMusic(128);
                            } break;
                        }
                        
                        ButtonWasPressed = true;
                        ToggleMenuOption = false;
                        LevelEntity->Options.Choice = OptionChoice;
                        
                        if (OptionChoice == RESTART_OPTION) {
                            LevelEntity->GearIsRotating = true;
                            Mix_PlayChannel(0, LevelEntity->Options.GearSound, 0);
                        }
                        else if (OptionChoice == SETTINGS_OPTION) {
                            LevelEntity->GearAngle = 0.0f;
                            LevelEntity->Options.InterpPoint = 0.0f;
                        }
                        
                        
                    }
                }
            }
        }
        
        if (Input->MouseButtons[0].EndedDown && !ButtonWasPressed)
        {
            ToggleMenuOption = false;
        }
    }
    
    if (LevelEntity->Options.ToggleMenu && !ToggleMenuOption && LevelEntity->Options.Choice != MAINMENU_OPTION) {
        LevelEntity->GearIsRotating = true;
        Mix_PlayChannel(0, LevelEntity->Options.GearSound, 0);
    }
    
    LevelEntity->Options.ToggleMenu = ToggleMenuOption;
    
    return (Result);
}


static void
FigureEntityAlignFigures(figure_entity *Entity)
{
    u32 Size = Entity->FigureAmount;
    r32 ColumnSize1 = 0.0f;
    r32 ColumnSize2 = 0.0f;
    u32 FigureIntervalX = roundf(IDLE_BLOCK_SIZE / 4.0f);
    u32 FigureIntervalY = roundf(IDLE_BLOCK_SIZE / 4.0f);
    
    rectangle2 AreaQuad    = {};
    rectangle2 DefaultZone = Entity->FigureArea;
    
    for (u32 i = 0; i < Size; ++i)
    {
        AreaQuad = FigureUnitGetArea(&Entity->FigureUnit[i]);
        v2 AreaQuadDimension = GetDim(AreaQuad);
        
        i % 2 == 0
            ? ColumnSize1 += AreaQuadDimension.h + FigureIntervalY
            : ColumnSize2 += AreaQuadDimension.h + FigureIntervalY;
    }
    
    u32 PitchX           = 0;
    s32 NewPositionX     = 0;
    s32 NewPositionY     = 0;
    v2 NewPosition = {};
    
    u32 CurrentColumnSize1  = 0;
    u32 CurrentColumnSize2  = 0;
    u32 FigureBoxWidth  = 0;
    s32 SpaceBetweenGrid = roundf(IDLE_BLOCK_SIZE / 4.0f);
    
    for (u32 i = 0; i < Size; ++i)
    {
        PitchX = i % 2;
        AreaQuad = FigureUnitGetArea(&Entity->FigureUnit[i]);
        v2 AreaQuadDimension = GetDim(AreaQuad);
        
        FigureBoxWidth = IDLE_BLOCK_SIZE * 4;
        NewPosition.x  = DefaultZone.Min.x + (FigureBoxWidth * PitchX);
        NewPosition.x += SpaceBetweenGrid;
        NewPosition.x += (FigureBoxWidth / 2 ) - (AreaQuadDimension.w / 2);
        NewPosition.x += FigureIntervalX * PitchX;
        
        if(i % 2 == 0)
        {
            NewPosition.y = DefaultZone.Min.y + FigureIntervalY;
            NewPosition.y += CurrentColumnSize1;
            
            CurrentColumnSize1 += AreaQuadDimension.h + FigureIntervalY;
        }
        else
        {
            NewPosition.y = DefaultZone.Min.y + FigureIntervalY;
            NewPosition.y += CurrentColumnSize2;
            
            CurrentColumnSize2 += AreaQuadDimension.h + FigureIntervalY;
        }
        
        FigureUnitDefineDefaultArea(&Entity->FigureUnit[i], NewPosition);
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

static inline v2
MoveObject2D(v2 Position, v2 TargetPosition, r32 dtForFrame, r32 Speed)
{
    v2 Result = TargetPosition - Position;
    v2 Step = Normalize(Result) * (Speed * dtForFrame);
    
    if (SquareRoot(Result) > (SquareRoot(Step) * 1.2f))
    {
        Result = Step;
    }
    
    return (Result);
}

static void
RenderFigureStructure(render_group *RenderGroup, figure_unit *Entity)
{
    v4 Color = V4(0.0f, 255.0f, 0.0f, 255.0f);
    
    rectangle2 Rectangle = {};
    Rectangle.Min.x = Entity->Position.x;
    Rectangle.Min.y = Entity->Position.y;
    Rectangle.Max.x = Rectangle.Min.x + Entity->Size.w;
    Rectangle.Max.y = Rectangle.Min.y + Entity->Size.h;
    
    PushRectangleOutline(RenderGroup, Rectangle, Color);
    
    r32 BlockSize = 20.0f;
    
    for (u32 Index = 0;
         Index < FIGURE_BLOCKS_MAXIMUM;
         ++Index)
    {
        Rectangle.Min.x = Entity->Shell[Index].x - (BlockSize * 0.5f);
        Rectangle.Min.y = Entity->Shell[Index].y - (BlockSize * 0.5f);
        Rectangle.Max.x = Rectangle.Min.x + BlockSize;
        Rectangle.Max.y = Rectangle.Min.y + BlockSize;
        PushRectangle(RenderGroup, Rectangle, Color);
        
        v2 Center = {};
        Center.x  = Entity->Position.x + (Entity->Size.w * 0.5);
        Center.y  = Entity->Position.y + (Entity->Size.h) * Entity->CenterOffset;
        
        Rectangle.Min.x = Center.x - (BlockSize * 0.5f);
        Rectangle.Min.y = Center.y - (BlockSize * 0.5f);
        Rectangle.Max.x = Rectangle.Min.x + BlockSize;
        Rectangle.Max.y = Rectangle.Min.y + BlockSize;
        PushRectangle(RenderGroup, Rectangle, {255.0f, 0.0f, 0.0f, 255.0f});
    }
}

static rectangle2
LerpRectangleDimension(v2 start, v2 finish, float time) {
    
    rectangle2 result = {};
    
    v2 lerp = Lerp2(start, finish, time);
    
    result.Min.x = Min2(start.x, lerp.x);
    result.Min.y = Min2(start.y, lerp.y);
    result.Max.x = Max2(start.x, lerp.x);
    result.Max.y = Max2(start.y, lerp.y);
    
    return (result);
}

static rectangle2
GridEntityGetGridRectangle(grid_entity *GridEntity) {
    
    rectangle2 Result = GridEntity->GridArea;
    
    v2 GridAreaDimension = GetDim(GridEntity->GridArea);
    r32 ActualGridWidth  = GridEntity->ColumnAmount * (r32)GRID_BLOCK_SIZE;
    r32 ActualGridHeight = GridEntity->RowAmount * (r32)GRID_BLOCK_SIZE;
    
    Result.Min.x = Result.Min.x + (GridAreaDimension.w / 2.0f) - (ActualGridWidth / 2.0f);
    Result.Min.y = Result.Min.y + (GridAreaDimension.h / 2.0f) - (ActualGridHeight / 2.0f);
    SetDim(&Result, ActualGridWidth, ActualGridHeight);
    
    return(Result);
}

static void
GridEntityRender(grid_entity *GridEntity, render_group *RenderGroup) {
    
    rectangle2 GridArea = GridEntityGetGridRectangle(GridEntity);
    u32 RowAmount = GridEntity->RowAmount;
    u32 ColumnAmount = GridEntity->ColumnAmount;
    r32 GridBlockSize = GRID_BLOCK_SIZE;
    
    u32 CellCounter = 0;
    rectangle2 GridCellRectangle = {};
    for (u32 Row = 0; Row < RowAmount; ++Row)
    {
        GridCellRectangle.Min.y = GridArea.Min.y + (GridBlockSize * Row);
        
        if (CellCounter != 0 && ColumnAmount % 2 == 0)
            CellCounter -= 1;
        
        for (u32 Col = 0; Col < ColumnAmount; ++Col)
        {
            GridCellRectangle.Min.x = GridArea.Min.x + (GridBlockSize * Col);
            
            GridCellRectangle.Max.x = GridCellRectangle.Min.x + GridBlockSize;
            GridCellRectangle.Max.y = GridCellRectangle.Min.y + GridBlockSize;
            
            if (CellCounter++ % 2 == 0) continue;
            
            u32 GridUnit = GridEntity->UnitField[(Row * ColumnAmount) + Col];
            if(GridUnit == 0 || GridUnit == 2 || GridUnit == 3)
            {
                rectangle2 TextureRectangle = {};
                TextureRectangle.Min.x = GridCellRectangle.Min.x - (25.0f * 0.5f);
                TextureRectangle.Min.y = GridCellRectangle.Min.y - (25.0f * 0.5f);
                SetDim(&TextureRectangle, GetDim(GridCellRectangle).w + 25.0f, GetDim(GridCellRectangle).h + 25.0f);
                
                PushBitmap(RenderGroup, GridEntity->GridCell2Texture, TextureRectangle);
                //PushRectangleOutline(RenderGroup, TextureRectangle, V4(255.0f, 0.0f, 0.0f, 255.0f));
                //PushRectangleOutline(RenderGroup, GridCellRectangle, V4(255.0f, 255.0f, 0.0f, 255.0f));
            }
        }
    }
    
    CellCounter = 0;
    for (u32 Row = 0; Row < RowAmount; ++Row)
    {
        GridCellRectangle.Min.y = GridArea.Min.y + (GridBlockSize * Row);
        
        if (CellCounter != 0 && ColumnAmount % 2 == 0)
            CellCounter -= 1;
        
        for (u32 Col = 0; Col < ColumnAmount; ++Col)
        {
            GridCellRectangle.Min.x = GridArea.Min.x + (GridBlockSize * Col);
            
            GridCellRectangle.Max.x = GridCellRectangle.Min.x + GridBlockSize;
            GridCellRectangle.Max.y = GridCellRectangle.Min.y + GridBlockSize;
            
            if (CellCounter++ % 2 != 0) continue;
            
            u32 GridUnit = GridEntity->UnitField[(Row * ColumnAmount) + Col];
            if(GridUnit == 0 || GridUnit == 2 || GridUnit == 3)
            {
                rectangle2 TextureRectangle = {};
                TextureRectangle.Min.x = GridCellRectangle.Min.x - (25.0f * 0.5f);
                TextureRectangle.Min.y = GridCellRectangle.Min.y - (25.0f * 0.5f);
                SetDim(&TextureRectangle, GetDim(GridCellRectangle).w + 25.0f, GetDim(GridCellRectangle).h + 25.0f);
                
                PushBitmap(RenderGroup, GridEntity->GridCell1Texture, TextureRectangle);
            }
        }
    }
}

static void
FigureEntityRenderFigures(figure_entity *FigureEntity, render_group *RenderGroup) {
    
    /* Figure Rendering */
    for(u32 i = 0; i < FigureEntity->FigureAmount; ++i)
    {
        u32 Index = FigureEntity->FigureOrder[i];
        
        figure_unit *Entity = &FigureEntity->FigureUnit[Index];
        
        v2 FigureCenter = {};
        FigureCenter.x   = Entity->Position.x + (Entity->Size.w / 2);
        FigureCenter.y   = Entity->Position.y + (Entity->Size.h) * Entity->CenterOffset;
        
        v2 Center;
        Center.x = FigureCenter.x - Entity->Position.x;
        Center.y = FigureCenter.y - Entity->Position.y;
        
        v2 ShadowOffset = {};
        if (!Entity->IsStick)
        {
            ShadowOffset += 8.0f;
        }
        
        if (Index != FigureEntity->FigureActive && (Entity->IsEnlarged && !Entity->IsStick))
        {
            ShadowOffset -= 5.0f;
        }
        
        rectangle2 Rectangle = {};
        Rectangle.Min.x = Entity->Position.x;
        Rectangle.Min.y = Entity->Position.y;
        Rectangle.Max.w = Rectangle.Min.x + Entity->Size.w;
        Rectangle.Max.h = Rectangle.Min.y + Entity->Size.h;
        
        rectangle2 ShadowRectangle = {};
        ShadowRectangle.Min = Rectangle.Min + ShadowOffset;
        ShadowRectangle.Max = Rectangle.Max + ShadowOffset;
        
        game_texture *Texture = PickFigureTexture(FigureEntity, Entity->Form);
        game_texture *ShadowTexture = PickFigureShadowTexture(Entity->Form, FigureEntity);
        
        PushBitmapEx(RenderGroup, ShadowTexture, ShadowRectangle, Entity->Angle, Center, Entity->Flip);
        PushBitmapEx(RenderGroup, Texture, Rectangle, Entity->Angle, Center, Entity->Flip);
        if (FigureEntity->FigureOutline == Index) {
            game_texture *OutlineTexture = PickFigureOutlineTexture(Entity->Form, FigureEntity);
            PushBitmapEx(RenderGroup, OutlineTexture, Rectangle, Entity->Angle, Center, Entity->Flip);
        }
        
        //RenderFigureStructure(RenderGroup, Entity);
    }
}

static b32
PlaygroundAnimationUpdateAndRender(playground *Playground, render_group *RenderGroup, 
                                   r32 dtForFrame, b32 IsStartup) {
    b32 Result = false;
    
    b32 AnimationDone = true;
    
    r32 InterpPoint = Playground->Animation.InterpPoint + (dtForFrame / Playground->Animation.TimeMax);
    if (InterpPoint >= 1.0f) 
        InterpPoint = 1.0f;
    
    if (!IsStartup) 
        InterpPoint = 1.0f - InterpPoint;
    
    /* Background */
    rectangle2 BackgroundRectangle = {V2(0.0, 0.0f), V2(VIRTUAL_GAME_WIDTH, VIRTUAL_GAME_HEIGHT)};
    PushBitmap(RenderGroup, Playground->BackgroundTexture, BackgroundRectangle);
    
    // NOTE(msokolov): UI Animation
    SDL_SetTextureAlphaMod(Playground->CornerLeftTopTexture, InterpPoint * 255.0f);
    SDL_SetTextureAlphaMod(Playground->CornerLeftBottomTexture, InterpPoint * 255.0f);
    SDL_SetTextureAlphaMod(Playground->CornerRightTopTexture, InterpPoint * 255.0f);
    SDL_SetTextureAlphaMod(Playground->CornerRightBottomTexture, InterpPoint * 255.0f);
    SDL_SetTextureAlphaMod(Playground->VerticalBorderTexture, InterpPoint * 255.0f);
    
    rectangle2 BorderRectangle = {};
    rectangle2 ClipRectangle   = {};
    
    v2 TextureDim = QueryTextureDim(Playground->CornerLeftTopTexture);
    v2 RectangleDim = {408.0f, 408.0f};
    
    /* Top Left */
    {
        v2 StartPos  = {20.0f, 20.0f};
        v2 FinishPos = {RectangleDim.w + 20.0f, RectangleDim.h + 20.0f};
        BorderRectangle = LerpRectangleDimension(StartPos, FinishPos, InterpPoint);
        ClipRectangle = LerpRectangleDimension(V2(0.0f, 0.0f), V2(TextureDim.w, TextureDim.h), InterpPoint);
        
        BorderRectangle += V2(5.0f, 5.0f);
        PushBitmapEx(RenderGroup, Playground->CornerLeftTopShadowTexture, BorderRectangle, ClipRectangle, 0.0f, V2(0.0f, 0.0f), SDL_FLIP_NONE);
        
        BorderRectangle -= V2(5.0f, 5.0f);
        PushBitmapEx(RenderGroup, Playground->CornerLeftTopTexture, BorderRectangle, ClipRectangle, 0.0f, V2(0.0f, 0.0f), SDL_FLIP_NONE);
    }
    
    /* Top Right */
    {
        rectangle2 Rectangle = BorderRectangle;
        Rectangle.Min += V2(VIRTUAL_GAME_WIDTH - 20.0f - 20.0f - 308.0f, 0.0f);
        Rectangle.Max += V2(VIRTUAL_GAME_WIDTH - 20.0f - 20.0f - 308.0f, 0.0f);
        
        Rectangle += V2(5.0f, 5.0f);
        PushBitmapEx(RenderGroup, Playground->CornerLeftTopShadowTexture, Rectangle, ClipRectangle, 90.0f, V2(154.0f, 154.0f), SDL_FLIP_NONE);
        
        Rectangle -= V2(5.0f, 5.0f);
        PushBitmapEx(RenderGroup, Playground->CornerLeftTopTexture, Rectangle, ClipRectangle, 90.0f, V2(154.0f, 154.0f), SDL_FLIP_NONE);
        
    }
    /* Bottom Left */
    {
        rectangle2 Rectangle = BorderRectangle;
        Rectangle.Min += V2(0.0f, VIRTUAL_GAME_HEIGHT - 20.0f - 20.0f - 308.0f);
        Rectangle.Max += V2(0.0f, VIRTUAL_GAME_HEIGHT - 20.0f - 20.0f - 308.0f);
        
        Rectangle += V2(5.0f, 5.0f);
        PushBitmapEx(RenderGroup, Playground->CornerLeftTopShadowTexture, Rectangle, ClipRectangle, -90.0f, V2(154.0f, 154.0f), SDL_FLIP_NONE);
        
        Rectangle -= V2(5.0f, 5.0f);
        PushBitmapEx(RenderGroup, Playground->CornerLeftTopTexture, Rectangle, ClipRectangle, -90.0f, V2(154.0f, 154.0f), SDL_FLIP_NONE);
    }
    
    /* Bottom Right */
    {
        rectangle2 Rectangle = BorderRectangle;
        Rectangle.Min += V2(VIRTUAL_GAME_WIDTH - 20.0f - 20.0f - 308.0f, VIRTUAL_GAME_HEIGHT - 20.0f - 20.0f - 308.0f);
        Rectangle.Max += V2(VIRTUAL_GAME_WIDTH - 20.0f - 20.0f - 308.0f, VIRTUAL_GAME_HEIGHT - 20.0f - 20.0f - 308.0f);
        
        Rectangle += V2(5.0f, 5.0f);
        PushBitmapEx(RenderGroup, Playground->CornerLeftTopShadowTexture, Rectangle, ClipRectangle, 180.0f, V2(154.0f, 154.0f), SDL_FLIP_NONE);
        
        Rectangle -= V2(5.0f, 5.0f);
        PushBitmapEx(RenderGroup, Playground->CornerLeftTopTexture, Rectangle, ClipRectangle, 180.0f, V2(154.0f, 154.0f), SDL_FLIP_NONE);
    }
    
    /* Vertical Border*/
    {
        v2 Dim = QueryTextureDim(Playground->VerticalBorderTexture);
        
        rectangle2 BorderRectangle = {V2(1200.0f, 100.0f), V2(1200.0f + Dim.w, 100.0f + Dim.h)};
        PushBitmap(RenderGroup, Playground->VerticalBorderTexture, BorderRectangle);
        
        // Upper half
        rectangle2 Rectangle = {};
        Rectangle.Min = V2(1200.0f, 100.0f);
        Rectangle.Max.x = Rectangle.Min.x + Dim.w;
        Rectangle.Max.y = Lerp1(Rectangle.Min.y + (Dim.h * 0.5f), 100.0f, InterpPoint);
        PushRectangle(RenderGroup, Rectangle, V4(51.0f, 8.0f, 23.0f, 255.0f));
        
        // Bottom half
        Rectangle.Max = BorderRectangle.Max;
        Rectangle.Min.x = BorderRectangle.Min.x;
        Rectangle.Min.y = Lerp1(Rectangle.Max.y - (Dim.h * 0.5f), Rectangle.Max.y, InterpPoint);
        PushRectangle(RenderGroup, Rectangle, V4(51.0f, 8.0f, 23.0f, 255.0f));
    }
    
    /* Grid Animation */
    if (IsStartup)
    {
        rectangle2 GridArea = GridEntityGetGridRectangle(&Playground->GridEntity);
        
        s32 DiagonalAmount = Playground->GridEntity.RowAmount + Playground->GridEntity.ColumnAmount - 1;
        r32 DiagonalMaxInterpP = (1.0f / ((DiagonalAmount / 2) + 1));
        
#if 0
        //r32 InterpPoint = Playground->Animation.InterpPoint + (dtForFrame / Playground->Animation.TimeMax);
        r32 GridInterpPoint = Playground->GridInterpPoint + (dtForFrame / 2.0f);
        Playground->GridInterpPoint = GridInterpPoint;
        
        if (GridInterpPoint < 1.0f)
            AnimationDone = false;
#endif
        
        r32 InterpP = InterpPoint < 0.5f ? InterpPoint * 2.0f : (InterpPoint * 2.0f) - 1.0f;
        //r32 InterpP = InterpPoint;
        
        if (InterpPoint > 0.5f) {
            for (int k = DiagonalAmount, i = 0; k >= 0; --k) {
                if (k % 2 != 0) continue;
                
                r32 StartInterpP = i * DiagonalMaxInterpP;
                r32 EndInterpP   = (i * DiagonalMaxInterpP) + DiagonalMaxInterpP;
                
                r32 DiagonalInterpP = (InterpP - StartInterpP) / DiagonalMaxInterpP;
                if (InterpP < StartInterpP)
                    DiagonalInterpP = 0.0f;
                if (InterpP > EndInterpP)
                    DiagonalInterpP = 1.0f;
                
                i++;
                
                for (int Col = 0; Col <= k; ++Col) {
                    int Row = k - Col;
                    
                    if (Row < Playground->GridEntity.RowAmount && Col < Playground->GridEntity.ColumnAmount) {
                        
                        s32 MirrorCol = Col - (Playground->GridEntity.ColumnAmount - 1);
                        if (MirrorCol < 0) MirrorCol = -MirrorCol;
                        
                        r32 LerpSize = Lerp1(0.0f, GRID_BLOCK_SIZE + 25.0f, DiagonalInterpP);
                        //r32 LerpSize = GRID_BLOCK_SIZE + 25.0f;
                        
                        rectangle2 CellRectangle = {};
                        CellRectangle.Min.x = GridArea.Min.x + (GRID_BLOCK_SIZE * MirrorCol);
                        CellRectangle.Min.y = GridArea.Min.y + (GRID_BLOCK_SIZE * Row);
                        
                        v2 StartPos  = V2(GridArea.Max.x - GRID_BLOCK_SIZE, GridArea.Min.y);
                        v2 TargetPos = CellRectangle.Min;
                        v2 LerpPos = Lerp2(StartPos, TargetPos, DiagonalInterpP);
                        //if (InterpPoint > 0.5f) 
                        //LerpPos = TargetPos;
                        
                        LerpPos = CellRectangle.Min;
                        rectangle2 TextureRectangle = {};
                        TextureRectangle.Min.x = LerpPos.x + (GRID_BLOCK_SIZE * 0.5f) - (LerpSize * 0.5f);
                        TextureRectangle.Min.y = LerpPos.y + (GRID_BLOCK_SIZE * 0.5f) - (LerpSize * 0.5f);
                        SetDim(&TextureRectangle, LerpSize, LerpSize);
                        
                        PushBitmap(RenderGroup, Playground->GridEntity.GridCell2Texture, TextureRectangle);
                    }
                }
            }
        }
        
        for (int k = DiagonalAmount, i = 0; k >=0; --k) {
            if (k % 2 != 0) continue;
            
            r32 StartInterpP = i * DiagonalMaxInterpP;
            r32 EndInterpP   = (i * DiagonalMaxInterpP) + DiagonalMaxInterpP;
            
            r32 DiagonalInterpP = (InterpP - StartInterpP) / DiagonalMaxInterpP;
            if (InterpP < StartInterpP)
                DiagonalInterpP = 0.0f;
            if (InterpP > EndInterpP)
                DiagonalInterpP = 1.0f;
            
            i++;
            
            for (int Col = 0; Col <= k; ++Col) {
                int Row = k - Col;
                
                if (Row < Playground->GridEntity.RowAmount && Col < Playground->GridEntity.ColumnAmount) {
                    
                    rectangle2 CellRectangle = {};
                    CellRectangle.Min.x = GridArea.Min.x + (GRID_BLOCK_SIZE * Col);
                    CellRectangle.Min.y = GridArea.Min.y + (GRID_BLOCK_SIZE * Row);
                    
                    r32 LerpSize = InterpPoint > 0.5f ? GRID_BLOCK_SIZE + 25.0f : Lerp1(0.0f, GRID_BLOCK_SIZE + 25.0f, DiagonalInterpP);
                    //r32 LerpSize = Lerp1(0.0f, GRID_BLOCK_SIZE + 25.0f, DiagonalInterpP);
                    //r32 LerpSize = GRID_BLOCK_SIZE + 25.0f;
                    if (InterpPoint > 0.5f)
                        LerpSize = GRID_BLOCK_SIZE + 25.0f;
                    
                    v2 StartPos  = GridArea.Min;
                    v2 TargetPos = CellRectangle.Min;
                    v2 LerpPos = Lerp2(StartPos, TargetPos, DiagonalInterpP);
                    if (InterpPoint > 0.5f) 
                        LerpPos = TargetPos;
                    
                    LerpPos = CellRectangle.Min;
                    rectangle2 TextureRectangle = {};
                    TextureRectangle.Min.x = LerpPos.x + (GRID_BLOCK_SIZE * 0.5f) - (LerpSize * 0.5f);
                    TextureRectangle.Min.y = LerpPos.y + (GRID_BLOCK_SIZE * 0.5f) - (LerpSize * 0.5f);
                    SetDim(&TextureRectangle, LerpSize, LerpSize);
                    
                    PushBitmap(RenderGroup, Playground->GridEntity.GridCell1Texture, TextureRectangle);
                }
            }
        }
    }
    
    /* Figure Animation */
    
    b32 FoundFirstThrough = false;
    figure_unit *FigureUnit = Playground->FigureEntity.FigureUnit;
    if (IsStartup) {
        for(u32 i = 0; 
            i < Playground->FigureEntity.FigureAmount;
            ++i) {
            
            r32 FigureInterpPoint = 1.0f;
            if (FoundFirstThrough) 
                FigureInterpPoint = 0.0f;
            
            if (!Playground->FInterpStop[i] && !FoundFirstThrough) {
                FoundFirstThrough = true;
                AnimationDone = false;
                
                FigureInterpPoint = Playground->FInterpPoint + (dtForFrame / 0.12f);
                if (FigureInterpPoint > 1.0f) {
                    FigureInterpPoint = 1.0f;
                    if (!Playground->FInterpStop[i]) {
                        Playground->FInterpStop[i] = true;
                        
                        Mix_PlayChannel(((i+3) % 5) + 3, Playground->DropSound, 0);
                        printf("Index %d is done\n", i);
                    }
                }
                
                Playground->FInterpPoint = FigureInterpPoint == 1.0f ? 0.0f : FigureInterpPoint;
            }
            
            v2 FinishDimension  = Playground->AnimFigureDim[i];
            v2 StartDimension   = FinishDimension * 2.0f;
            
            v2 LerpDim = Lerp2(StartDimension, FinishDimension, FigureInterpPoint);
            FigureUnit[i].Size = LerpDim;
            
            v2 StartPos = {VIRTUAL_GAME_WIDTH, 0.0f};
            v2 LerpPos = Lerp2(StartPos, FigureUnit[i].HomePosition, FigureInterpPoint);
            FigureUnit[i].Position = LerpPos;
            
            r32 AngleOffset = 45.0f;
            r32 FinishAngle = FigureUnit[i].HomeAngle;
            r32 StartAngle  = FinishAngle + AngleOffset;
            r32 LerpAngle   = Lerp1(StartAngle, FinishAngle, FigureInterpPoint);
            FigureUnit[i].Angle = LerpAngle;
            
            if (FigureInterpPoint == 0.0f)
                FigureUnit[i].Position = V2(-VIRTUAL_GAME_WIDTH, -VIRTUAL_GAME_HEIGHT);
        }
        
        FigureEntityRenderFigures(&Playground->FigureEntity, RenderGroup);
    }
    
    else {
        
        FigureEntityRenderFigures(&Playground->FigureEntity, RenderGroup);
        
        rectangle2 GridArea = GridEntityGetGridRectangle(&Playground->GridEntity);
        GridArea.Min -= 10.0f;
        GridArea.Max += 20.0f;
        
        PushRectangle(RenderGroup, GridArea, V4(51.0f, 8.0f, 23.0f, (1.0f - InterpPoint) * 255.0f));
    }
    
    /* Level Indicator Animation */
    {
        v2 IndicatorSize = {30.0f, 30.0f};
        u32 IndicatorAmount = 8;
        s32 CurrentLevelIndicator = (Playground->LevelNumber - 1) % IndicatorAmount;
        r32 MaxTimeForIndicator = Playground->Animation.TimeMax / (r32)IndicatorAmount;
        r32 AnimationTimeLeft = (InterpPoint / MaxTimeForIndicator) * 255.0f;
        
        for (s32 Index = 0; 
             Index < IndicatorAmount;
             ++Index)
        {
            rectangle2 Rectangle = {};
            Rectangle.Min.x = (VIRTUAL_GAME_WIDTH * 0.5f) - (IndicatorSize.w * (IndicatorAmount / 2)) - ((IndicatorSize.w) * (IndicatorAmount / 2));
            Rectangle.Min.x += (Index * IndicatorSize.w) + ((IndicatorSize.w) * Index);
            Rectangle.Min.y = VIRTUAL_GAME_HEIGHT - (70.0f);
            SetDim(&Rectangle, IndicatorSize);
            
            if (Index == CurrentLevelIndicator)
                PushBitmap(RenderGroup, Playground->IndicatorEmptyTexture, Rectangle);
            else 
                PushBitmap(RenderGroup, Playground->IndicatorFilledTexture, Rectangle);
            
            r32 AlphaChannel = AnimationTimeLeft > 0.0f ? (s32)roundf(AnimationTimeLeft) % 256 : 0.0f;
            if (AnimationTimeLeft > 255.0f) 
                AlphaChannel = 255;
            
            PushRectangle(RenderGroup, Rectangle, V4(51.0f, 8.0f, 23.0f, 255.0f - AlphaChannel));
            
            AnimationTimeLeft -= 255.0f;
            if (AnimationTimeLeft < 0.0f) AnimationTimeLeft = 0.0f; 
        }
    }
    
    /* Gear Button Animation */
    {
        playground_options *Options = &Playground->Options;
        
        r32 LerpAngle = Lerp1(45.0f, 0.0f, InterpPoint);
        Playground->GearAngle = LerpAngle;
        
        rectangle2 GearRectangle = {};
        GearRectangle.Min.x = VIRTUAL_GAME_WIDTH  - 200.0f + 8.0f;
        GearRectangle.Min.y = VIRTUAL_GAME_HEIGHT - 200.0f + 8.0f;
        SetDim(&GearRectangle, 100.0f, 100.0f);
        
        // Gear Shadow Texture
        PushBitmapEx(RenderGroup, Options->GearShadowTexture, GearRectangle, Playground->GearAngle, V2(50.0f, 50.0f), SDL_FLIP_NONE);
        
        GearRectangle.Min.x = VIRTUAL_GAME_WIDTH  - 200.0f;
        GearRectangle.Min.y = VIRTUAL_GAME_HEIGHT - 200.0f;
        SetDim(&GearRectangle, 100.0f, 100.0f);
        
        // Gear Texture
        PushBitmapEx(RenderGroup, Options->GearTexture, GearRectangle, Playground->GearAngle, V2(50.0f, 50.0f), SDL_FLIP_NONE);
        
        r32 AlphaChannel = Lerp1(255.0f, 0.0f, InterpPoint);
        PushRectangle(RenderGroup, GearRectangle, V4(51.0f, 8.0f, 23.0f, AlphaChannel));
        //PushRectangleOutline(RenderGroup, GearRectangle, V4(255.0f, 0.0f, 0.0f, 255.0f));
    }
    
    r32 AlphaChannel = Lerp1(0.0f, 255.0f, 1.0f - InterpPoint);
    
    SDL_SetTextureAlphaMod(Playground->BackgroundTexture, AlphaChannel);
    PushBitmap(RenderGroup, Playground->BackgroundTexture);
    
    Playground->Animation.InterpPoint = IsStartup ? InterpPoint : 1.0f - InterpPoint;
    
    if ((IsStartup && InterpPoint == 1.0f && AnimationDone) ||  (!IsStartup && InterpPoint <= 0.0f)) Result = true;
    
    return (Result);
}


static playground_status
PlaygroundUpdateAndRender(playground *LevelEntity, render_group *RenderGroup, game_input *Input)
{
    playground_status Result = playground_status::LEVEL_RUNNING;
    
    grid_entity   *GridEntity   = &LevelEntity->GridEntity;
    figure_entity *FigureEntity = &LevelEntity->FigureEntity;
    figure_unit   *FigureUnit   = FigureEntity->FigureUnit;
    
    r32 GridBlockSize     = GRID_BLOCK_SIZE;
    r32 InActiveBlockSize = IDLE_BLOCK_SIZE;
    
    u32 RowAmount    = GridEntity->RowAmount;
    u32 ColumnAmount = GridEntity->ColumnAmount;
    u32 FigureAmount = FigureEntity->FigureAmount;
    s32 ActiveIndex  = FigureEntity->FigureActive;
    
    rectangle2 GridArea = GridEntityGetGridRectangle(GridEntity);
    
    /* game_input checking */
    if (LevelEntity->Animation.Finished) {
        options_choice OptionChoice = PlaygroundUpdateEvents(Input, LevelEntity, RenderGroup->Width, RenderGroup->Height);
        
        if (OptionChoice == options_choice::QUIT_OPTION)
        {
            Result = playground_status::LEVEL_GAME_QUIT;
        }
        else if (OptionChoice == options_choice::MAINMENU_OPTION)
        {
            Result = playground_status::LEVEL_MENU_QUIT;
        }
        else if (OptionChoice == options_choice::RESTART_OPTION)
        {
            Result = playground_status::LEVEL_RESTARTED;
        }
        else if (OptionChoice == options_choice::SETTINGS_OPTION)
        {
            Result = playground_status::LEVEL_SETTINGS_QUIT;
        }
    }
    
    // TODO(msokolov): this should be a texture (or not)
    //Clear(RenderGroup, {51, 8, 23, 255});
    Clear(RenderGroup, V4(51, 8, 23, 255));
    
    
    // NOTE(msokolov): Animation interpolation startup update
    if (!LevelEntity->Animation.Finished) {
        LevelEntity->Animation.Finished = PlaygroundAnimationUpdateAndRender(LevelEntity, RenderGroup, Input->dtForFrame, LevelEntity->IsStartup);
        
        if (LevelEntity->Animation.Finished) {
            if (LevelEntity->LevelFinished) {
                Result = playground_status::LEVEL_FINISHED;
            }
        }
        
        return (Result);
    }
    
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
                v2 Offset = {};
                
                u32 Count   = 0;
                u32 RowIndex[4] = {};
                u32 ColIndex[4] = {};
                
                for (u32 i = 0 ; i < RowAmount && Count != 4; ++i)
                {
                    for (u32 j = 0; j < ColumnAmount && Count != 4; ++j)
                    {
                        rectangle2 Rect = {};
                        Rect.Min.x = GridArea.Min.x + (j * GridBlockSize);
                        Rect.Min.y = GridArea.Min.y + (i * GridBlockSize);
                        Rect.Max.x = Rect.Min.x + GridBlockSize;
                        Rect.Max.y = Rect.Min.y + GridBlockSize;
                        
                        for (u32 l = 0; l < FIGURE_BLOCKS_MAXIMUM; ++l)
                        {
                            if (IsInRectangle(FigureUnit[FigureIndex].Shell[l], Rect))
                            {
                                Offset = Rect.Min + (GridBlockSize * 0.5f) - FigureUnit[FigureIndex].Shell[l];
                                
                                RowIndex[l] = i;
                                ColIndex[l] = j;
                                
                                Count++;
                                break;
                            }    
                        }
                    }
                }
                
                if(Count == 4)
                {
                    bool IsFree = true;
                    for (u32 i = 0; i < FIGURE_BLOCKS_MAXIMUM; ++i)
                    {
                        if(GridEntity->UnitField[(RowIndex[i] * ColumnAmount) + ColIndex[i]] > 0)
                        {
                            IsFree = false;
                        }
                    }
                    
                    if(IsFree)
                    {
                        FigureUnit[FigureIndex].IsStick = true;
                        v2 NewPosition = FigureUnit[FigureIndex].Position + Offset;
                        
                        for (u32 i = 0; i < GridEntity->StickUnitsAmount; ++i)
                        {
                            if(GridEntity->StickUnits[i].Index == -1)
                            {
                                GridEntity->StickUnits[i].Index     = FigureIndex;
                                GridEntity->StickUnits[i].Center    = NewPosition;
                                GridEntity->StickUnits[i].IsSticked = false;
                                
                                for (u32 j = 0; j < FIGURE_BLOCKS_MAXIMUM; ++j)
                                {
                                    GridEntity->StickUnits[i].Row[j] = RowIndex[j];
                                    GridEntity->StickUnits[i].Col[j] = ColIndex[j];
                                }
                                
                                break;
                            }
                        }
                        
                        FigureEntityLowPriority(FigureEntity, FigureIndex);
                    }
                }
            }
        }
        
        
    }
    
    /* StickUnits aka figures that are moving to a grid */
    b32 IsAllSticked = GridEntity->StickUnitsAmount > 0;
    for(u32 i = 0; i < GridEntity->StickUnitsAmount; ++i)
    {
        b32 IsSticked = GridEntity->StickUnits[i].IsSticked;
        
        if(!IsSticked)
        {
            IsAllSticked = false;
        }
        
        s32 Index = GridEntity->StickUnits[i].Index;
        if(!IsSticked && Index >= 0)
        {
            v2 Position = FigureUnit[Index].Position;
            v2 TargetPosition = GridEntity->StickUnits[i].Center;
            
            v2 dt = MoveObject2D(Position, TargetPosition, Input->dtForFrame, FigureEntity->FigureVelocity);
            
            Position = FigureUnitMove(&FigureUnit[Index], dt);
            if(Position == TargetPosition)
            {
                GridEntity->StickUnits[i].IsSticked = true;
                
                Mix_PlayChannel(1, LevelEntity->StickSound, 0);
                
                u32 RowIndex = 0;
                u32 ColIndex = 0;
                for(u32 j = 0; j < FIGURE_BLOCKS_MAXIMUM; j++)
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
        FigureEntity->FigureOutline = -1;
        
        /* Level is completed */
        LevelEntity->LevelFinished = true;
        
        LevelEntity->IsStartup = false;
        LevelEntity->Animation.Finished = false;
        LevelEntity->Animation.InterpPoint = 0.0f;
        LevelEntity->FInterpPoint = 0.0f;
        
        Mix_FadeOutMusic(128);
        Mix_PlayChannel(2, LevelEntity->CompleteSound, 0);
        
        for (int i = 0; i < FigureAmount; ++i) 
            LevelEntity->AnimFigureDim[i] = {FigureUnit[i].Position.x, FigureUnit[i].Position.y};
        
        printf("level is completed\n");
    }
    
    /* Check if a player is left with only one figure */
    {
        u32 RowIndex[FIGURE_BLOCKS_MAXIMUM] = {};
        u32 ColumnIndex[FIGURE_BLOCKS_MAXIMUM] = {};
        u32 CellCounter = 0;
        u32 LeastRowIndex    = GridEntity->RowAmount - 1;
        u32 LeastColumnIndex = GridEntity->ColumnAmount - 1;
        
        for (u32 Row = 0; Row < RowAmount; ++Row)
        {
            for (u32 Column = 0; Column < ColumnAmount; ++Column)
            {
                u32 Index = (Row * ColumnAmount) + Column;
                u32 Value = GridEntity->UnitField[Index];
                if (Value == 0)
                {
                    if (CellCounter < 4)
                    {
                        RowIndex[CellCounter]    = Row;
                        ColumnIndex[CellCounter] = Column;
                        
                        if (LeastColumnIndex > Column) 
                            LeastColumnIndex = Column;
                        
                        if (LeastRowIndex > Row)
                            LeastRowIndex = Row;
                    }
                    
                    ++CellCounter;
                }
            }
        }
        
        if (CellCounter == 4 && !LevelEntity->ShowTimer)
        {
            for (u32 Index = 0;
                 Index < FIGURE_BLOCKS_MAXIMUM;
                 ++Index)
            {
                RowIndex[Index] -= LeastRowIndex;
                ColumnIndex[Index] -= LeastColumnIndex;
            }
            
            u32 LastFigureIndex = FigureAmount;
            for (u32 FigureIndex = 0; 
                 FigureIndex < FigureAmount; 
                 ++FigureIndex)
            {
                if (!FigureUnit[FigureIndex].IsStick) 
                    LastFigureIndex = FigureIndex;
            }
            
            if (LastFigureIndex < FigureAmount)
            {
                u32 BlockSize = (FigureEntity->FigureUnit[LastFigureIndex].IsEnlarged) ? GRID_BLOCK_SIZE : IDLE_BLOCK_SIZE;
                
                rectangle2 FigureUnitArea = FigureUnitGetArea(&FigureEntity->FigureUnit[LastFigureIndex]);
                
                rectangle2 NormFigureArea = {};
                NormFigureArea.Max -= FigureUnitArea.Min;
                NormFigureArea.Min -= FigureUnitArea.Min;
                
                u32 FigureRowIndex[FIGURE_BLOCKS_MAXIMUM] = {};
                u32 FigureColumnIndex[FIGURE_BLOCKS_MAXIMUM] = {};
                
                b32 IsIdentical = true;
                
                v2 FigureShell[FIGURE_BLOCKS_MAXIMUM] = {};
                for (u32 Index = 0;
                     Index < FIGURE_BLOCKS_MAXIMUM;
                     ++Index)
                {
                    FigureShell[Index].x = FigureEntity->FigureUnit[LastFigureIndex].Shell[Index].x - FigureUnitArea.Min.x;
                    FigureShell[Index].y = FigureEntity->FigureUnit[LastFigureIndex].Shell[Index].y - FigureUnitArea.Min.y;
                    
                    FigureRowIndex[Index] = FigureShell[Index].y / BlockSize;
                    FigureColumnIndex[Index] = FigureShell[Index].x / BlockSize;
                    
                    rectangle2 ShellRectangle = {};
                    ShellRectangle.Min.x = FigureShell[Index].x - (BlockSize * 0.5f);
                    ShellRectangle.Min.y = FigureShell[Index].y - (BlockSize * 0.5f);
                    SetDim(&ShellRectangle, BlockSize, BlockSize);
                }
                
                for (u32 Index = 0; 
                     Index < FIGURE_BLOCKS_MAXIMUM;
                     ++Index)
                {
                    u32 OverlapCounter = 0;
                    for (u32 A = 0;
                         A < FIGURE_BLOCKS_MAXIMUM;
                         A++)
                    {
                        for (u32 B = 0;
                             B < FIGURE_BLOCKS_MAXIMUM;
                             ++B)
                        {
                            if ((RowIndex[A] == FigureRowIndex[B]) && (ColumnIndex[A] == FigureColumnIndex[B]))
                                OverlapCounter++;
                        }
                    }
                    
                    if (OverlapCounter == 4)
                    {
                        printf("Show timer\n");
                        LevelEntity->ShowTimer = true;
                        break;
                    }
                    
                    u32 FigureMatrix[4][4] = {};
                    for (u32 CellIndex = 0;
                         CellIndex < FIGURE_BLOCKS_MAXIMUM;
                         ++CellIndex)
                    {
                        FigureMatrix[FigureRowIndex[CellIndex]][FigureColumnIndex[CellIndex]] = 1;
                    }
                    
                    RotateMatrixClockwise(FigureMatrix);
                    
                    u32 CellCounter = 0;
                    LeastColumnIndex = FIGURE_BLOCKS_MAXIMUM;
                    LeastRowIndex = FIGURE_BLOCKS_MAXIMUM;
                    for (u32 Row = 0;
                         Row < FIGURE_BLOCKS_MAXIMUM;
                         ++Row)
                    {
                        for (u32 Column = 0;
                             Column < FIGURE_BLOCKS_MAXIMUM;
                             ++Column)
                        {
                            if (FigureMatrix[Row][Column] == 1)
                            {
                                FigureRowIndex[CellCounter] = Row;
                                FigureColumnIndex[CellCounter] = Column;
                                
                                if (LeastRowIndex > Row)
                                    LeastRowIndex = Row;
                                
                                if (LeastColumnIndex > Column)
                                    LeastColumnIndex = Column;
                                
                                CellCounter++;
                            }
                        }
                    }
                    
                    for (u32 Index = 0;
                         Index < FIGURE_BLOCKS_MAXIMUM;
                         ++Index)
                    {
                        FigureRowIndex[Index] -= LeastRowIndex;
                        FigureColumnIndex[Index] -= LeastColumnIndex;
                    }
                    
                }
            }
        }
        else if (CellCounter > 4)
        {
            LevelEntity->ShowTimer = false;
        }
    }
    
#if 0
    printf("grid---------\n");
    for (int i = 0; i < RowAmount; ++i) {
        for (int j = 0; j < ColumnAmount; ++j) {
            u32 GridUnit = GridEntity->UnitField[(i * ColumnAmount) + j];
            printf("%d ", GridUnit);
        }
        printf("\n");
    }
#endif
    
    /* Background */
    rectangle2 BackgroundRectangle = {V2(0.0, 0.0f), V2(VIRTUAL_GAME_WIDTH, VIRTUAL_GAME_HEIGHT)};
    PushBitmap(RenderGroup, LevelEntity->BackgroundTexture, BackgroundRectangle);
    
    // NOTE(msokolov): GridEntity Rendering
    GridEntityRender(GridEntity, RenderGroup);
    
    if (FigureEntity->IsGrabbed) {
        u32 FigureIndex = FigureEntity->FigureActive;
        if (!FigureEntity->IsRotating) {
            
            b32 IsInsideGrid = true;
            for (int i = 0; i < FIGURE_BLOCKS_MAXIMUM && IsInsideGrid; ++i) {
                
                if (!IsInRectangle(FigureUnit[FigureIndex].Shell[i], GridArea))
                    IsInsideGrid = false;
            }
            
            if (IsInsideGrid) {
                v2 Offset = {};
                
                u32 Count = 0;
                for (u32 i = 0 ; i < RowAmount && Count != 4 && IsInsideGrid; ++i) {
                    for (u32 j = 0; j < ColumnAmount && Count != 4 && IsInsideGrid; ++j) {
                        rectangle2 Rect = {};
                        Rect.Min.x = GridArea.Min.x + (j * GridBlockSize);
                        Rect.Min.y = GridArea.Min.y + (i * GridBlockSize);
                        Rect.Max.x = Rect.Min.x + GridBlockSize;
                        Rect.Max.y = Rect.Min.y + GridBlockSize;
                        
                        for (u32 l = 0; l < FIGURE_BLOCKS_MAXIMUM; ++l) {
                            if (IsInRectangle(FigureUnit[FigureIndex].Shell[l], Rect)) {
                                if(GridEntity->UnitField[(i * ColumnAmount) + j] > 0) {
                                    IsInsideGrid = false;
                                    break;
                                }
                                
                                Offset = Rect.Min + (GridBlockSize * 0.5f) - FigureUnit[FigureIndex].Shell[l];
                                Count++;
                            }
                        }
                    }
                    
                    
                }
                
                if (Count == 4) {
                    
                    rectangle2 Rectangle = {};
                    Rectangle.Min.x = FigureUnit[FigureIndex].Position.x + Offset.x;
                    Rectangle.Min.y = FigureUnit[FigureIndex].Position.y + Offset.y;
                    Rectangle.Max.w = Rectangle.Min.x + FigureUnit[FigureIndex].Size.w;
                    Rectangle.Max.h = Rectangle.Min.y + FigureUnit[FigureIndex].Size.h;
                    
                    v2 FigureCenter = {};
                    FigureCenter.x   = FigureUnit[FigureIndex].Position.x + (FigureUnit[FigureIndex].Size.w * 0.5f);
                    FigureCenter.y   = FigureUnit[FigureIndex].Position.y + (FigureUnit[FigureIndex].Size.h) * FigureUnit[FigureIndex].CenterOffset;
                    
                    v2 Center;
                    Center.x = FigureCenter.x - FigureUnit[FigureIndex].Position.x;
                    Center.y = FigureCenter.y - FigureUnit[FigureIndex].Position.y;
                    
                    game_texture *Texture = PickFigureGroundTexture(FigureUnit[FigureIndex].Form, FigureEntity);
                    PushBitmapEx(RenderGroup, Texture, Rectangle, FigureUnit[FigureIndex].Angle, Center, FigureUnit[FigureIndex].Flip);
                }
            }
        }
    }
    
    /* FigureEntity Update and Rendering */
    if(FigureEntity->IsRestarting)
    {
        bool AllFiguresReturned = true;
        for(u32 i = 0; i < FigureAmount; ++i)
        {
            v2 Position = FigureUnit[i].Position;
            v2 TargetPosition = FigureUnit[i].HomePosition;
            
            if(!FigureUnit[i].IsIdle)
            {
                AllFiguresReturned = false;
                
                v2 dt = MoveObject2D(Position, TargetPosition, Input->dtForFrame, FigureEntity->FigureVelocity);
                
                Position = FigureUnitMove(&FigureUnit[i], dt);
                if(Position == TargetPosition)
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
    
    /* Figure returning to the idle zone */
    if(FigureEntity->IsReturning)
    {
        u32 ReturnIndex = FigureEntity->ReturnIndex;
        
        v2 Position = FigureUnit[ReturnIndex].Position;
        v2 TargetPosition = FigureUnit[ReturnIndex].HomePosition;
        
        v2 dt = MoveObject2D(Position, TargetPosition, Input->dtForFrame, FigureEntity->FigureVelocity);
        Position = FigureUnitMove(&FigureUnit[ReturnIndex], dt);
        
        if (Position == TargetPosition)
        {
            FigureEntity->IsReturning = false;
            FigureEntity->ReturnIndex = -1;
        }
    }
    
    /* Rotation Animation */
    if(FigureEntity->IsRotating)
    {
        r32 AngleDt = Input->dtForFrame * LevelEntity->FigureEntity.RotationVelocity;
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
            
            if(Mix_PlayChannel(1, LevelEntity->RotateSound, 0)==-1) {
                printf("Mix_PlayChannel: %s\n", Mix_GetError());
            }
        }
    }
    
    /* Figure Rendering */
    FigureEntityRenderFigures(FigureEntity, RenderGroup);
    
    /* UI Rendering */
    {
        rectangle2 BorderRectangle = {};
        v2 TextureDim = QueryTextureDim(LevelEntity->CornerLeftTopTexture);
        v2 RectangleDim = {408.0f, 408.0f};
        v2 OffsetFromWall = {20.0f, 20.0f};
        
        /* Top Left */
        {
            BorderRectangle = {OffsetFromWall};
            SetDim(&BorderRectangle, RectangleDim);
            
            BorderRectangle += V2(5.0f, 5.0f);
            PushBitmap(RenderGroup, LevelEntity->CornerLeftTopShadowTexture, BorderRectangle);
            
            BorderRectangle -= V2(5.0f, 5.0f);
            PushBitmap(RenderGroup, LevelEntity->CornerLeftTopTexture, BorderRectangle);
        }
        
        /* Top Right */
        {
            rectangle2 Rectangle = BorderRectangle;
            Rectangle.Min += V2(VIRTUAL_GAME_WIDTH - 20.0f - 20.0f - 308.0f, 0.0f);
            Rectangle.Max += V2(VIRTUAL_GAME_WIDTH - 20.0f - 20.0f - 308.0f, 0.0f);
            
            Rectangle += V2(5.0f, 5.0f);
            PushBitmapEx(RenderGroup, LevelEntity->CornerLeftTopShadowTexture, Rectangle, 90.0f, V2(154.0f, 154.0f), SDL_FLIP_NONE);
            
            Rectangle -= V2(5.0f, 5.0f);
            PushBitmapEx(RenderGroup, LevelEntity->CornerLeftTopTexture, Rectangle, 90.0f, V2(154.0f, 154.0f), SDL_FLIP_NONE);
        }
        
        /* Bottom Left */
        {
            rectangle2 Rectangle = BorderRectangle;
            Rectangle.Min += V2(0.0f, VIRTUAL_GAME_HEIGHT - 20.0f - 20.0f - 308.0f);
            Rectangle.Max += V2(0.0f, VIRTUAL_GAME_HEIGHT - 20.0f - 20.0f - 308.0f);
            
            Rectangle += V2(5.0f, 5.0f);
            PushBitmapEx(RenderGroup, LevelEntity->CornerLeftTopShadowTexture, Rectangle, -90.0f, V2(154.0f, 154.0f), SDL_FLIP_NONE);
            
            Rectangle -= V2(5.0f, 5.0f);
            PushBitmapEx(RenderGroup, LevelEntity->CornerLeftTopTexture, Rectangle, -90.0f, V2(154.0f, 154.0f), SDL_FLIP_NONE);
        }
        
        /* Bottom Right */
        {
            rectangle2 Rectangle = BorderRectangle;
            Rectangle.Min += V2(VIRTUAL_GAME_WIDTH - 20.0f - 20.0f - 308.0f, VIRTUAL_GAME_HEIGHT - 20.0f - 20.0f - 308.0f);
            Rectangle.Max += V2(VIRTUAL_GAME_WIDTH - 20.0f - 20.0f - 308.0f, VIRTUAL_GAME_HEIGHT - 20.0f - 20.0f - 308.0f);
            
            Rectangle += V2(5.0f, 5.0f);
            PushBitmapEx(RenderGroup, LevelEntity->CornerLeftTopShadowTexture, Rectangle, 180.0f, V2(154.0f, 154.0f), SDL_FLIP_NONE);
            
            Rectangle -= V2(5.0f, 5.0f);
            PushBitmapEx(RenderGroup, LevelEntity->CornerLeftTopTexture, Rectangle, 180.0f, V2(154.0f, 154.0f), SDL_FLIP_NONE);
        }
        
        /* Vertical Border*/
        {
            v2 Dim = QueryTextureDim(LevelEntity->VerticalBorderTexture);
            rectangle2 Rectangle = {{1200.0f, 100.0f}, {}};
            SetDim(&Rectangle, Dim);
            PushBitmap(RenderGroup, LevelEntity->VerticalBorderTexture, Rectangle);
        }
        
        {
            /* Playground Options Gear Button */
            playground_options *Options = &LevelEntity->Options;
            
            if (LevelEntity->GearIsRotating) {
                
                Options->InterpPoint = Options->ToggleMenu ? (Options->InterpPoint + Input->dtForFrame * 10.0f)
                    : (Options->InterpPoint - Input->dtForFrame * 10.0f);
                
                r32 GearAngle = Lerp1(0.0f, 90.0f, Options->InterpPoint);
                if (Options->ToggleMenu && GearAngle >= 90.0f) {
                    GearAngle = 90.0f;
                    LevelEntity->GearIsRotating = false;
                }
                if (!Options->ToggleMenu && GearAngle <= 0.0f) {
                    GearAngle = 0.0f;
                    LevelEntity->GearIsRotating = false;
                }
                
                LevelEntity->GearAngle = GearAngle;
            }
            else if (Options->ToggleMenu) {
                
                v2 ButtonRectangleDim = Options->ButtonDimension;
                v2 MenuPosition       = Options->MenuPosition;
                v2 TextureDim         = {};
                
                rectangle2 ButtonRectangle = {};
                rectangle2 TextRectangle = {};
                rectangle2 TextShadowRectangle = {};
                
                for (u32 Index = 0;
                     Index < 4;
                     ++Index)
                {
                    ButtonRectangle.Min.x = MenuPosition.x;
                    ButtonRectangle.Min.y = MenuPosition.y + (ButtonRectangleDim.h * Index);
                    SetDim(&ButtonRectangle, ButtonRectangleDim);
                    
                    TextureDim = QueryTextureDim(LevelEntity->Options.MenuTexture[Index]);
                    TextRectangle.Min.x = ButtonRectangle.Min.x + (ButtonRectangleDim.w / 2.0f);
                    TextRectangle.Min.y = ButtonRectangle.Min.y + (ButtonRectangleDim.h / 2.0f);
                    SetDim(&TextRectangle, TextureDim);
                    
                    TextShadowRectangle.Min = TextRectangle.Min + 5.0f;
                    SetDim(&TextShadowRectangle, TextureDim);
                    
                    PushFontBitmap(RenderGroup, Options->MenuShadowTexture[Index], TextShadowRectangle);
                    PushFontBitmap(RenderGroup, Options->MenuTexture[Index], TextRectangle);
                }
                
                if (Options->Choice != options_choice::NONE_OPTION)
                {
                    TextureDim = QueryTextureDim(Options->HorizontalLineTexture);
                    u32 ButtonIndex  = (u32)Options->Choice;
                    
                    v2 LineDim = {200.0f, 33.0f};
                    TextRectangle.Min.x = MenuPosition.x + (GetDim(ButtonRectangle).w / 2.0f) - (LineDim.w / 2.0f);
                    TextRectangle.Min.y = MenuPosition.y + (ButtonRectangleDim.h * (ButtonIndex + 1)) - (LineDim.h * 0.8f);
                    SetDim(&TextRectangle, LineDim);
                    PushBitmap(RenderGroup, Options->HorizontalLineTexture, TextRectangle);
                }
                
            }
            
            rectangle2 GearRectangle = {};
            GearRectangle.Min.x = VIRTUAL_GAME_WIDTH  - 200.0f + 8.0f;
            GearRectangle.Min.y = VIRTUAL_GAME_HEIGHT - 200.0f + 8.0f;
            SetDim(&GearRectangle, 100.0f, 100.0f);
            
            // Gear Shadow Texture
            PushBitmapEx(RenderGroup, Options->GearShadowTexture, GearRectangle, LevelEntity->GearAngle, V2(50.0f, 50.0f), SDL_FLIP_NONE);
            
            GearRectangle.Min.x = VIRTUAL_GAME_WIDTH  - 200.0f;
            GearRectangle.Min.y = VIRTUAL_GAME_HEIGHT - 200.0f;
            SetDim(&GearRectangle, 100.0f, 100.0f);
            
            // Gear Texture
            PushBitmapEx(RenderGroup, Options->GearTexture, GearRectangle, LevelEntity->GearAngle, V2(50.0f, 50.0f), SDL_FLIP_NONE);
        }
        
        /* Level Indicator Bar */ 
        v2 IndicatorSize = {30.0f, 30.0f};
        u32 IndicatorAmount = 8;
        s32 CurrentLevelIndicator = (LevelEntity->LevelNumber - 1) % IndicatorAmount;
        for (s32 Index = 0; 
             Index < IndicatorAmount;
             ++Index)
        {
            rectangle2 Rectangle = {};
            Rectangle.Min.x = (VIRTUAL_GAME_WIDTH * 0.5f) - (IndicatorSize.w * (IndicatorAmount / 2)) - ((IndicatorSize.w) * (IndicatorAmount / 2));
            Rectangle.Min.x += (Index * IndicatorSize.w) + ((IndicatorSize.w) * Index);
            Rectangle.Min.y = VIRTUAL_GAME_HEIGHT - (70.0f);
            SetDim(&Rectangle, IndicatorSize);
            
            if (Index == CurrentLevelIndicator)
                PushBitmap(RenderGroup, LevelEntity->IndicatorEmptyTexture, Rectangle);
            else 
                PushBitmap(RenderGroup, LevelEntity->IndicatorFilledTexture, Rectangle);
        }
    }
    
    return (Result);
}


static playground_data
ReadPlaygroundData(playground_data *Playground, u32 Index)
{
    Assert(Index < PLAYGROUND_MAXIMUM);
    
    playground_data Result = {};
    Result.LevelNumber = Playground[Index].LevelNumber;
    Result.RowAmount = Playground[Index].RowAmount;
    Result.ColumnAmount = Playground[Index].ColumnAmount;
    Result.FigureAmount = Playground[Index].FigureAmount;
    
    for(u32 FigureIndex = 0;
        FigureIndex < Result.FigureAmount;
        ++FigureIndex)
    {
        Result.Figures[FigureIndex] = Playground[Index].Figures[FigureIndex];
    }
    
    return (Result);
}

static void
WritePlaygroundData(playground_data *Playground, playground *Entity, u32 Index)
{
    Assert(Index < PLAYGROUND_MAXIMUM);
    
    Playground[Index].LevelNumber = Entity->LevelNumber;
    Playground[Index].RowAmount = Entity->GridEntity.RowAmount;
    Playground[Index].ColumnAmount = Entity->GridEntity.ColumnAmount;
    Playground[Index].FigureAmount = Entity->FigureEntity.FigureAmount;
    
    s32 *UnitFieldSource = Entity->GridEntity.UnitField;
    for (u32 Row = 0; 
         Row < Playground[Index].RowAmount;
         ++Row)
    {
        for (u32 Column = 0; 
             Column < Playground[Index].ColumnAmount;
             ++Column)
        {
            s32 UnitIndex = (Row * Playground[Index].ColumnAmount) + Column;
        }
    }
    
    
    figure_data *Figures = Playground[Index].Figures;
    for (u32 FigureIndex = 0;
         FigureIndex < Playground[Index].FigureAmount;
         ++FigureIndex)
    {
        Figures[FigureIndex].Angle = Entity->FigureEntity.FigureUnit[FigureIndex].Angle;
        Figures[FigureIndex].Flip  = Entity->FigureEntity.FigureUnit[FigureIndex].Flip;
        Figures[FigureIndex].Form  = Entity->FigureEntity.FigureUnit[FigureIndex].Form;
    }
}

static void
PrepareNextPlayground(playground *Playground, playground_config *Configuration, playground_data *Data, u32 Index)
{
    Assert(Index >= 0 || Index < PLAYGROUND_MAXIMUM);
    
    for (int i = 0; i < Playground->FigureEntity.FigureAmount; ++i) {
        figure_unit *FigureUnit = &Playground->FigureEntity.FigureUnit[i];
        game_texture *Texture = PickFigureTexture(&Playground->FigureEntity, FigureUnit->Form);
        game_texture *ShadowTexture = PickFigureShadowTexture(FigureUnit->Form, &Playground->FigureEntity);
        
        SDL_SetTextureAlphaMod(Texture, 255.0f);
        SDL_SetTextureAlphaMod(ShadowTexture, 255.0f);
    }
    
    playground_data PlaygroundData = ReadPlaygroundData(Data, Index);
    
    Playground->LevelStarted  = true;
    Playground->LevelFinished = false;
    Playground->LevelNumber  = Index + 1;
    Playground->TimeElapsed  = 0.0f;
    Playground->ShowTimer    = false;
    
    Playground->IsStartup = true;
    Playground->GridInterpPoint = 0.0f;
    Playground->FInterpPoint = 0.0f;
    Playground->FAlphaInterPoint  = 1.0f;
    Playground->Animation.Finished = false;
    Playground->Animation.InterpPoint = 0.0f;
    Playground->Animation.TimeMax = 1.0f;
    
    Playground->GearAngle = 0.0f;
    Playground->GearIsRotating = false;
    Playground->Options.ToggleMenu  = false;
    Playground->Options.InterpPoint = 0.0f;
    
    Playground->GridEntity.RowAmount          = PlaygroundData.RowAmount;
    Playground->GridEntity.ColumnAmount       = PlaygroundData.ColumnAmount;
    Playground->GridEntity.StickUnitsAmount   = PlaygroundData.FigureAmount;
    
    Playground->GridEntity.GridArea.Min.x = 100;
    Playground->GridEntity.GridArea.Min.y = 81;
    Playground->GridEntity.GridArea.Max.x = Playground->GridEntity.GridArea.Min.x + 1128;
    Playground->GridEntity.GridArea.Max.y = Playground->GridEntity.GridArea.Min.y + 930;
    
    s32 *UnitFieldTarget = Playground->GridEntity.UnitField;
    for (u32 Row = 0; 
         Row < ROW_AMOUNT_MAXIMUM;
         ++Row)
    {
        for (u32 Column = 0; 
             Column < COLUMN_AMOUNT_MAXIMUM;
             ++Column)
        {
            s32 UnitIndex = (Row * COLUMN_AMOUNT_MAXIMUM) + Column;
            UnitFieldTarget[UnitIndex] = 0;
        }
    }
    
    for (u32 i = 0; i < FIGURE_AMOUNT_MAXIMUM; ++i)
    {
        Playground->FInterpStop[i] = false;
        
        Playground->GridEntity.StickUnits[i].Index = -1;
        Playground->GridEntity.StickUnits[i].IsSticked = false;
    }
    
    u32 CurrentType = (Index % 32) / 8;
    Playground->FigureEntity.CurrentType = (figure_type)CurrentType;
    Playground->FigureEntity.FigureAmount     = PlaygroundData.FigureAmount;
    Playground->FigureEntity.ReturnIndex      = -1;
    Playground->FigureEntity.FigureActive     = -1;
    Playground->FigureEntity.FigureOutline    = -1;
    Playground->FigureEntity.FigureVelocity   = Configuration->FigureVelocity;
    Playground->FigureEntity.RotationVelocity = Configuration->RotationVel;
    Playground->FigureEntity.FigureArea.Min.x = 1300;
    Playground->FigureEntity.FigureArea.Min.y = 81;
    Playground->FigureEntity.FigureArea.Max.x = Playground->FigureEntity.FigureArea.Min.x + 552;
    Playground->FigureEntity.FigureArea.Max.y = Playground->FigureEntity.FigureArea.Min.y + 930;
    
    
    figure_unit *FigureUnits = Playground->FigureEntity.FigureUnit;
    
    // NOTE(msokolov): just for clearing fields that may be changed during animation and other stuff
    for(u32 Index = 0; Index < FIGURE_AMOUNT_MAXIMUM; ++Index) {
        Playground->FigureEntity.FigureOrder[Index] = Index;
        
        FigureUnits[Index].Angle      = 0.0f;
        FigureUnits[Index].IsIdle     = true;
        FigureUnits[Index].IsStick    = false;
        FigureUnits[Index].IsEnlarged = false;
    }
    
    for (u32 FigureIndex = 0;
         FigureIndex < PlaygroundData.FigureAmount;
         ++FigureIndex)
    {
        FigureUnits[FigureIndex] = {};
        
        figure_form Form = PlaygroundData.Figures[FigureIndex].Form;
        figure_type Type = PlaygroundData.Figures[FigureIndex].Type;
        
        Playground->FigureEntity.FigureUnit[FigureIndex].Angle = PlaygroundData.Figures[FigureIndex].Angle;
        FigureUnitInitFigure(&Playground->FigureEntity.FigureUnit[FigureIndex], Form);
    }
    
    FigureEntityAlignFigures(&Playground->FigureEntity);
    
    // For animation
    for (int i = 0; i < Playground->FigureEntity.FigureAmount; ++i) 
        Playground->AnimFigureDim[i] = FigureUnits[i].Size;
    
}

