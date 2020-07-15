/* ========================================= */
//     $File: tetroman_playground.h
//     $Date: October 18th 2017 08:52 pm 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */

#if !defined(ENTITY_H)

#define GRID_BLOCK_SIZE 100
#define IDLE_BLOCK_SIZE 62

#define FIGURE_BLOCKS_MAXIMUM 4
#define FIGURE_AMOUNT_MAXIMUM 20
#define MOVING_BLOCKS_MAXIMUM 10
#define COLUMN_AMOUNT_MAXIMUM 10
#define ROW_AMOUNT_MAXIMUM    10
#define PLAYGROUND_MAXIMUM    100

enum figure_form
{
    O_figure, I_figure, L_figure, J_figure,
    Z_figure, S_figure, T_figure
};

enum figure_type
{
    classic, stone, mirror
};

struct figure_unit
{
    bool IsStick;
    bool IsEnlarged; // not sure if we need it
    bool IsIdle;
    
    r32 Angle;
    r32 HomeAngle;
    
    v2 Size;
    v2 Position;
    v2 HomePosition;
    
    r32 CenterOffset;
    v2 Shell[FIGURE_BLOCKS_MAXIMUM];
    
    figure_flip Flip;
    figure_form Form;
    figure_type Type;
};

struct figure_entity
{
    rectangle2 FigureArea;
    
    u32 ReturnIndex;
    s32 FigureActive;
    
    u32 FigureAmount;
    u32 FigureOrder[FIGURE_AMOUNT_MAXIMUM];
    figure_unit FigureUnit[FIGURE_AMOUNT_MAXIMUM];
    
    b32 IsGrabbed;
    b32 IsRotating;
    b32 IsFlipping;
    b32 IsReturning;
    b32 IsRestarting;
    
    r32 AreaAlpha;
    r32 FigureAlpha;
    r32 FadeInSum;
    r32 FadeOutSum;
    r32 RotationSum;
    
    r32 FigureVelocity;
    r32 FlippingVelocity;
    r32 RotationVelocity;
    
    game_texture *O_ClassicTexture;
    game_texture *O_StoneTexture;
    game_texture *O_MirrorTexture;
    game_texture *O_ShadowTexture;
    
    game_texture *I_ClassicTexture;
    game_texture *I_StoneTexture;
    game_texture *I_MirrorTexture;
    game_texture *I_ShadowTexture;
    
    game_texture *L_ClassicTexture;
    game_texture *L_StoneTexture;
    game_texture *L_MirrorTexture;
    game_texture *L_ShadowTexture;
    
    game_texture *J_ClassicTexture;
    game_texture *J_ShadowTexture;
    game_texture *J_StoneTexture;
    game_texture *J_MirrorTexture;
    
    game_texture *Z_ClassicTexture;
    game_texture *Z_StoneTexture;
    game_texture *Z_MirrorTexture;
    game_texture *Z_ShadowTexture;
    
    game_texture *S_ClassicTexture;
    game_texture *S_StoneTexture;
    game_texture *S_MirrorTexture;
    game_texture *S_ShadowTexture;
    
    game_texture *T_ClassicTexture;
    game_texture *T_StoneTexture;
    game_texture *T_MirrorTexture;
    game_texture *T_ShadowTexture;
};

struct sticked_unit
{
    s32 Index;
    u32 Row[FIGURE_BLOCKS_MAXIMUM];
    u32 Col[FIGURE_BLOCKS_MAXIMUM];
    
    bool IsSticked;
    v2 Center;
};

struct moving_block
{
    rectangle2 Area;
    
    u32 RowNumber;
    u32 ColNumber;
    
    bool IsMoving;
    bool IsVertical;
    bool MoveSwitch;
};

struct grid_entity
{
    rectangle2 GridArea;
    
    s32 UnitField[COLUMN_AMOUNT_MAXIMUM * ROW_AMOUNT_MAXIMUM];
    u32 RowAmount;
    u32 ColumnAmount;
    r32 MovingBlockVelocity;
    
    moving_block MovingBlocks[COLUMN_AMOUNT_MAXIMUM * ROW_AMOUNT_MAXIMUM];
    u32 MovingBlocksAmount;
    
