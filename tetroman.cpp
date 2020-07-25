/* ========================================= */
//     $File: tetroman.cpp
//     $Date: October 10th 2017 10:32 am 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */

#include "tetroman.h"

#include "tetroman_render_group.cpp"
#include "tetroman_playground.cpp"
#include "tetroman_asset.cpp"
#include "tetroman_menu.cpp"
#include "tetroman_editor.cpp"

static game_return_values
GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
    game_return_values Result = {};
    
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);    
    game_state *GameState = (game_state *) Memory->PermanentStorage;
    
    if(!Memory->IsInitialized)
    {
        // NOTE(msokolov): figure colors 
        // 212, 151, 0
        // 212, 10,  128
        // 116, 0,   40
        // 23,  156, 234
        // 108, 174, 0
        /* NOTE(msokolov): game_memory initialization starts here */
        
        /* NOTE(msokolov): game_state initialization starts here */
        InitializeMemoryGroup(&GameState->MemoryGroup, Memory->PermanentStorageSize - sizeof(game_state), (u8*)Memory->PermanentStorage + sizeof(game_state));
        
        GameState->Font = TTF_OpenFont(FontPath, 50);
        Assert(GameState->Font);
        
        GameState->TimerFont = TTF_OpenFont(FontPath, 80);
        Assert(GameState->TimerFont);
        
        GameState->MenuTimerFont = TTF_OpenFont(FontPath, 40);
        Assert(GameState->MenuTimerFont);
        
        GameState->PlaygroundIndex = 0;
        GameState->CurrentMode     = game_mode::PLAYGROUND;
        
        playground_config *Configuration = &GameState->Configuration;
        Configuration->StartUpTimeToFinish = 0.0f;
        Configuration->RotationVel         = 1000.0f;
        Configuration->StartAlphaPerSec    = 500.0f;
        Configuration->FlippingAlphaPerSec = 1000.0f;
        Configuration->FigureVelocity      = 600.0f;
        Configuration->MovingBlockVelocity = 555.0f;
        
        /* NOTE(msokolov): playground initialization starts here */
        playground* Playground = &GameState->Playground;
        Playground->LevelNumber           = 0;
        Playground->LevelStarted          = true;
        Playground->LevelFinished         = false;
        Playground->LevelPaused           = false;
        Playground->CornerLeftTopTexture      = GetTexture(Memory, "corner_left_top.png", Buffer->Renderer);
        Playground->CornerLeftBottomTexture   = GetTexture(Memory, "corner_left_bottom.png", Buffer->Renderer);
        Playground->CornerRightTopTexture     = GetTexture(Memory, "corner_right_top.png", Buffer->Renderer);
        Playground->CornerRightBottomTexture  = GetTexture(Memory, "corner_right_bottom.png", Buffer->Renderer);
        Playground->VerticalBorderTexture     = GetTexture(Memory, "vertical_border.png", Buffer->Renderer);
        
        Playground->IndicatorEmptyTexture     = GetTexture(Memory, "level_indicator_empty.png", Buffer->Renderer);
        Playground->IndicatorFilledTexture    = GetTexture(Memory, "level_indicator_filled.png", Buffer->Renderer);
        
        
        Playground->PickSound = GetSound(Memory, "figure_pick.wav");
        Playground->StickSound = GetSound(Memory, "figure_stick.wav");
        Playground->RotateSound = GetSound(Memory, "figure_drop.wav");
        
        Mix_Volume(-1, 8);
        
        /* Playground Option Menu */ 
        playground_options *PlaygroundOptions = &Playground->Options;
        PlaygroundOptions->ToggleMenu  = false;
        PlaygroundOptions->Choice      = options_choice::RESTART_OPTION;
        PlaygroundOptions->MenuPosition = { 1500.0f, 650.0f };
        PlaygroundOptions->ButtonDimension = { 180.0f, 80.0f };
        PlaygroundOptions->GearTexture = GetTexture(Memory, "gear_new.png", Buffer->Renderer);
        PlaygroundOptions->GearShadowTexture = GetTexture(Memory, "gear_new_shadow.png", Buffer->Renderer);
        PlaygroundOptions->HorizontalLineTexture = GetTexture(Memory, "horizontal_border_2.png", Buffer->Renderer);
        PlaygroundOptions->MenuTexture[0] = MakeTextureFromString(Buffer, GameState->Font, "Restart", {255, 255, 255, 255});
        PlaygroundOptions->MenuTexture[1] = MakeTextureFromString(Buffer, GameState->Font, "Settings", {255, 255, 255, 255});
        PlaygroundOptions->MenuTexture[2] = MakeTextureFromString(Buffer, GameState->Font, "Menu", {255, 255, 255, 255});
        PlaygroundOptions->MenuTexture[3] = MakeTextureFromString(Buffer, GameState->Font, "Quit", {255, 255, 255, 255});
        
        PlaygroundOptions->MenuShadowTexture[0] = MakeTextureFromString(Buffer, GameState->Font, "Restart", {0, 0, 0, 127});
        PlaygroundOptions->MenuShadowTexture[1] = MakeTextureFromString(Buffer, GameState->Font, "Settings", {0, 0, 0, 127});
        PlaygroundOptions->MenuShadowTexture[2] = MakeTextureFromString(Buffer, GameState->Font, "Menu", {0, 0, 0, 127});
        PlaygroundOptions->MenuShadowTexture[3] = MakeTextureFromString(Buffer, GameState->Font, "Quit", {0, 0, 0, 127});
        
        /* NOTE(msokolov): figure_entity initialization starts here */
        figure_entity* FigureEntity  = &Playground->FigureEntity;
        FigureEntity->FigureAmount   = 0;
        FigureEntity->ReturnIndex    = -1;
        FigureEntity->FigureActive   = -1;
        FigureEntity->IsGrabbed      = false;
        FigureEntity->IsRotating     = false;
        FigureEntity->IsReturning    = false;
        FigureEntity->IsRestarting   = false;
        FigureEntity->IsFlipping     = false;
        FigureEntity->RotationSum    = 0;
        FigureEntity->AreaAlpha      = 0;
        FigureEntity->FigureAlpha    = 0;
        FigureEntity->FadeInSum      = 0;
        FigureEntity->FadeOutSum     = 0;
        FigureEntity->FigureVelocity = 400.0f;
        FigureEntity->RotationVelocity = Configuration->RotationVel;
        
        FigureEntity->FigureArea.Min.x = 1500;
        FigureEntity->FigureArea.Min.y = 81;
        FigureEntity->FigureArea.Max.x = FigureEntity->FigureArea.Min.x + 552;
        FigureEntity->FigureArea.Max.y = FigureEntity->FigureArea.Min.y + 972;
        
        FigureUnitAddNewFigure(FigureEntity, L_figure, classic);
        FigureUnitAddNewFigure(FigureEntity, O_figure, stone);
        FigureUnitAddNewFigure(FigureEntity, O_figure, mirror);
        
        FigureEntityAlignFigures(&Playground->FigureEntity);
        
        for(u32 i = 0; i < FIGURE_AMOUNT_MAXIMUM; ++i) 
        {
            FigureEntity->FigureOrder[i] = i;
        }
        
        FigureEntity->O_ClassicTexture = GetTexture(Memory, "o_d_new.png", Buffer->Renderer);
        FigureEntity->O_ShadowTexture  = GetTexture(Memory, "o_d_new_shadow.png", Buffer->Renderer);
        
        FigureEntity->I_ClassicTexture = GetTexture(Memory, "i_d_new.png", Buffer->Renderer);
        FigureEntity->I_ShadowTexture  = GetTexture(Memory, "i_d_new_shadow.png", Buffer->Renderer);
        
        FigureEntity->L_ClassicTexture = GetTexture(Memory, "l_d_new.png", Buffer->Renderer);
        FigureEntity->L_ShadowTexture  = GetTexture(Memory, "l_d_new_shadow.png", Buffer->Renderer);
        
        FigureEntity->J_ClassicTexture = GetTexture(Memory, "j_d_new.png", Buffer->Renderer);
        FigureEntity->J_ShadowTexture  = GetTexture(Memory, "j_d_new_shadow.png", Buffer->Renderer);
        
        FigureEntity->Z_ClassicTexture = GetTexture(Memory, "z_d_new.png", Buffer->Renderer);
        FigureEntity->Z_ShadowTexture = GetTexture(Memory, "z_d_new_shadow.png", Buffer->Renderer);
        
        FigureEntity->S_ClassicTexture = GetTexture(Memory, "s_d_new.png", Buffer->Renderer);
        FigureEntity->S_ShadowTexture  = GetTexture(Memory, "s_d_new_shadow.png", Buffer->Renderer);
        
        FigureEntity->T_ClassicTexture = GetTexture(Memory, "t_d_new.png", Buffer->Renderer);
        FigureEntity->T_ShadowTexture  = GetTexture(Memory, "t_d_new_shadow.png", Buffer->Renderer);
        
        /* NOTE(msokolov): grid_entity initialization starts here */
        grid_entity *GridEntity = &Playground->GridEntity;
        GridEntity->RowAmount           = 8;
        GridEntity->ColumnAmount        = 6;
        GridEntity->StickUnitsAmount    = FigureEntity->FigureAmount;
        GridEntity->MovingBlocksAmount  = 1;
        GridEntity->MovingBlockVelocity = Configuration->MovingBlockVelocity;
        
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
        GridEntity->GridCell1Texture        = GetTexture(Memory, "grid_cell_new.png", Buffer->Renderer);
        GridEntity->GridCell2Texture        = GetTexture(Memory, "grid_cell_new2.png", Buffer->Renderer);
        
        GridEntity->VerticalSquareTexture   = GetTexture(Memory, "o_s.png", Buffer->Renderer);
        GridEntity->HorizontlaSquareTexture = GetTexture(Memory, "o_m.png", Buffer->Renderer);
        GridEntity->TopLeftCornerFrame      = GetTexture(Memory, "frame3.png", Buffer->Renderer);
        GridEntity->TopRightCornerFrame     = GetTexture(Memory, "frame4.png", Buffer->Renderer);
        GridEntity->DownLeftCornerFrame     = GetTexture(Memory, "frame2.png", Buffer->Renderer);
        GridEntity->DownRightCornerFrame    = GetTexture(Memory, "frame1.png", Buffer->Renderer);
        
        for (u32 BlockIndex = 0; 
             BlockIndex < GridEntity->MovingBlocksAmount;
             ++BlockIndex)
        {
            moving_block *Block = &GridEntity->MovingBlocks[BlockIndex];
            
            Block->Area.Min.x = GridEntity->GridArea.Min.x + (Block->ColNumber * GRID_BLOCK_SIZE);
            Block->Area.Min.y = GridEntity->GridArea.Min.y + (Block->RowNumber * GRID_BLOCK_SIZE);
            Block->Area.Max.x = Block->Area.Min.x + GRID_BLOCK_SIZE;
            Block->Area.Max.y = Block->Area.Min.y + GRID_BLOCK_SIZE;
            
            GridEntity->UnitField[(Block->RowNumber * COLUMN_AMOUNT_MAXIMUM) + Block->ColNumber] = 2;
        }
        
        Memory->IsInitialized = true;
        printf("Memory has been initialized!\n");
        
        /* NOTE(msokolov): just for testing */
        /* playground data from binary initialization */
        playground_data *PlaygroundData = (playground_data *) Memory->LevelStorage;
        Assert(PlaygroundData);
        
        GameState->PlaygroundData = PlaygroundData;
        
        /* NOTE(msokolov): playground_menu initialization */
        playground_menu *PlaygroundMenu = &GameState->PlaygroundMenu;
        PlaygroundMenu->MenuPage = menu_page::DIFFICULTY_PAGE;
        PlaygroundMenu->DiffMode = difficulty::EASY;
        PlaygroundMenu->Resolution = resolution_standard::FULLHD;
        PlaygroundMenu->IsFullScreen = false;
        PlaygroundMenu->PlaygroundSwitch = false;
        PlaygroundMenu->ButtonIndex = 1;
        
        PlaygroundMenu->LevelButtonTexture = GetTexture(Memory, "grid_cell.png", Buffer->Renderer);
        Assert(PlaygroundMenu->LevelButtonTexture);
        
        
        PlaygroundMenu->DifficultyTexture[0] = MakeTextureFromString(Buffer, GameState->TimerFont, "I", V4(255, 255, 255, 255));
        PlaygroundMenu->DifficultyShadowTexture[0] = MakeTextureFromString(Buffer, GameState->TimerFont, "I", V4(0, 0, 0, 128));
        
        PlaygroundMenu->DifficultyTexture[1] = MakeTextureFromString(Buffer, GameState->TimerFont, "II", V4(255, 255, 255, 255));
        PlaygroundMenu->DifficultyShadowTexture[1] = MakeTextureFromString(Buffer, GameState->TimerFont, "II", V4(0, 0, 0, 128));
        
        PlaygroundMenu->DifficultyTexture[2] = MakeTextureFromString(Buffer, GameState->TimerFont, "III", V4(255, 255, 255, 255));
        PlaygroundMenu->DifficultyShadowTexture[2] = MakeTextureFromString(Buffer, GameState->TimerFont, "III", V4(0, 0, 0, 128));
        
        for(u32 Index = 0;
            Index < PLAYGROUND_MAXIMUM;
            ++Index)
        {
            char TimeString[64] = {};
            GetTimeString(TimeString, GameState->PlaygroundData[Index].TimeElapsed);
            
            GameState->PlaygroundMenu.LevelNumberTexture[Index] = MakeTextureFromString(Buffer, GameState->MenuTimerFont, TimeString, {255, 255, 255, 255});
            
            GameState->PlaygroundMenu.LevelNumberShadowTexture[Index] = MakeTextureFromString(Buffer, GameState->MenuTimerFont, TimeString, {0, 0, 0, 255});
        }
        
        
        {
            PlaygroundMenu->MainMenuTexture[0] = MakeTextureFromString(Buffer, GameState->Font, "Play", V4(255, 255, 255, 255));
            PlaygroundMenu->MainMenuTexture[1] = MakeTextureFromString(Buffer, GameState->Font, "Settings", V4(255, 255, 255, 255));
            PlaygroundMenu->MainMenuTexture[2] = MakeTextureFromString(Buffer, GameState->Font, "Quit", V4(255, 255, 255, 255));
            
            PlaygroundMenu->MainMenuShadowTexture[0] = MakeTextureFromString(Buffer, GameState->Font, "Play", V4(0, 0, 0, 128));
            PlaygroundMenu->MainMenuShadowTexture[1] = MakeTextureFromString(Buffer, GameState->Font, "Settings", V4(0, 0, 0, 128));
            PlaygroundMenu->MainMenuShadowTexture[2] = MakeTextureFromString(Buffer, GameState->Font, "Quit", V4(0, 0, 0, 128));
        }
        
        PlaygroundMenu->CornerTexture[0] = GetTexture(Memory, "corner_left_top.png", Buffer->Renderer);
        PlaygroundMenu->CornerTexture[1] = GetTexture(Memory, "corner_left_bottom.png", Buffer->Renderer);
        PlaygroundMenu->CornerTexture[2] = GetTexture(Memory, "corner_right_top.png", Buffer->Renderer);
        PlaygroundMenu->CornerTexture[3] = GetTexture(Memory, "corner_right_bottom.png", Buffer->Renderer);
        PlaygroundMenu->HorizontalLineTexture = GetTexture(Memory, "horizontal_border_2.png", Buffer->Renderer);
        
        PlaygroundMenu->LevelCornerTexture[0] = GetTexture(Memory, "corner_menu_1.png", Buffer->Renderer);
        PlaygroundMenu->LevelCornerTexture[1] = GetTexture(Memory, "corner_menu_2.png", Buffer->Renderer);
        PlaygroundMenu->LevelCornerTexture[2] = GetTexture(Memory, "corner_menu_3.png", Buffer->Renderer);
        PlaygroundMenu->LevelCornerTexture[3] = GetTexture(Memory, "corner_menu_4.png", Buffer->Renderer);
        PlaygroundMenu->SquareFrameLocked = GetTexture(Memory, "square_frame_locked.png", Buffer->Renderer);
        PlaygroundMenu->SquareFrameUnlocked = GetTexture(Memory, "square_frame_unlocked.png", Buffer->Renderer);
        
        PlaygroundMenu->ColorBarTexture[0] = GetTexture(Memory, "green_bar.png", Buffer->Renderer);
        PlaygroundMenu->ColorBarTexture[1] = GetTexture(Memory, "blue_bar.png", Buffer->Renderer);
        PlaygroundMenu->ColorBarTexture[2] = GetTexture(Memory, "orange_bar.png", Buffer->Renderer);
        PlaygroundMenu->ColorBarTexture[3] = GetTexture(Memory, "red_bar.png", Buffer->Renderer);
        
        PlaygroundMenu->ResolutionNameTexture = MakeTextureFromString(Buffer, GameState->Font, "Resolution: ", V4(255, 255, 255, 128));
        PlaygroundMenu->ResolutionNameShadowTexture = MakeTextureFromString(Buffer, GameState->Font, "Resolution: ", V4(0, 0, 0, 255));
        
        PlaygroundMenu->ResolutionTexture[0] = MakeTextureFromString(Buffer, GameState->Font, "720p", V4(255, 255, 255, 255));
        PlaygroundMenu->ResolutionShadowTexture[0] = MakeTextureFromString(Buffer, GameState->Font, "720p", V4(0, 0, 0, 128));
        
        PlaygroundMenu->ResolutionTexture[1] = MakeTextureFromString(Buffer, GameState->Font, "1080p", V4(255, 255, 255, 255));
        PlaygroundMenu->ResolutionShadowTexture[1] = MakeTextureFromString(Buffer, GameState->Font, "1080p", V4(0, 0, 0, 128));
        
        PlaygroundMenu->ResolutionTexture[2] = MakeTextureFromString(Buffer, GameState->Font, "1440p", V4(255, 255, 255, 255));
        PlaygroundMenu->ResolutionShadowTexture[2] = MakeTextureFromString(Buffer, GameState->Font, "1440p", V4(0, 0, 0, 128));
        
        PlaygroundMenu->FullScreenNameTexture = MakeTextureFromString(Buffer, GameState->Font, "FullScreen: ", V4(255, 255, 255, 255));
        PlaygroundMenu->FullScreenNameShadowTexture = MakeTextureFromString(Buffer, GameState->Font, "FullScreen: ", V4(0, 0, 0, 128));
        
        PlaygroundMenu->FullScreenTexture[0] = MakeTextureFromString(Buffer, GameState->Font, "Off", V4(255, 255, 255, 255));
        PlaygroundMenu->FullScreenShadowTexture[0] = MakeTextureFromString(Buffer, GameState->Font, "Off", V4(0, 0, 0, 128));
        
        PlaygroundMenu->FullScreenTexture[1] = MakeTextureFromString(Buffer, GameState->Font, "On", V4(255, 255, 255, 255));
        PlaygroundMenu->FullScreenShadowTexture[1] = MakeTextureFromString(Buffer, GameState->Font, "On", V4(0, 0, 0, 128));
        
        PlaygroundMenu->BackTexture = MakeTextureFromString(Buffer, GameState->Font, "Back", V4(255, 255, 255, 255));
        PlaygroundMenu->BackShadowTexture = MakeTextureFromString(Buffer, GameState->Font, "Back", V4(0, 0, 0, 128));
        
        
#if DEBUG_BUILD
        
        GameState->EditorMode      = false;
        
        playground_editor *PlaygroundEditor = PushStruct(&GameState->MemoryGroup, playground_editor);
        GameState->PlaygroundEditor = PlaygroundEditor;
        
        PlaygroundEditor->Font = TTF_OpenFont(FontPath, 50);
        Assert(PlaygroundEditor->Font);
        
        PlaygroundEditor->SelectedArea = selected_area::GRID_PLAYGROUND;
        PlaygroundEditor->FigureIndex  = 0;
        
        PlaygroundEditor->FigureFormTexture   = MakeTextureFromString(Buffer, PlaygroundEditor->Font, "Shape", {255, 255, 255, 255});
        PlaygroundEditor->FigureRotateTexture = MakeTextureFromString(Buffer, PlaygroundEditor->Font, "Rotate", {255, 255, 255, 255});
        PlaygroundEditor->FigureAddTexture    = MakeTextureFromString(Buffer, PlaygroundEditor->Font, "Add", {255, 255, 255, 255});
        PlaygroundEditor->FigureDeleteTexture = MakeTextureFromString(Buffer, PlaygroundEditor->Font, "Delete", {255, 255, 255, 255});
        
        PlaygroundEditor->FigureButtonsArea.Min.x = FigureEntity->FigureArea.Min.x - 180.0f; 
        PlaygroundEditor->FigureButtonsArea.Min.y = FigureEntity->FigureArea.Min.y + (GetDim(FigureEntity->FigureArea).h / 2.0f) - (180.0f / 2.0f); 
        SetDim(&PlaygroundEditor->FigureButtonsArea, 180, 240);
        
        PlaygroundEditor->GridRowTexture = MakeTextureFromString(Buffer, PlaygroundEditor->Font, "Row", {255, 255, 255, 255});;
        PlaygroundEditor->GridColumnTexture = MakeTextureFromString(Buffer, PlaygroundEditor->Font, "Column", {255, 255, 255, 255});;
        PlaygroundEditor->GridPlusTexture = MakeTextureFromString(Buffer, PlaygroundEditor->Font, "+", {255, 255, 255, 255});;
        PlaygroundEditor->GridMinusTexture = MakeTextureFromString(Buffer, PlaygroundEditor->Font, "-", {255, 255, 255, 255});;
        
        PlaygroundEditor->GridButtonsArea.Min.x = FigureEntity->FigureArea.Min.x; 
        PlaygroundEditor->GridButtonsArea.Min.y = FigureEntity->FigureArea.Max.y - (GetDim(FigureEntity->FigureArea).h / 2.0f) - (60.0f);
        SetDim(&PlaygroundEditor->GridButtonsArea, 300.0f, 120.0f);
        
        //PlaygroundEditor->SavedTexture = MakeTextureFromString(Buffer, PlaygroundEditor->Font, "Saved.", {255, 255, 255, 255});
        
        // NOTE(msokolov): temporary
        PlaygroundData[0].IsUnlocked   = true;
        PlaygroundData[0].FigureAmount = 4;
        PlaygroundData[0].Figures[0].Form = figure_form::I_figure;
        PlaygroundData[0].Figures[0].Type = figure_type::classic;
        
        PlaygroundData[0].Figures[1].Form = figure_form::O_figure;
        PlaygroundData[0].Figures[1].Type = figure_type::classic;
        
        PlaygroundData[0].Figures[2].Form = figure_form::L_figure;
        PlaygroundData[0].Figures[2].Type = figure_type::classic;
        
        PlaygroundData[0].Figures[3].Form = figure_form::J_figure;
        PlaygroundData[0].Figures[3].Type = figure_type::classic;
        
        PlaygroundData[0].TimeElapsed  = 2.36f;
        PlaygroundData[0].RowAmount    = 4;
        PlaygroundData[0].ColumnAmount = 4;
        PlaygroundData[0].MovingBlocksAmount = 0;
        PlaygroundData[0].MovingBlocks[0].RowNumber = 1;
        PlaygroundData[0].MovingBlocks[0].ColNumber = 1;
        
        PrepareNextPlayground(Playground, Configuration, PlaygroundData, 0);
        
        char LevelString[64] = {};
        sprintf(LevelString, "%d", Playground->LevelNumber);
        
        Playground->LevelNumberTexture = MakeTextureFromString(Buffer, GameState->Font, LevelString, {255, 255, 255, 255});
        Assert(Playground->LevelNumberTexture);
        
        for (s32 Index = 1;
             Index < PLAYGROUND_MAXIMUM;
             ++Index)
        {
            PlaygroundData[Index].IsUnlocked = false;
        }
        
        //WriteLevelsToFile(Memory->LevelStorage, Memory->LevelStorageSize);
        
        PlaygroundEditor->IsInitialized = true;
#endif
        
        game_music *Music = GetMusic(Memory, "amb_ending_water.ogg");
#if 0
        if(Mix_PlayMusic(Music, -1) ==-1) {
            printf("Mix_PlayMusic: %s\n", Mix_GetError());
        }
#endif
        
    } // if (!Memory->IsInitialized)
    
    Assert(sizeof(transient_state) < Memory->TransientStorageSize);
    transient_state *TransState = (transient_state *)Memory->TransientStorage;
    if (!TransState->IsInitialized)
    {
        InitializeMemoryGroup(&TransState->TransGroup, Memory->TransientStorageSize - sizeof(transient_state), (u8*)Memory->TransientStorage + sizeof(transient_state));
        
        TransState->IsInitialized = true;
    }
    
    playground *Playground   = &GameState->Playground;
    playground_menu *PlaygroundMenu = &GameState->PlaygroundMenu;
    playground_data *PlaygroundData = GameState->PlaygroundData;
    
