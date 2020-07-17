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
FigureUnitInitFigure(figure_unit *FigureUnit, figure_form Form,
                     figure_type Type)
{
    FigureUnit->IsIdle = true;
    FigureUnit->Form   = Form;
    FigureUnit->Type   = Type;
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
FigureUnitAddNewFigure(figure_entity *FigureEntity, figure_form Form, figure_type Type)

{
    Assert(FigureEntity->FigureAmount < FIGURE_AMOUNT_MAXIMUM);
    
    u32 Index = FigureEntity->FigureAmount;
    FigureEntity->FigureAmount += 1;
    
    FigureUnitInitFigure(&FigureEntity->FigureUnit[Index], Form, Type);
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

static void
FigureUnitMove(figure_unit *Entity, v2 dt)
{
    Entity->Position += dt;
    
    for (u32 Index = 0;
         Index < FIGURE_BLOCKS_MAXIMUM;
         Index++)
    {
        Entity->Shell[Index] += dt;
    }
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


static game_texture*
PickFigureShadowTexture(figure_form Form, figure_entity *FigureEntity)
{
    game_texture *Result = NULL;
    
    switch(Form)
    {
        case O_figure:
        {
            Result = FigureEntity->O_ShadowTexture;
        } break;
        
        case I_figure:
        {
            Result = FigureEntity->I_ShadowTexture;
        } break;
        
        case L_figure:
        {
            Result = FigureEntity->L_ShadowTexture;
        } break;
        
        case J_figure:
        {
            Result = FigureEntity->J_ShadowTexture;
        } break;
        
        case Z_figure:
        {
            Result = FigureEntity->Z_ShadowTexture;
        } break;
        
        case S_figure:
        {
            Result = FigureEntity->S_ShadowTexture;
        } break;
        
        case T_figure:
        {
            Result = FigureEntity->T_ShadowTexture;
        } break;
    }
    
    
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
    
    r32 Magnitude = Square(Offset);
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
            }
            
            if(!FigureEntity->IsGrabbed)
            {
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
                        
                        //return ShouldExit;
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
                            } break;
                            case QUIT_OPTION:
                            {
                                Result = options_choice::QUIT_OPTION;
                            } break;
                        }
                        
                        ButtonWasPressed = true;
                        ToggleMenuOption = false;
                        LevelEntity->Options.Choice = OptionChoice;
                    }
                }
            }
        }
        
        if (Input->MouseButtons[0].EndedDown && !ButtonWasPressed)
        {
            ToggleMenuOption = false;
        }
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
        Rectangle.Min.x = Entity->Shell[Index].x - (BlockSize / 2.0f);
        Rectangle.Min.y = Entity->Shell[Index].y - (BlockSize / 2.0f);
        Rectangle.Max.x = Rectangle.Min.x + BlockSize;
        Rectangle.Max.y = Rectangle.Min.y + BlockSize;
        PushRectangle(RenderGroup, Rectangle, Color);
        
        v2 Center = {};
        Center.x  = Entity->Position.x + (Entity->Size.w / 2);
        Center.y  = Entity->Position.y + (Entity->Size.h) * Entity->CenterOffset;
        
        Rectangle.Min.x = Center.x - (BlockSize / 2.0f);
        Rectangle.Min.y = Center.y - (BlockSize / 2.0f);
        Rectangle.Max.x = Rectangle.Min.x + BlockSize;
        Rectangle.Max.y = Rectangle.Min.y + BlockSize;
        PushRectangle(RenderGroup, Rectangle, {255, 0, 0, 255});
    }
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
    
    r32 TimeElapsed  = Input->TimeElapsedMs;
    r32 MaxVel       = GridBlockSize / 6.0f;
    u32 RowAmount    = GridEntity->RowAmount;
    u32 ColumnAmount = GridEntity->ColumnAmount;
    u32 FigureAmount = FigureEntity->FigureAmount;
    s32 ActiveIndex  = FigureEntity->FigureActive;
    
    r32 ActualGridWidth  = ColumnAmount * GridBlockSize;
    r32 ActualGridHeight = RowAmount * GridBlockSize;
    
    v2 GridAreaDimension = GetDim(GridEntity->GridArea);
    
    rectangle2 GridArea = GridEntity->GridArea;
    GridArea.Min.x = GridArea.Min.x + (GridAreaDimension.w / 2.0f) - (ActualGridWidth / 2.0f);
    GridArea.Min.y = GridArea.Min.y + (GridAreaDimension.h / 2.0f) - (ActualGridHeight / 2.0f);
    SetDim(&GridArea, ActualGridWidth, ActualGridHeight);
    
    bool ToggleHighlight = false;
    s32 StartX = 0;
    s32 StartY = 0;
    
    /* game_input checking */
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
    
    
    // TODO(msokolov): this should be a texture
    //Clear(RenderGroup, {42, 6, 21, 255});
    //Clear(RenderGroup, {51, 14, 5, 255});
    Clear(RenderGroup, {51, 8, 23, 255});
    
    //PushBitmap(RenderGroup, LevelEntity->BackgroundTexture, ScreenRectangle);
    
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
                                if(Count == 0)
                                {
                                    OffsetX = Rect.Min.x + (GridBlockSize / 2.0f) - FigureUnit[FigureIndex].Shell[l].x;
                                    OffsetY = Rect.Min.y + (GridBlockSize / 2.0f) - FigureUnit[FigureIndex].Shell[l].y;
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
                        
                        u32 StickSize = GridEntity->StickUnitsAmount;
                        
                        v2 FigureCenter = {};
                        FigureCenter.x   = FigureUnit[FigureIndex].Position.x + (FigureUnit[FigureIndex].Size.w / 2.0f);
                        FigureCenter.y   = FigureUnit[FigureIndex].Position.y + (FigureUnit[FigureIndex].Size.h) * FigureUnit[FigureIndex].CenterOffset;
                        
                        for (u32 i = 0; i < StickSize; ++i)
                        {
                            if(GridEntity->StickUnits[i].Index == -1)
                            {
                                GridEntity->StickUnits[i].Index     = FigureIndex;
                                GridEntity->StickUnits[i].Center.x  = FigureCenter.x + OffsetX;
                                GridEntity->StickUnits[i].Center.y  = FigureCenter.y + OffsetY;
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
            v2 FigureCenter = {};
            FigureCenter.x = FigureUnit[Index].Position.x + (FigureUnit[Index].Size.w / 2.0f);
            FigureCenter.y = FigureUnit[Index].Position.y + (FigureUnit[Index].Size.h) * FigureUnit[Index].CenterOffset;
            
            v2 TargetCenter = GridEntity->StickUnits[i].Center;
            v2 dt = TargetCenter - FigureCenter;
            
            r32 Distance = Square(dt);
            r32 DeadZone = 5.0f;
            if (Distance > DeadZone)
            {
                dt  = Normalize(dt);
                dt *= (FigureEntity->FigureVelocity * Input->dtForFrame);
            }
            
            FigureUnitMove(&FigureUnit[Index], dt);
            
            FigureCenter += dt;
            if((FigureCenter.x == TargetCenter.x) && (FigureCenter.y == TargetCenter.y))
            {
                GridEntity->StickUnits[i].IsSticked = true;
                
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
        /* Level is completed */
        // TODO(msokolov): also check if there is any figures left??
        LevelEntity->LevelFinished = true;
        Result = playground_status::LEVEL_FINISHED;
        printf("level is completed\n");
        //LevelEntityFinishAnimationInit(LevelEntity, Memory, Buffer);
    }
    
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
                TextureRectangle.Min.x = GridCellRectangle.Min.x - (25.0f / 2.0f);
                TextureRectangle.Min.y = GridCellRectangle.Min.y - (25.0f / 2.0f);
                SetDim(&TextureRectangle, GetDim(GridCellRectangle).w + 25.0f, GetDim(GridCellRectangle).h + 25.0f);
                PushBitmap(RenderGroup, GridEntity->GridCell2Texture, TextureRectangle);
                
                //PushRectangle(RenderGroup, GridCellRectangle, {255, 255, 255, 50});
                //PushRectangleOutline(RenderGroup, GridCellRectangle, {255, 255, 255, 255});
            }
        }
    }
    
    CellCounter = 0;
    GridCellRectangle = {};
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
                TextureRectangle.Min.x = GridCellRectangle.Min.x - (25.0f / 2.0f);
                TextureRectangle.Min.y = GridCellRectangle.Min.y - (25.0f / 2.0f);
                SetDim(&TextureRectangle, GetDim(GridCellRectangle).w + 25.0f, GetDim(GridCellRectangle).h + 25.0f);
                PushBitmap(RenderGroup, GridEntity->GridCell1Texture, TextureRectangle);
                
                //PushRectangle(RenderGroup, GridCellRectangle, {255, 255, 255, 50});
                //PushRectangleOutline(RenderGroup, GridCellRectangle, {255, 255, 255, 255});
            }
        }
    }
    
    
    
