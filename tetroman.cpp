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
        /* initialize random seed: */
        srand (time(NULL));
        
        // NOTE(msokolov): figure colors 
        // 212, 151, 0
        // 212, 10,  128
        // 116, 0,   40
        // 23,  156, 234
        // 108, 174, 0
        
        /* NOTE(msokolov): game_memory initialization starts here */
        /* NOTE(msokolov): game_state initialization starts here */
        InitializeMemoryGroup(&GameState->MemoryGroup, Memory->PermanentStorageSize - sizeof(game_state), (u8*)Memory->PermanentStorage + sizeof(game_state));
        
        r32 WidthRatio = 0.00052083333f;
        r32 HeightRatio = 0.00092592592;
        
		{
            r32 StandardSize = 50.0f;
            r32 PixelHeight = roundf(StandardSize * HeightRatio * Buffer->ViewportHeight);
            GameState->Font = TTF_OpenFont(FontPath, PixelHeight);
            Assert(GameState->Font);
        }
        
        {
            r32 StandardSize = 80.0f;
            r32 PixelHeight = roundf(StandardSize * HeightRatio * Buffer->ViewportHeight);
            GameState->TimerFont = TTF_OpenFont(FontPath, PixelHeight);
            Assert(GameState->TimerFont);
        }
        
		{
            r32 StandardSize = 30.0f;
            r32 PixelHeight = roundf(StandardSize * HeightRatio * Buffer->ViewportHeight);
            GameState->MenuTimerFont = TTF_OpenFont(FontPath, PixelHeight);
            Assert(GameState->MenuTimerFont);
        }
        
        {
            r32 StandardSize = 100.0f;
            r32 PixelHeight = roundf(StandardSize * HeightRatio * Buffer->ViewportHeight);
            GameState->MenuDifficultyFont = TTF_OpenFont(FontPath, PixelHeight);
            Assert(GameState->MenuTimerFont);
        }
		
        GameState->PlaygroundIndex = 0;
        GameState->CurrentMode     = game_mode::MENU;
		
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
        
        Playground->BackgroundTexture          = GetTexture(Memory, "background_.png", Buffer->Renderer);
        Playground->BackgroundDimTexture       = GetTexture(Memory, "background_dimmed.png", Buffer->Renderer);
        Playground->CornerLeftTopTexture       = GetTexture(Memory, "corner_left_top1.png", Buffer->Renderer, LINEAR_SCALE);
        Playground->CornerLeftTopShadowTexture = GetTexture(Memory, "corner_left_top_shadow.png", Buffer->Renderer, NEAREST_SCALE);
        Playground->VerticalBorderTexture      = GetTexture(Memory, "vertical_border.png", Buffer->Renderer, LINEAR_SCALE);
        Playground->IndicatorEmptyTexture      = GetTexture(Memory, "level_indicator_empty.png", Buffer->Renderer, LINEAR_SCALE);
        Playground->IndicatorFilledTexture     = GetTexture(Memory, "level_indicator_filled.png", Buffer->Renderer, LINEAR_SCALE);
        
        SDL_SetTextureBlendMode(Playground->CornerLeftTopTexture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(Playground->CornerLeftBottomTexture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(Playground->CornerRightTopTexture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(Playground->CornerRightBottomTexture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(Playground->VerticalBorderTexture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(Playground->BackgroundDimTexture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(Playground->BackgroundTexture, SDL_BLENDMODE_BLEND);
        
        Playground->DropSound     = GetSound(Memory, "figure_rotate.wav");
        Playground->PickSound     = GetSound(Memory, "figure_pick.wav");
        Playground->StickSound    = GetSound(Memory, "figure_drop.wav");
        Playground->RotateSound   = GetSound(Memory, "figure_rotation.wav");
        Playground->CompleteSound = GetSound(Memory, "complete_sound2.wav");
        Mix_VolumeChunk(Playground->CompleteSound, MIX_MAX_VOLUME * 0.1f);
        Mix_VolumeChunk(Playground->PickSound, MIX_MAX_VOLUME * 0.1f);
        Mix_VolumeChunk(Playground->StickSound, MIX_MAX_VOLUME * 0.1f);
        Mix_VolumeChunk(Playground->DropSound, MIX_MAX_VOLUME * 0.1f);
        Mix_VolumeChunk(Playground->RotateSound, MIX_MAX_VOLUME * 0.2f);
        
        Playground->Music[0] = GetMusic(Memory, "music_1.ogg");
        Playground->Music[1] = GetMusic(Memory, "music_2.ogg");
        Playground->Music[2] = GetMusic(Memory, "music_3.ogg");
        Playground->Music[3] = GetMusic(Memory, "music_4.ogg");
        Playground->Music[4] = GetMusic(Memory, "music_5.ogg");
        Playground->Music[5] = GetMusic(Memory, "music_6.ogg");
        Playground->PrevMusicIndex = -1;
        
        /* Playground Option Menu */ 
        playground_options *PlaygroundOptions = &Playground->Options;
        
        PlaygroundOptions->ToggleMenu  = false;
        PlaygroundOptions->InterpPoint = 0.0f;
        PlaygroundOptions->Choice      = options_choice::RESTART_OPTION;
        PlaygroundOptions->MenuPosition    = V2(1500.0f, 650.0f);
        PlaygroundOptions->ButtonDimension = V2(180.0f, 80.0f);
        
        PlaygroundOptions->GearTexture             = GetTexture(Memory, "gear_new.png", Buffer->Renderer, LINEAR_SCALE);
        PlaygroundOptions->GearShadowTexture       = GetTexture(Memory, "gear_new_shadow.png", Buffer->Renderer, LINEAR_SCALE);
        PlaygroundOptions->HorizontalLineTexture   = GetTexture(Memory, "horizontal_border_2.png", Buffer->Renderer, LINEAR_SCALE);
        PlaygroundOptions->MenuTexture[0] = MakeTextureFromString(Buffer, GameState->Font, "Restart", V4(255, 255, 255, 255), LINEAR_SCALE);
        PlaygroundOptions->MenuTexture[1] = MakeTextureFromString(Buffer, GameState->Font, "Settings", V4(255, 255, 255, 255), LINEAR_SCALE);
        PlaygroundOptions->MenuTexture[2] = MakeTextureFromString(Buffer, GameState->Font, "Menu", V4(255, 255, 255, 255), LINEAR_SCALE);
        PlaygroundOptions->MenuTexture[3] = MakeTextureFromString(Buffer, GameState->Font, "Quit", V4(255, 255, 255, 255), LINEAR_SCALE);
        
        PlaygroundOptions->MenuShadowTexture[0] = MakeTextureFromString(Buffer, GameState->Font, "Restart", V4(0, 0, 0, 127), LINEAR_SCALE);
        PlaygroundOptions->MenuShadowTexture[1] = MakeTextureFromString(Buffer, GameState->Font, "Settings", V4(0, 0, 0, 127), LINEAR_SCALE);
        PlaygroundOptions->MenuShadowTexture[2] = MakeTextureFromString(Buffer, GameState->Font, "Menu", V4(0, 0, 0, 127), LINEAR_SCALE);
        PlaygroundOptions->MenuShadowTexture[3] = MakeTextureFromString(Buffer, GameState->Font, "Quit", V4(0, 0, 0, 127), LINEAR_SCALE);
        
        PlaygroundOptions->GearSound = GetSound(Memory, "gear_sound.wav");
        Mix_VolumeChunk(PlaygroundOptions->GearSound, MIX_MAX_VOLUME * 0.1f);
        
        // Animation control
        Playground->Animation.Finished = false;
        Playground->Animation.InterpPoint = 0.0f;
        Playground->Animation.TimeMax = 1.0f;
        
        Playground->GearIsRotating = false;
        Playground->GearRotationSum = 0.0f;
        Playground->GearAngle = 0.0f;
        
        /* NOTE(msokolov): figure_entity initialization starts here */
        figure_entity* FigureEntity  = &Playground->FigureEntity;
        FigureEntity->FigureAmount   = 0;
        FigureEntity->ReturnIndex    = -1;
        FigureEntity->FigureActive   = -1;
        FigureEntity->FigureOutline  = -1;
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
        
        FigureEntityAlignFigures(&Playground->FigureEntity);
        
        for(u32 i = 0; i < FIGURE_AMOUNT_MAXIMUM; ++i) 
        {
            FigureEntity->FigureOrder[i] = i;
        }
        
        FigureEntity->BlueTexture[0] = GetTexture(Memory, "o_blue.png", Buffer->Renderer);
        FigureEntity->BlueTexture[1] = GetTexture(Memory, "i_blue.png", Buffer->Renderer);
        FigureEntity->BlueTexture[2] = GetTexture(Memory, "l_blue.png", Buffer->Renderer);
        FigureEntity->BlueTexture[3] = GetTexture(Memory, "j_blue.png", Buffer->Renderer);
        FigureEntity->BlueTexture[4] = GetTexture(Memory, "z_blue.png", Buffer->Renderer);
        FigureEntity->BlueTexture[5] = GetTexture(Memory, "s_blue.png", Buffer->Renderer);
        FigureEntity->BlueTexture[6] = GetTexture(Memory, "t_blue.png", Buffer->Renderer);
        
        FigureEntity->GreenTexture[0] = GetTexture(Memory, "o_green.png", Buffer->Renderer);
        FigureEntity->GreenTexture[1] = GetTexture(Memory, "i_green.png", Buffer->Renderer);
        FigureEntity->GreenTexture[2] = GetTexture(Memory, "l_green.png", Buffer->Renderer);
        FigureEntity->GreenTexture[3] = GetTexture(Memory, "j_green.png", Buffer->Renderer);
        FigureEntity->GreenTexture[4] = GetTexture(Memory, "z_green.png", Buffer->Renderer);
        FigureEntity->GreenTexture[5] = GetTexture(Memory, "s_green.png", Buffer->Renderer);
        FigureEntity->GreenTexture[6] = GetTexture(Memory, "t_green.png", Buffer->Renderer);
        
        FigureEntity->OrangeTexture[0] = GetTexture(Memory, "o_orange.png", Buffer->Renderer);
        FigureEntity->OrangeTexture[1] = GetTexture(Memory, "i_orange.png", Buffer->Renderer);
        FigureEntity->OrangeTexture[2] = GetTexture(Memory, "l_orange.png", Buffer->Renderer);
        FigureEntity->OrangeTexture[3] = GetTexture(Memory, "j_orange.png", Buffer->Renderer);
        FigureEntity->OrangeTexture[4] = GetTexture(Memory, "z_orange.png", Buffer->Renderer);
        FigureEntity->OrangeTexture[5] = GetTexture(Memory, "s_orange.png", Buffer->Renderer);
        FigureEntity->OrangeTexture[6] = GetTexture(Memory, "t_orange.png", Buffer->Renderer);
        
        FigureEntity->RedTexture[0] = GetTexture(Memory, "o_red.png", Buffer->Renderer);
        FigureEntity->RedTexture[1] = GetTexture(Memory, "i_red.png", Buffer->Renderer);
        FigureEntity->RedTexture[2] = GetTexture(Memory, "l_red.png", Buffer->Renderer);
        FigureEntity->RedTexture[3] = GetTexture(Memory, "j_red.png", Buffer->Renderer);
        FigureEntity->RedTexture[4] = GetTexture(Memory, "z_red.png", Buffer->Renderer);
        FigureEntity->RedTexture[5] = GetTexture(Memory, "s_red.png", Buffer->Renderer);
        FigureEntity->RedTexture[6] = GetTexture(Memory, "t_red.png", Buffer->Renderer);
        
        FigureEntity->ShadowTexture[0] = GetTexture(Memory, "o_shadow.png", Buffer->Renderer);
        FigureEntity->ShadowTexture[1] = GetTexture(Memory, "i_shadow.png", Buffer->Renderer);
        FigureEntity->ShadowTexture[2] = GetTexture(Memory, "l_shadow.png", Buffer->Renderer);
        FigureEntity->ShadowTexture[3] = GetTexture(Memory, "j_shadow.png", Buffer->Renderer);
        FigureEntity->ShadowTexture[4] = GetTexture(Memory, "z_shadow.png", Buffer->Renderer);
        FigureEntity->ShadowTexture[5] = GetTexture(Memory, "s_shadow.png", Buffer->Renderer);
        FigureEntity->ShadowTexture[6] = GetTexture(Memory, "t_shadow.png", Buffer->Renderer);
        
        FigureEntity->OutlineTexture[0] = GetTexture(Memory, "o_outline.png", Buffer->Renderer);
        FigureEntity->OutlineTexture[1] = GetTexture(Memory, "i_outline.png", Buffer->Renderer);
        FigureEntity->OutlineTexture[2] = GetTexture(Memory, "l_outline.png", Buffer->Renderer);
        FigureEntity->OutlineTexture[3] = GetTexture(Memory, "j_outline.png", Buffer->Renderer);
        FigureEntity->OutlineTexture[4] = GetTexture(Memory, "z_outline.png", Buffer->Renderer);
        FigureEntity->OutlineTexture[5] = GetTexture(Memory, "s_outline.png", Buffer->Renderer);
        FigureEntity->OutlineTexture[6] = GetTexture(Memory, "t_outline.png", Buffer->Renderer);
        
        FigureEntity->GroundTexture[0] = GetTexture(Memory, "o_ground.png", Buffer->Renderer);
        FigureEntity->GroundTexture[1] = GetTexture(Memory, "i_ground.png", Buffer->Renderer);
        FigureEntity->GroundTexture[2] = GetTexture(Memory, "l_ground.png", Buffer->Renderer);
        FigureEntity->GroundTexture[3] = GetTexture(Memory, "j_ground.png", Buffer->Renderer);
        FigureEntity->GroundTexture[4] = GetTexture(Memory, "z_ground.png", Buffer->Renderer);
        FigureEntity->GroundTexture[5] = GetTexture(Memory, "s_ground.png", Buffer->Renderer);
        FigureEntity->GroundTexture[6] = GetTexture(Memory, "t_ground.png", Buffer->Renderer);
        
        for (int i = 0; i < 7; ++i) {
            SDL_SetTextureBlendMode(FigureEntity->GroundTexture[i], SDL_BLENDMODE_BLEND);
            SDL_SetTextureAlphaMod(FigureEntity->GroundTexture[i], 100.0f);
        }
        
        /* NOTE(msokolov): grid_entity initialization starts here */
        grid_entity *GridEntity = &Playground->GridEntity;
        GridEntity->RowAmount           = 8;
        GridEntity->ColumnAmount        = 6;
        GridEntity->StickUnitsAmount    = FigureEntity->FigureAmount;
        GridEntity->MovingBlocksAmount  = 1;
        GridEntity->MovingBlockVelocity = Configuration->MovingBlockVelocity;
        
        GridEntity->GridArea.Min.x = 100.0f;
        GridEntity->GridArea.Min.y = 81.0f;
        GridEntity->GridArea.Max.x = GridEntity->GridArea.Min.x + 1128.0f;
        GridEntity->GridArea.Max.y = GridEntity->GridArea.Min.y + 972.0f;
        
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
        
        GridEntity->GridCell1Texture        = GetTexture(Memory, "grid_cell_new.png", Buffer->Renderer, LINEAR_SCALE);
        GridEntity->GridCell2Texture        = GetTexture(Memory, "grid_cell_new2.png", Buffer->Renderer, LINEAR_SCALE);
        
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
        
        /* player_data from binary initialization */
        player_data *PlayerData = (player_data *) Memory->SettingsStorage;
        Assert(PlayerData);
        
        // The first level should always be unlocked
        PlayerData->PlaygroundUnlocked[0] = true;
        GameState->PlayerData = PlayerData;
        
        game_settings *Settings = &PlayerData->Settings;
        
        /* playground data from binary initialization */
        playground_data *PlaygroundData = (playground_data *) Memory->LevelStorage;
        Assert(PlaygroundData);
        
        GameState->PlaygroundData = PlaygroundData;
        
        /* NOTE(msokolov): playground_menu initialization */
        playground_menu *PlaygroundMenu = &GameState->PlaygroundMenu;
        PlaygroundMenu->MenuPage = menu_page::MAIN_PAGE;
        PlaygroundMenu->DiffMode = difficulty::EASY;
        PlaygroundMenu->PlaygroundSwitch = false;
        PlaygroundMenu->ButtonIndex = 1;
        PlaygroundMenu->AnimationFinished = false;
        PlaygroundMenu->AnimationFinishedHalf = true;
        PlaygroundMenu->InterpPoint = 0.0f;
        PlaygroundMenu->InterpPointDiff = 1.0f;
        PlaygroundMenu->TimeMax = 0.2f;
        
        PlaygroundMenu->ForwardAnimation = false;
        PlaygroundMenu->InterpPointNext  = 1.0f;
        
        PlaygroundMenu->MenuDown = GetSound(Memory, "menu_slider_down.wav");
        Mix_VolumeChunk(PlaygroundMenu->MenuDown, MIX_MAX_VOLUME * 0.1f);
        
        PlaygroundMenu->MenuUp   = GetSound(Memory, "menu_slider_up.wav");
        Mix_VolumeChunk(PlaygroundMenu->MenuUp, MIX_MAX_VOLUME * 0.1f);
        
        PlaygroundMenu->SoundOn = Settings->SoundIsOn;
        PlaygroundMenu->MusicOn = Settings->MusicIsOn;
        
        PlaygroundMenu->BackgroundArea = {V2((r32)VIRTUAL_GAME_WIDTH * 0.5f, 0.0f), V2((r32) VIRTUAL_GAME_WIDTH * 0.5f, VIRTUAL_GAME_HEIGHT)};
        
        PlaygroundMenu->BackgroundTexture = GetTexture(Memory, "background_.png", Buffer->Renderer);
        PlaygroundMenu->LevelButtonTexture = GetTexture(Memory, "grid_cell.png", Buffer->Renderer);
        Assert(PlaygroundMenu->LevelButtonTexture);
        
        v2 Canvas = V2((8 * 150.0f) + 30.0f + 10.0f, 4 * 100.0f);
        v2 SizeRatio = V2(WidthRatio, HeightRatio);
        v2 NDC = V2(SizeRatio * Canvas);
        v2 RealSizeCanvas = V2(NDC.x * Buffer->ViewportWidth, NDC.y * Buffer->ViewportHeight);
        
        PlaygroundMenu->LevelsCanvasTexture = SDL_CreateTexture(Buffer->Renderer, SDL_PIXELFORMAT_RGBA8888,
                                                                SDL_TEXTUREACCESS_TARGET, RealSizeCanvas.w, RealSizeCanvas.h);
        PlaygroundMenu->DifficultyTexture[0] = MakeTextureFromString(Buffer, GameState->MenuDifficultyFont, "I", V4(255.0f, 255.0f, 255.0f, 255.0f), LINEAR_SCALE);
        PlaygroundMenu->DifficultyShadowTexture[0] = MakeTextureFromString(Buffer, GameState->MenuDifficultyFont, "I", V4(0, 0, 0, 128), LINEAR_SCALE);
        
        PlaygroundMenu->DifficultyTexture[1] = MakeTextureFromString(Buffer, GameState->MenuDifficultyFont, "II", V4(255, 255, 255, 255), LINEAR_SCALE);
        PlaygroundMenu->DifficultyShadowTexture[1] = MakeTextureFromString(Buffer, GameState->MenuDifficultyFont, "II", V4(0, 0, 0, 128), LINEAR_SCALE);
        
        PlaygroundMenu->DifficultyTexture[2] = MakeTextureFromString(Buffer, GameState->MenuDifficultyFont, "III", V4(255, 255, 255, 255), LINEAR_SCALE);
        PlaygroundMenu->DifficultyShadowTexture[2] = MakeTextureFromString(Buffer, GameState->MenuDifficultyFont, "III", V4(0, 0, 0, 128), LINEAR_SCALE);
        
        for(u32 Index = 0;
            Index < PLAYGROUND_MAXIMUM;
            ++Index) {
            
            char TimeString[64] = {};
            GetTimeString(TimeString, PlayerData->PlaygroundTime[Index]);
            
            GameState->PlaygroundMenu.LevelTimeTexture[Index]       = MakeTextureFromString(Buffer, GameState->MenuTimerFont, TimeString, V4(255, 255, 255, 255), LINEAR_SCALE);
            GameState->PlaygroundMenu.LevelTimeShadowTexture[Index] = MakeTextureFromString(Buffer, GameState->MenuTimerFont, TimeString, V4(0, 0, 0, 255), LINEAR_SCALE);
            
            char LevelString[8] = {};
            sprintf(LevelString, "%d", Index + 1);
            
            GameState->PlaygroundMenu.LevelNumberTexture[Index]       = MakeTextureFromString(Buffer, GameState->MenuTimerFont, LevelString, V4(255.0f, 255.0f, 255.0f, 255.0f), LINEAR_SCALE);
            GameState->PlaygroundMenu.LevelNumberShadowTexture[Index] = MakeTextureFromString(Buffer, GameState->MenuTimerFont, LevelString, V4(0.0f, 0.0f, 0.0f, 255.0f), LINEAR_SCALE);
        }
        
        PlaygroundMenu->NextLevelTexture = GetTexture(Memory, "next_level_indicator.png", Buffer->Renderer, LINEAR_SCALE);
        PlaygroundMenu->NextLevelBackgroundTexture = GetTexture(Memory, "square_frame_unlocked.png", Buffer->Renderer, LINEAR_SCALE);
        
        PlaygroundMenu->MainMenuTexture[0] = MakeTextureFromString(Buffer, GameState->Font, "Play", V4(255, 255, 255, 255), LINEAR_SCALE);
        PlaygroundMenu->MainMenuTexture[1] = MakeTextureFromString(Buffer, GameState->Font, "Settings", V4(255, 255, 255, 255), LINEAR_SCALE);
        PlaygroundMenu->MainMenuTexture[2] = MakeTextureFromString(Buffer, GameState->Font, "Quit", V4(255, 255, 255, 255), LINEAR_SCALE);
        
        PlaygroundMenu->MainMenuShadowTexture[0] = MakeTextureFromString(Buffer, GameState->Font, "Play", V4(0, 0, 0, 128), LINEAR_SCALE);
        PlaygroundMenu->MainMenuShadowTexture[1] = MakeTextureFromString(Buffer, GameState->Font, "Settings", V4(0, 0, 0, 128), LINEAR_SCALE);
        PlaygroundMenu->MainMenuShadowTexture[2] = MakeTextureFromString(Buffer, GameState->Font, "Quit", V4(0, 0, 0, 128), LINEAR_SCALE);
        
        PlaygroundMenu->CornerTexture       = GetTexture(Memory, "corner_left_top1.png", Buffer->Renderer, LINEAR_SCALE);
        PlaygroundMenu->CornerShadowTexture = GetTexture(Memory, "corner_left_top_shadow.png", Buffer->Renderer, LINEAR_SCALE);
        
        PlaygroundMenu->HorizontalLineTexture = GetTexture(Memory, "horizontal_border_2.png", Buffer->Renderer, LINEAR_SCALE);
        
        PlaygroundMenu->LevelCornerTexture = GetTexture(Memory, "corner_menu_left_top.png", Buffer->Renderer, LINEAR_SCALE);
        PlaygroundMenu->LevelCornerShadowTexture = GetTexture(Memory, "corner_menu_left_top_shadow.png", Buffer->Renderer, LINEAR_SCALE);
        
        PlaygroundMenu->SquareFrameLocked   = GetTexture(Memory, "square_frame_locked.png", Buffer->Renderer, LINEAR_SCALE);
        PlaygroundMenu->SquareFrameUnlocked = GetTexture(Memory, "square_frame_unlocked.png", Buffer->Renderer, LINEAR_SCALE);
        
        PlaygroundMenu->ColorBarTexture[0] = GetTexture(Memory, "blue_bar.png", Buffer->Renderer, LINEAR_SCALE);
        PlaygroundMenu->ColorBarTexture[1] = GetTexture(Memory, "green_bar.png", Buffer->Renderer, LINEAR_SCALE);
        PlaygroundMenu->ColorBarTexture[2] = GetTexture(Memory, "orange_bar.png", Buffer->Renderer, LINEAR_SCALE);
        PlaygroundMenu->ColorBarTexture[3] = GetTexture(Memory, "red_bar.png", Buffer->Renderer, LINEAR_SCALE);
        
        PlaygroundMenu->SoundNameTexture = MakeTextureFromString(Buffer, GameState->Font, "Sound: ", V4(255, 255, 255, 255), LINEAR_SCALE);
        PlaygroundMenu->SoundNameShadowTexture = MakeTextureFromString(Buffer, GameState->Font, "Sound: ", V4(0, 0, 0, 255), LINEAR_SCALE);
        
        PlaygroundMenu->MusicNameTexture = MakeTextureFromString(Buffer, GameState->Font, "Music: ", V4(255, 255, 255, 255), LINEAR_SCALE);
        PlaygroundMenu->MusicNameShadowTexture = MakeTextureFromString(Buffer, GameState->Font, "Music: ", V4(0, 0, 0, 255), LINEAR_SCALE);
        
        // For sound and music settings
        PlaygroundMenu->SoundOnTexture = MakeTextureFromString(Buffer, GameState->Font, "On", V4(255, 255, 255, 255), LINEAR_SCALE);
        PlaygroundMenu->SoundOnShadowTexture = MakeTextureFromString(Buffer, GameState->Font, "On", V4(0, 0, 0, 255), LINEAR_SCALE);
        PlaygroundMenu->SoundOffTexture = MakeTextureFromString(Buffer, GameState->Font, "Off", V4(255, 255, 255, 255), LINEAR_SCALE);
        PlaygroundMenu->SoundOffShadowTexture = MakeTextureFromString(Buffer, GameState->Font, "Off", V4(0, 0, 0, 255), LINEAR_SCALE);
        
        PlaygroundMenu->BackTexture = MakeTextureFromString(Buffer, GameState->Font, "Back", V4(255, 255, 255, 255), LINEAR_SCALE);
        PlaygroundMenu->BackShadowTexture = MakeTextureFromString(Buffer, GameState->Font, "Back", V4(0, 0, 0, 128), LINEAR_SCALE);
        
        Memory->IsInitialized = true;
        printf("Memory has been initialized!\n");
        
        if (!Settings->MusicIsOn)
            Mix_PauseMusic();
        
        if(!Settings->SoundIsOn) {
            
            // playground sound channels
            // 0 channel - gear button
            // 1 channel - figure stick/drop/pick/rotate sound
            // 2 channel - level complete sound
            // 3-7 channel - figure animation drop sound
            
            // menu sound channels
            // 0 - menu switches
            
            Mix_Volume(1, 0);
        }
        
        
#if DEBUG_BUILD
        
        GameState->EditorMode = false;
        
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
        
        PlaygroundEditor->FigureButtonsArea.Min.x = FigureEntity->FigureArea.Min.x - 400.0f; 
        PlaygroundEditor->FigureButtonsArea.Min.y = FigureEntity->FigureArea.Min.y + (GetDim(FigureEntity->FigureArea).h / 2.0f) - (180.0f / 2.0f); 
        SetDim(&PlaygroundEditor->FigureButtonsArea, 180, 240);
        
        PlaygroundEditor->GridRowTexture = MakeTextureFromString(Buffer, PlaygroundEditor->Font, "Row", {255, 255, 255, 255});;
        PlaygroundEditor->GridColumnTexture = MakeTextureFromString(Buffer, PlaygroundEditor->Font, "Column", {255, 255, 255, 255});;
        PlaygroundEditor->GridPlusTexture = MakeTextureFromString(Buffer, PlaygroundEditor->Font, "+", {255, 255, 255, 255});;
        PlaygroundEditor->GridMinusTexture = MakeTextureFromString(Buffer, PlaygroundEditor->Font, "-", {255, 255, 255, 255});;
        
        PlaygroundEditor->GridButtonsArea.Min.x = FigureEntity->FigureArea.Min.x - 200.0f; 
        PlaygroundEditor->GridButtonsArea.Min.y = FigureEntity->FigureArea.Max.y - (GetDim(FigureEntity->FigureArea).h / 2.0f) - (60.0f);
        SetDim(&PlaygroundEditor->GridButtonsArea, 300.0f, 120.0f);
        
        for (u32 Index = 0; Index < PLAYGROUND_MAXIMUM; ++ Index) {
            PlayerData->PlaygroundUnlocked[Index] = false;
        }
        PlayerData->PlaygroundUnlocked[0] = true;
        
        // NOTE(msokolov): temporary
        PlaygroundData[0].FigureAmount = 4;
        PlaygroundData[0].Figures[0].Form = figure_form::I_figure;
        PlaygroundData[0].Figures[0].Type = figure_type::Red;
        
        PlaygroundData[0].Figures[1].Form = figure_form::O_figure;
        PlaygroundData[0].Figures[1].Type = figure_type::Red;
        
        PlaygroundData[0].Figures[2].Form = figure_form::L_figure;
        PlaygroundData[0].Figures[2].Type = figure_type::Red;
        
        PlaygroundData[0].Figures[3].Form = figure_form::J_figure;
        PlaygroundData[0].Figures[3].Type = figure_type::Red;
        
        PlaygroundData[0].RowAmount    = 4;
        PlaygroundData[0].ColumnAmount = 4;
        
        PrepareNextPlayground(Playground, Configuration, PlaygroundData, 0);
        
        char LevelString[64] = {};
        sprintf(LevelString, "%d", Playground->LevelNumber);
        
        Playground->LevelNumberTexture = MakeTextureFromString(Buffer, GameState->Font, LevelString, {255, 255, 255, 255});
        Assert(Playground->LevelNumberTexture);
        
        PlaygroundEditor->IsInitialized = true;
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
    
    player_data *PlayerData = GameState->PlayerData;
    game_settings *Settings = &PlayerData->Settings;
    
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
                
                Playground->LevelNumberTexture = MakeTextureFromString(Buffer, GameState->Font, LevelString, {255.0f, 255.0f, 255.0f, 255.0f});
                Assert(Playground->LevelNumberTexture);
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
                
                Playground->LevelNumberTexture = MakeTextureFromString(Buffer, GameState->Font, LevelString, {255.0f, 255.0f, 255.0f, 255.0f});
                Assert(Playground->LevelNumberTexture);
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
                /* Updating the timer UI */ 
                // Don't update the timer once a level is finished and we're waiting for the animation to finish
                if (!Playground->LevelFinished && Playground->Animation.Finished)
                    Playground->TimeElapsed += Input->dtForFrame;
                
                if (Playground->ShowTimer)
                {
                    if (!Playground->LevelFinished) {
                        char TimeString[64] = {};
                        GetTimeString(TimeString, Playground->TimeElapsed);
                        
                        if (Playground->TimerShadowTexture)
                            SDL_DestroyTexture(Playground->TimerShadowTexture);
                        
                        if (Playground->TimerTexture)
                            SDL_DestroyTexture(Playground->TimerTexture);
                        
                        Playground->TimerShadowTexture = MakeTextureFromString(Buffer, GameState->TimerFont, TimeString, V4(0.0f, 0.0f, 0.0f, 128.0f));
                        Playground->TimerTexture       = MakeTextureFromString(Buffer, GameState->TimerFont, TimeString, V4(255.0f, 255.0f, 255.0f, 255.0f));
                    }
                    else {
                        r32 AlphaChannel = Lerp1(255.0f, 0.0f, Playground->Animation.InterpPoint);
                        SDL_SetTextureAlphaMod(Playground->TimerTexture, AlphaChannel);
                        SDL_SetTextureAlphaMod(Playground->TimerShadowTexture, AlphaChannel);
                    }
                    
                    rectangle2 TimerRectangle = {};
                    TimerRectangle.Min.x = (VIRTUAL_GAME_WIDTH * 0.5f);
                    TimerRectangle.Min.y = 50.0f;
                    SetDim(&TimerRectangle, 0.0f, 0.0f);
                    
                    TimerRectangle = GetTextOnTheCenterOfRectangle(TimerRectangle, Playground->TimerTexture);
                    
                    TimerRectangle.Min += V2(5.0f, 5.0f);
                    TimerRectangle.Max += V2(5.0f, 5.0f);
                    PushFontBitmap(RenderGroup, Playground->TimerShadowTexture, TimerRectangle);
                    
                    TimerRectangle.Min -= V2(5.0f, 5.0f);
                    TimerRectangle.Max -= V2(5.0f, 5.0f);
                    PushFontBitmap(RenderGroup, Playground->TimerTexture, TimerRectangle);
                }
            }
            else if (PlaygroundStatus == playground_status::LEVEL_FINISHED)
            {
                if (GameState->PlaygroundIndex < PLAYGROUND_MAXIMUM)
                {
                    Result.SettingsChanged = true;
                    
                    /* Check if this is a record */
                    u32 PlaygroundIndex = GameState->PlaygroundIndex;
                    if (Playground->TimeElapsed < PlayerData->PlaygroundTime[PlaygroundIndex]
                        || PlayerData->PlaygroundTime[PlaygroundIndex] == 0.0f)
                    {
                        PlayerData->PlaygroundTime[PlaygroundIndex] = Playground->TimeElapsed;
                        RestartLevelEntity(Playground);
                        
                        char TimeString[64] = {};
                        GetTimeString(TimeString, PlayerData->PlaygroundTime[PlaygroundIndex]);
                        
                        PlaygroundMenu->LevelTimeTexture[PlaygroundIndex] = MakeTextureFromString(Buffer, GameState->MenuTimerFont, TimeString, {255.0f, 255.0f, 255.0f, 255.0f});
                        
                        PlaygroundMenu->LevelTimeShadowTexture[PlaygroundIndex] = MakeTextureFromString(Buffer, GameState->MenuTimerFont, TimeString, {0.0f, 0.0f, 0.0f, 255.0f});
                    }
                    
                    if (PlaygroundIndex < PLAYGROUND_MAXIMUM)
                    {
                        GameState->PlaygroundIndex = ++PlaygroundIndex;
                        PlayerData->PlaygroundUnlocked[PlaygroundIndex] = true;
                        
                        if (PlaygroundIndex % 8 == 0) {
                            u32 NextUnlockIndex = PlaygroundIndex - 8 + 32;
                            if (NextUnlockIndex < PLAYGROUND_MAXIMUM) {
                                PlayerData->PlaygroundUnlocked[NextUnlockIndex] = true;
                            }
                        }
                        
                        PrepareNextPlayground(Playground, &GameState->Configuration, PlaygroundData, GameState->PlaygroundIndex);
                        
                        RestartLevelEntity(Playground);
                        
                        s32 RandomMusicIndex = rand() % 6;
                        if (Playground->PrevMusicIndex == RandomMusicIndex) {
                            RandomMusicIndex += 1;
                            if (RandomMusicIndex >= 6)
                                RandomMusicIndex = 0;
                            
                        }
                        Playground->PrevMusicIndex = RandomMusicIndex;
                        
                        if (!Mix_PausedMusic()) {
                            Mix_PlayMusic(Playground->Music[RandomMusicIndex], -1);
                            
                            if (Mix_PlayingMusic())
                                Mix_FadeInMusic(Playground->Music[RandomMusicIndex], -1, 256);
                            
                            Mix_VolumeMusic(MIX_MAX_VOLUME * 0.2f);
                        }
                        
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
                PlaygroundMenu->AnimationFinished = false;
                PlaygroundMenu->InterpPoint       = 0.0f;
                PlaygroundMenu->ForwardAnimation  = false;
                PlaygroundMenu->InterpPointNext   = 1.0f;
                
                UpdateTextureForLevels(PlaygroundMenu, PlayerData, RenderGroup);
                SDL_ShowCursor(SDL_ENABLE);
                
                Mix_PlayChannel(0, PlaygroundMenu->MenuUp, 0);
                
#if DEBUG_BUILD
                GameState->EditorMode = false;
                GameState->PlaygroundEditor->FigureIndex = 0;
#endif
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
            menu_result_option MenuResult = PlaygroundMenuUpdateAndRender(PlaygroundMenu, PlayerData, Settings, Input, RenderGroup);
            
            if (MenuResult.SwitchToPlayground)
            {
                if (PlaygroundMenu->PlaygroundSwitch)
                {
                    PlaygroundMenu->PlaygroundSwitch = false;
                    
                    if ((!Mix_PlayingMusic() || Mix_PausedMusic()) && Settings->MusicIsOn) {
                        s32 RandomMusicIndex = rand() % 6;
                        
                        if (Playground->PrevMusicIndex == RandomMusicIndex) {
                            RandomMusicIndex += 1;
                            if (RandomMusicIndex >= 6)
                                RandomMusicIndex = 0;
                        }
                        
                        Playground->PrevMusicIndex = RandomMusicIndex;
                        
                        Mix_PlayMusic(Playground->Music[RandomMusicIndex], -1);
                        if (Mix_PlayingMusic())
                            Mix_FadeInMusic(Playground->Music[RandomMusicIndex], -1, 256);
                        Mix_VolumeMusic(MIX_MAX_VOLUME * 0.2f);
                    }
                }
                else
                {
                    u32 ResultLevelIndex = MenuResult.PlaygroundIndex;
                    RestartLevelEntity(Playground);
                    GameState->PlaygroundIndex = ResultLevelIndex;
                    
                    PrepareNextPlayground(Playground, &GameState->Configuration, PlaygroundData, ResultLevelIndex);
                    
                    s32 RandomMusicIndex = rand() % 6;
                    
                    if (Playground->PrevMusicIndex == RandomMusicIndex) {
                        RandomMusicIndex += 1;
                        if (RandomMusicIndex >= 6)
                            RandomMusicIndex = 0;
                    }
                    
                    Playground->PrevMusicIndex = RandomMusicIndex;
                    
                    if (!Mix_PausedMusic()) {
                        Mix_PlayMusic(Playground->Music[RandomMusicIndex], -1);
                        if (Mix_PlayingMusic())
                            Mix_FadeInMusic(Playground->Music[RandomMusicIndex], -1, 256);
                        Mix_VolumeMusic(MIX_MAX_VOLUME * 0.2f);
                    }
                    
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
            if (MenuResult.SettingsChanged) {
                Result.SettingsChanged = true;
            }
        } break;
    }
    
    RenderGroupToOutput(RenderGroup, Buffer);
    TransState->TransGroup = TemporaryMemory;
    
    return(Result);
}
