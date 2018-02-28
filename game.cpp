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
//#include "menu.h"

#include "entity.cpp"
#include "assets.cpp"
#include "menu.cpp"
#include "editor.cpp"

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
        GridEntity->UnitField = (s32*)calloc(GridEntity->ColumnAmount * GridEntity->RowAmount, sizeof(s32));
        Assert(GridEntity->UnitField);
        for (u32 Row = 0; Row < GridEntity->RowAmount; ++Row)
        {
            for (u32 Col = 0; Col < GridEntity->ColumnAmount; ++Col)
            {
                GridEntity->UnitField[(Row * GridEntity->ColumnAmount) + Col] = 0;
            }
        }
        
        GridEntity->UnitSize = (r32*)calloc(GridEntity->ColumnAmount *GridEntity->RowAmount, sizeof(r32));
        Assert(GridEntity->UnitSize);
        for(u32 Row = 0; Row < GridEntity->RowAmount; ++Row)
        {
            for(u32 Col = 0; Col < GridEntity->ColumnAmount; ++Col)
            {
                s32 UnitIndex = (Row * GridEntity->ColumnAmount) + Col;
                GridEntity->UnitSize[UnitIndex] = 0;
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
        
        Memory->Sound = GetSound(Memory, "chunk.wav");
        Assert(Memory->Sound);
        
        
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
        MenuUpdateAndRender(Memory->MenuEntity, Memory, Input, Buffer);
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
