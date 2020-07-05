// game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

#include "tetroman.h"

#include "tetroman_render_group.cpp"
#include "tetroman_entity.cpp"
#include "tetroman_asset.cpp"
#include "tetroman_menu.cpp"
#include "tetroman_editor.cpp"

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
        
        /* NOTE(msokolov): game_state initialization starts here */
        InitializeMemoryGroup(&GameState->MemoryGroup, Memory->PermanentStorageSize - sizeof(game_state), (u8*)Memory->PermanentStorage + sizeof(game_state));
        
        GameState->EditorMode  = false;
        GameState->PlaygroundIndex = 0;
        GameState->CurrentMode = game_mode::LEVEL;
        
        /* NOTE(msokolov): level_entity initialization starts here */
        GameState->Playground = PushStruct(&GameState->MemoryGroup, playground);
        playground* Playground = GameState->Playground;
        Playground->LevelNumber           = 0;
        Playground->LevelStarted          = true;
        Playground->LevelFinished         = false;
        Playground->LevelPaused           = false;
        Playground->LevelNumberQuad       = {};
        Playground->LevelNumberShadowQuad = {};
        
        Playground->Configuration.InActiveBlockSize   = 62;
        Playground->Configuration.GridBlockSize       = 108;
        Playground->Configuration.StartUpTimeToFinish = 2.0f;
        Playground->Configuration.StartUpTimeToFinish = 0.0f;
        Playground->Configuration.StartUpTimeToFinish = 0.0f; // TODO(msokolov): replace that with overall time accumulator from game_input like TimeElapsed
        Playground->Configuration.RotationVel         = 600.0f;
        Playground->Configuration.StartAlphaPerSec    = 500.0f;
        Playground->Configuration.FlippingAlphaPerSec = 1000.0f;
        Playground->Configuration.PixelsDrawn         = 0;
        Playground->Configuration.PixelsToDraw        = 0;
        
        /* NOTE(msokolov): figure_entity initialization starts here */
        figure_entity* FigureEntity = &Playground->FigureEntity;
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
        FigureEntity->FigureVelocity = 400.0f;
        
        FigureEntity->FigureArea.Min.x = 1200;
        FigureEntity->FigureArea.Min.y = 81;
        //FigureEntity->FigureArea.Max.w = 552;
        //FigureEntity->FigureArea.Max.h = 972;
        FigureEntity->FigureArea.Max.x = FigureEntity->FigureArea.Min.x + 552;
        FigureEntity->FigureArea.Max.y = FigureEntity->FigureArea.Min.y + 972;
        
        FigureUnitAddNewFigure(FigureEntity, L_figure, classic, Playground->Configuration.InActiveBlockSize);
        FigureUnitAddNewFigure(FigureEntity, O_figure, stone, Playground->Configuration.InActiveBlockSize);
        FigureUnitAddNewFigure(FigureEntity, O_figure, mirror, Playground->Configuration.InActiveBlockSize);
        
        FigureEntityAlignFigures(&Playground->FigureEntity, Playground->Configuration.InActiveBlockSize);
        
        for(u32 i = 0; i < FIGURE_AMOUNT_MAXIMUM; ++i) 
        {
            FigureEntity->FigureOrder[i] = i;
        }
        
        FigureEntity->O_ClassicTexture = GetTexture(Memory, "o_d.png", Buffer->Renderer);
        FigureEntity->O_StoneTexture   = GetTexture(Memory, "o_s.png", Buffer->Renderer);
        FigureEntity->O_MirrorTexture  = GetTexture(Memory, "o_m.png", Buffer->Renderer);
        
        FigureEntity->I_ClassicTexture = GetTexture(Memory, "i_d.png", Buffer->Renderer);
        FigureEntity->I_StoneTexture   = GetTexture(Memory, "i_s.png", Buffer->Renderer);
        FigureEntity->I_MirrorTexture  = GetTexture(Memory, "i_m.png", Buffer->Renderer);
        
        FigureEntity->L_ClassicTexture = GetTexture(Memory, "l_d.png", Buffer->Renderer);
        FigureEntity->L_StoneTexture   = GetTexture(Memory, "l_s.png", Buffer->Renderer);
        FigureEntity->L_MirrorTexture  = GetTexture(Memory, "l_m.png", Buffer->Renderer);
        
        FigureEntity->J_ClassicTexture = GetTexture(Memory, "j_d.png", Buffer->Renderer);
        FigureEntity->J_StoneTexture   = GetTexture(Memory, "j_s.png", Buffer->Renderer);
        FigureEntity->J_MirrorTexture  = GetTexture(Memory, "j_m.png", Buffer->Renderer);
        
        FigureEntity->Z_ClassicTexture = GetTexture(Memory, "z_d.png", Buffer->Renderer);
        FigureEntity->Z_StoneTexture   = GetTexture(Memory, "z_s.png", Buffer->Renderer);
        FigureEntity->Z_MirrorTexture  = GetTexture(Memory, "z_m.png", Buffer->Renderer);
        
        FigureEntity->S_ClassicTexture = GetTexture(Memory, "s_d.png", Buffer->Renderer);
        FigureEntity->S_StoneTexture   = GetTexture(Memory, "s_s.png", Buffer->Renderer);
        FigureEntity->S_MirrorTexture  = GetTexture(Memory, "s_m.png", Buffer->Renderer);
        
        FigureEntity->T_ClassicTexture = GetTexture(Memory, "t_d.png", Buffer->Renderer);
        FigureEntity->T_StoneTexture   = GetTexture(Memory, "t_s.png", Buffer->Renderer);
        FigureEntity->T_MirrorTexture  = GetTexture(Memory, "t_m.png", Buffer->Renderer);
        
        /* NOTE(msokolov): grid_entity initialization starts here */
        grid_entity *GridEntity = &Playground->GridEntity;
        GridEntity->RowAmount           = 8;
        GridEntity->ColumnAmount        = 6;
        GridEntity->StickUnitsAmount    = FigureEntity->FigureAmount;
        GridEntity->MovingBlocksAmount  = 0;
        
        GridEntity->GridArea.Min.x = 100;
        GridEntity->GridArea.Min.y = 81;
        GridEntity->GridArea.Max.x = GridEntity->GridArea.Min.x + 1128;
        GridEntity->GridArea.Max.y = GridEntity->GridArea.Min.y + 972;
        
        for (u32 Row = 0; Row < ROW_AMOUNT_MAXIMUM; ++Row)
        {
            for (u32 Col = 0; Col < COLUMN_AMOUNT_MAXIMUM; ++Col)
            {
                GridEntity->UnitField[(Row * COLUMN_AMOUNT_MAXIMUM) + Col] = 0;
            }
        }
        
        for (u32 i = 0; i < FIGURE_AMOUNT_MAXIMUM; ++i)
        {
            GridEntity->StickUnits[i].Index     = -1;
            GridEntity->StickUnits[i].IsSticked = false;
        }
        
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
        
        /* NOTE(msokolov): game_editor initialization starts here */ 
        
        //GameState->GameEditor   = PushStruct(&GameState->MemoryGroup, game_editor);
        //game_editor *GameEditor = GameState->GameEditor;
        
        //GameEditorInit(Buffer, Playground, MenuEntity, Memory, GameEditor);
        
        Memory->IsInitialized = true;
        printf("Memory has been initialized!\n");
    }
    
    Assert(sizeof(transient_state) < Memory->TransientStorageSize);
    transient_state *TransState = (transient_state *)Memory->TransientStorage;
    if (!TransState->IsInitialized)
    {
        InitializeMemoryGroup(&TransState->TransGroup, Memory->TransientStorageSize - sizeof(transient_state), (u8*)Memory->TransientStorage + sizeof(transient_state));
        
        TransState->IsInitialized = true;
    }
    
    playground *Playground  = GameState->Playground;
    menu_entity *MenuEntity  = GameState->MenuEntity;
    
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
    
    /*
 NOTE(msokolov): Render 
    */
    
    memory_group TemporaryMemory = TransState->TransGroup;
    render_group *RenderGroup = AllocateRenderGroup(&TransState->TransGroup, Kilobytes(10), Buffer->Width, Buffer->Height);
    
    game_mode CurrentMode = GameState->CurrentMode;
    switch (CurrentMode)
    {
        case LEVEL:
        {
            playground_status PlaygroundStatus = LevelEntityUpdateAndRender(Playground, RenderGroup, Input);
        } break;
        
        case LEVEL_MENU:
        {
            //MenuUpdateAndRender(GameState, MenuEntity, Memory, Input, Buffer);
        } break;
        
        case MAIN_MENU:
        {
            // TODO(msokolov): do we actually need that?
        } break;
    }
    
    RenderGroupToOutput(RenderGroup, Buffer);
    TransState->TransGroup = TemporaryMemory;
    
    
    //game_editor *GameEditor = GameState->GameEditor;
    //GameEditorUpdateAndRender(Buffer, GameState, Memory, Input, GameEditor, Playground, MenuEntity);
    
    return(ShouldQuit);
}