#if 0
    rectangle2 FrameRectangle = {};
    r32 FrameSize = GridBlockSize / 2.0f;
    r32 SpaceOverFrame = GridBlockSize / 4.0f;
    
    // Top left corner
    FrameRectangle.Min.x = GridArea.Min.x - SpaceOverFrame;
    FrameRectangle.Min.y = GridArea.Min.y - SpaceOverFrame;
    SetDim(&FrameRectangle, FrameSize, FrameSize);
    PushBitmap(RenderGroup, GridEntity->TopLeftCornerFrame, FrameRectangle);
    
    // Top right corner
    FrameRectangle.Min.x = GridArea.Min.x - SpaceOverFrame + ActualGridWidth;
    FrameRectangle.Min.y = GridArea.Min.y - SpaceOverFrame;
    SetDim(&FrameRectangle, FrameSize, FrameSize);
    PushBitmap(RenderGroup, GridEntity->TopRightCornerFrame, FrameRectangle);
    
    // Down left corner
    FrameRectangle.Min.x = GridArea.Min.x - SpaceOverFrame;
    FrameRectangle.Min.y = GridArea.Min.y - SpaceOverFrame + ActualGridHeight;
    SetDim(&FrameRectangle, FrameSize, FrameSize);
    PushBitmap(RenderGroup, GridEntity->DownLeftCornerFrame, FrameRectangle);
    
    // Down right corner
    FrameRectangle.Min.x = GridArea.Min.x - SpaceOverFrame + ActualGridWidth;
    FrameRectangle.Min.y = GridArea.Min.y - SpaceOverFrame + ActualGridHeight;
    SetDim(&FrameRectangle, FrameSize, FrameSize);
    PushBitmap(RenderGroup, GridEntity->DownRightCornerFrame, FrameRectangle);
