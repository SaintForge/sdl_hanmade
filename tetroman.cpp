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
#include "tetroman_render_group.cpp"

static bool
GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
    bool ShouldQuit = false;
    
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);    
    game_state *GameState = (game_state *) Memory->PermanentStorage;
    
    if(!Memory->IsInitialized)
    {
        // TODO(msokolov): should get rid of this initialization
        /* NOTE(msokolov): game_memory initialization starts here */
        Memory->CurrentLevelIndex = 0;
        Memory->LevelMemoryAmount = 1;
        
        Memory->LevelNumberFont = TTF_OpenFont(FontPath, 50);
        Assert(Memory->LevelNumberFont);
        
        /* NOTE(msokolov): game_state initialization starts here */
        InitializeMemoryGroup(&GameState->MemoryGroup, Memory->PermanentStorageSize - sizeof(game_state), (u8*)Memory->PermanentStorage + sizeof(game_state));
        
        GameState->EditorMode  = false;
        GameState->CurrentMode = game_mode::LEVEL;
        
        /* NOTE(msokolov): level_entity initialization starts here */
        GameState->LevelEntity = PushStruct(&GameState->MemoryGroup, level_entity);
        level_entity *LevelEntity = GameState->LevelEntity;
        LevelEntity->LevelNumber           = 0;
        LevelEntity->LevelStarted          = true;
        LevelEntity->LevelFinished         = false;
        LevelEntity->LevelPaused           = false;
        LevelEntity->LevelNumberQuad       = {};
        LevelEntity->LevelNumberShadowQuad = {};
        
        LevelEntity->Configuration.InActiveBlockSize   = 62;
        LevelEntity->Configuration.GridBlockSize       = 108;
        LevelEntity->Configuration.StartUpTimeToFinish = 2.0f;
        LevelEntity->Configuration.StartUpTimeToFinish = 0.0f;
        LevelEntity->Configuration.StartUpTimeToFinish = 0.0f; // TODO(msokolov): replace that with overall time accumulator from game_input like TimeElapsed
        LevelEntity->Configuration.RotationVel         = 600.0f;
        LevelEntity->Configuration.StartAlphaPerSec    = 500.0f;
        LevelEntity->Configuration.FlippingAlphaPerSec = 1000.0f;
        LevelEntity->Configuration.PixelsDrawn         = 0;
        LevelEntity->Configuration.PixelsToDraw        = 0;
        
        /* NOTE(msokolov): figure_entity initialization starts here */
        LevelEntity->FigureEntity   = PushStruct(&GameState->MemoryGroup, figure_entity);
        figure_entity* FigureEntity = LevelEntity->FigureEntity;
        FigureEntity->FigureAmount  = 0;
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
        FigureEntity->FigureVelocity = 12;
        FigureEntity->FigureArea.x = 1200;
        FigureEntity->FigureArea.y = 81;
        FigureEntity->FigureArea.w = 552;
        FigureEntity->FigureArea.h = 972;
        
        FigureEntity->FigureUnit = PushArray(&GameState->MemoryGroup, FIGURE_AMOUNT_MAXIMUM, figure_unit);
        
        FigureUnitAddNewFigure(FigureEntity, O_figure, classic, 0.0f, LevelEntity->Configuration.InActiveBlockSize, Memory, Buffer);
        FigureUnitAddNewFigure(FigureEntity, O_figure, stone,   0.0f, LevelEntity->Configuration.InActiveBlockSize, Memory, Buffer);
        FigureUnitAddNewFigure(FigureEntity, O_figure, mirror,  0.0f, LevelEntity->Configuration.InActiveBlockSize, Memory, Buffer);
        
        FigureEntityAlignFigures(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
        
        FigureEntity->FigureOrder = PushArray(&GameState->MemoryGroup, FIGURE_AMOUNT_MAXIMUM, u32);
        for(u32 i = 0; i < FIGURE_AMOUNT_MAXIMUM; ++i) 
        {
            FigureEntity->FigureOrder[i] = i;
        }
        
        FigureEntity->ClassicO_Figure = GetTexture(Memory, "o_d.png", Buffer->Renderer);
        FigureEntity->StoneO_Figure   = GetTexture(Memory, "o_s.png", Buffer->Renderer);
        FigureEntity->MirrorO_Figure  = GetTexture(Memory, "o_m.png", Buffer->Renderer);
        
        /* NOTE(msokolov): grid_entity initialization starts here */
        LevelEntity->GridEntity = PushStruct(&GameState->MemoryGroup, grid_entity);
        grid_entity *GridEntity = LevelEntity->GridEntity;
        GridEntity->RowAmount           = 8;
        GridEntity->ColumnAmount        = 6;
        GridEntity->StickUnitsAmount    = FigureEntity->FigureAmount;
        GridEntity->MovingBlocksAmount  = 0;
        
        GridEntity->GridArea.w = 1128;
        GridEntity->GridArea.h = 972;
        GridEntity->GridArea.x = 0;
        GridEntity->GridArea.y = 81;
        
        GridEntity->UnitField = PushArray(&GameState->MemoryGroup, COLUMN_AMOUNT_MAXIMUM * ROW_AMOUNT_MAXIMUM, s32);
        for (u32 Row = 0; Row < GridEntity->RowAmount; ++Row)
        {
            for (u32 Col = 0; Col < GridEntity->ColumnAmount; ++Col)
            {
                GridEntity->UnitField[(Row * GridEntity->ColumnAmount) + Col] = 0;
            }
        }
        
        GridEntity->UnitSize = PushArray(&GameState->MemoryGroup, COLUMN_AMOUNT_MAXIMUM * ROW_AMOUNT_MAXIMUM, r32);
        for(u32 Row = 0; Row < GridEntity->RowAmount; ++Row)
        {
            for(u32 Col = 0; Col < GridEntity->ColumnAmount; ++Col)
            {
                s32 UnitIndex = (Row * GridEntity->ColumnAmount) + Col;
                GridEntity->UnitSize[UnitIndex] = 0;
            }
        }
        
        GridEntity->StickUnits = PushArray(&GameState->MemoryGroup, FIGURE_AMOUNT_MAXIMUM, sticked_unit);
        
        for (u32 i = 0; i < FIGURE_AMOUNT_MAXIMUM; ++i)
        {
            GridEntity->StickUnits[i].Index     = -1;
            GridEntity->StickUnits[i].IsSticked = false;
        }
        
        GridEntity->MovingBlocks = PushArray(&GameState->MemoryGroup, MOVING_BLOCKS_MAXIMUM, moving_block);
        
        GridEntity->NormalSquareTexture     = GetTexture(Memory, "grid_cell.png", Buffer->Renderer);
        GridEntity->VerticalSquareTexture   = GetTexture(Memory, "o_s.png", Buffer->Renderer);
        GridEntity->HorizontlaSquareTexture = GetTexture(Memory, "o_m.png", Buffer->Renderer);
        GridEntity->TopLeftCornerFrame      = GetTexture(Memory, "frame3.png", Buffer->Renderer);
        GridEntity->TopRightCornerFrame     = GetTexture(Memory, "frame4.png", Buffer->Renderer);
        GridEntity->DownLeftCornerFrame     = GetTexture(Memory, "frame2.png", Buffer->Renderer);
        GridEntity->DownRightCornerFrame    = GetTexture(Memory, "frame1.png", Buffer->Renderer);
        
        /* NOTE(msokolov): menu_entity initialization starts here */ 
        
        GameState->MenuEntity   = PushStruct(&GameState->MemoryGroup, menu_entity);
        menu_entity *MenuEntity = GameState->MenuEntity;
        MenuEntity->MaxVelocity = 20.0f;
        MenuEntity->ButtonIndex = -1;
        MenuEntity->BackTexture = GetTexture(Memory, "grid_cell.png", Buffer->Renderer);
        
        MenuEntityUpdatePositionsLandscape(Buffer, MenuEntity, Memory);
        
        /* NOTE(msokolov): game_editor initialization starts here */ 
        
        GameState->GameEditor   = PushStruct(&GameState->MemoryGroup, game_editor);
        game_editor *GameEditor = GameState->GameEditor;
        
        GameEditorInit(Buffer, LevelEntity, MenuEntity, Memory, GameEditor);
        
        Memory->IsInitialized     = true;
        printf("Memory has been initialized!\n");
    }
    
    Assert(sizeof(transient_state) < Memory->TransientStorageSize);
    transient_state *TransState = (transient_state *)Memory->TransientStorage;
    if (!TransState->IsInitialized)
    {
        InitializeMemoryGroup(&TransState->TransGroup, Memory->TransientStorageSize - sizeof(transient_state), (u8*)Memory->TransientStorage + sizeof(transient_state));
        
        
        TransState->IsInitialized = true;
    }
    
    level_entity *LevelEntity = GameState->LevelEntity;
    menu_entity  *MenuEntity  = GameState->MenuEntity;
    
    game_rect FigureArea = LevelEntity->FigureEntity->FigureArea;
    game_rect GridArea = LevelEntity->GridEntity->GridArea;
    
    level_config LevelConfig = LevelEntity->Configuration;
    level_animation LevelAnimation = LevelEntity->AnimationData;
    
    if(Input->Keyboard.Tab.EndedDown)
    {
        if(GameState->CurrentMode == LEVEL) 
        {
            GameState->CurrentMode = LEVEL_MENU;
        }
        else if(GameState->CurrentMode == LEVEL_MENU)
        {
            GameState->CurrentMode = LEVEL;
        }
    }
    
    if(Input->Keyboard.Escape.EndedDown)
    {
        ShouldQuit = true;
    }
    
    game_mode CurrentMode = GameState->CurrentMode;
    
    switch (CurrentMode)
    {
        case LEVEL:
        {
            // TODO(msokolov): replace that with texture
            game_rect ScreenArea = { 0, 0, Buffer->Width, Buffer->Height};
            DEBUGRenderQuadFill(Buffer, &ScreenArea, { 42, 6, 21 }, 255);
            
            LevelEntityUpdateAndRender(LevelEntity, Memory, Input, Buffer);
        } break;
        
        case LEVEL_MENU:
        {
            MenuUpdateAndRender(GameState, MenuEntity, Memory, Input, Buffer);
        } break;
        
        case MAIN_MENU:
        {
            
        } break;
    }
    
    /*
 NOTE(msokolov): Render 
    */
    
    memory_group TemporaryMemory = TransState->TransGroup;
    
    render_group *RenderGroup = AllocateRenderGroup(&TransState->TransGroup, Kilobytes(1));
    Clear(RenderGroup, {255, 255, 0, 255});
    PushRect(RenderGroup, {0, 0, 100, 100}, {255, 0, 0, 255});
    
    RenderGroupToOutput(RenderGroup, Buffer);
    TransState->TransGroup = TemporaryMemory;
    
    game_editor *GameEditor = GameState->GameEditor;
    GameEditorUpdateAndRender(Buffer, GameState, Memory, Input, GameEditor, LevelEntity, MenuEntity);
    
    return(ShouldQuit);
}
