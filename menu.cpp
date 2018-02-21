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
    
    //
    // Additional buttons for confirming deletion of a level
    //
    
    game_surface *Surface = {};
    
    Memory->MenuEntity->ConfirmButtons = (menu_button *) malloc(sizeof(menu_button) * 2);
    Assert(Memory->MenuEntity->ConfirmButtons);
    
    Memory->MenuEntity->ConfirmButtons[0].ButtonQuad.w = Memory->MenuEntity->ButtonSizeWidth / 4;
    Memory->MenuEntity->ConfirmButtons[0].ButtonQuad.h = Memory->MenuEntity->ButtonSizeHeight / 4;
    
    Surface = TTF_RenderUTF8_Blended(Memory->LevelNumberFont, "Y", {0, 0, 255});
    Assert(Surface);
    
    Memory->MenuEntity->ConfirmButtons[0].LevelNumberTextureQuad.w = Surface->w < Memory->MenuEntity->ConfirmButtons[0].ButtonQuad.w ? Surface->w : Memory->MenuEntity->ConfirmButtons[0].ButtonQuad.w;
    
    Memory->MenuEntity->ConfirmButtons[0].LevelNumberTextureQuad.h = Surface->h < Memory->MenuEntity->ConfirmButtons[0].ButtonQuad.h ? Surface->h : Memory->MenuEntity->ConfirmButtons[0].ButtonQuad.h;
    
    Memory->MenuEntity->ConfirmButtons[0].LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    
    Memory->MenuEntity->ConfirmButtons[1].ButtonQuad.w = Memory->MenuEntity->ButtonSizeWidth / 4;
    Memory->MenuEntity->ConfirmButtons[1].ButtonQuad.h = Memory->MenuEntity->ButtonSizeHeight / 4;
    
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(Memory->LevelNumberFont, "N", {255, 0, 0});
    Assert(Surface);
    
    Memory->MenuEntity->ConfirmButtons[1].LevelNumberTextureQuad.w = Surface->w < Memory->MenuEntity->ConfirmButtons[1].ButtonQuad.w ? Surface->w : Memory->MenuEntity->ConfirmButtons[1].ButtonQuad.w;
    
    Memory->MenuEntity->ConfirmButtons[1].LevelNumberTextureQuad.h = Surface->h < Memory->MenuEntity->ConfirmButtons[1].ButtonQuad.h ? Surface->h : Memory->MenuEntity->ConfirmButtons[1].ButtonQuad.h;
    
    Memory->MenuEntity->ConfirmButtons[1].LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    
    SDL_FreeSurface(Surface);
    
    //
    // Save/Load Buttons for menu
    //
    
    Memory->MenuEntity->SaveAndLoadButtons = (menu_button *) malloc(sizeof(menu_button) * 2);
    Assert(Memory->MenuEntity->SaveAndLoadButtons);
    
    Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad.x = 0;
    Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad.y = 0;
    Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad.w = Memory->MenuEntity->ButtonSizeWidth;
    Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad.h = Memory->MenuEntity->ButtonSizeHeight / 2;
    
    Surface = TTF_RenderUTF8_Blended(Memory->LevelNumberFont, "save", {255, 255, 255});
    Assert(Surface);
    
    Memory->MenuEntity->SaveAndLoadButtons[0].LevelNumberTextureQuad.w = Surface->w < Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad.w ? Surface->w : Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad.w;
    
    Memory->MenuEntity->SaveAndLoadButtons[0].LevelNumberTextureQuad.h = Surface->h < Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad.h ? Surface->h : Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad.h;
    
    Memory->MenuEntity->SaveAndLoadButtons[0].LevelNumberTextureQuad.x = Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad.x + (Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad.w / 2) - (Memory->MenuEntity->SaveAndLoadButtons[0].LevelNumberTextureQuad.w / 2) ;
    Memory->MenuEntity->SaveAndLoadButtons[0].LevelNumberTextureQuad.y = Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad.y + (Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad.h / 2) - (Memory->MenuEntity->SaveAndLoadButtons[0].LevelNumberTextureQuad.h / 2) ;
    
    Memory->MenuEntity->SaveAndLoadButtons[0].LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    
    SDL_FreeSurface(Surface);
    
    Memory->MenuEntity->SaveAndLoadButtons[1].ButtonQuad.x = 0;
    Memory->MenuEntity->SaveAndLoadButtons[1].ButtonQuad.y = Memory->MenuEntity->ButtonSizeHeight / 2;
    Memory->MenuEntity->SaveAndLoadButtons[1].ButtonQuad.w = Memory->MenuEntity->ButtonSizeWidth;
    Memory->MenuEntity->SaveAndLoadButtons[1].ButtonQuad.h = Memory->MenuEntity->ButtonSizeHeight / 2;
    
    Surface = TTF_RenderUTF8_Blended(Memory->LevelNumberFont, "load", {255, 255, 255});
    Assert(Surface);
    
    Memory->MenuEntity->SaveAndLoadButtons[1].LevelNumberTextureQuad.w = Surface->w < Memory->MenuEntity->SaveAndLoadButtons[1].ButtonQuad.w ? Surface->w : Memory->MenuEntity->SaveAndLoadButtons[1].ButtonQuad.w;
    
    Memory->MenuEntity->SaveAndLoadButtons[1].LevelNumberTextureQuad.h = Surface->h < Memory->MenuEntity->SaveAndLoadButtons[1].ButtonQuad.h ? Surface->h : Memory->MenuEntity->SaveAndLoadButtons[1].ButtonQuad.h;
    
    Memory->MenuEntity->SaveAndLoadButtons[1].LevelNumberTextureQuad.x = Memory->MenuEntity->SaveAndLoadButtons[1].ButtonQuad.x + (Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad.w / 2) - (Memory->MenuEntity->SaveAndLoadButtons[1].LevelNumberTextureQuad.w / 2) ;
    Memory->MenuEntity->SaveAndLoadButtons[1].LevelNumberTextureQuad.y = Memory->MenuEntity->SaveAndLoadButtons[1].ButtonQuad.y + (Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad.h / 2) - (Memory->MenuEntity->SaveAndLoadButtons[1].LevelNumberTextureQuad.h / 2) ;
    
    Memory->MenuEntity->SaveAndLoadButtons[1].LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
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
MenuDeleteLevel(game_offscreen_buffer *Buffer, 
                game_memory *Memory, u32 Index)
{
    if(Index >= Memory->LevelMemoryAmount) return;
    
    //TODO gotta delete the one that needs to be deleted first!!!
    
    if(Memory->LevelMemory[Index].UnitField)
    {
        free(Memory->LevelMemory[Index].UnitField);
    }
    
    if(Memory->LevelMemory[Index].MovingBlocks)
    {
        free(Memory->LevelMemory[Index].MovingBlocks);
    }
    
    if(Memory->LevelMemory[Index].Figures)
    {
        free(Memory->LevelMemory[Index].Figures);
    }
    
    for(u32 i = Index; i < Memory->LevelMemoryAmount-1; ++i)
    {
        Memory->LevelMemory[i].LevelNumber  = Memory->LevelMemory[i+1].LevelNumber;
        Memory->LevelMemory[i].RowAmount    = Memory->LevelMemory[i+1].RowAmount;
        Memory->LevelMemory[i].ColumnAmount = Memory->LevelMemory[i+1].ColumnAmount;
        Memory->LevelMemory[i].MovingBlocksAmount = Memory->LevelMemory[i+1].MovingBlocksAmount;
        Memory->LevelMemory[i].FigureAmount = Memory->LevelMemory[i+1].FigureAmount;
        
        Memory->LevelMemory[i].UnitField    = Memory->LevelMemory[i+1].UnitField;
        Memory->LevelMemory[i].MovingBlocks = Memory->LevelMemory[i+1].MovingBlocks;
        Memory->LevelMemory[i].Figures      = Memory->LevelMemory[i+1].Figures;
    }
    
    Memory->LevelMemoryAmount  -= 1;
    
    Memory->MenuEntity->ButtonsAmount  -= 1;
    Memory->MenuEntity->NewButtonIndex -= 1;
    
    MenuChangeButtonText(Memory->LevelNumberFont, "+", 
                         Memory->MenuEntity, 
                         &Memory->MenuEntity->Buttons[Memory->MenuEntity->NewButtonIndex], 
                         {255, 255 ,255}, 
                         Buffer);
    
    
    Memory->LevelMemory[Memory->LevelMemoryAmount].RowAmount          = 0;
    Memory->LevelMemory[Memory->LevelMemoryAmount].ColumnAmount       = 0;
    Memory->LevelMemory[Memory->LevelMemoryAmount].MovingBlocksAmount = 0;
    Memory->LevelMemory[Memory->LevelMemoryAmount].FigureAmount       = 0;
    
    Memory->LevelMemory[Memory->LevelMemoryAmount].UnitField    = NULL;
    Memory->LevelMemory[Memory->LevelMemoryAmount].MovingBlocks = NULL;
    Memory->LevelMemory[Memory->LevelMemoryAmount].Figures      = NULL;
}