#endif
    
    /* MovingBlocks Update and Rendering */
    moving_block *MovingBlocks = GridEntity->MovingBlocks;
    for(u32 i = 0; i < GridEntity->MovingBlocksAmount; ++i)
    {
        if(GridEntity->MovingBlocks[i].IsMoving)
        {
            s32 RowNumber = MovingBlocks[i].RowNumber;
            s32 ColNumber = MovingBlocks[i].ColNumber;
            v2 MovingBlockDim = MovingBlocks[i].Area.Max - MovingBlocks[i].Area.Min;
            
            v2 Position = MovingBlocks[i].Area.Min;
            v2 Destination = {};
            Destination.x = GridArea.Min.x + (ColNumber * GridBlockSize);
            Destination.y = GridArea.Min.y + (RowNumber * GridBlockSize);
            
            v2 dt = Destination - Position;
            r32 Distance = Square(dt);
            r32 DeadZone = 5.0f;
            if (Distance > DeadZone)
            {
                dt  = Normalize(dt);
                dt *= (GridEntity->MovingBlockVelocity * Input->dtForFrame);
            }
            
            Position += dt;
            MovingBlocks[i].Area.Min = Position;
            MovingBlocks[i].Area.Max = MovingBlocks[i].Area.Min + MovingBlockDim;
            
            if((Position.x == Destination.x) && (Position.y == Destination.y))
            {
                MovingBlocks[i].IsMoving = false;
                GridEntity->UnitField[(RowNumber * ColumnAmount) + ColNumber] = 1;
            }
        }
        
        if(GridEntity->MovingBlocks[i].IsVertical)
        {
            PushBitmap(RenderGroup, GridEntity->VerticalSquareTexture, GridEntity->MovingBlocks[i].Area);
        }
        else
        {
            PushBitmap(RenderGroup, GridEntity->HorizontlaSquareTexture, GridEntity->MovingBlocks[i].Area);
        }
    }
    
    /* FigureEntity Update and Rendering */
    /* Figure Area Highlight */
    if(FigureEntity->IsRestarting)
    {
        bool AllFiguresReturned = true;
        for(u32 i = 0; i < FigureAmount; ++i)
        {
            v2 Position = FigureUnit[i].Position;
            v2 HomePosition = FigureUnit[i].HomePosition;
            
            if(!FigureUnit[i].IsIdle)
            {
                AllFiguresReturned = false;
                
                v2 dt = HomePosition - Position;
                r32 Distance = Square(dt);
                r32 DeadZone = 5.0f;
                
                if (Distance > DeadZone)
                {
                    dt  = Normalize(dt);
                    dt *= (FigureEntity->FigureVelocity * Input->dtForFrame);
                }
                
                FigureUnitMove(&FigureUnit[i], dt);
                
                Position += dt;
                if((Position.x == HomePosition.x) && (Position.y == HomePosition.y))
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
        rectangle2 ScreenArea = {};
        ScreenArea.Max.w = RenderGroup->Width;
        ScreenArea.Max.h = RenderGroup->Height;
        
        v2 Center = {};
        Center.x   = FigureUnit[ActiveIndex].Position.x + (FigureUnit[ActiveIndex].Position.w / 2);
        Center.y   = FigureUnit[ActiveIndex].Position.y + (FigureUnit[ActiveIndex].Size.h) * FigureUnit[ActiveIndex].CenterOffset;
        
        ShouldHighlight = 
            IsInRectangle(FigureUnit[ActiveIndex].Shell, FIGURE_BLOCKS_MAXIMUM, FigureEntity->FigureArea) ||
            IsInRectangle(Center, ScreenArea); 
    }
    
    if(ShouldHighlight) 
    {
        Change1DUnitPerSec(&FigureEntity->AreaAlpha, 255, LevelEntity->FigureEntity.FlippingVelocity, Input->dtForFrame);
        
    }
    else
    {
        Change1DUnitPerSec(&FigureEntity->AreaAlpha, 0, LevelEntity->FigureEntity.FlippingVelocity, Input->dtForFrame);
    }
    
    if(FigureEntity->AreaAlpha != 0) 
    {
        ToggleHighlight = true;
    }
    
    if(ToggleHighlight)
    {
        v4 HightlightColor = {255, 255, 255, 255};
        
        rectangle2 HighlightRect = FigureEntity->FigureArea;
        
        u32 RectThickness = InActiveBlockSize / 6;
        for(u32 i = 0; i < RectThickness; i++)
        {
            PushRectangleOutline(RenderGroup, HighlightRect, HightlightColor);
            
            HighlightRect.Min.x += 1;
            HighlightRect.Min.y += 1;
            HighlightRect.Max.x -= 1;
            HighlightRect.Max.y -= 1;
        }
    }
    
    /* Figure returning to the idle zone */
    if(FigureEntity->IsReturning)
    {
        u32 ReturnIndex = FigureEntity->ReturnIndex;
        
        v2 Position = FigureUnit[ReturnIndex].Position;
        v2 Destination = FigureUnit[ReturnIndex].HomePosition;
        
        v2 dt = Destination - Position;
        r32 Distance = Square(dt);
        r32 DeadZone = 5.0f;
        if (Distance > DeadZone)
        {
            dt  = Normalize(dt);
            dt *= (FigureEntity->FigureVelocity * Input->dtForFrame);
        }
        
        Position += dt;
        FigureUnitMove(&FigureUnit[ReturnIndex], dt);
        
        if ((Position.x == Destination.x) && (Position.y == Destination.y))
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
        }
    }
    
    /* Flipping Animation */
    if(FigureEntity->IsFlipping)
    {
        if(FigureEntity->FadeInSum > 0)
        {
            if(Change1DUnitPerSec(&FigureEntity->FigureAlpha, 0, 10, TimeElapsed))
            {
                FigureEntity->FigureAlpha = 0;
                FigureEntity->FadeInSum   = 0;
                FigureUnitFlipHorizontally(&FigureUnit[ActiveIndex]);
            }
        }
        else if(FigureEntity->FadeOutSum < 255)
        {
            if(Change1DUnitPerSec(&FigureEntity->FigureAlpha, 255, 10, TimeElapsed))
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
        
        v2 ShadowOffset = {};
        if (!Entity->IsStick)
        {
            ShadowOffset += 8.0f;
        }
        
        rectangle2 Rectangle = {};
        Rectangle.Min.x = Entity->Position.x;
        Rectangle.Min.y = Entity->Position.y;
        Rectangle.Max.w = Rectangle.Min.x + Entity->Size.w;
        Rectangle.Max.h = Rectangle.Min.y + Entity->Size.h;
        
        v2 Dim = QueryTextureDim(FigureEntity->O_ShadowTexture);
        
        rectangle2 ShadowRectangle = {};
        ShadowRectangle.Min = Rectangle.Min + ShadowOffset;
        ShadowRectangle.Max = Rectangle.Max + ShadowOffset;
        
        game_texture *Texture = PickFigureTexture(Entity->Form, Entity->Type, FigureEntity);
        game_texture *ShadowTexture = PickFigureShadowTexture(Entity->Form, FigureEntity);
        
        PushBitmapEx(RenderGroup, ShadowTexture, ShadowRectangle, Entity->Angle, Center, Entity->Flip);
        PushBitmapEx(RenderGroup, Texture, Rectangle, Entity->Angle, Center, Entity->Flip);
        
        //PushRectangleOutline(RenderGroup, Rectangle, {0, 0, 255, 255});
        //PushRectangleOutline(RenderGroup, ShadowRectangle, {0, 255, 0, 255});
        
        
        //RenderFigureStructure(RenderGroup, Entity);
        
        //rectangle2 AreaRect = FigureUnitGetArea(Entity);
        //PushRectangleOutline(RenderGroup, AreaRect, {0, 0, 0, 255});
    }
    
    //PushRectangleOutline(RenderGroup, GridEntity->GridArea, {255, 0, 255, 255});
    //PushRectangleOutline(RenderGroup, FigureEntity->FigureArea, {0, 255, 255, 255});
    
    {
        /* Corner Rendering */ 
        
        /* Top Left */
        rectangle2 BorderRectangle = {};
        BorderRectangle.Min.x = 20.0f; 
        BorderRectangle.Min.y = 20.0f;
        SetDim(&BorderRectangle, 410.0f, 410.0f);
        PushBitmap(RenderGroup, LevelEntity->CornerLeftTopTexture, BorderRectangle);
        //PushRectangleOutline(RenderGroup, BorderRectangle, {255, 255, 255, 255});
        
        /* Bottom Left */
        BorderRectangle.Min.x = 20.0f; 
        BorderRectangle.Min.y = VIRTUAL_GAME_HEIGHT - 410.0f - 20.0f;
        SetDim(&BorderRectangle, 410.0f, 410.0f);
        PushBitmap(RenderGroup, LevelEntity->CornerLeftBottomTexture, BorderRectangle);
        
        /* Top Right */
        BorderRectangle.Min.x = VIRTUAL_GAME_WIDTH  - 410.0f - 20.0f; 
        BorderRectangle.Min.y = 20.0f;
        SetDim(&BorderRectangle, 410.0f, 410.0f);
        PushBitmap(RenderGroup, LevelEntity->CornerRightTopTexture, BorderRectangle);
        
        /* Bottom Right */
        BorderRectangle.Min.x = VIRTUAL_GAME_WIDTH  - 410.0f - 20.0f; 
        BorderRectangle.Min.y = VIRTUAL_GAME_HEIGHT - 410.0f - 20.0f;
        SetDim(&BorderRectangle, 410.0f, 410.0f);
        PushBitmap(RenderGroup, LevelEntity->CornerRightBottomTexture, BorderRectangle);
        
        /* Vertical Border*/
        BorderRectangle.Min.x = 1200.0f; 
        BorderRectangle.Min.y = 100.0f;
        v2 Dim = QueryTextureDim(LevelEntity->VerticalBorderTexture);
        SetDim(&BorderRectangle, Dim.w, Dim.h);
        PushBitmap(RenderGroup, LevelEntity->VerticalBorderTexture, BorderRectangle);
        
        /* Level Number Rendering */
        Dim = QueryTextureDim(LevelEntity->LevelNumberTexture);
        PushBitmap(RenderGroup, LevelEntity->LevelNumberTexture, {0.0f, 0.0f, Dim.w, Dim.h});
    }
    
    {
        /* Playground Options */
        playground_options *Options = &LevelEntity->Options;
        
        rectangle2 GearRectangle = {};
        GearRectangle.Min.x = VIRTUAL_GAME_WIDTH  - 200.0f + 8.0f;
        GearRectangle.Min.y = VIRTUAL_GAME_HEIGHT - 200.0f + 8.0f;
        SetDim(&GearRectangle, 100.0f, 100.0f);
        PushBitmap(RenderGroup, Options->GearShadowTexture, GearRectangle);
        
        GearRectangle.Min.x = VIRTUAL_GAME_WIDTH  - 200.0f;
        GearRectangle.Min.y = VIRTUAL_GAME_HEIGHT - 200.0f;
        SetDim(&GearRectangle, 100.0f, 100.0f);
        PushBitmap(RenderGroup, Options->GearTexture, GearRectangle);
        
        if (Options->ToggleMenu)
        {
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
                
                TextureDim   = QueryTextureDim(LevelEntity->Options.MenuTexture[Index]);
                TextRectangle.Min.x = ButtonRectangle.Min.x + (ButtonRectangleDim.w / 2.0f) - (TextureDim.w / 2.0f);
                TextRectangle.Min.y = ButtonRectangle.Min.y + (ButtonRectangleDim.h / 2.0f) - (TextureDim.h / 2.0f);
                SetDim(&TextRectangle, TextureDim);
                //PushRectangleOutline(RenderGroup, TextRectangle, { 0, 255, 255, 255});
                //PushRectangleOutline(RenderGroup, ButtonRectangle, { 255, 255, 255, 255});
                
                TextShadowRectangle.Min = TextRectangle.Min + 5.0f;
                SetDim(&TextShadowRectangle, TextureDim);
                
                PushBitmap(RenderGroup, Options->MenuShadowTexture[Index], TextShadowRectangle);
                PushBitmap(RenderGroup, Options->MenuTexture[Index], TextRectangle);
            }
            
            if (Options->Choice != options_choice::NONE_OPTION)
            {
                TextureDim = QueryTextureDim(Options->HorizontalLineTexture);
                r32 Ratio  = TextureDim.w / TextureDim.h;
                
                u32 ButtonIndex  = (u32)Options->Choice;
                
                v2 LineDim = {200.0f, 33.0f};
                TextRectangle.Min.x = MenuPosition.x + (GetDim(ButtonRectangle).w / 2.0f) - (LineDim.w / 2.0f);
                TextRectangle.Min.y = MenuPosition.y + (ButtonRectangleDim.h * (ButtonIndex + 1)) - (LineDim.h * 0.8f);
                SetDim(&TextRectangle, LineDim);
                PushBitmap(RenderGroup, Options->HorizontalLineTexture, TextRectangle);
            }
        }
    }
    
    return (Result);
}


