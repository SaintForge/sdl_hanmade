/* ========================================= */
//     $File: tetroman_menu.h
//     $Date: July 12th 2020 8:35 pm 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */

#ifndef TETROMAN_MENU_H
#define TETROMAN_MENU_H

enum menu_page
{
    DIFFICULTY_PAGE = 0,
    SETTINGS_PAGE   = 1,
    QUIT_PAGE       = 2,
    MAIN_PAGE       = 3,
    IDLE_PAGE       = 4
};

enum difficulty
{
    EASY   = 0,
    MEDIUM = 1,
    HARD   = 2
};

struct menu_result_option
{
    b32 QuitGame;
    b32 MusicOn;
    b32 MusicOff;
    b32 SoundOn;
    b32 SoundOff;
    
    b32 ToggleFullScreen;
    b32 ChangeResolution;
    resolution_standard Resolution;
    
    b32 SwitchToPlayground;
    u32 PlaygroundIndex;
};

struct playground_menu
{
    menu_page MenuPage;
    difficulty DiffMode;
    
    b32 IsFullScreen;
    b32 PlaygroundSwitch;
    u32 ButtonIndex;
    
    resolution_standard Resolution;
    
    /* Main Menu textures*/
    game_texture *MainMenuTexture[3];
    game_texture *MainMenuShadowTexture[3];
    game_texture *CornerTexture[4];
    
    /* Settings textures*/
    game_texture *FullScreenNameTexture;
    game_texture *FullScreenNameShadowTexture;
    
    game_texture *FullScreenTexture[2];
    game_texture *FullScreenShadowTexture[2];
    
    game_texture *ResolutionNameTexture;
    game_texture *ResolutionNameShadowTexture;
    
    game_texture *ResolutionTexture[3];
    game_texture *ResolutionShadowTexture[3];
    
    /* Playground Menu textures*/
    game_texture *DifficultyTexture[3];
    game_texture *DifficultyShadowTexture[3];
    
    game_texture *LevelButtonTexture;
    game_texture *LevelNumberTexture[100];
    game_texture *LevelCornerTexture[4];
    game_texture *SquareFrameLocked;
    game_texture *SquareFrameUnlocked;
    game_texture *ColorBarTexture[4];
    
    /* Miscellaneous */
    game_texture *HorizontalLineTexture;
    game_texture *BackTexture;
    game_texture *BackShadowTexture;
};

#endif //TETROMAN_MENU_H
