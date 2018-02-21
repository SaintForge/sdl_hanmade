// menu_game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

#include "menu.h"
#include "editor.h"

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
        MenuEntity->ButtonsArea[i].y = StartY;;
        MenuEntity->ButtonsArea[i].w = (ButtonWidth * ButtonsPerRow) + ((ButtonsPerRow - 1) * SpaceBetweenButtons);
        MenuEntity->ButtonsArea[i].h = (ButtonHeight * ButtonsPerColumn) + ((ButtonsPerColumn - 1) * SpaceBetweenButtons);
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

static void
MenuLoadButtonsFromMemory(menu_entity *MenuEntity, game_memory *Memory, 
                          game_offscreen_buffer *Buffer)
{
    MenuEntity->ButtonsAmountReserved = Memory->LevelMemoryReserved;
    MenuEntity->ButtonsAmount  = Memory->LevelMemoryAmount + 1;
    MenuEntity->NewButtonIndex = Memory->MenuEntity->ButtonsAmount - 1;
    
    if(MenuEntity->ButtonsArea)
    {
        free(MenuEntity->ButtonsArea);
    }
    if(MenuEntity->Buttons)
    {
        free(MenuEntity->Buttons);
    }
    
    MenuEntity->ButtonsArea = (game_rect *) malloc(sizeof(game_rect) * (MenuEntity->ButtonsAmountReserved / 20));
    Assert(MenuEntity->ButtonsArea);
    
    MenuEntity->Buttons = (menu_button *) calloc (MenuEntity->ButtonsAmountReserved, sizeof(menu_button));
    Assert(MenuEntity->Buttons);
    
    game_surface *Surface = 0; 
    
    for(u32 i = 0; i < MenuEntity->ButtonsAmount; ++i)
    {
        if(i == MenuEntity->ButtonsAmountReserved - 1) continue;
        
        char LevelNumber[3] = {0};
        sprintf(LevelNumber, "%d", Memory->LevelMemory[i].LevelNumber);
        
        Surface = TTF_RenderUTF8_Blended(Memory->LevelNumberFont, LevelNumber, {255, 255, 255});
        Assert(Surface);
        
        MenuEntity->Buttons[i].LevelNumberTextureQuad.w = Surface->w;
        MenuEntity->Buttons[i].LevelNumberTextureQuad.h = Surface->h;
        
        if(MenuEntity->Buttons[i].LevelNumberTexture)
        {
            FreeTexture(MenuEntity->Buttons[i].LevelNumberTexture);
        }
        
        MenuEntity->Buttons[i].LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
        Assert(MenuEntity->Buttons[i].LevelNumberTexture);
        
        SDL_FreeSurface(Surface);
        
        MenuEntity->Buttons[i].ButtonQuad.w = Memory->MenuEntity->ButtonSizeWidth;
        MenuEntity->Buttons[i].ButtonQuad.h = Memory->MenuEntity->ButtonSizeHeight; 
    }
    
    if(MenuEntity->ButtonsAmount < MenuEntity->ButtonsAmountReserved)
    {
        u32 Index = MenuEntity->ButtonsAmount - 1;
        
        Surface = TTF_RenderUTF8_Blended(Memory->LevelNumberFont, "+", {255, 255, 255 });
        Assert(Surface);
        
        MenuEntity->Buttons[Index].LevelNumberTextureQuad.w = Surface->w;
        MenuEntity->Buttons[Index].LevelNumberTextureQuad.h = Surface->h;
        
        if(MenuEntity->Buttons[Index].LevelNumberTexture)
        {
            FreeTexture(MenuEntity->Buttons[Index].LevelNumberTexture);
        }
        
        MenuEntity->Buttons[Index].LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
        Assert(MenuEntity->Buttons[Index].LevelNumberTexture);
        
        SDL_FreeSurface(Surface);
        
        MenuEntity->Buttons[Index].ButtonQuad.w = Memory->MenuEntity->ButtonSizeWidth;
        MenuEntity->Buttons[Index].ButtonQuad.h = Memory->MenuEntity->ButtonSizeHeight;
    }
    
    MenuEntityAlignButtons(MenuEntity, Buffer->Width, Buffer->Height);
}

