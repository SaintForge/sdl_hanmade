// editor.h --- 
// 
// Filename: editor.h
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//


#if !defined(EDITOR_H)

struct level_editor
{
    bool EditorMode;
    
    u32 SelectedFigure;
    bool ButtonPressed;
    
    s32 CurrentLevelIndex;
    
    s32 OldLevelNumber;
    s32 LevelNumberBufferIndex;
    char LevelNumberBuffer[4];
    bool LevelNumberSelected;
    
    game_rect ActiveButton;
    
    game_rect PrevLevelQuad;
    game_rect NextLevelQuad;
    
    game_rect GridButtonLayer;
    game_rect GridButtonQuad[6];
    game_rect GridButton[6];
    
    game_rect FigureButtonLayer;
    game_rect FigureButtonQuad[6];
    game_rect FigureButton[6];
    
    game_rect SaveButtonQuad;
    game_rect SaveButtonLayer;
    game_rect LoadButtonQuad;
    game_rect LoadButtonLayer;
    
    game_rect LevelIndexQuad;
    game_rect LevelNumberQuad;
    game_rect LevelInfoQuad;
    
    game_texture *LevelIndexTexture;
    game_texture *LevelNumberTexture;
    
    game_texture *PrevLevelTexture;
    game_texture *NextLevelTexture;
    
    game_texture *PlusTexture;
    game_texture *MinusTexture;
    game_texture *RowTexture;
    game_texture *ColumnTexture;
    game_texture *RotateTexture;
    game_texture *FlipTexture;
    game_texture *FormTexture;
    game_texture *TypeTexture;
    game_texture *SaveTexture;
    game_texture *LoadTexture;
    
    game_font *Font;
    game_font *StatsFont;
};

struct menu_editor
{
    /* For showing gui */
    bool EditorMode;
    bool DeleteBarOn;
    
    /* Button data for adding a new level in memory */
    game_rect NewLevelButtonQuad;
    game_rect NewLevelTextureQuad;
    game_texture *NewLevelTexture;
    
    /* Button data for loading levels from memory*/
    game_rect LoadButtonQuad;
    game_rect LoadButtonTextureQuad;
    game_texture *LoadButtonTexture;
    
    /* Button data for saving levels on the disk */
    game_rect SaveButtonQuad;
    game_rect SaveButtonTextureQuad;
    game_texture *SaveButtonTexture;
    
    /* Button data for sorting levels in memory by their level number*/
    game_rect SortButtonQuad;
    game_rect SortButtonTextureQuad;
    game_texture *SortButtonTexture;
    
    /* Button data for confirming deletion of a level */
    game_rect DeleteButtonQuad;
    game_rect DeleteButtonTextureQuad;
    game_texture *DeleteButtonTexture;
    
    /* Button data for canceling deletion of a level */
    game_rect CancelButtonQuad;
    game_rect CancelButtonTextureQuad;
    game_texture *CancelButtonTexture;
};

#define EDITOR_H
#endif
