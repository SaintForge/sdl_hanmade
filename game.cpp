// game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

#include "game_math.h"
#include "game.h"

#include "entity.cpp"
#include "assets.cpp"
#include "menu.cpp"
#include "editor.cpp"

#if 0
static void
GameCopyImageToBuffer(game_bitmap* GameBitmap, u32 X, u32 Y,
                      game_offscreen_buffer *Buffer)
{
    u8 BytesPerPixel = GameBitmap->BytesPerPixel;
    
    u8 *RowBuffer = (u8*)Buffer->Memory + (Y * GameBitmap->Pitch);
    u8 *RowTarget = (u8*)GameBitmap->Pixels;
    
    for (u32 y = 0; y < GameBitmap->Height; ++y)
    {
        u32 *PixelBuffer = (u32*)RowBuffer + X;
        u32 *PixelTarget = (u32*)RowTarget;
        
        for (u32 x = 0; x < GameBitmap->Width; ++x)
        {
            u8 Alpha = (*PixelTarget >> 24) & 0xFF;
            u8 Blue  = (*PixelTarget >> 16) & 0xFF;
            u8 Green = (*PixelTarget >> 8)  & 0xFF;
            u8 Red   = (*PixelTarget & 0xFF);
            
            *PixelBuffer = ((Red << 24) | (Green << 16) | (Blue << 8) | (Alpha));
            
            *PixelBuffer++;
            *PixelTarget++;
        }
        
        RowBuffer += Buffer->Pitch;
        RowTarget += GameBitmap->Width * BytesPerPixel;
    }
    
}
#endif