static void
MenuUpdateAndRender(game_offscreen_buffer *Buffer, game_memory *Memory, game_input *Input)
{
    s32 OffsetX = 0;
    
    if(Input->Keyboard.BackQuote.EndedDown)
    {
        if(Memory->MenuEntity->DevMode)
        {
            Memory->MenuEntity->DevMode = false;
        }
        else
        {
            Memory->MenuEntity->DevMode = true;
        }
        
        Input->Keyboard.BackQuote.EndedDown = false;
    }
    
    if(Input->MouseButtons[0].EndedDown)
    {
        Memory->MenuEntity->IsMoving    = true;
        Memory->MenuEntity->IsAnimating = false;
        
        Memory->MenuEntity->OldMouseX      = Input->MouseX;
        Memory->MenuEntity->ScrollingTicks = SDL_GetTicks();
        
        Memory->MenuEntity->TargetPosition  = 0;
        Memory->MenuEntity->Velocity.x      = 0;
        
        u32 BeginIndex = Memory->MenuEntity->TargetIndex * 20;
        u32 EndIndex   = BeginIndex + 20;
        
        for(u32 i = BeginIndex; i < EndIndex; ++i)
        {
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Memory->MenuEntity->Buttons[i].ButtonQuad))
            {
                Memory->MenuEntity->ButtonIndex = i;
                break;
            }
        }
        
        if(Memory->MenuEntity->DevMode)
        {
            if(Memory->MenuEntity->IsShowingDelete)
            {
                if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Memory->MenuEntity->ConfirmButtons[0].ButtonQuad))
                {
                    Memory->MenuEntity->IsToBeDeleted = true;
                }
                
                Memory->MenuEntity->IsShowingDelete = false;
            }
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad))
            {
                /* Saving levels to the file */ 
                
                SaveLevelMemoryToFile(Memory);
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Memory->MenuEntity->SaveAndLoadButtons[1].ButtonQuad))
            {
                /* Loading levels from the file */ 
                
                LoadLevelMemoryFromFile("package2.bin", Memory);
                MenuLoadButtonsFromMemory(Memory->MenuEntity, Memory, Buffer);
            }
        }
        
    }
    else if(Input->MouseButtons[0].EndedUp)
    {
        Memory->MenuEntity->IsMoving    = false;
        Memory->MenuEntity->IsAnimating = true;
        Memory->MenuEntity->ScrollingTicks = SDL_GetTicks() - Memory->MenuEntity->ScrollingTicks;
        Memory->MenuEntity->MaxVelocity = 20.0f;
        //Memory->MenuEntity->ButtonIndex = -1;
        
        u32 ButtonsAreaAmount = (Memory->MenuEntity->ButtonsAmount / 20) + 1;
        
        if(abs(Memory->MenuEntity->TargetPosition) >= Memory->MenuEntity->ButtonSizeWidth * 0.5f)
        {
            s32 Center_x = 0;
            s32 Center_y = 0;
            s32 CenterOffset = 0;
            
            if(Memory->MenuEntity->ScrollingTicks < 500)
            {
                bool ShouldJump = false;
                
                s32 LeftBorder  = Memory->MenuEntity->ButtonsArea[0].x;
                s32 RightBorder = Memory->MenuEntity->ButtonsArea[ButtonsAreaAmount-1].x + Memory->MenuEntity->ButtonsArea[ButtonsAreaAmount-1].w;
                
                ShouldJump = (LeftBorder >= Buffer->Width / 2) || (RightBorder <= Buffer->Width / 2);
                
                if(!ShouldJump)
                {
                    s32 TargetOffset = Memory->MenuEntity->TargetPosition;
                    if(abs(TargetOffset) < Memory->MenuEntity->ButtonsArea[0].w)
                    {
                        TargetOffset = TargetOffset > 0
                            ? Buffer->Width
                            : -Buffer->Width;
                        
                        Memory->MenuEntity->MaxVelocity   *= 2;
                        Memory->MenuEntity->TargetPosition = TargetOffset;
                        Memory->MenuEntity->ButtonIndex    = -1;
                    }
                }
            }
            
            if(Memory->MenuEntity->TargetPosition > 0)
            {
                Center_x = Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x + (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w * 0.2f);
            }
            else
            {
                Center_x = Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x + (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w * 0.8f);
            }
            
            CenterOffset = Center_x - Memory->MenuEntity->TargetPosition;
            
            for(u32 i = 0; i < ButtonsAreaAmount; ++i)
            {
                game_rect TargetArea;
                TargetArea.x = Memory->MenuEntity->ButtonsArea[i].x + (Memory->MenuEntity->ButtonsArea[i].w / 2) - (Buffer->Width / 2);
                TargetArea.y = 0;
                TargetArea.w = Buffer->Width;
                TargetArea.h = Buffer->Height;
                
                if(IsPointInsideRect(CenterOffset, Center_y, &TargetArea))
                {
                    Memory->MenuEntity->TargetIndex = i;
                    break;
                }
            }
            
            Memory->MenuEntity->TargetPosition = Buffer->Width / 2;
        }
        else
        {
            Memory->MenuEntity->TargetPosition = Buffer->Width / 2;
            
            s32 Index = Memory->MenuEntity->ButtonIndex;
            if(Index == Memory->MenuEntity->NewButtonIndex)
            {
                if(Memory->MenuEntity->DevMode)
                {
                    LevelEntityUpdateLevelEntityFromMemory(&Memory->LevelEntity,
                                                           Index, false,
                                                           Memory, Buffer);
                    
                    LevelEditorChangeGridCounters(Memory->LevelEditor,
                                                  Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount, Buffer);
                    
                    LevelEditorUpdateLevelStats(Memory->LevelEditor, 
                                                Memory->LevelEntity.LevelNumber, Index, Buffer);
                    
                    Memory->MenuEntity->ButtonsAmount += 1;
                    
                    char LevelNumber[3] = {0};
                    sprintf(LevelNumber, "%d", Memory->LevelEntity.LevelNumber);
                    
                    MenuChangeButtonText(Memory->LevelNumberFont, LevelNumber, 
                                         Memory->MenuEntity, 
                                         &Memory->MenuEntity->Buttons[Index], 
                                         {255, 255, 255}, 
                                         Buffer);
                    MenuChangeButtonText(Memory->LevelNumberFont, "+", 
                                         Memory->MenuEntity, 
                                         &Memory->MenuEntity->Buttons[Index+1], 
                                         {255, 255, 255}, 
                                         Buffer);
                    Memory->MenuEntity->NewButtonIndex = Index+1;
                    
                    MenuEntityAlignButtons(Memory->MenuEntity, Buffer->Width, Buffer->Height);
                    
                    Memory->LevelMemoryAmount += 1;
                }
                
            }
            else if(Index >= 0)
            {
                if(Memory->MenuEntity->DevMode)
                {
                    if(Memory->MenuEntity->IsToBeDeleted)
                    {
                        MenuDeleteLevel(Buffer, Memory, Memory->MenuEntity->ButtonIndex);
                        Memory->MenuEntity->IsToBeDeleted = false;
                        Memory->CurrentLevelIndex -= 1;
                    }
                }
                else
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
            
            Memory->MenuEntity->ButtonIndex = -1;
        }
        
    }
    else if(Input->MouseButtons[1].EndedUp)
    {
        printf("Right mouse click\n");
        
        if(Memory->MenuEntity->DevMode)
        {
            s32 Index = -1;
            
            u32 BeginIndex = Memory->MenuEntity->TargetIndex * 20;
            u32 EndIndex   = BeginIndex + 20;
            
            for(u32 i = BeginIndex; i < EndIndex; ++i)
            {
                if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Memory->MenuEntity->Buttons[i].ButtonQuad))
                {
                    Index = i;
                    break;
                }
            }
            
            if(Index >= 0 && Index != Memory->MenuEntity->NewButtonIndex)
            {
                Memory->MenuEntity->IsShowingDelete = true;
                
                Memory->MenuEntity->ConfirmButtons[0].ButtonQuad.x = Memory->MenuEntity->Buttons[Index].ButtonQuad.x + Memory->MenuEntity->Buttons[Index].ButtonQuad.w - (Memory->MenuEntity->ConfirmButtons[0].ButtonQuad.w * 2);
                Memory->MenuEntity->ConfirmButtons[0].ButtonQuad.y = Memory->MenuEntity->Buttons[Index].ButtonQuad.y;
                
                Memory->MenuEntity->ConfirmButtons[0].LevelNumberTextureQuad.x = Memory->MenuEntity->ConfirmButtons[0].ButtonQuad.x + (Memory->MenuEntity->ConfirmButtons[0].ButtonQuad.w / 2) - (Memory->MenuEntity->ConfirmButtons[0].LevelNumberTextureQuad.w / 2);
                
                Memory->MenuEntity->ConfirmButtons[0].LevelNumberTextureQuad.y = Memory->MenuEntity->ConfirmButtons[0].ButtonQuad.y + (Memory->MenuEntity->ConfirmButtons[0].ButtonQuad.h / 2) - (Memory->MenuEntity->ConfirmButtons[0].LevelNumberTextureQuad.h / 2);
                
                Memory->MenuEntity->ConfirmButtons[1].ButtonQuad.x = Memory->MenuEntity->Buttons[Index].ButtonQuad.x + Memory->MenuEntity->Buttons[Index].ButtonQuad.w - (Memory->MenuEntity->ConfirmButtons[1].ButtonQuad.w);
                Memory->MenuEntity->ConfirmButtons[1].ButtonQuad.y = Memory->MenuEntity->Buttons[Index].ButtonQuad.y;
                
                Memory->MenuEntity->ConfirmButtons[1].LevelNumberTextureQuad.x = Memory->MenuEntity->ConfirmButtons[1].ButtonQuad.x + (Memory->MenuEntity->ConfirmButtons[1].ButtonQuad.w / 2) - (Memory->MenuEntity->ConfirmButtons[1].LevelNumberTextureQuad.w / 2);
                
                Memory->MenuEntity->ConfirmButtons[1].LevelNumberTextureQuad.y = Memory->MenuEntity->ConfirmButtons[1].ButtonQuad.y + (Memory->MenuEntity->ConfirmButtons[1].ButtonQuad.h / 2) - (Memory->MenuEntity->ConfirmButtons[1].LevelNumberTextureQuad.h / 2);
            }
            
        }
    }
    
    if(Memory->MenuEntity->IsMoving)
    {
        OffsetX += Input->MouseRelX;
        Memory->MenuEntity->TargetPosition += OffsetX;
        
        if(abs(Memory->MenuEntity->TargetPosition) >= Memory->MenuEntity->ButtonSizeWidth * 0.5f)
        {
            Memory->MenuEntity->ButtonIndex = -1;
        }
    }
    
    
    if(Memory->MenuEntity->IsAnimating)
    {
        s32 RelativeCenter = Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x + (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w/2);
        
        vector2 Vector = { Memory->MenuEntity->TargetPosition - RelativeCenter, 0 };
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
                Vector.x *= Distance / ApproachRadius * Memory->MenuEntity->MaxVelocity;
                Vector.y *= Distance / ApproachRadius * Memory->MenuEntity->MaxVelocity;
            }
            else
            {
                s32 Width = (Buffer->Width / 2) - (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w / 2);
                s32 Offset = Width - Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x;
                
                Vector.x = 0;
                Vector.y = 0;
                
                Memory->MenuEntity->Velocity.x     = 0;
                Memory->MenuEntity->TargetPosition = 0;
                Memory->MenuEntity->IsAnimating    = false;
                
                u32 ButtonsAreaAmount = (Memory->MenuEntity->ButtonsAmount / 20) + 1;
                for(u32 i = 0; i < ButtonsAreaAmount; ++i)
                {
                    Memory->MenuEntity->ButtonsArea[i].x += Offset;
                }
                
                for(u32 i = 0; i < Memory->MenuEntity->ButtonsAmount; ++i)
                {
                    Memory->MenuEntity->Buttons[i].ButtonQuad.x += Offset;
                    Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad.x += Offset;
                }
            }
        }
        else
        {
            Vector.x *= Memory->MenuEntity->MaxVelocity;
            Vector.y *= Memory->MenuEntity->MaxVelocity;
        }
        
        
        Acceleration.x = Vector.x - Memory->MenuEntity->Velocity.x;
        Vector2Add(&Memory->MenuEntity->Velocity, &Acceleration);
    }
    
    //
    // Menu Rendering
    //
    
    game_rect ScreenQuad = {0, 0, Buffer->Width, Buffer->Height };
    DEBUGRenderQuadFill(Buffer, &ScreenQuad, {0, 0, 0}, 0);
    
    u32 ButtonsAreaAmount = (Memory->MenuEntity->ButtonsAmount / 20) + 1;
    for(u32 i = 0; i < ButtonsAreaAmount; ++i)
    {
        if(Memory->MenuEntity->IsMoving)
        {
            Memory->MenuEntity->ButtonsArea[i].x += OffsetX;
            {
                game_rect TargetArea;
                TargetArea.x = Memory->MenuEntity->ButtonsArea[i].x + (Memory->MenuEntity->ButtonsArea[i].w / 2) - (Buffer->Width / 2);
                TargetArea.y = 0;
                TargetArea.w = Buffer->Width;
                TargetArea.h = Buffer->Height;
                
                if(Memory->MenuEntity->DevMode)
                {
                    DEBUGRenderQuad(Buffer, &TargetArea, {255, 0, 0}, 255);
                }
                
            }
        }
        
        if(Memory->MenuEntity->IsAnimating)
        {
            Memory->MenuEntity->ButtonsArea[i].x += roundf(Memory->MenuEntity->Velocity.x);
        }
        
        if(Memory->MenuEntity->DevMode)
        {
            DEBUGRenderQuad(Buffer, &Memory->MenuEntity->ButtonsArea[i], {255, 255, 255}, 255);
        }
        
    }
    
    for(u32 i = 0; i < Memory->MenuEntity->ButtonsAmount; ++i)
    {
        if(Memory->MenuEntity->IsMoving)
        {
            Memory->MenuEntity->Buttons[i].ButtonQuad.x += OffsetX;
            Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad.x += OffsetX;
        }
        
        if(Memory->MenuEntity->IsAnimating)
        {
            Memory->MenuEntity->Buttons[i].ButtonQuad.x += roundf(Memory->MenuEntity->Velocity.x);
            Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad.x += roundf(Memory->MenuEntity->Velocity.x);
        }
        if(Memory->MenuEntity->NewButtonIndex == i)
        {
            if(Memory->MenuEntity->DevMode)
            {
                GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->BackTexture, &Memory->MenuEntity->Buttons[i].ButtonQuad);
                GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->Buttons[i].LevelNumberTexture,
                                         &Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad);
            }
        }
        else if(Memory->MenuEntity->ButtonIndex == i)
        {
            game_point AreaCenter = {0};
            AreaCenter.x = Memory->MenuEntity->Buttons[i].ButtonQuad.x + (Memory->MenuEntity->Buttons[i].ButtonQuad.w / 2);
            AreaCenter.y = Memory->MenuEntity->Buttons[i].ButtonQuad.y + (Memory->MenuEntity->Buttons[i].ButtonQuad.h / 2);
            
            game_rect AreaQuad = {0};
            AreaQuad.w = Memory->MenuEntity->Buttons[i].ButtonQuad.w * 1.2f;
            AreaQuad.h = Memory->MenuEntity->Buttons[i].ButtonQuad.h * 1.2f;
            AreaQuad.x = AreaCenter.x - (AreaQuad.w / 2);
            AreaQuad.y = AreaCenter.y - (AreaQuad.h / 2);
            
            game_rect NumberQuad = {0};
            NumberQuad.w = Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad.w * 1.2f;
            NumberQuad.h = Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad.h * 1.2f;
            NumberQuad.x = AreaCenter.x - (NumberQuad.w / 2);
            NumberQuad.y = AreaCenter.y - (NumberQuad.h / 2);
            
            GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->BackTexture, &AreaQuad);
            GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->Buttons[i].LevelNumberTexture,
                                     &NumberQuad);
        }
        else
        {
            GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->BackTexture, &Memory->MenuEntity->Buttons[i].ButtonQuad);
            GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->Buttons[i].LevelNumberTexture,
                                     &Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad);
        }
    }
    
    if(Memory->MenuEntity->DevMode)
    {
        if(Memory->MenuEntity->IsMoving)
        {
            s32 ButtonCenter = Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x + (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w / 2);
            s32 Target = ButtonCenter + Memory->MenuEntity->TargetPosition;
            DEBUGRenderLine(Buffer, 
                            ButtonCenter, Buffer->Height / 2,
                            Target, Buffer->Height / 2,
                            {0, 255, 0}, 255);
        }
        
        if(Memory->MenuEntity->IsShowingDelete)
        {
            DEBUGRenderQuadFill(Buffer, &Memory->MenuEntity->ConfirmButtons[0].ButtonQuad, {0, 255, 0}, 255);
            DEBUGRenderQuad(Buffer, &Memory->MenuEntity->ConfirmButtons[0].ButtonQuad, {0, 0, 0}, 255);
            GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->ConfirmButtons[0].LevelNumberTexture, &Memory->MenuEntity->ConfirmButtons[0].LevelNumberTextureQuad);
            
            DEBUGRenderQuadFill(Buffer, &Memory->MenuEntity->ConfirmButtons[1].ButtonQuad, {0, 255, 0}, 255);
            DEBUGRenderQuad(Buffer, &Memory->MenuEntity->ConfirmButtons[1].ButtonQuad, {0, 0, 0}, 255);
            GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->ConfirmButtons[1].LevelNumberTexture, &Memory->MenuEntity->ConfirmButtons[1].LevelNumberTextureQuad);
        }
        
        // Save and load buttons
        
        DEBUGRenderQuadFill(Buffer, &Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad, {0, 255, 0}, 100);
        DEBUGRenderQuad(Buffer, &Memory->MenuEntity->SaveAndLoadButtons[0].ButtonQuad, {0, 0, 255}, 255);
        GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->SaveAndLoadButtons[0].LevelNumberTexture, &Memory->MenuEntity->SaveAndLoadButtons[0].LevelNumberTextureQuad);
        
        DEBUGRenderQuadFill(Buffer, &Memory->MenuEntity->SaveAndLoadButtons[1].ButtonQuad, {0, 255, 0}, 100);
        DEBUGRenderQuad(Buffer, &Memory->MenuEntity->SaveAndLoadButtons[1].ButtonQuad, {0, 0, 255}, 255);
        GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->SaveAndLoadButtons[1].LevelNumberTexture, &Memory->MenuEntity->SaveAndLoadButtons[1].LevelNumberTextureQuad);
        
    }
}


