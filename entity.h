/* entity.h --- 
 * 
 * Filename: entity.h
 * Author: 
 * Created: Ср окт 18 20:58:16 2017 (+0400)
 * Last-Updated: Пт окт 20 16:50:56 2017 (+0300)
 *           By: Sierra
 */


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
    
    u32 Index;     
    r32 Angle;
    r32 DefaultAngle;
    
    game_point Center;
    game_point DefaultCenter;
    game_point Shell[4];
    game_point DefaultShell[4];
    game_rect AreaQuad;
    
    figure_flip Flip;
    figure_form Form;
    figure_type Type;
    
    game_texture *Texture;
};


struct figure_entity
{
    u32 FigureAmountReserved;
    u32 ReturnIndex;
    s32 FigureActive;
    u32 FigureAmount;
    u32 *FigureOrder;
    figure_unit *FigureUnit;
    
    game_rect FigureArea;
    
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
};

struct sticked_unit
{
    s32 Index;
    u32 Row[4];
    u32 Col[4];
    
    bool IsSticked;
    game_point Center;
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
    u32 RowAmount;
    u32 ColumnAmount;
    
    u32 MovingBlocksAmount;
    u32 MovingBlocksAmountReserved;
    
    u32 StickUnitsAmount;
    game_rect GridArea;
    
    r32 **UnitSize;
    s32 **UnitField;
    
    sticked_unit *StickUnits;
    moving_block *MovingBlocks;
    
    game_texture *NormalSquareTexture;
    game_texture *VerticalSquareTexture;
    game_texture *HorizontlaSquareTexture;
};

struct level_entity
{
    grid_entity   *GridEntity;
    figure_entity *FigureEntity;
    
    game_texture *LevelNumberTexture;
    game_texture *LevelNumberShadowTexture;
    
    game_rect LevelNumberQuad;
    game_rect LevelNumberShadowQuad;
    
    u32 LevelNumber;
    u32 DefaultBlocksInRow;
    u32 DefaultBlocksInCol;
    u32 ActiveBlockSize;
    u32 InActiveBlockSize;
    
    r32 LevelTimeInitMs;
    r32 LevelTimeInitElapsedMs;
    
    r32 RotationVel;
    r32 StartAlphaPerSec;
    r32 FlippingAlphaPerSec;
    r32 GridScalePerSec;
    
    bool LevelStarted;
    bool LevelPaused;
    bool LevelFinished;
};


struct level_editor
{
    u32 SelectedFigure;
    bool ButtonPressed;
    
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

struct figure_memory
{
    r32 Angle;
    figure_flip Flip;
    figure_form Form;
    figure_type Type;
};

struct moving_block_memory
{
    u32 RowNumber;
    u32 ColNumber;
    bool IsVertical;
    bool MoveSwitch;
};

struct level_memory
{
    u32 LevelNumber;
    u32 RowAmount;
    u32 ColumnAmount;
    u32 MovingBlocksAmount;
    u32 FigureAmount;
    
    s32 *UnitField;
    moving_block_memory *MovingBlocks;
    figure_memory *Figures;
};

struct menu_button
{
    game_rect ButtonQuad;
    game_rect LevelNumberTextureQuad;
    game_texture *LevelNumberTexture;
};

struct menu_entity
{
    bool IsMoving;
    bool IsAnimating;
    bool IsShowingDelete;
    bool IsToBeDeleted;
    bool IsToBeSaved;
    bool IsToBeLoaded;
    bool DevMode;
    
    u32 ButtonSizeWidth;
    u32 ButtonSizeHeight;
    u32 ButtonsAmount;
    u32 ButtonsAmountReserved;
    
    s32 OldMouseX;
    r32 MaxVelocity;
    s32 ScrollingTicks;
    r32 TargetPosition;
    u32 TargetIndex;
    s32 ButtonIndex;
    u32 NewButtonIndex;
    vector2 Velocity;
    
    game_texture *BackTexture;
    game_texture *FrontTexture;
    
    game_rect    *ButtonsArea;
    menu_button  *Buttons;
    menu_button  *ConfirmButtons;
    menu_button  *SaveAndLoadButtons;
};


