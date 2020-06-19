// game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

#include "tetroman.h"

#include "tetroman_entity.cpp"
#include "tetroman_asset.cpp"
#include "tetroman_menu.cpp"
#include "tetroman_editor.cpp"

static bool
GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
    bool ShouldQuit = false;
    
    if(!Memory->IsInitialized)
    {
        Memory->CurrentState = game_mode::LEVEL;
        
        Memory->RefWidth  = 800;
        Memory->RefHeight = 600;
        
        math_rect ScreenArea    = {0.0f, 0.0f, (r32)Buffer->Width, (r32)Buffer->Height};
        math_rect RefScreenArea = {0.0f, 0.0f, (r32)Memory->RefWidth, (r32)Memory->RefHeight};
        
        Memory->PadRect.x = 40;
        Memory->PadRect.y = 30;
        Memory->PadRect.w = 720;
        Memory->PadRect.h = 570;
        
        math_rect GameArea   = CreateMathRect(0.05f, 0.95f, 0.95f, 0.05f, ScreenArea);
        math_rect GridArea   = CreateMathRect(0.0f, 1.0f, 1.0f, 0.5f, GameArea);
        math_rect FigureArea = CreateMathRect(0.0f, 0.3f, 1.0f, 0.0f, GameArea);
        
        /* game_memory initialization */
        
        Memory->ToggleMenu        = false;
        Memory->CurrentLevelIndex = 0;
        
        Memory->LevelNumberFont = TTF_OpenFont("..\\data\\Karmina-Bold.otf", 50);
        Assert(Memory->LevelNumberFont);
        
        /* LocalMemoryStorage allocation */
        /* level_entity initialization */
        
        Memory->LocalMemoryStorage = calloc(1, sizeof(level_entity) + (sizeof(menu_entity)));
        Assert(Memory->LocalMemoryStorage);
        Memory->LevelMemoryAmount = 1;
        
        level_entity *LevelEntity  = (level_entity*) Memory->LocalMemoryStorage;
        LevelEntity->LevelNumber   = 0;
        LevelEntity->LevelStarted  = false;
        LevelEntity->LevelFinished = false;
        LevelEntity->LevelPaused   = false;
        LevelEntity->LevelNumberQuad       = {};
        LevelEntity->LevelNumberShadowQuad = {};
        
        u32 RowAmount           = 8;
        u32 ColumnAmount        = 6;
        u32 FigureAmountReserve = 20;
        u32 MovingBlocksAmountReserved  = 10;
        
        //for DEBUG purposes only
        game_rect FigureAreaRect = ConvertMathRectToGameRect(FigureArea);
        
        if(Buffer->Width < Buffer->Height)
        {
            LevelEntity->Configuration.InActiveBlockSize = CalculateFigureBlockSizeByHeight(3,FigureAreaRect.h);
        }
        else
        {
            LevelEntity->Configuration.InActiveBlockSize = CalculateFigureBlockSizeByWidth(3, FigureAreaRect.w);
        }
        
        u32 GridBlockSize = LevelEntity->Configuration.GridBlockSize;
        u32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
        
        /* Change values below to be time configured */
        LevelEntity->Configuration.StartUpTimeToFinish = 2.0f;
        LevelEntity->Configuration.RotationVel         = 600.0f;
        LevelEntity->Configuration.StartAlphaPerSec    = 500.0f;
        LevelEntity->Configuration.FlippingAlphaPerSec = 1000.0f;
        
        /* figure_entity initialization */
        LevelEntity->FigureEntity= (figure_entity*)malloc(sizeof(figure_entity));
        Assert(LevelEntity->FigureEntity);
        
        figure_entity* FigureEntity = LevelEntity->FigureEntity;
        
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
        FigureEntity->FigureAlpha   = 0;
        FigureEntity->FadeInSum     = 0;
        FigureEntity->FadeOutSum    = 0;
        
        FigureEntity->FigureArea = FigureAreaRect;
        
        //FigureEntity->FigureArea = ConvertMathRectToGameRect(FigureArea);
        
        FigureEntity->FigureUnit = (figure_unit*)calloc(FigureEntity->FigureAmountReserved, sizeof(figure_unit));
        Assert(FigureEntity->FigureUnit);
        
        FigureUnitAddNewFigure(FigureEntity, Z_figure, classic, 0.0f, Memory, Buffer);
        FigureUnitAddNewFigure(FigureEntity, I_figure, stone,   0.0f, Memory, Buffer);
        FigureUnitAddNewFigure(FigureEntity, J_figure, mirror,  0.0f, Memory, Buffer);
        
        FigureEntity->FigureAmount = 0;
        FigureUnitAddNewFigure(FigureEntity, Z_figure, classic, 0.0f, Memory, Buffer);
        FigureUnitAddNewFigure(FigureEntity, I_figure, stone,   0.0f, Memory, Buffer);
        FigureUnitAddNewFigure(FigureEntity, J_figure, mirror,  0.0f, Memory, Buffer);
        
        FigureEntity->FigureOrder = (u32*)malloc(sizeof(u32) * FigureEntity->FigureAmountReserved);
        Assert(FigureEntity->FigureOrder);
        
        for(u32 i = 0; i < FigureEntity->FigureAmountReserved; ++i) 
        {
            FigureEntity->FigureOrder[i] = i;
        }
        
        /* grid_entity initialization */
        
        LevelEntity->GridEntity  = (grid_entity *) malloc(sizeof(grid_entity));
        Assert(LevelEntity->GridEntity);
        
        grid_entity *GridEntity = LevelEntity->GridEntity;
        GridEntity->RowAmount           = RowAmount;
        GridEntity->ColumnAmount        = ColumnAmount;
        GridEntity->StickUnitsAmount    = FigureEntity->FigureAmount;
        GridEntity->MovingBlocksAmount  = 0;
        GridEntity->MovingBlocksAmountReserved  = MovingBlocksAmountReserved;
        
        GridEntity->GridArea.w = GridBlockSize * ColumnAmount;
        GridEntity->GridArea.h = GridBlockSize * RowAmount;
        GridEntity->GridArea.x = (Buffer->Width / 2) - (GridEntity->GridArea.w / 2);
        GridEntity->GridArea.y = (Buffer->Height - FigureEntity->FigureArea.h) / 2 - (GridEntity->GridArea.h / 2);
        
        LevelEntity->Configuration.GridBlockSize = CalculateGridBlockSize(RowAmount, ColumnAmount, 
                                                                          GridEntity->GridArea.w, GridEntity->GridArea.h);
        
        //LevelEntity->Configuration.GridBlockSize = ActiveBlockSize;
        
        //GridEntity->GridArea = ConvertMathRectToGameRect(GridArea);
        
        /* UnitField initialization */
        
        GridEntity->UnitField = (s32*)calloc(ColumnAmount * RowAmount, sizeof(s32));
        Assert(GridEntity->UnitField);
        for (u32 Row = 0; Row < RowAmount; ++Row)
        {
            for (u32 Col = 0; Col < ColumnAmount; ++Col)
            {
                GridEntity->UnitField[(Row * GridEntity->ColumnAmount) + Col] = 0;
            }
        }
        
        GridEntity->UnitSize = (r32*)calloc(ColumnAmount * RowAmount, sizeof(r32));
        Assert(GridEntity->UnitSize);
        for(u32 Row = 0; Row < RowAmount; ++Row)
        {
            for(u32 Col = 0; Col < ColumnAmount; ++Col)
            {
                s32 UnitIndex = (Row * ColumnAmount) + Col;
                GridEntity->UnitSize[UnitIndex] = 0;
            }
        }
        
        FigureEntityAlignHorizontally(FigureEntity, InActiveBlockSize);
        
        /* StickUnits initialization */
        
        GridEntity->StickUnits = (sticked_unit*)calloc(sizeof(sticked_unit), FigureEntity->FigureAmountReserved);
        Assert(GridEntity->StickUnits);
        for (u32 i = 0; i < FigureEntity->FigureAmountReserved; ++i)
        {
            GridEntity->StickUnits[i].Index     = -1;
            GridEntity->StickUnits[i].IsSticked = false;
        }
        
        /* MovingBlocks initialization */
        
        GridEntity->MovingBlocks = (moving_block*)malloc(sizeof(moving_block) * MovingBlocksAmountReserved);
        Assert(GridEntity->MovingBlocks);
        
        
        /* GridEntity texture initialization */
        
        GridEntity->NormalSquareTexture     = GetTexture(Memory, "grid_cell.png", Buffer->Renderer);
        GridEntity->VerticalSquareTexture   = GetTexture(Memory, "o_s.png", Buffer->Renderer);
        GridEntity->HorizontlaSquareTexture = GetTexture(Memory, "o_m.png", Buffer->Renderer);
        
        GridEntity->TopLeftCornerFrame = GetTexture(Memory, "frame3.png", Buffer->Renderer);
        GridEntity->TopRightCornerFrame = GetTexture(Memory, "frame4.png", Buffer->Renderer);
        GridEntity->DownLeftCornerFrame = GetTexture(Memory, "frame2.png", Buffer->Renderer);
        GridEntity->DownRightCornerFrame = GetTexture(Memory, "frame1.png", Buffer->Renderer);
        
        /* Adjusting object positions on the screen */
        LevelEntityUpdatePositionsLandscape(Buffer, Memory);
        
        /* menu_entity initialization */ 
        
        menu_entity *MenuEntity  = (menu_entity*) (((char*)Memory->LocalMemoryStorage) + (sizeof(level_entity))); 
        Assert(MenuEntity);
        
        MenuInit(MenuEntity, Memory, Buffer);
        MenuEntityUpdatePositionsLandscape(Buffer, MenuEntity, Memory);
        
        /* EditorMemoryStorage allocation */
        
        Memory->EditorMemoryStorage = calloc(1, sizeof(game_editor) + sizeof(level_editor) + sizeof(menu_editor) + sizeof(resolution_editor));
        Assert(Memory->EditorMemoryStorage);
        
        /* game_editor initialization */ 
        game_editor *GameEditor = (game_editor *)Memory->EditorMemoryStorage;
        Assert(GameEditor);
        
        GameEditorInit(Buffer, Memory, GameEditor);
        
        Memory->IsInitialized = true;
        printf("Memory has been initialized!\n");
        
        //TODO(msokolov): temporary issue
        //LevelEntityStartAnimationInit(LevelEntity, Buffer);
    }
    
    level_entity *LevelEntity  = (level_entity *)Memory->LocalMemoryStorage;
    menu_entity  *MenuEntity   = (menu_entity*) (((char*)Memory->LocalMemoryStorage) + (sizeof(level_entity))); 
    
    
    if(Input->Keyboard.Tab.EndedDown)
    {
        if(Memory->CurrentState == LEVEL) 
        {
            Memory->CurrentState = LEVEL_MENU;
        }
        else if(Memory->CurrentState == LEVEL_MENU)
        {
            Memory->CurrentState = LEVEL;
        }
        
    }
    
    
    if(Input->Keyboard.Escape.EndedDown)
    {
        ShouldQuit = true;
    }
    
    game_mode CurrentState = Memory->CurrentState;
    
    switch (CurrentState)
    {
        case LEVEL:
        {
            game_rect ScreenArea = { 0, 0, Buffer->Width, Buffer->Height};
            DEBUGRenderQuadFill(Buffer, &ScreenArea, { 42, 6, 21 }, 255);
            
            LevelEntityUpdateAndRender(LevelEntity, Memory, Input, Buffer);
        } break;
        
        case LEVEL_MENU:
        {
            MenuUpdateAndRender(MenuEntity, Memory, Input, Buffer);
        } break;
    }
    
    game_editor *GameEditor = (game_editor *)Memory->EditorMemoryStorage;
    GameEditorUpdateAndRender(Buffer, Memory, Input, GameEditor);
    
    return(ShouldQuit);
}