    sticked_unit StickUnits[FIGURE_AMOUNT_MAXIMUM];
    u32 StickUnitsAmount;
    
    game_texture *NormalSquareTexture;
    game_texture *GridCell1Texture;
    game_texture *GridCell2Texture;
    
    game_texture *VerticalSquareTexture;
    game_texture *HorizontlaSquareTexture;
    
    game_texture *TopLeftCornerFrame;
    game_texture *TopRightCornerFrame;
    game_texture *DownLeftCornerFrame;
    game_texture *DownRightCornerFrame;
};

struct playground_config
{
    r32 StartUpTimeToFinish;
    r32 StartUpTimeElapsed;
    
    r32 RotationVel;
    r32 FigureVelocity;
    r32 MovingBlockVelocity;
    
    r32 StartAlphaPerSec;
    r32 FlippingAlphaPerSec;
    r32 GridScalePerSec;
    r32 PixelsToDraw;
    r32 PixelsDrawn;
};

struct level_animation
{
    game_rect FinishQuad;
    game_texture *FinishTexture;
    
    s32 AlphaChannel;
    
    r32 MaxTileDelaySec;
    r32 TilePixelPerSec;
    r32 TileAlphaPerSec;
    r32 TileAnglePerSec;
    r32 TileCountPerSec;
    
    s32 OldRowAmount;
    s32 OldColAmount;
    
    s32 TileAlphaChannel;
    
    r32 *TileAngle;
    r32 *TileOffset;
    s32 *TileAlpha;
    v3  *TilePos;
    game_texture *TileTexture;
    game_rect *TileQuad;
    
    game_rect TileRect;
    
    r32 TimeElapsed;
};

enum playground_status
{
    LEVEL_RUNNING,
    LEVEL_PAUSED,
    LEVEL_FINISHED,
    
    LEVEL_RESTARTED,
    LEVEL_SETTINGS_QUIT,
    LEVEL_MENU_QUIT,
    LEVEL_GAME_QUIT
};

enum options_choice
{
    RESTART_OPTION  = 0,
    SETTINGS_OPTION = 1,
    MAINMENU_OPTION = 2,
    QUIT_OPTION     = 3,
    NONE_OPTION     = 4
};

struct playground_options
{
    b32 ToggleMenu;
    options_choice Choice;
    
    v2 MenuPosition;
    v2 ButtonDimension;
    
    game_texture *GearTexture;
    game_texture *GearShadowTexture;
    game_texture *HorizontalLineTexture;
    
    game_texture *MenuTexture[4];
    game_texture *MenuShadowTexture[4];
};

struct playground
{
    grid_entity GridEntity;
    figure_entity FigureEntity;
    playground_options Options;
    
    u32 LevelNumber;
    b32 LevelStarted;
    b32 LevelPaused;
    b32 LevelFinished;
    
    game_texture *CornerLeftTopTexture;
    game_texture *CornerLeftBottomTexture;
    game_texture *CornerRightTopTexture;
    game_texture *CornerRightBottomTexture;
    game_texture *VerticalBorderTexture;
    game_texture *LevelNumberTexture;
};

struct menu_button
{
    b32 IsLocked;
    game_rect ButtonQuad;
    game_rect LevelNumberTextureQuad;
    game_texture *LevelNumberTexture;
};

struct menu_entity
{
    bool IsPaused;
    bool IsMoving;
    bool IsAnimating;
    
    s32 SpaceBetweenButtons;
    u32 ButtonSizeWidth;
    u32 ButtonSizeHeight;
    u32 ButtonsAmount;
    u32 ButtonsAmountReserved;
    
    r32 MaxVelocity;
    s32 ScrollingTicks;
    
    u32 TargetIndex;
    s32 ButtonIndex;
    
    s32 MouseOffsetX;
    s32 MouseOffsetY;
    v2 Velocity;
    
    game_texture *BackTexture;
    game_texture *FrontTexture;
    
    game_rect ButtonsArea[5];
    menu_button  *Buttons;
    
    game_font *MainFont;
    game_font *LevelNumberFont;
};



#define ENTITY_H
#endif