static playground_data
ReadPlaygroundData(playground_data *Playground, u32 Index)
{
    Assert(Index < PLAYGROUND_MAXIMUM);
    
    playground_data Result = {};
    Result.IsLocked = Playground[Index].IsLocked;
    Result.LevelNumber = Playground[Index].LevelNumber;
    Result.RowAmount = Playground[Index].RowAmount;
    Result.ColumnAmount = Playground[Index].ColumnAmount;
    Result.MovingBlocksAmount = Playground[Index].MovingBlocksAmount;
    Result.FigureAmount = Playground[Index].FigureAmount;
    
    for(u32 UnitIndex = 0; 
        UnitIndex < Result.RowAmount * Result.ColumnAmount;
        ++UnitIndex)
    {
        Result.UnitField[UnitIndex] = Playground[Index].UnitField[UnitIndex];
    }
    
    for(u32 BlockIndex = 0;
        BlockIndex < Result.MovingBlocksAmount;
        ++BlockIndex)
    {
        Result.MovingBlocks[BlockIndex] = Playground[Index].MovingBlocks[BlockIndex];
    }
    
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
    
    Playground[Index].IsLocked = 0;
    Playground[Index].LevelNumber = Entity->LevelNumber;
    Playground[Index].RowAmount = Entity->GridEntity.RowAmount;
    Playground[Index].ColumnAmount = Entity->GridEntity.ColumnAmount;
    Playground[Index].MovingBlocksAmount = Entity->GridEntity.MovingBlocksAmount;
    Playground[Index].FigureAmount = Entity->FigureEntity.FigureAmount;
    
    s32 *UnitFieldSource = Entity->GridEntity.UnitField;
    s32 *UnitFieldTarget = Playground->UnitField;
    for (u32 Row = 0; 
         Row < Playground[Index].RowAmount;
         ++Row)
    {
        for (u32 Column = 0; 
             Column < Playground[Index].ColumnAmount;
             ++Column)
        {
            s32 UnitIndex = (Row * Playground[Index].ColumnAmount) + Column;
            UnitFieldTarget[UnitIndex] = UnitFieldSource[UnitIndex];
        }
    }
    
    moving_block_data *MovingBlocks = Playground[Index].MovingBlocks;
    
    for (u32 BlockIndex = 0;
         BlockIndex < Playground[Index].MovingBlocksAmount;
         ++BlockIndex)
    {
        MovingBlocks[BlockIndex].RowNumber = Entity->GridEntity.MovingBlocks[BlockIndex].RowNumber;
        MovingBlocks[BlockIndex].ColNumber = Entity->GridEntity.MovingBlocks[BlockIndex].ColNumber;
        MovingBlocks[BlockIndex].IsVertical = Entity->GridEntity.MovingBlocks[BlockIndex].IsVertical;
        MovingBlocks[BlockIndex].MoveSwitch = Entity->GridEntity.MovingBlocks[BlockIndex].MoveSwitch;
        
    }
    
    figure_data *Figures = Playground[Index].Figures;
    for (u32 FigureIndex = 0;
         FigureIndex < Playground[Index].FigureAmount;
         ++FigureIndex)
    {
        Figures[FigureIndex].Angle = Entity->FigureEntity.FigureUnit[FigureIndex].Angle;
        Figures[FigureIndex].Flip  = Entity->FigureEntity.FigureUnit[FigureIndex].Flip;
        Figures[FigureIndex].Form  = Entity->FigureEntity.FigureUnit[FigureIndex].Form;
        Figures[FigureIndex].Type  = Entity->FigureEntity.FigureUnit[FigureIndex].Type;
    }
}