#if DEBUG_BUILD
    if(Input->Keyboard.BackQuote.EndedDown)
    {
        RestartLevelEntity(Playground);
        
        if (!GameState->EditorMode)
        {
            GameState->EditorMode   = true;
            Playground->LevelPaused = true;
        }
        else
        {
            GameState->EditorMode   = false;
            Playground->LevelPaused = false;
        }
    }
    
    if (GameState->EditorMode)
    {
        if(Input->Keyboard.E_Button.EndedDown)
        {
            u32 PlaygroundIndex = GameState->PlaygroundIndex;
            if (PlaygroundIndex < PLAYGROUND_MAXIMUM)
            {
                RestartLevelEntity(Playground);
                PlaygroundIndex++;
                
                PrepareNextPlayground(Playground, &GameState->Configuration, PlaygroundData, PlaygroundIndex);
                
                char LevelString[64] = {};
                sprintf(LevelString, "%d", Playground->LevelNumber);
                if (Playground->LevelNumberTexture)
                {
                    SDL_DestroyTexture(Playground->LevelNumberTexture);
                }
                
                Playground->LevelNumberTexture = MakeTextureFromString(Buffer, GameState->Font, LevelString, {255, 255, 255, 255});
                Assert(Playground->LevelNumberTexture);
                
                printf("LevelIndex = %d\n", PlaygroundIndex);
            }
            
            GameState->PlaygroundEditor->FigureIndex = 0;
            GameState->PlaygroundIndex = PlaygroundIndex;
        }
        
        if (Input->Keyboard.Q_Button.EndedDown)
        {
            u32 PlaygroundIndex = GameState->PlaygroundIndex;
            if (PlaygroundIndex > 0)
            {
                RestartLevelEntity(Playground);
                
                PlaygroundIndex--;
                
                PrepareNextPlayground(Playground, &GameState->Configuration, PlaygroundData, PlaygroundIndex);
                
                char LevelString[64] = {};
                sprintf(LevelString, "%d", Playground->LevelNumber);
                if (Playground->LevelNumberTexture)
                {
                    SDL_DestroyTexture(Playground->LevelNumberTexture);
                }
                
                Playground->LevelNumberTexture = MakeTextureFromString(Buffer, GameState->Font, LevelString, {255, 255, 255, 255});
                Assert(Playground->LevelNumberTexture);
                printf("LevelIndex = %d\n", PlaygroundIndex);
            }
            
            GameState->PlaygroundEditor->FigureIndex = 0;
            GameState->PlaygroundIndex = PlaygroundIndex;
        }
        
        if(Input->Keyboard.S_Button.EndedDown)
        {
            WritePlaygroundData(PlaygroundData, Playground, GameState->PlaygroundIndex);
            WriteLevelsToFile(Memory->LevelStorage, Memory->LevelStorageSize);
            printf("saved.\n");
        }
    }
    
