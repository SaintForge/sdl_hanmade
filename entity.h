/* entity.h --- 
 * 
 * Filename: entity.h
 * Author: 
 * Created: Ср окт 18 20:58:16 2017 (+0400)
 * Last-Updated: Пт окт 20 16:50:56 2017 (+0300)
 *           By: Sierra
 */

#if !defined(ENTITY_H)

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
    
    r32 *UnitSize;
    s32 *UnitField;
    
    sticked_unit *StickUnits;
    moving_block *MovingBlocks;
    
    game_texture *NormalSquareTexture;
    game_texture *VerticalSquareTexture;
    game_texture *HorizontlaSquareTexture;
};

struct level_config
{
    u32 DefaultBlocksInRow;
    u32 DefaultBlocksInCol;
    u32 InActiveBlockSize;
    
    r32 StartUpTimeToFinish;
    r32 StartUpTimeElapsed;
    
    r32 RotationVel;
    r32 StartAlphaPerSec;
    r32 FlippingAlphaPerSec;
    r32 GridScalePerSec;
    r32 PixelsToDraw;
    r32 PixelsDrawn;
    
    s32 GridBlockSize;
};

struct level_entity
{
    grid_entity   *GridEntity;
    figure_entity *FigureEntity;
    
    game_texture *LevelNumberTexture;
    game_texture *LevelNumberShadowTexture;
    
    game_rect LevelNumberQuad;
    game_rect LevelNumberShadowQuad;
    
    level_config Configuration;
    
    u32 LevelNumber;
    bool LevelStarted;
    bool LevelPaused;
    bool LevelFinished;
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
    bool IsPaused;
    bool IsMoving;
    bool IsAnimating;
    
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

static void
MenuMakeTextButton(char* Text, s32 X, s32 Y, s32 Width, s32 Height,
                   game_rect *ButtonQuad, game_rect *TextureQuad,
                   game_texture *&Texture, game_font *&Font,
                   game_color Color, game_offscreen_buffer *Buffer)
{
    ButtonQuad->w  = Width;
    ButtonQuad->h  = Height;
    ButtonQuad->x  = X;
    ButtonQuad->y  = Y;
    
    GameMakeTextureFromString(Texture, Text, TextureQuad, Font, 
                              {Color.r, Color.g, Color.b}, Buffer);
    TextureQuad->w = (TextureQuad->w < ButtonQuad->w) ? TextureQuad->w : ButtonQuad->w;
    TextureQuad->h = (TextureQuad->h < ButtonQuad->h) ? TextureQuad->h : ButtonQuad->h;
    
    TextureQuad->x = ButtonQuad->x + (ButtonQuad->w / 2) - (TextureQuad->w / 2);
    TextureQuad->y = ButtonQuad->y + (ButtonQuad->h / 2) - (TextureQuad->h / 2);
}

static void
MenuLoadButtonsFromMemory(menu_entity *MenuEntity, game_memory *Memory, 
                          game_offscreen_buffer *Buffer)
{
    level_memory *LevelMemory = (level_memory *) Memory->GlobalMemoryStorage;
    
    MenuEntity->ButtonsAmount         = Memory->LevelMemoryAmount;
    MenuEntity->ButtonsAmountReserved = Memory->LevelMemoryReserved;
    
    if(MenuEntity->Buttons)
    {
        for(s32 i = 0; i < MenuEntity->ButtonsAmountReserved; ++i)
        {
            if(MenuEntity->Buttons[i].LevelNumberTexture)
            {
                FreeTexture(MenuEntity->Buttons[i].LevelNumberTexture);
            }
        }
        
        free(MenuEntity->Buttons);
    }
    
    MenuEntity->Buttons = (menu_button *) calloc (MenuEntity->ButtonsAmountReserved, sizeof(menu_button));
    Assert(MenuEntity->Buttons);
    
    for(u32 i = 0; i < MenuEntity->ButtonsAmountReserved; ++i)
    {
        char LevelNumber[3] = {0};
        sprintf(LevelNumber, "%d", LevelMemory[i].LevelNumber);
        
        MenuMakeTextButton(LevelNumber, 0, 0, MenuEntity->ButtonSizeWidth, MenuEntity->ButtonSizeHeight,
                           &MenuEntity->Buttons[i].ButtonQuad, &MenuEntity->Buttons[i].LevelNumberTextureQuad,
                           MenuEntity->Buttons[i].LevelNumberTexture, MenuEntity->LevelNumberFont, {255, 255, 255}, Buffer);
        
    }
}


static void
MenuEntityAlignButtons(menu_entity *MenuEntity, 
                       u32 ScreenWidth, 
                       u32 ScreenHeight)
{
    u32 ButtonsPerRow       = 4;
    u32 ButtonsPerColumn    = 5;
    u32 SpaceBetweenButtons = 10;
    
    s32 XOffset = 0;
    s32 YOffset = 0;
    
    s32 XPosition = 0;
    s32 YPosition = 0;
    
    u32 ButtonWidth  = MenuEntity->ButtonSizeWidth;
    u32 ButtonHeight = MenuEntity->ButtonSizeHeight;
    
    s32 StartX = (ScreenWidth / 2)  - (((ButtonWidth * ButtonsPerRow) + ((ButtonsPerRow - 1) * SpaceBetweenButtons)) / 2);
    s32 StartY = (ScreenHeight / 2) - ((ButtonHeight * ButtonsPerColumn) / 2)- ((ButtonsPerColumn * SpaceBetweenButtons) / 2);
    
    u32 ButtonsAreaAmount = MenuEntity->ButtonsAmountReserved / 20;
    for(u32 i = 0; i < ButtonsAreaAmount; ++i)
    {
        MenuEntity->ButtonsArea[i].x = StartX + (i * ScreenWidth);
        MenuEntity->ButtonsArea[i].y = StartY;
    }
    
    for(u32 i = 0; i < MenuEntity->ButtonsAmount; ++i)
    {
        XOffset = i % ButtonsPerRow;
        
        if((i % 20 == 0) && i != 0)
        {
            StartX += ScreenWidth;
        }
        
        if(i % ButtonsPerRow == 0 && i != 0)
        {
            YOffset += 1;
        }
        
        if(YOffset >= ButtonsPerColumn)
        {
            YOffset = 0;
        }
        
        XPosition = StartX + (XOffset * ButtonWidth) + (XOffset * SpaceBetweenButtons);
        YPosition = StartY + (YOffset * ButtonHeight) + (YOffset * SpaceBetweenButtons);
        MenuEntity->Buttons[i].ButtonQuad.x = XPosition;
        MenuEntity->Buttons[i].ButtonQuad.y = YPosition;
        
        MenuEntity->Buttons[i].LevelNumberTextureQuad.x = XPosition + (MenuEntity->Buttons[i].ButtonQuad.w / 2) - (MenuEntity->Buttons[i].LevelNumberTextureQuad.w / 2);
        MenuEntity->Buttons[i].LevelNumberTextureQuad.y = YPosition + (MenuEntity->Buttons[i].ButtonQuad.h / 2) - (MenuEntity->Buttons[i].LevelNumberTextureQuad.h / 2);
    }
}


#define ENTITY_H
#endif