static void
PrepareNextPlayground(playground *Playground, playground_config *Configuration, playground_data *Data, u32 Index)
{
    Assert(Index >= 0 || Index < PLAYGROUND_MAXIMUM);
    
    playground_data PlaygroundData = ReadPlaygroundData(Data, Index);
    
    //*Playground  = {};
    Playground->LevelStarted = true;
    Playground->LevelNumber  = Index + 1;
    
    Playground->GridEntity.RowAmount          = PlaygroundData.RowAmount;
    Playground->GridEntity.ColumnAmount       = PlaygroundData.ColumnAmount;
    Playground->GridEntity.MovingBlocksAmount = PlaygroundData.MovingBlocksAmount;
    Playground->GridEntity.StickUnitsAmount   = PlaygroundData.FigureAmount;
    
    Playground->GridEntity.GridArea.Min.x = 100;
    Playground->GridEntity.GridArea.Min.y = 81;
    Playground->GridEntity.GridArea.Max.x = Playground->GridEntity.GridArea.Min.x + 1128;
    Playground->GridEntity.GridArea.Max.y = Playground->GridEntity.GridArea.Min.y + 930;
    
    s32 *UnitFieldSource = PlaygroundData.UnitField;
    s32 *UnitFieldTarget = Playground->GridEntity.UnitField;
    for (u32 Row = 0; 
         Row < PlaygroundData.RowAmount;
         ++Row)
    {
        for (u32 Column = 0; 
             Column < PlaygroundData.ColumnAmount;
             ++Column)
        {
            s32 UnitIndex = (Row * PlaygroundData.ColumnAmount) + Column;
            UnitFieldTarget[UnitIndex] = UnitFieldSource[UnitIndex];
        }
    }
    
    for (u32 i = 0; i < FIGURE_AMOUNT_MAXIMUM; ++i)
    {
        Playground->GridEntity.StickUnits[i].Index = -1;
        Playground->GridEntity.StickUnits[i].IsSticked = false;
        
    }
    
    moving_block *MovingBlocks = Playground->GridEntity.MovingBlocks;
    for (u32 BlockIndex = 0;
         BlockIndex < PlaygroundData.MovingBlocksAmount;
         ++BlockIndex)
    {
        MovingBlocks[BlockIndex].RowNumber  = PlaygroundData.MovingBlocks[BlockIndex].RowNumber;
        MovingBlocks[BlockIndex].ColNumber  = PlaygroundData.MovingBlocks[BlockIndex].ColNumber;
        MovingBlocks[BlockIndex].IsVertical = PlaygroundData.MovingBlocks[BlockIndex].IsVertical;
        MovingBlocks[BlockIndex].MoveSwitch = PlaygroundData.MovingBlocks[BlockIndex].MoveSwitch;
        MovingBlocks[BlockIndex].Area.Min.x = Playground->GridEntity.GridArea.Min.x + (MovingBlocks[BlockIndex].ColNumber * GRID_BLOCK_SIZE);
        MovingBlocks[BlockIndex].Area.Min.y = Playground->GridEntity.GridArea.Min.y + (MovingBlocks[BlockIndex].RowNumber * GRID_BLOCK_SIZE);
        MovingBlocks[BlockIndex].Area.Max.x = MovingBlocks[BlockIndex].Area.Min.x + GRID_BLOCK_SIZE;
        MovingBlocks[BlockIndex].Area.Max.y = MovingBlocks[BlockIndex].Area.Min.y + GRID_BLOCK_SIZE;
        
        Playground->GridEntity.UnitField[(MovingBlocks[BlockIndex].RowNumber * PlaygroundData.ColumnAmount) + MovingBlocks[BlockIndex].ColNumber] = 1;
    }
    
    Playground->FigureEntity.FigureAmount     = PlaygroundData.FigureAmount;
    Playground->FigureEntity.ReturnIndex      = -1;
    Playground->FigureEntity.FigureVelocity   = Configuration->FigureVelocity;
    Playground->FigureEntity.RotationVelocity = Configuration->RotationVel;
    Playground->FigureEntity.FigureArea.Min.x = 1300;
    Playground->FigureEntity.FigureArea.Min.y = 81;
    Playground->FigureEntity.FigureArea.Max.x = Playground->FigureEntity.FigureArea.Min.x + 552;
    Playground->FigureEntity.FigureArea.Max.y = Playground->FigureEntity.FigureArea.Min.y + 930;
    
    figure_unit *FigureUnits = Playground->FigureEntity.FigureUnit;
    for (u32 FigureIndex = 0;
         FigureIndex < PlaygroundData.FigureAmount;
         ++FigureIndex)
    {
        figure_form Form = PlaygroundData.Figures[FigureIndex].Form;
        figure_type Type = PlaygroundData.Figures[FigureIndex].Type;
        
        Playground->FigureEntity.FigureUnit[FigureIndex].Angle = PlaygroundData.Figures[FigureIndex].Angle;
        FigureUnitInitFigure(&Playground->FigureEntity.FigureUnit[FigureIndex], Form, Type);
    }
    
    for(u32 i = 0; i < FIGURE_AMOUNT_MAXIMUM; ++i) 
    {
        Playground->FigureEntity.FigureOrder[i] = i;
    }
    
    FigureEntityAlignFigures(&Playground->FigureEntity);
}

