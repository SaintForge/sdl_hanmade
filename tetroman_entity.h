/* entity.h --- 
 * 
 * Filename: entity.h
 * Author: 
 * Created: Ср окт 18 20:58:16 2017 (+0400)
 * Last-Updated: Пт окт 20 16:50:56 2017 (+0300)
 *           By: Sierra
 */

#if !defined(ENTITY_H)



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
    game_rect FigureArea;
    
    u32 ReturnIndex;
    s32 FigureActive;
    
    u32 FigureAmount;
    u32 FigureOrder[FIGURE_AMOUNT_MAXIMUM];
    figure_unit FigureUnit[FIGURE_AMOUNT_MAXIMUM];
    
    bool IsGrabbed;
    bool IsRotating;
    bool IsFlipping;
    bool IsReturning;
    bool IsRestarting;
    
    r32 AreaAlpha;
    r32 FigureAlpha;
    r32 FadeInSum;
    r32 FadeOutSum;
    r32 RotationSum;
    s32 FigureVelocity;
    
    game_texture *O_ClassicTexture;
    game_texture *O_StoneTexture;
    game_texture *O_MirrorTexture;
    
    game_texture *I_ClassicTexture;
    game_texture *I_StoneTexture;
    game_texture *I_MirrorTexture;
    
    game_texture *L_ClassicTexture;
    game_texture *L_StoneTexture;
    game_texture *L_MirrorTexture;
    
    game_texture *J_ClassicTexture;
    game_texture *J_StoneTexture;
    game_texture *J_MirrorTexture;
    
    game_texture *Z_ClassicTexture;
    game_texture *Z_StoneTexture;
    game_texture *Z_MirrorTexture;
    
    game_texture *S_ClassicTexture;
    game_texture *S_StoneTexture;
    game_texture *S_MirrorTexture;
    
    game_texture *T_ClassicTexture;
    game_texture *T_StoneTexture;
    game_texture *T_MirrorTexture;
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
    game_rect AreaQuad;
    
    u32 RowNumber;
    u32 ColNumber;
    
    bool IsMoving;
    bool IsVertical;
    bool MoveSwitch;
};

struct grid_entity
{
    game_rect GridArea;
    
    s32 UnitField[COLUMN_AMOUNT_MAXIMUM * ROW_AMOUNT_MAXIMUM];
    u32 RowAmount;
    u32 ColumnAmount;
    
    moving_block MovingBlocks[COLUMN_AMOUNT_MAXIMUM * ROW_AMOUNT_MAXIMUM];
    u32 MovingBlocksAmount;
    
    sticked_unit StickUnits[FIGURE_AMOUNT_MAXIMUM];
    u32 StickUnitsAmount;
    
    game_texture *NormalSquareTexture;
    game_texture *VerticalSquareTexture;
    game_texture *HorizontlaSquareTexture;
    
    game_texture *TopLeftCornerFrame;
    game_texture *TopRightCornerFrame;
    game_texture *DownLeftCornerFrame;
    game_texture *DownRightCornerFrame;
};

struct level_config
{
    s32 InActiveBlockSize;
    s32 GridBlockSize;
    
    r32 StartUpTimeToFinish;
    r32 StartUpTimeElapsed;
    
    r32 RotationVel;
    r32 StartAlphaPerSec;
    r32 FlippingAlphaPerSec;
    r32 GridScalePerSec;
    r32 PixelsToDraw;
    r32 PixelsDrawn;
};

struct p_texture
{
    game_texture *Texture;
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
    p_texture *TileTexture;
    game_rect *TileQuad;
    
    game_rect TileRect;
    
    r32 TimeElapsed;
};

enum playground_status
{
    LEVEL_RUNNING,
    LEVEL_PAUSED,
    LEVEL_FINISHED,
};

// TODO(msokolov): name it playground or something
struct playground
{
    // TODO(msokolov): maybe we should declare them as just non-pointers
    grid_entity GridEntity;
    figure_entity FigureEntity;
    
    game_texture *LevelNumberTexture;
    game_texture *LevelNumberShadowTexture;
    
    game_rect LevelNumberQuad;
    game_rect LevelNumberShadowQuad;
    
    level_config Configuration;
    level_animation AnimationData;
    
    u32 LevelNumber;
    b32 LevelStarted;
    b32 LevelPaused;
    b32 LevelFinished;
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
    vector2 Velocity;
    
    game_texture *BackTexture;
    game_texture *FrontTexture;
    
    game_rect ButtonsArea[5];
    menu_button  *Buttons;
    
    game_font *MainFont;
    game_font *LevelNumberFont;
};



#define ENTITY_H
#endif