static void 
MenuInit(menu_entity* MenuEntity, game_memory *Memory, game_offscreen_buffer *Buffer)
{
    Memory->MenuEntity = (menu_entity *) calloc(1, sizeof(menu_entity));
    Assert(Memory->MenuEntity);
    
    MenuEntity = Memory->MenuEntity;
    MenuEntity->IsMoving         = false;
    MenuEntity->IsAnimating      = false;
    MenuEntity->DevMode          = false;
    MenuEntity->IsShowingDelete  = false;
    MenuEntity->IsToBeDeleted    = false;
    MenuEntity->MaxVelocity      = 20.0f;
    MenuEntity->ButtonIndex      = -1;
    MenuEntity->ButtonSizeWidth  = 100;
    MenuEntity->ButtonSizeHeight = 100;
    
    MenuLoadButtonsFromMemory(MenuEntity, Memory, Buffer);
    
    MenuEntity->BackTexture = GetTexture(Memory, "grid_cell.png", Buffer->Renderer);
    
    /* Additional buttons for confirming deletion of a level*/
    
    game_surface *Surface = {};
    
    MenuEntity->ConfirmButtons = (menu_button *) malloc(sizeof(menu_button) * 2);
    Assert(MenuEntity->ConfirmButtons);
    
    MenuEntity->ConfirmButtons[0].ButtonQuad.w = MenuEntity->ButtonSizeWidth / 4;
    MenuEntity->ConfirmButtons[0].ButtonQuad.h = MenuEntity->ButtonSizeHeight / 4;
    
    Surface = TTF_RenderUTF8_Blended(Memory->LevelNumberFont, "Y", {0, 0, 255});
    Assert(Surface);
    
    MenuEntity->ConfirmButtons[0].LevelNumberTextureQuad.w = Surface->w < MenuEntity->ConfirmButtons[0].ButtonQuad.w ? Surface->w : MenuEntity->ConfirmButtons[0].ButtonQuad.w;
    
    MenuEntity->ConfirmButtons[0].LevelNumberTextureQuad.h = Surface->h < MenuEntity->ConfirmButtons[0].ButtonQuad.h ? Surface->h : MenuEntity->ConfirmButtons[0].ButtonQuad.h;
    
    MenuEntity->ConfirmButtons[0].LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    
    MenuEntity->ConfirmButtons[1].ButtonQuad.w = MenuEntity->ButtonSizeWidth / 4;
    MenuEntity->ConfirmButtons[1].ButtonQuad.h = MenuEntity->ButtonSizeHeight / 4;
    
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(Memory->LevelNumberFont, "N", {255, 0, 0});
    Assert(Surface);
    
    MenuEntity->ConfirmButtons[1].LevelNumberTextureQuad.w = Surface->w < MenuEntity->ConfirmButtons[1].ButtonQuad.w ? Surface->w : MenuEntity->ConfirmButtons[1].ButtonQuad.w;
    
    MenuEntity->ConfirmButtons[1].LevelNumberTextureQuad.h = Surface->h < MenuEntity->ConfirmButtons[1].ButtonQuad.h ? Surface->h : MenuEntity->ConfirmButtons[1].ButtonQuad.h;
    
    MenuEntity->ConfirmButtons[1].LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    
    SDL_FreeSurface(Surface);
    
    /* Save/Load Buttons for menu */
    
    MenuEntity->SaveAndLoadButtons = (menu_button *) malloc(sizeof(menu_button) * 2);
    Assert(MenuEntity->SaveAndLoadButtons);
    
    MenuEntity->SaveAndLoadButtons[0].ButtonQuad.x = 0;
    MenuEntity->SaveAndLoadButtons[0].ButtonQuad.y = 0;
    MenuEntity->SaveAndLoadButtons[0].ButtonQuad.w = MenuEntity->ButtonSizeWidth;
    MenuEntity->SaveAndLoadButtons[0].ButtonQuad.h = MenuEntity->ButtonSizeHeight / 2;
    
    Surface = TTF_RenderUTF8_Blended(Memory->LevelNumberFont, "save", {255, 255, 255});
    Assert(Surface);
    
    MenuEntity->SaveAndLoadButtons[0].LevelNumberTextureQuad.w = Surface->w < MenuEntity->SaveAndLoadButtons[0].ButtonQuad.w ? Surface->w : MenuEntity->SaveAndLoadButtons[0].ButtonQuad.w;
    
    MenuEntity->SaveAndLoadButtons[0].LevelNumberTextureQuad.h = Surface->h < MenuEntity->SaveAndLoadButtons[0].ButtonQuad.h ? Surface->h : MenuEntity->SaveAndLoadButtons[0].ButtonQuad.h;
    
    MenuEntity->SaveAndLoadButtons[0].LevelNumberTextureQuad.x = MenuEntity->SaveAndLoadButtons[0].ButtonQuad.x + (MenuEntity->SaveAndLoadButtons[0].ButtonQuad.w / 2) - (MenuEntity->SaveAndLoadButtons[0].LevelNumberTextureQuad.w / 2) ;
    MenuEntity->SaveAndLoadButtons[0].LevelNumberTextureQuad.y = MenuEntity->SaveAndLoadButtons[0].ButtonQuad.y + (MenuEntity->SaveAndLoadButtons[0].ButtonQuad.h / 2) - (MenuEntity->SaveAndLoadButtons[0].LevelNumberTextureQuad.h / 2) ;
    
    MenuEntity->SaveAndLoadButtons[0].LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    
    SDL_FreeSurface(Surface);
    
    MenuEntity->SaveAndLoadButtons[1].ButtonQuad.x = 0;
    MenuEntity->SaveAndLoadButtons[1].ButtonQuad.y = MenuEntity->ButtonSizeHeight / 2;
    MenuEntity->SaveAndLoadButtons[1].ButtonQuad.w = MenuEntity->ButtonSizeWidth;
    MenuEntity->SaveAndLoadButtons[1].ButtonQuad.h = MenuEntity->ButtonSizeHeight / 2;
    
    Surface = TTF_RenderUTF8_Blended(Memory->LevelNumberFont, "load", {255, 255, 255});
    Assert(Surface);
    
    MenuEntity->SaveAndLoadButtons[1].LevelNumberTextureQuad.w = Surface->w < MenuEntity->SaveAndLoadButtons[1].ButtonQuad.w ? Surface->w : MenuEntity->SaveAndLoadButtons[1].ButtonQuad.w;
    
    MenuEntity->SaveAndLoadButtons[1].LevelNumberTextureQuad.h = Surface->h < MenuEntity->SaveAndLoadButtons[1].ButtonQuad.h ? Surface->h : MenuEntity->SaveAndLoadButtons[1].ButtonQuad.h;
    
    MenuEntity->SaveAndLoadButtons[1].LevelNumberTextureQuad.x = MenuEntity->SaveAndLoadButtons[1].ButtonQuad.x + (MenuEntity->SaveAndLoadButtons[0].ButtonQuad.w / 2) - (MenuEntity->SaveAndLoadButtons[1].LevelNumberTextureQuad.w / 2) ;
    MenuEntity->SaveAndLoadButtons[1].LevelNumberTextureQuad.y = MenuEntity->SaveAndLoadButtons[1].ButtonQuad.y + (MenuEntity->SaveAndLoadButtons[0].ButtonQuad.h / 2) - (MenuEntity->SaveAndLoadButtons[1].LevelNumberTextureQuad.h / 2) ;
    
    MenuEntity->SaveAndLoadButtons[1].LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_FreeSurface(Surface);
    
    /* Sort Button */
    
    MenuEntity->SortButton = (menu_button *) malloc(sizeof(menu_button));
    Assert(MenuEntity->SortButton);
    
    MenuEntity->SortButton->ButtonQuad.x = 0;
    MenuEntity->SortButton->ButtonQuad.y = MenuEntity->SaveAndLoadButtons[1].ButtonQuad.y + MenuEntity->SaveAndLoadButtons[1].ButtonQuad.h;
    MenuEntity->SortButton->ButtonQuad.w = MenuEntity->ButtonSizeWidth;
    MenuEntity->SortButton->ButtonQuad.h = MenuEntity->ButtonSizeHeight / 2;
    
    Surface = TTF_RenderUTF8_Blended(Memory->LevelNumberFont, "sort", {255, 255, 255});
    Assert(Surface);
    
    MenuEntity->SortButton->LevelNumberTextureQuad.w = Surface->w < MenuEntity->SortButton->ButtonQuad.w ? Surface->w : MenuEntity->SortButton->ButtonQuad.w;
    
    MenuEntity->SortButton->LevelNumberTextureQuad.h = Surface->h < MenuEntity->SortButton->ButtonQuad.h ? Surface->h : MenuEntity->SortButton->ButtonQuad.h;
    
    MenuEntity->SortButton->LevelNumberTextureQuad.x = MenuEntity->SortButton->ButtonQuad.x + (MenuEntity->SortButton->ButtonQuad.w / 2) - (MenuEntity->SortButton->LevelNumberTextureQuad.w / 2);
    MenuEntity->SortButton->LevelNumberTextureQuad.y = MenuEntity->SortButton->ButtonQuad.y + (MenuEntity->SortButton->ButtonQuad.h / 2) - (MenuEntity->SortButton->LevelNumberTextureQuad.h / 2);
    
    MenuEntity->SortButton->LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    
    SDL_FreeSurface(Surface);
}