#endif
    
    /*
    NOTE(msokolov): Render 
    */
    
    memory_group TemporaryMemory = TransState->TransGroup;
    render_group *RenderGroup = AllocateRenderGroup(&TransState->TransGroup, Kilobytes(100), Buffer->Width, Buffer->Height);
    
    game_mode CurrentMode = GameState->CurrentMode;
    switch (CurrentMode)
    {
        case PLAYGROUND:
        {
            playground_status PlaygroundStatus = PlaygroundUpdateAndRender(Playground, RenderGroup, Input);
            
            if (PlaygroundStatus == playground_status::LEVEL_RUNNING)
            {
                Playground->TimeElapsed += Input->dtForFrame;
                
                if (Playground->ShowTimer)
                {
                    char TimeString[64] = {};
                    GetTimeString(TimeString, Playground->TimeElapsed);
                    
                    if (Playground->TimerShadowTexture)
                        SDL_DestroyTexture(Playground->TimerShadowTexture);
                    
                    if (Playground->TimerTexture)
                        SDL_DestroyTexture(Playground->TimerTexture);
                    
                    Playground->TimerShadowTexture = MakeTextureFromString(Buffer, GameState->TimerFont, TimeString, V4(0, 0, 0, 128));
                    Playground->TimerTexture       = MakeTextureFromString(Buffer, GameState->TimerFont, TimeString, V4(255, 255, 255, 255));
                    
                    rectangle2 TimerRectangle = {};
                    TimerRectangle.Min.x = (VIRTUAL_GAME_WIDTH * 0.5f) - (25.0f);
                    TimerRectangle.Min.y = 50.0f;
                    SetDim(&TimerRectangle, 50.0f, 50.0f);
                    
                    TimerRectangle = GetTextOnTheCenterOfRectangle(TimerRectangle, Playground->TimerTexture);
                    
                    TimerRectangle.Min += V2(5.0f, 5.0f);
                    TimerRectangle.Max += V2(5.0f, 5.0f);
                    PushBitmap(RenderGroup, Playground->TimerShadowTexture, TimerRectangle);
                    
                    TimerRectangle.Min -= V2(5.0f, 5.0f);
                    TimerRectangle.Max -= V2(5.0f, 5.0f);
                    PushBitmap(RenderGroup, Playground->TimerTexture, TimerRectangle);
                }
                
                v2 IndicatorSize = {30.0f, 30.0f};
                u32 IndicatorAmount = 8;
                
                s32 RowLevelStart = GameState->PlaygroundIndex - (GameState->PlaygroundIndex % IndicatorAmount);
                for (s32 Index = 0; 
                     Index < IndicatorAmount;
                     ++Index)
                {
                    rectangle2 Rectangle = {};
                    Rectangle.Min.x = (VIRTUAL_GAME_WIDTH * 0.5f) - (IndicatorSize.w * (IndicatorAmount / 2)) - ((IndicatorSize.w) * (IndicatorAmount / 2));
                    Rectangle.Min.x += (Index * IndicatorSize.w) + ((IndicatorSize.w) * Index);
                    Rectangle.Min.y = VIRTUAL_GAME_HEIGHT - (70.0f);
                    SetDim(&Rectangle, IndicatorSize);
                    
                    PushBitmap(RenderGroup, Playground->IndicatorEmptyTexture, Rectangle);
                    b32 IsUnlocked = PlaygroundData[RowLevelStart + Index].IsUnlocked;
                    if ((GameState->PlaygroundIndex % IndicatorAmount) + 1 > Index)
                    {
                        PushBitmap(RenderGroup, Playground->IndicatorFilledTexture, Rectangle);
                    }
                }
                
            }
            else if (PlaygroundStatus == playground_status::LEVEL_FINISHED)
            {
                if (GameState->PlaygroundIndex < PLAYGROUND_MAXIMUM)
                {
                    u32 PlaygroundIndex = GameState->PlaygroundIndex;
                    if (Playground->TimeElapsed < PlaygroundData[PlaygroundIndex].TimeElapsed
                        || PlaygroundData[PlaygroundIndex].TimeElapsed == 0.0f)
                    {
                        PlaygroundData[PlaygroundIndex].TimeElapsed = Playground->TimeElapsed;
                        RestartLevelEntity(Playground);
                        
                        char TimeString[64] = {};
                        GetTimeString(TimeString, PlaygroundData[PlaygroundIndex].TimeElapsed);
                        
                        PlaygroundMenu->LevelNumberTexture[PlaygroundIndex] = MakeTextureFromString(Buffer, GameState->MenuTimerFont, TimeString, {255, 255, 255, 255});
                        
                        PlaygroundMenu->LevelNumberShadowTexture[PlaygroundIndex] = MakeTextureFromString(Buffer, GameState->MenuTimerFont, TimeString, {0, 0, 0, 255});
                    }
                    
                    if (PlaygroundIndex < PLAYGROUND_MAXIMUM)
                    {
                        GameState->PlaygroundIndex = ++PlaygroundIndex;
                        PlaygroundData[PlaygroundIndex].IsUnlocked = true;
                        
                        PrepareNextPlayground(Playground, &GameState->Configuration, PlaygroundData, GameState->PlaygroundIndex);
                        
                        RestartLevelEntity(Playground);
#if DEBUG_BUILD
                        char LevelString[64] = {};
                        sprintf(LevelString, "%d", Playground->LevelNumber);
                        if (Playground->LevelNumberTexture)
                        {
                            SDL_DestroyTexture(Playground->LevelNumberTexture);
                        }
                        
                        Playground->LevelNumberTexture = MakeTextureFromString(Buffer, GameState->Font, LevelString, {255, 255, 255, 255});
                        Assert(Playground->LevelNumberTexture);
#endif
                    }
                    
                }
            }
            else if (PlaygroundStatus == playground_status::LEVEL_MENU_QUIT)
            {
                GameState->CurrentMode = MENU;
                PlaygroundMenu->MenuPage = menu_page::DIFFICULTY_PAGE;
                SDL_ShowCursor(SDL_ENABLE);
            }
            else if (PlaygroundStatus == playground_status::LEVEL_GAME_QUIT)
            {
                Result.ShouldQuit = true;
            }
            else if (PlaygroundStatus == playground_status::LEVEL_SETTINGS_QUIT)
            {
                GameState->CurrentMode = MENU;
                PlaygroundMenu->MenuPage = menu_page::SETTINGS_PAGE;
                PlaygroundMenu->PlaygroundSwitch = true;
            }
            else if (PlaygroundStatus == playground_status::LEVEL_RESTARTED)
            {
                RestartLevelEntity(Playground);
            }
            
#if DEBUG_BUILD
            if (GameState->EditorMode)
            {
                PlaygroundEditorUpdateAndRender(Playground, PlaygroundData, &GameState->Configuration, GameState->PlaygroundEditor, RenderGroup, Input);
            }
#endif
            
        } break;
        
        case MENU:
        {
            menu_result_option MenuResult = PlaygroundMenuUpdateAndRender(PlaygroundMenu, PlaygroundData, Input, RenderGroup);
            
            if (MenuResult.SwitchToPlayground)
            {
                if (PlaygroundMenu->PlaygroundSwitch)
                {
                    PlaygroundMenu->PlaygroundSwitch = false;
                }
                else
                {
                    u32 ResultLevelIndex = MenuResult.PlaygroundIndex;
                    RestartLevelEntity(Playground);
                    GameState->PlaygroundIndex = ResultLevelIndex;
                    
                    PrepareNextPlayground(Playground, &GameState->Configuration, PlaygroundData, ResultLevelIndex);
                    
#if DEBUG_BUILD
                    char LevelString[64] = {};
                    sprintf(LevelString, "%d", Playground->LevelNumber);
                    if (Playground->LevelNumberTexture)
                    {
                        SDL_DestroyTexture(Playground->LevelNumberTexture);
                    }
                    
                    Playground->LevelNumberTexture = MakeTextureFromString(Buffer, GameState->Font, LevelString, {255, 255, 255, 255});
                    Assert(Playground->LevelNumberTexture);
#endif
                }
                
                GameState->CurrentMode = PLAYGROUND;
            }
            if (MenuResult.QuitGame)
            {
                Result.ShouldQuit = true;
            }
            if (MenuResult.ToggleFullScreen)
            {
                Result.ToggleFullScreen = MenuResult.ToggleFullScreen;
            }
            if (MenuResult.ChangeResolution)
            {
                Result.ChangeResolution = true;
                Result.Resolution = MenuResult.Resolution;
            }
        } break;
    }
    
    RenderGroupToOutput(RenderGroup, Buffer);
    TransState->TransGroup = TemporaryMemory;
    
    return(Result);
}