static bool
GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
    bool ShouldQuit = false;
    
    if(!Memory->IsInitialized)
    {
        Memory->RefWidth  = 800;
        Memory->RefHeight = 600;
        
        math_rect ScreenArea =
        {0.0f, 0.0f, (r32)Buffer->Width, (r32)Buffer->Height};
        
        math_rect RefScreenArea =
        {0.0f, 0.0f, (r32)Memory->RefWidth, (r32)Memory->RefHeight};
        
        Memory->PadRect.x = 40;
        Memory->PadRect.y = 30;
        Memory->PadRect.w = 720;
        Memory->PadRect.h = 570;
        
        math_rect GameArea   = CreateMathRect(0.05f, 0.95f, 0.95f, 0.05f, ScreenArea);
        math_rect GridArea   = CreateMathRect(0.0f, 1.0f, 1.0f, 0.5f, GameArea);
        math_rect FigureArea = CreateMathRect(0.0f, 0.3f, 1.0f, 0.0f, GameArea);
        
        /* game_memory initialization */
        
        Memory->ToggleMenu        = false;
        Memory->CurrentLevelIndex = 0;
        
        Memory->LevelNumberFont = TTF_OpenFont("..\\data\\Karmina-Bold.otf", 50);
        Assert(Memory->LevelNumberFont);
        
        /* LocalMemoryStorage allocation */
        /* level_entity initialization */
        
        Memory->LocalMemoryStorage = calloc(1, sizeof(level_entity) + (sizeof(menu_entity)));
        Assert(Memory->LocalMemoryStorage);
        
        level_entity *LevelEntity  = (level_entity*) Memory->LocalMemoryStorage;
        LevelEntity->LevelNumber   = 0;
        LevelEntity->LevelStarted  = false;
        LevelEntity->LevelFinished = false;
        LevelEntity->LevelPaused   = false;
        LevelEntity->LevelNumberQuad       = {};
        LevelEntity->LevelNumberShadowQuad = {};
        
        LevelEntity->Configuration.DefaultBlocksInRow = 5;
        LevelEntity->Configuration.DefaultBlocksInCol = 5;
        
        u32 RowAmount           = 5;
        u32 ColumnAmount        = 5;
        u32 FigureAmountReserve = 20;
        u32 MovingBlocksAmountReserved  = 10;
        
        RescaleGameField(Buffer, RowAmount, ColumnAmount,
                         FigureAmountReserve, LevelEntity->Configuration.DefaultBlocksInRow, LevelEntity->Configuration.DefaultBlocksInCol, LevelEntity);
        
        //for DEBUG purposes only
        game_rect FigureAreaRect = ConvertMathRectToGameRect(FigureArea);
        
        LevelEntity->Configuration.InActiveBlockSize = CalculateFigureBlockSize(3, FigureAreaRect.w, FigureAreaRect.h);
        
        u32 ActiveBlockSize   = LevelEntity->Configuration.ActiveBlockSize;
        u32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
        
        /* Change values below to be time configured */
        
        LevelEntity->Configuration.StartUpTimeToFinish = 2.0f;
        LevelEntity->Configuration.RotationVel         = 600.0f;
        LevelEntity->Configuration.StartAlphaPerSec    = 500.0f;
        LevelEntity->Configuration.FlippingAlphaPerSec = 1000.0f;
        
        LevelEntity->Configuration.GridScalePerSec     = (ActiveBlockSize * ((RowAmount + ColumnAmount) - 1)) / LevelEntity->Configuration.StartUpTimeToFinish;
        
        /* figure_entity initialization */
        
        LevelEntity->FigureEntity= (figure_entity*)malloc(sizeof(figure_entity));
        Assert(LevelEntity->FigureEntity);
        
        figure_entity* FigureEntity = LevelEntity->FigureEntity;
        
        FigureEntity->FigureAmount         = 0;
        FigureEntity->FigureAmountReserved = FigureAmountReserve;
        
        FigureEntity->ReturnIndex   = -1;
        FigureEntity->FigureActive  = -1;
        
        FigureEntity->IsGrabbed     = false;
        FigureEntity->IsRotating    = false;
        FigureEntity->IsReturning   = false;
        FigureEntity->IsRestarting  = false;
        FigureEntity->IsFlipping    = false;
        
        FigureEntity->RotationSum   = 0;
        
        FigureEntity->FigureAlpha   = 0;
        FigureEntity->FadeInSum     = 0;
        FigureEntity->FadeOutSum    = 0;
        
        //FigureEntity->FigureArea.w  = Buffer->Width;
        //FigureEntity->FigureArea.h  = InActiveBlockSize * //LevelEntity->Configuration.DefaultBlocksInCol;
        //FigureEntity->FigureArea.y  = Buffer->Height - (FigureEntity->FigureArea.h);
        //FigureEntity->FigureArea.x  = 0;
        
        FigureEntity->FigureArea = FigureAreaRect;
        
        //FigureEntity->FigureArea = ConvertMathRectToGameRect(FigureArea);
        
        FigureEntity->FigureUnit = (figure_unit*)calloc(FigureEntity->FigureAmountReserved, sizeof(figure_unit));
        Assert(FigureEntity->FigureUnit);
        
        FigureUnitAddNewFigure(FigureEntity, Z_figure, classic, 0.0f, Memory, Buffer);
        FigureUnitAddNewFigure(FigureEntity, I_figure, stone,   0.0f, Memory, Buffer);
        FigureUnitAddNewFigure(FigureEntity, J_figure, mirror,  0.0f, Memory, Buffer);
        
        FigureEntity->FigureAmount = 0;
        FigureUnitAddNewFigure(FigureEntity, Z_figure, classic, 0.0f, Memory, Buffer);
        FigureUnitAddNewFigure(FigureEntity, I_figure, stone,   0.0f, Memory, Buffer);
        FigureUnitAddNewFigure(FigureEntity, J_figure, mirror,  0.0f, Memory, Buffer);
        
        
        FigureEntity->FigureOrder = (u32*)malloc(sizeof(u32) * FigureEntity->FigureAmountReserved);
        Assert(FigureEntity->FigureOrder);
        
        for(u32 i = 0; i < FigureEntity->FigureAmountReserved; ++i) 
        {
            FigureEntity->FigureOrder[i] = i;
        }
        
        /* grid_entity initialization */
        
        LevelEntity->GridEntity  = (grid_entity *) malloc(sizeof(grid_entity));
        Assert(LevelEntity->GridEntity);
        
        grid_entity *GridEntity = LevelEntity->GridEntity;
        GridEntity->RowAmount           = RowAmount;
        GridEntity->ColumnAmount        = ColumnAmount;
        GridEntity->StickUnitsAmount    = FigureEntity->FigureAmount;
        GridEntity->MovingBlocksAmount  = 0;
        GridEntity->MovingBlocksAmountReserved  = MovingBlocksAmountReserved;
        
        GridEntity->GridArea.w = ActiveBlockSize * ColumnAmount;
        GridEntity->GridArea.h = ActiveBlockSize * RowAmount;
        GridEntity->GridArea.x = (Buffer->Width / 2) - (GridEntity->GridArea.w / 2);
        GridEntity->GridArea.y = (Buffer->Height - FigureEntity->FigureArea.h) / 2 - (GridEntity->GridArea.h / 2);
        
        LevelEntity->Configuration.GridBlockSize = CalculateGridBlockSize(RowAmount, ColumnAmount, 
                                                                          GridEntity->GridArea.w, GridEntity->GridArea.h);
        
        //LevelEntity->Configuration.GridBlockSize = ActiveBlockSize;
        printf("GridBlockSize = %d\n", LevelEntity->Configuration.GridBlockSize);
        
        //GridEntity->GridArea = ConvertMathRectToGameRect(GridArea);
        
        /* UnitField initialization */
        
        GridEntity->UnitField = (s32*)calloc(GridEntity->ColumnAmount * GridEntity->RowAmount, sizeof(s32));
        Assert(GridEntity->UnitField);
        for (u32 Row = 0; Row < GridEntity->RowAmount; ++Row)
        {
            for (u32 Col = 0; Col < GridEntity->ColumnAmount; ++Col)
            {
                GridEntity->UnitField[(Row * GridEntity->ColumnAmount) + Col] = 0;
            }
        }
        
        GridEntity->UnitSize = (r32*)calloc(GridEntity->ColumnAmount *GridEntity->RowAmount, sizeof(r32));
        Assert(GridEntity->UnitSize);
        for(u32 Row = 0; Row < GridEntity->RowAmount; ++Row)
        {
            for(u32 Col = 0; Col < GridEntity->ColumnAmount; ++Col)
            {
                s32 UnitIndex = (Row * GridEntity->ColumnAmount) + Col;
                GridEntity->UnitSize[UnitIndex] = 0;
            }
        }
        
        FigureEntityAlignHorizontally(FigureEntity, InActiveBlockSize);
        
        /* StickUnits initialization */
        
        GridEntity->StickUnits = (sticked_unit*)calloc(sizeof(sticked_unit), FigureEntity->FigureAmountReserved);
        Assert(GridEntity->StickUnits);
        for (u32 i = 0; i < FigureEntity->FigureAmountReserved; ++i)
        {
            GridEntity->StickUnits[i].Index     = -1;
            GridEntity->StickUnits[i].IsSticked = false;
        }
        
        /* MovingBlocks initialization */
        
        GridEntity->MovingBlocks = (moving_block*)malloc(sizeof(moving_block) * MovingBlocksAmountReserved);
        Assert(GridEntity->MovingBlocks);
        
        
        /* GridEntity texture initialization */
        
        GridEntity->NormalSquareTexture     = GetTexture(Memory, "grid_cell.png", Buffer->Renderer);
        GridEntity->VerticalSquareTexture   = GetTexture(Memory, "o_s.png", Buffer->Renderer);
        GridEntity->HorizontlaSquareTexture = GetTexture(Memory, "o_m.png", Buffer->Renderer);
        
        LevelEntityUpdateLevelNumber(LevelEntity, Memory, Buffer);
        
        /* menu_entity initialization */ 
        
        menu_entity *MenuEntity  = (menu_entity*) (((char*)Memory->LocalMemoryStorage) + (sizeof(level_entity))); 
        Assert(MenuEntity);
        
        MenuInit(MenuEntity, Memory, Buffer);
        
        /* EditorMemoryStorage allocation */
        
        Memory->EditorMemoryStorage = malloc(sizeof(level_editor) + sizeof(menu_editor));
        Assert(Memory->EditorMemoryStorage);
        
        level_editor *LevelEditor = (level_editor *) Memory->EditorMemoryStorage;
        menu_editor  *MenuEditor  = (menu_editor *) (((char*)Memory->EditorMemoryStorage) + (sizeof(level_editor))); 
        
        /* level_editor initialization */ 
        GameUpdateRelativePositions(Buffer, LevelEntity, Memory);
        LevelEditorInit(LevelEditor, LevelEntity, Memory, Buffer);
        
        /* menu_editor initialization */ 
        MenuEditorInit(MenuEditor, MenuEntity, Memory, Buffer);
        
        Memory->IsInitialized = true;
        printf("Memory has been initialized!\n");
    }
    
    level_entity *LevelEntity  = (level_entity *)Memory->LocalMemoryStorage;
    menu_entity  *MenuEntity   = (menu_entity*) (((char*)Memory->LocalMemoryStorage) + (sizeof(level_entity))); 
    
    level_editor *LevelEditor = (level_editor *) Memory->EditorMemoryStorage;
    menu_editor  *MenuEditor  = (menu_editor *) (((char*)Memory->EditorMemoryStorage) + (sizeof(level_editor))); 
    
    
    if(Input->Keyboard.Tab.EndedDown)
    {
        if (!Memory->ToggleMenu) 
        {
            Memory->ToggleMenu = true;
        }
        else
        {
            Memory->ToggleMenu = false;
        }
        
    }
    
    if(Input->Keyboard.Escape.EndedDown)
    {
        ShouldQuit = true;
    }
    
    if(Memory->ToggleMenu)
    {
        MenuUpdateAndRender(MenuEntity, Memory, Input, Buffer);
        MenuEditorUpdateAndRender(MenuEditor, MenuEntity, Memory, Input, Buffer);
    }
    else
    {
        game_rect ScreenArea = { 0, 0, Buffer->Width, Buffer->Height};
        DEBUGRenderQuadFill(Buffer, &ScreenArea, { 42, 6, 21 }, 255);
        DEBUGRenderQuad(Buffer, &Memory->PadRect, { 255, 255, 255 }, 255);
        DEBUGRenderQuad(Buffer, &LevelEntity->GridEntity->GridArea, { 0, 255, 0 }, 255);
        
        LevelEntityUpdateAndRender(LevelEntity, Memory, Input, Buffer);
        LevelEditorUpdateAndRender(LevelEditor, LevelEntity, Memory, Buffer, Input);
    }
    
    return(ShouldQuit);
}