static void
MenuChangeButtonText(game_font *&Font, char *Text, 
                     menu_entity *MenuEntity, 
                     menu_button *Button, 
                     game_color Color, 
                     game_offscreen_buffer *Buffer)
{
    
    if(Button->LevelNumberTexture)
    {
        SDL_DestroyTexture(Button->LevelNumberTexture);
    }
    
    game_surface *Surface = TTF_RenderUTF8_Blended(Font, Text, Color);
    Assert(Surface);
    
    Button->LevelNumberTextureQuad.w = Surface->w;
    Button->LevelNumberTextureQuad.h = Surface->h;
    Button->LevelNumberTextureQuad.x = Button->ButtonQuad.x + (Button->ButtonQuad.w / 2) - (Surface->w / 2);
    Button->LevelNumberTextureQuad.y = Button->ButtonQuad.y + (Button->ButtonQuad.h / 2) - (Surface->h / 2);
    
    Button->LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    Assert(Button->LevelNumberTexture);
    
    SDL_FreeSurface(Surface);
    
    Button->ButtonQuad.w = MenuEntity->ButtonSizeWidth;
    Button->ButtonQuad.h = MenuEntity->ButtonSizeHeight;
    
}

static void
MenuDeleteLevel(menu_entity *MenuEntity,
                u32 Index, game_memory *Memory,
                game_offscreen_buffer *Buffer)
{
    if(Index >= Memory->LevelMemoryAmount) return;
    
    level_memory *LevelMemory = Memory->LevelMemory;
    
    if(LevelMemory[Index].UnitField)
    {
        free(LevelMemory[Index].UnitField);
    }
    
    if(LevelMemory[Index].MovingBlocks)
    {
        free(LevelMemory[Index].MovingBlocks);
    }
    
    if(LevelMemory[Index].Figures)
    {
        free(LevelMemory[Index].Figures);
    }
    
    if(MenuEntity->Buttons[Index].LevelNumberTexture)
    {
        FreeTexture(MenuEntity->Buttons[Index].LevelNumberTexture);
    }
    
    s32 LevelIndex = --Memory->LevelMemoryAmount;
    
    for(u32 i = Index, j = i + 1; i < LevelIndex; ++i,++j)
    {
        LevelMemory[i].LevelNumber  = LevelMemory[j].LevelNumber;
        LevelMemory[i].RowAmount    = LevelMemory[j].RowAmount;
        LevelMemory[i].ColumnAmount = LevelMemory[j].ColumnAmount;
        LevelMemory[i].MovingBlocksAmount = LevelMemory[j].MovingBlocksAmount;
        LevelMemory[i].FigureAmount = LevelMemory[j].FigureAmount;
        
        LevelMemory[i].UnitField    = LevelMemory[j].UnitField;
        LevelMemory[i].MovingBlocks = LevelMemory[j].MovingBlocks;
        LevelMemory[i].Figures      = LevelMemory[j].Figures;
    }
    
    LevelMemory[LevelIndex].LevelNumber = 0;
    LevelMemory[LevelIndex].RowAmount          = 0;
    LevelMemory[LevelIndex].ColumnAmount       = 0;
    LevelMemory[LevelIndex].MovingBlocksAmount = 0;
    LevelMemory[LevelIndex].FigureAmount       = 0;
    
    LevelMemory[LevelIndex].UnitField    = 0;
    LevelMemory[LevelIndex].MovingBlocks = 0;
    LevelMemory[LevelIndex].Figures      = 0;
    
    MenuLoadButtonsFromMemory(MenuEntity, Memory, Buffer);
}

