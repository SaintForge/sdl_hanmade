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

enum difficulty_mode
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
    b32 SwitchToPlayground;
    u32 PlaygroundIndex;
};

struct playground_menu
{
    menu_page MenuPage;
    difficulty_mode DiffMode;
    
    game_texture *MainMenuTexture[3];
    game_texture *DifficultyTexture[3];
    game_texture *LevelButtonTexture;
    game_texture *LevelNumberTexture[100];
};

#endif //TETROMAN_MENU_H