inline void
SwapLevelMemory(level_memory *LevelMemory, s32 IndexA, s32 IndexB)
{
    level_memory TempLevel = LevelMemory[IndexA];
    LevelMemory[IndexA] = LevelMemory[IndexB];
    LevelMemory[IndexB] = TempLevel;
}

static void
MenuEntitySortButtons(menu_entity *MenuEntity, game_memory *Memory)
{
    level_memory *LevelMemory = Memory->LevelMemory;
    s32 LevelAmount = Memory->LevelMemoryAmount;
    
    for(s32 i = 0; i < LevelAmount - 1; ++i)
    {
        for(s32 j = 0; j < LevelAmount - i - 1; ++j)
        {
            if(LevelMemory[j].LevelNumber > LevelMemory[j+1].LevelNumber)
            {
                SwapLevelMemory(LevelMemory, j, j+1);
            }
        }
    }
}

static void
MenuUpdateAndRender(menu_entity *MenuEntity, game_memory *Memory, 
                    game_input *Input, game_offscreen_buffer *Buffer)
{
    
    if(Input->Keyboard.BackQuote.EndedDown)
    {
        if(MenuEntity->DevMode)
        {
            MenuEntity->DevMode = false;
        }
        else
        {
            MenuEntity->DevMode = true;
        }
        
        Input->Keyboard.BackQuote.EndedDown = false;
    }
    
    if(Input->MouseButtons[0].EndedDown)
    {
        MenuEntity->IsMoving    = true;
        MenuEntity->IsAnimating = false;
        
        MenuEntity->OldMouseX      = Input->MouseX;
        MenuEntity->ScrollingTicks = SDL_GetTicks();
        
        MenuEntity->TargetPosition  = 0;
        MenuEntity->Velocity.x      = 0;
        
        u32 BeginIndex = MenuEntity->TargetIndex * 20;
        u32 EndIndex   = BeginIndex + 20;
        
        for(u32 i = BeginIndex; i < EndIndex; ++i)
        {
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEntity->Buttons[i].ButtonQuad))
            {
                MenuEntity->ButtonIndex = i;
                break;
            }
        }
        
        if(MenuEntity->DevMode)
        {
            if(MenuEntity->IsShowingDelete)
            {
                if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEntity->ConfirmButtons[0].ButtonQuad))
                {
                    /* Deleting the level */
                    MenuDeleteLevel(MenuEntity, MenuEntity->ButtonIndex,
                                    Memory, Buffer);
                    Memory->CurrentLevelIndex -= 1;
                }
                
                MenuEntity->IsShowingDelete = false;
            }
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEntity->SaveAndLoadButtons[0].ButtonQuad))
            {
                /* Saving levels to the file */ 
                SaveLevelMemoryToFile(Memory);
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEntity->SaveAndLoadButtons[1].ButtonQuad))
            {
                /* Loading levels from the file */ 
                LoadLevelMemoryFromFile("package2.bin", Memory);
                MenuLoadButtonsFromMemory(MenuEntity, Memory, Buffer);
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEntity->SortButton->ButtonQuad))
            {
                /* Sorting levels in level_memory */
                MenuEntitySortButtons(MenuEntity, Memory);
                MenuLoadButtonsFromMemory(MenuEntity, Memory, Buffer);
                
            }
        }
        
    }
    else if(Input->MouseButtons[0].EndedUp)
    {
        MenuEntity->IsMoving    = false;
        MenuEntity->IsAnimating = true;
        MenuEntity->ScrollingTicks = SDL_GetTicks() - MenuEntity->ScrollingTicks;
        MenuEntity->MaxVelocity = 20.0f;
        
        u32 ButtonsAreaAmount = (MenuEntity->ButtonsAmount / 20) + 1;
        
        if(abs(MenuEntity->TargetPosition) >= MenuEntity->ButtonSizeWidth * 0.5f)
        {
            s32 Center_x = 0;
            s32 Center_y = 0;
            s32 CenterOffset = 0;
            
            if(MenuEntity->ScrollingTicks < 500)
            {
                bool ShouldJump = false;
                
                s32 LeftBorder  = MenuEntity->ButtonsArea[0].x;
                s32 RightBorder = MenuEntity->ButtonsArea[ButtonsAreaAmount-1].x + MenuEntity->ButtonsArea[ButtonsAreaAmount-1].w;
                
                ShouldJump = (LeftBorder >= Buffer->Width / 2) || (RightBorder <= Buffer->Width / 2);
                
                if(!ShouldJump)
                {
                    s32 TargetOffset = MenuEntity->TargetPosition;
                    if(abs(TargetOffset) < MenuEntity->ButtonsArea[0].w)
                    {
                        TargetOffset = TargetOffset > 0
                            ? Buffer->Width
                            : -Buffer->Width;
                        
                        MenuEntity->MaxVelocity   *= 2;
                        MenuEntity->TargetPosition = TargetOffset;
                        MenuEntity->ButtonIndex    = -1;
                    }
                }
            }
            
            if(MenuEntity->TargetPosition > 0)
            {
                Center_x = MenuEntity->ButtonsArea[MenuEntity->TargetIndex].x + (MenuEntity->ButtonsArea[MenuEntity->TargetIndex].w * 0.2f);
            }
            else
            {
                Center_x = MenuEntity->ButtonsArea[MenuEntity->TargetIndex].x + (MenuEntity->ButtonsArea[MenuEntity->TargetIndex].w * 0.8f);
            }
            
            CenterOffset = Center_x - MenuEntity->TargetPosition;
            
            for(u32 i = 0; i < ButtonsAreaAmount; ++i)
            {
                game_rect TargetArea;
                TargetArea.x = MenuEntity->ButtonsArea[i].x + (MenuEntity->ButtonsArea[i].w / 2) - (Buffer->Width / 2);
                TargetArea.y = 0;
                TargetArea.w = Buffer->Width;
                TargetArea.h = Buffer->Height;
                
                if(IsPointInsideRect(CenterOffset, Center_y, &TargetArea))
                {
                    MenuEntity->TargetIndex = i;
                    break;
                }
            }
            
            MenuEntity->TargetPosition = Buffer->Width / 2;
        }
        else
        {
            MenuEntity->TargetPosition = Buffer->Width / 2;
            
            s32 Index = MenuEntity->ButtonIndex;
            if(Index == MenuEntity->NewButtonIndex)
            {
                if(MenuEntity->DevMode)
                {
                    LevelEntityUpdateLevelEntityFromMemory(&Memory->LevelEntity,
                                                           Index, false,
                                                           Memory, Buffer);
                    
                    LevelEditorChangeGridCounters(Memory->LevelEditor,
                                                  Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount, Buffer);
                    
                    LevelEditorUpdateLevelStats(Memory->LevelEditor, 
                                                Memory->LevelEntity.LevelNumber, Index, Buffer);
                    
                    MenuEntity->ButtonsAmount += 1;
                    
                    char LevelNumber[3] = {0};
                    sprintf(LevelNumber, "%d", Memory->LevelEntity.LevelNumber);
                    
                    MenuChangeButtonText(Memory->LevelNumberFont, LevelNumber, 
                                         MenuEntity, 
                                         &MenuEntity->Buttons[Index], 
                                         {255, 255, 255}, 
                                         Buffer);
                    MenuChangeButtonText(Memory->LevelNumberFont, "+", 
                                         MenuEntity, 
                                         &MenuEntity->Buttons[Index+1], 
                                         {255, 255, 255}, 
                                         Buffer);
                    MenuEntity->NewButtonIndex = Index+1;
                    
                    MenuEntityAlignButtons(MenuEntity, Buffer->Width, Buffer->Height);
                    
                    Memory->LevelMemoryAmount += 1;
                }
                
            }
            else if(Index >= 0)
            {
                if(!MenuEntity->DevMode)
                {
                    LevelEntityUpdateLevelEntityFromMemory(&Memory->LevelEntity, 
                                                           Index, false,
                                                           Memory, Buffer);
                    LevelEditorChangeGridCounters(Memory->LevelEditor, 
                                                  Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount,Buffer);
                    
                    
                    LevelEditorUpdateLevelStats(Memory->LevelEditor, 
                                                Memory->LevelEntity.LevelNumber, Index, Buffer);
                    Memory->ToggleMenu = false;
                    Memory->CurrentLevelIndex = Index;
                }
            }
            
            MenuEntity->ButtonIndex = -1;
        }
        
    }
    else if(Input->MouseButtons[1].EndedUp)
    {
        printf("Right mouse click\n");
        
        if(MenuEntity->DevMode)
        {
            s32 Index = -1;
            
            u32 BeginIndex = MenuEntity->TargetIndex * 20;
            u32 EndIndex   = BeginIndex + 20;
            
            for(u32 i = BeginIndex; i < EndIndex; ++i)
            {
                if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEntity->Buttons[i].ButtonQuad))
                {
                    Index = i;
                    break;
                }
            }
            
            if(Index >= 0 && Index != MenuEntity->NewButtonIndex)
            {
                MenuEntity->IsShowingDelete = true;
                
                MenuEntity->ConfirmButtons[0].ButtonQuad.x = MenuEntity->Buttons[Index].ButtonQuad.x + MenuEntity->Buttons[Index].ButtonQuad.w - (MenuEntity->ConfirmButtons[0].ButtonQuad.w * 2);
                MenuEntity->ConfirmButtons[0].ButtonQuad.y = MenuEntity->Buttons[Index].ButtonQuad.y;
                
                MenuEntity->ConfirmButtons[0].LevelNumberTextureQuad.x = MenuEntity->ConfirmButtons[0].ButtonQuad.x + (MenuEntity->ConfirmButtons[0].ButtonQuad.w / 2) - (MenuEntity->ConfirmButtons[0].LevelNumberTextureQuad.w / 2);
                
                MenuEntity->ConfirmButtons[0].LevelNumberTextureQuad.y = MenuEntity->ConfirmButtons[0].ButtonQuad.y + (MenuEntity->ConfirmButtons[0].ButtonQuad.h / 2) - (MenuEntity->ConfirmButtons[0].LevelNumberTextureQuad.h / 2);
                
                MenuEntity->ConfirmButtons[1].ButtonQuad.x = MenuEntity->Buttons[Index].ButtonQuad.x + MenuEntity->Buttons[Index].ButtonQuad.w - (MenuEntity->ConfirmButtons[1].ButtonQuad.w);
                MenuEntity->ConfirmButtons[1].ButtonQuad.y = MenuEntity->Buttons[Index].ButtonQuad.y;
                
                MenuEntity->ConfirmButtons[1].LevelNumberTextureQuad.x = MenuEntity->ConfirmButtons[1].ButtonQuad.x + (MenuEntity->ConfirmButtons[1].ButtonQuad.w / 2) - (MenuEntity->ConfirmButtons[1].LevelNumberTextureQuad.w / 2);
                
                MenuEntity->ConfirmButtons[1].LevelNumberTextureQuad.y = MenuEntity->ConfirmButtons[1].ButtonQuad.y + (MenuEntity->ConfirmButtons[1].ButtonQuad.h / 2) - (MenuEntity->ConfirmButtons[1].LevelNumberTextureQuad.h / 2);
            }
            
        }
    }
    
    s32 OffsetX = 0;
    if(MenuEntity->IsMoving)
    {
        OffsetX += Input->MouseRelX;
        MenuEntity->TargetPosition += OffsetX;
        
        if(abs(MenuEntity->TargetPosition) >= MenuEntity->ButtonSizeWidth * 0.5f)
        {
            MenuEntity->ButtonIndex = -1;
        }
    }
    
    
    if(MenuEntity->IsAnimating)
    {
        s32 RelativeCenter = MenuEntity->ButtonsArea[MenuEntity->TargetIndex].x + (MenuEntity->ButtonsArea[MenuEntity->TargetIndex].w/2);
        
        vector2 Vector = { MenuEntity->TargetPosition - RelativeCenter, 0 };
        vector2 Acceleration = {0};
        
        r32 Ratio = 0;
        r32 Distance = Vector2Mag(&Vector);
        r32 ApproachRadius = 50.0f;
        Vector2Norm(&Vector);
        if(Distance < ApproachRadius)
        {
            Ratio = Distance / ApproachRadius;
            if(Ratio > 0.02f)
            {
                Vector.x *= Distance / ApproachRadius * MenuEntity->MaxVelocity;
                Vector.y *= Distance / ApproachRadius * MenuEntity->MaxVelocity;
            }
            else
            {
                s32 Width = (Buffer->Width / 2) - (MenuEntity->ButtonsArea[MenuEntity->TargetIndex].w / 2);
                s32 Offset = Width - MenuEntity->ButtonsArea[MenuEntity->TargetIndex].x;
                
                Vector.x = 0;
                Vector.y = 0;
                
                MenuEntity->Velocity.x     = 0;
                MenuEntity->TargetPosition = 0;
                MenuEntity->IsAnimating    = false;
                
                u32 ButtonsAreaAmount = (MenuEntity->ButtonsAmount / 20) + 1;
                for(u32 i = 0; i < ButtonsAreaAmount; ++i)
                {
                    MenuEntity->ButtonsArea[i].x += Offset;
                }
                
                for(u32 i = 0; i < MenuEntity->ButtonsAmount; ++i)
                {
                    MenuEntity->Buttons[i].ButtonQuad.x += Offset;
                    MenuEntity->Buttons[i].LevelNumberTextureQuad.x += Offset;
                }
            }
        }
        else
        {
            Vector.x *= MenuEntity->MaxVelocity;
            Vector.y *= MenuEntity->MaxVelocity;
        }
        
        
        Acceleration.x = Vector.x - MenuEntity->Velocity.x;
        Vector2Add(&MenuEntity->Velocity, &Acceleration);
    }
    
    //
    // Menu Rendering
    //
    
    game_rect ScreenQuad = {0, 0, Buffer->Width, Buffer->Height };
    DEBUGRenderQuadFill(Buffer, &ScreenQuad, {0, 0, 0}, 0);
    
    u32 ButtonsAreaAmount = (MenuEntity->ButtonsAmount / 20) + 1;
    for(u32 i = 0; i < ButtonsAreaAmount; ++i)
    {
        if(MenuEntity->IsMoving)
        {
            MenuEntity->ButtonsArea[i].x += OffsetX;
            {
                game_rect TargetArea;
                TargetArea.x = MenuEntity->ButtonsArea[i].x + (MenuEntity->ButtonsArea[i].w / 2) - (Buffer->Width / 2);
                TargetArea.y = 0;
                TargetArea.w = Buffer->Width;
                TargetArea.h = Buffer->Height;
                
                if(MenuEntity->DevMode)
                {
                    DEBUGRenderQuad(Buffer, &TargetArea, {255, 0, 0}, 255);
                }
                
            }
        }
        
        if(MenuEntity->IsAnimating)
        {
            MenuEntity->ButtonsArea[i].x += roundf(MenuEntity->Velocity.x);
        }
        
        if(MenuEntity->DevMode)
        {
            DEBUGRenderQuad(Buffer, &MenuEntity->ButtonsArea[i], {255, 255, 255}, 255);
        }
        
    }
    
    for(u32 i = 0; i < MenuEntity->ButtonsAmount; ++i)
    {
        if(MenuEntity->IsMoving)
        {
            MenuEntity->Buttons[i].ButtonQuad.x += OffsetX;
            MenuEntity->Buttons[i].LevelNumberTextureQuad.x += OffsetX;
        }
        
        if(MenuEntity->IsAnimating)
        {
            MenuEntity->Buttons[i].ButtonQuad.x += roundf(MenuEntity->Velocity.x);
            MenuEntity->Buttons[i].LevelNumberTextureQuad.x += roundf(MenuEntity->Velocity.x);
        }
        if(MenuEntity->NewButtonIndex == i)
        {
            if(MenuEntity->DevMode)
            {
                GameRenderBitmapToBuffer(Buffer, MenuEntity->BackTexture, &MenuEntity->Buttons[i].ButtonQuad);
                GameRenderBitmapToBuffer(Buffer, MenuEntity->Buttons[i].LevelNumberTexture,
                                         &MenuEntity->Buttons[i].LevelNumberTextureQuad);
            }
        }
        else if(MenuEntity->ButtonIndex == i)
        {
            game_point AreaCenter = {0};
            AreaCenter.x = MenuEntity->Buttons[i].ButtonQuad.x + (MenuEntity->Buttons[i].ButtonQuad.w / 2);
            AreaCenter.y = MenuEntity->Buttons[i].ButtonQuad.y + (MenuEntity->Buttons[i].ButtonQuad.h / 2);
            
            game_rect AreaQuad = {0};
            AreaQuad.w = MenuEntity->Buttons[i].ButtonQuad.w * 1.2f;
            AreaQuad.h = MenuEntity->Buttons[i].ButtonQuad.h * 1.2f;
            AreaQuad.x = AreaCenter.x - (AreaQuad.w / 2);
            AreaQuad.y = AreaCenter.y - (AreaQuad.h / 2);
            
            game_rect NumberQuad = {0};
            NumberQuad.w = MenuEntity->Buttons[i].LevelNumberTextureQuad.w * 1.2f;
            NumberQuad.h = MenuEntity->Buttons[i].LevelNumberTextureQuad.h * 1.2f;
            NumberQuad.x = AreaCenter.x - (NumberQuad.w / 2);
            NumberQuad.y = AreaCenter.y - (NumberQuad.h / 2);
            
            GameRenderBitmapToBuffer(Buffer, MenuEntity->BackTexture, &AreaQuad);
            GameRenderBitmapToBuffer(Buffer, MenuEntity->Buttons[i].LevelNumberTexture,
                                     &NumberQuad);
        }
        else
        {
            GameRenderBitmapToBuffer(Buffer, MenuEntity->BackTexture, &MenuEntity->Buttons[i].ButtonQuad);
            GameRenderBitmapToBuffer(Buffer, MenuEntity->Buttons[i].LevelNumberTexture,
                                     &MenuEntity->Buttons[i].LevelNumberTextureQuad);
        }
    }
    
    if(MenuEntity->DevMode)
    {
        if(MenuEntity->IsMoving)
        {
            s32 ButtonCenter = MenuEntity->ButtonsArea[MenuEntity->TargetIndex].x + (MenuEntity->ButtonsArea[MenuEntity->TargetIndex].w / 2);
            s32 Target = ButtonCenter + MenuEntity->TargetPosition;
            DEBUGRenderLine(Buffer, 
                            ButtonCenter, Buffer->Height / 2,
                            Target, Buffer->Height / 2,
                            {0, 255, 0}, 255);
        }
        
        if(MenuEntity->IsShowingDelete)
        {
            DEBUGRenderQuadFill(Buffer, &MenuEntity->ConfirmButtons[0].ButtonQuad, {0, 255, 0}, 255);
            DEBUGRenderQuad(Buffer, &MenuEntity->ConfirmButtons[0].ButtonQuad, {0, 0, 0}, 255);
            GameRenderBitmapToBuffer(Buffer, MenuEntity->ConfirmButtons[0].LevelNumberTexture, &MenuEntity->ConfirmButtons[0].LevelNumberTextureQuad);
            
            DEBUGRenderQuadFill(Buffer, &MenuEntity->ConfirmButtons[1].ButtonQuad, {0, 255, 0}, 255);
            DEBUGRenderQuad(Buffer, &MenuEntity->ConfirmButtons[1].ButtonQuad, {0, 0, 0}, 255);
            GameRenderBitmapToBuffer(Buffer, MenuEntity->ConfirmButtons[1].LevelNumberTexture, &MenuEntity->ConfirmButtons[1].LevelNumberTextureQuad);
        }
        
        /* Save button rendering */
        
        DEBUGRenderQuadFill(Buffer, &MenuEntity->SaveAndLoadButtons[0].ButtonQuad, {0, 255, 0}, 100);
        DEBUGRenderQuad(Buffer, &MenuEntity->SaveAndLoadButtons[0].ButtonQuad, {0, 0, 255}, 255);
        GameRenderBitmapToBuffer(Buffer, MenuEntity->SaveAndLoadButtons[0].LevelNumberTexture, &MenuEntity->SaveAndLoadButtons[0].LevelNumberTextureQuad);
        
        /* Load button rendering */
        
        DEBUGRenderQuadFill(Buffer, &MenuEntity->SaveAndLoadButtons[1].ButtonQuad, {0, 255, 0}, 100);
        DEBUGRenderQuad(Buffer, &MenuEntity->SaveAndLoadButtons[1].ButtonQuad, {0, 0, 255}, 255);
        GameRenderBitmapToBuffer(Buffer, MenuEntity->SaveAndLoadButtons[1].LevelNumberTexture, &MenuEntity->SaveAndLoadButtons[1].LevelNumberTextureQuad);
        
        /* Sort button rendering */
        
        DEBUGRenderQuadFill(Buffer, &MenuEntity->SortButton->ButtonQuad, {0, 255, 0}, 100);
        DEBUGRenderQuad(Buffer, &MenuEntity->SortButton->ButtonQuad, {0, 0, 255}, 255);
        GameRenderBitmapToBuffer(Buffer, MenuEntity->SortButton->LevelNumberTexture, &MenuEntity->SortButton->LevelNumberTextureQuad);
        
    }
}


