// game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

#include "editor.h"

#include "game.h"
#include "entity.h"


static void
LevelEditorInit(level_entity *LevelEntity, game_memory *Memory, game_offscreen_buffer *Buffer)
{
    Memory->LevelEditor = (level_editor*)malloc(sizeof(level_editor));
    Assert(Memory->LevelEditor);
    
    Memory->LevelEditor->SelectedFigure = 0;
    Memory->LevelEditor->ButtonPressed  = false;
    Memory->LevelEditor->Q_Pressed = 0;
    Memory->LevelEditor->E_Pressed = 0;
    
    s32 ButtonSize   = LevelEntity->InActiveBlockSize * 2;
    u32 RowAmount    = LevelEntity->GridEntity->RowAmount;
    u32 ColumnAmount = LevelEntity->GridEntity->ColumnAmount;
    
    char RowString[2] = {0};
    char ColString[2] = {0};
    sprintf(RowString, "%d", RowAmount);
    sprintf(ColString, "%d", ColumnAmount);
    
    level_editor *&LevelEditor = Memory->LevelEditor;
    LevelEditor->GridButtonLayer.w = ButtonSize;
    LevelEditor->GridButtonLayer.h = ButtonSize * 6;
    //LevelEditor->GridButtonLayer.x = (Buffer->Width / 2) - (LevelEditor->GridButtonLayer.w / 2);
    //LevelEditor->GridButtonLayer.y = LevelEntity->FigureEntity->FigureArea.y - ButtonSize;
    
    LevelEditor->GridButtonLayer.x = 0;
    LevelEditor->GridButtonLayer.y = 0;
    
    LevelEditor->Font = TTF_OpenFont("..\\data\\Karmina-Bold.otf", ButtonSize);
    Assert(LevelEditor->Font);
    
    game_surface *Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, "+", {0, 0, 0});
    LevelEditor->PlusTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->PlusTexture, 0, 0, &LevelEditor->GridButtonQuad[0].w, &LevelEditor->GridButtonQuad[0].h);
    SDL_QueryTexture(LevelEditor->PlusTexture, 0, 0, &LevelEditor->GridButtonQuad[3].w, &LevelEditor->GridButtonQuad[3].h);
    SDL_FreeSurface(Surface);
    
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, "-", {0, 0, 0});
    LevelEditor->MinusTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->MinusTexture, 0, 0, &LevelEditor->GridButtonQuad[2].w,
                     &LevelEditor->GridButtonQuad[2].h);
    SDL_QueryTexture(LevelEditor->MinusTexture, 0, 0, &LevelEditor->GridButtonQuad[5].w,
                     &LevelEditor->GridButtonQuad[5].h);
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, RowString, {0, 0, 0});
    LevelEditor->RowTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->RowTexture, 0, 0, &LevelEditor->GridButtonQuad[1].w,
                     &LevelEditor->GridButtonQuad[1].h);
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, ColString, {0, 0, 0});
    LevelEditor->ColumnTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->ColumnTexture, 0, 0, &LevelEditor->GridButtonQuad[4].w, &LevelEditor->GridButtonQuad[4].h);
    SDL_FreeSurface(Surface);
    
    
    game_rect UiQuad = 
    {
        LevelEditor->GridButtonLayer.x, LevelEditor->GridButtonLayer.y, 
        ButtonSize, ButtonSize
    };
    
    for(u32 i = 0; i < 6; i++)
    {
        LevelEditor->GridButtonQuad[i].x = (UiQuad.x + UiQuad.w / 2) - (LevelEditor->GridButtonQuad[i].w / 2);
        LevelEditor->GridButtonQuad[i].y = (UiQuad.y + UiQuad.h / 2) - (LevelEditor->GridButtonQuad[i].h / 2);
        LevelEditor->GridButton[i] = UiQuad;
        UiQuad.y += UiQuad.h;
    }
    
    LevelEditor->FigureButtonLayer.w = ButtonSize;
    LevelEditor->FigureButtonLayer.h = ButtonSize * 6;
    //LevelEditor->FigureButtonLayer.x = (Buffer->Width / 2) - (LevelEditor->FigureButtonLayer.w / 2);
    //LevelEditor->FigureButtonLayer.y = Buffer->Height - LevelEditor->FigureButtonLayer.h;
    
    LevelEditor->FigureButtonLayer.x = LevelEditor->GridButtonLayer.x + LevelEditor->GridButtonLayer.w;
    LevelEditor->FigureButtonLayer.y = LevelEditor->GridButtonLayer.y;
    
    LevelEditor->FigureButtonQuad[0].w = LevelEditor->GridButtonQuad[0].w;
    LevelEditor->FigureButtonQuad[0].h = LevelEditor->GridButtonQuad[0].h;
    LevelEditor->FigureButtonQuad[1].w = LevelEditor->GridButtonQuad[2].w;
    LevelEditor->FigureButtonQuad[1].h = LevelEditor->GridButtonQuad[2].h;
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, "R", {0, 0, 0});
    LevelEditor->RotateTexture  = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->RotateTexture, 0, 0, &LevelEditor->FigureButtonQuad[2].w, &LevelEditor->FigureButtonQuad[2].h);
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, "F", {0, 0, 0});
    LevelEditor->FlipTexture  = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->FlipTexture, 0, 0, &LevelEditor->FigureButtonQuad[3].w, &LevelEditor->FigureButtonQuad[3].h);
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, "<", {0, 0, 0});
    LevelEditor->FormTexture  = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->FormTexture, 0, 0, &LevelEditor->FigureButtonQuad[4].w, &LevelEditor->FigureButtonQuad[4].h);
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, ">", {0, 0, 0});
    LevelEditor->TypeTexture  = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->TypeTexture, 0, 0, &LevelEditor->FigureButtonQuad[5].w, &LevelEditor->FigureButtonQuad[5].h);
    SDL_FreeSurface(Surface);
    
    UiQuad.x = LevelEditor->FigureButtonLayer.x;
    UiQuad.y = LevelEditor->FigureButtonLayer.y;
    UiQuad.w = ButtonSize;
    UiQuad.h = ButtonSize;
    
    for(u32 i = 0; i < 6; i++)
    {
        LevelEditor->FigureButtonQuad[i].x = (UiQuad.x + UiQuad.w / 2) - (LevelEditor->FigureButtonQuad[i].w / 2);
        LevelEditor->FigureButtonQuad[i].y = (UiQuad.y + UiQuad.h / 2) - (LevelEditor->FigureButtonQuad[i].h / 2);
        LevelEditor->FigureButton[i] = UiQuad;
        UiQuad.y += UiQuad.h;
    }
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, "save", {255, 255, 255});
    LevelEditor->SaveTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->SaveTexture, 0, 0,
                     &LevelEditor->SaveButtonQuad.w, &LevelEditor->SaveButtonQuad.h);
    SDL_FreeSurface(Surface);
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, "load", {255, 255, 255});
    LevelEditor->LoadTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->LoadTexture, 0, 0, &LevelEditor->LoadButtonQuad.w, &LevelEditor->LoadButtonQuad.h);
    SDL_FreeSurface(Surface);
    
    LevelEditor->SaveButtonLayer.w = LevelEditor->FigureButtonLayer.w * 2;
    LevelEditor->SaveButtonLayer.h = LevelEditor->FigureButtonLayer.w;
    LevelEditor->SaveButtonLayer.x = 0;
    LevelEditor->SaveButtonLayer.y = LevelEditor->FigureButtonLayer.y + LevelEditor->FigureButtonLayer.h;
    
    LevelEditor->SaveButtonQuad.x = LevelEditor->SaveButtonLayer.x + (LevelEditor->SaveButtonLayer.w / 2) - (LevelEditor->SaveButtonQuad.w / 2);
    LevelEditor->SaveButtonQuad.y = LevelEditor->SaveButtonLayer.y + (LevelEditor->SaveButtonLayer.h / 2) - (LevelEditor->SaveButtonQuad.h / 2);
    
    LevelEditor->LoadButtonLayer.w = LevelEditor->FigureButtonLayer.w * 2;
    LevelEditor->LoadButtonLayer.h = LevelEditor->FigureButtonLayer.w;
    LevelEditor->LoadButtonLayer.x = 0;
    LevelEditor->LoadButtonLayer.y = LevelEditor->SaveButtonLayer.y + (LevelEditor->LoadButtonLayer.h);
    
    LevelEditor->LoadButtonQuad.x = LevelEditor->LoadButtonLayer.x + (LevelEditor->LoadButtonLayer.w / 2) - (LevelEditor->LoadButtonQuad.w / 2);
    LevelEditor->LoadButtonQuad.y = LevelEditor->LoadButtonLayer.y + (LevelEditor->LoadButtonLayer.h / 2) - (LevelEditor->LoadButtonQuad.h / 2);
    
    
    Memory->LevelEditor->ActiveButton.x = LevelEditor->GridButtonLayer.x;
    Memory->LevelEditor->ActiveButton.y = -100;
    Memory->LevelEditor->ActiveButton.w = ButtonSize;
    Memory->LevelEditor->ActiveButton.h = ButtonSize;
    
    //
    // Next/Prev Level Buttons
    //
    
    Memory->LevelEditor->PrevLevelTexture = GetTexture(Memory, "left_arrow.png", Buffer->Renderer);
    Assert(Memory->LevelEditor->PrevLevelTexture);
    
    Memory->LevelEditor->PrevLevelQuad.w = ButtonSize;
    Memory->LevelEditor->PrevLevelQuad.h = ButtonSize;
    Memory->LevelEditor->PrevLevelQuad.x = Memory->LevelEditor->PrevLevelQuad.w;
    Memory->LevelEditor->PrevLevelQuad.y = Buffer->Height - (Memory->LevelEditor->PrevLevelQuad.h * 2);
    
    Memory->LevelEditor->NextLevelTexture = GetTexture(Memory, "right_arrow.png", Buffer->Renderer);
    Assert(Memory->LevelEditor->NextLevelTexture);
    
    Memory->LevelEditor->NextLevelQuad.w = ButtonSize;
    Memory->LevelEditor->NextLevelQuad.h = ButtonSize;
    Memory->LevelEditor->NextLevelQuad.x = Buffer->Width - (Memory->LevelEditor->NextLevelQuad.w * 2);
    Memory->LevelEditor->NextLevelQuad.y = Buffer->Height - (Memory->LevelEditor->NextLevelQuad.h * 2);
}


static void
LevelEditorChangeGridCounters(level_editor *LevelEditor, 
                              u32 NewRowAmount, u32 NewColumnAmount, 
                              u32 OldRowAmount, u32 OldColumnAmount,
                              game_offscreen_buffer *Buffer)
{
    char RowString[3] = {0};
    char ColString[3] = {0};
    sprintf(RowString, "%d", NewRowAmount);
    sprintf(ColString, "%d", NewColumnAmount);
    
    if(NewRowAmount != OldRowAmount)
    {
        game_surface *Surface = 0;
        FreeTexture(LevelEditor->RowTexture);
        
        Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, RowString, { 0, 0, 0 });
        
        LevelEditor->RowTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
        SDL_QueryTexture(LevelEditor->RowTexture, 0, 0, &LevelEditor->GridButtonQuad[1].w, &LevelEditor->GridButtonQuad[1].h);
        
        LevelEditor->GridButtonQuad[1].x = LevelEditor->GridButton[1].x + (LevelEditor->GridButton[1].w /2) - (LevelEditor->GridButtonQuad[1].w / 2);
        LevelEditor->GridButtonQuad[1].y = LevelEditor->GridButton[1].y + (LevelEditor->GridButton[4].h /2) - (LevelEditor->GridButtonQuad[1].h / 2);
        
        SDL_FreeSurface(Surface);
    }
    
    if(NewColumnAmount != OldColumnAmount)
    {
        game_surface *Surface = 0;
        FreeTexture(LevelEditor->ColumnTexture);
        
        Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, ColString, { 0, 0, 0 });
        LevelEditor->ColumnTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
        SDL_QueryTexture(LevelEditor->ColumnTexture, 0, 0, &LevelEditor->GridButtonQuad[4].w, &LevelEditor->GridButtonQuad[4].h);
        
        LevelEditor->GridButtonQuad[4].x = LevelEditor->GridButton[4].x + (LevelEditor->GridButton[4].w /2) - (LevelEditor->GridButtonQuad[4].w / 2);
        LevelEditor->GridButtonQuad[4].y = LevelEditor->GridButton[4].y + (LevelEditor->GridButton[4].h /2) - (LevelEditor->GridButtonQuad[4].h / 2);
        
        SDL_FreeSurface(Surface);
    }
}

static figure_form
LevelEditorGetNextFigureForm(figure_form CurrentForm)
{
    switch(CurrentForm)
    {
        case I_figure: return O_figure;
        case O_figure: return Z_figure;
        case Z_figure: return S_figure;
        case S_figure: return T_figure;
        case T_figure: return L_figure;
        case L_figure: return J_figure;
        case J_figure: return I_figure;
    }
    
    return O_figure;
}

static figure_type
LevelEditorGetNextFigureType(figure_type CurrentType)
{
    switch(CurrentType)
    {
        case classic: return stone;
        case stone:   return mirror;
        case mirror:  return classic;
    }
    
    return classic;
}

static void
GridEntityDeleteMovingBlock(grid_entity *GridEntity, u32 Index)
{
    printf("GridEntityDeleteMovingBlock\n");
    u32 Amount = GridEntity->MovingBlocksAmount;
    
    if(Index < 0 || Index >= Amount) return;
    if(Amount <= 0) return;
    
    moving_block *MovingBlocks = GridEntity->MovingBlocks;
    
    for(u32 i = Index; i < Amount-1; ++i)
    {
        MovingBlocks[i].AreaQuad   = MovingBlocks[i+1].AreaQuad;
        MovingBlocks[i].RowNumber  = MovingBlocks[i+1].RowNumber;
        MovingBlocks[i].ColNumber  = MovingBlocks[i+1].ColNumber;
        MovingBlocks[i].IsVertical = MovingBlocks[i+1].IsVertical;
        MovingBlocks[i].IsMoving   = MovingBlocks[i+1].IsMoving;
        MovingBlocks[i].MoveSwitch = MovingBlocks[i+1].MoveSwitch;
    }
    
    GridEntity->MovingBlocksAmount -= 1;
}



static void
GridEntityNewGrid(game_offscreen_buffer *Buffer, level_entity *LevelEntity, 
                  s32 NewRowAmount, s32 NewColumnAmount, level_editor *LevelEditor)
{
    if(NewRowAmount < 0 || NewColumnAmount < 0) return;
    
    grid_entity *&GridEntity = LevelEntity->GridEntity;
    
    s32 **UnitField = (s32**)malloc(sizeof(s32*) * NewRowAmount);
    Assert(UnitField);
    for(u32 i = 0; i < NewRowAmount; ++i){
        UnitField[i] = (s32*)malloc(sizeof(s32) * NewColumnAmount);
        Assert(UnitField[i]);
        for(u32 j = 0; j < NewColumnAmount; j ++){
            UnitField[i][j] = 0;
        }
    }
    
    u32 CurrentRowAmount = NewRowAmount < GridEntity->RowAmount ? NewRowAmount : GridEntity->RowAmount;
    u32 CurrentColumnAmount = NewColumnAmount < GridEntity->ColumnAmount ? NewColumnAmount : GridEntity->ColumnAmount;
    
    for(u32 i = 0; i < CurrentRowAmount; ++i){
        for(u32 j = 0; j < CurrentColumnAmount; ++j){
            UnitField[i][j] = GridEntity->UnitField[i][j];
        }
    }
    
    for(u32 i = 0; i < GridEntity->RowAmount; ++i){
        free(GridEntity->UnitField[i]);
    }
    
    free(GridEntity->UnitField);
    
    LevelEditorChangeGridCounters(LevelEditor, 
                                  NewRowAmount, NewColumnAmount, 
                                  GridEntity->RowAmount, GridEntity->ColumnAmount,
                                  Buffer);
    
    u32 DefaultBlocksInRow = 12;
    u32 DefaultBlocksInCol = 9;
    
    RescaleGameField(Buffer, NewRowAmount, NewColumnAmount,
                     LevelEntity->FigureEntity->FigureAmount, DefaultBlocksInRow, DefaultBlocksInCol, LevelEntity);
    
    GridEntity->UnitField    = UnitField;
    GridEntity->RowAmount    = NewRowAmount;
    GridEntity->ColumnAmount = NewColumnAmount;
    
    u32 ActiveBlockSize   = LevelEntity->ActiveBlockSize;
    u32 InActiveBlockSize = LevelEntity->InActiveBlockSize;
    
    LevelEntity->GridEntity->GridArea.w = ActiveBlockSize * NewColumnAmount;
    LevelEntity->GridEntity->GridArea.h = ActiveBlockSize * NewRowAmount;
    LevelEntity->GridEntity->GridArea.x = (Buffer->Width / 2) - (GridEntity->GridArea.w / 2);
    LevelEntity->GridEntity->GridArea.y = (Buffer->Height - LevelEntity->FigureEntity->FigureArea.h) / 2 - (GridEntity->GridArea.h / 2);
    
    for(u32 i = 0; i < LevelEntity->GridEntity->MovingBlocksAmount; ++i)
    {
        u32 RowNumber = LevelEntity->GridEntity->MovingBlocks[i].RowNumber;
        u32 ColNumber = LevelEntity->GridEntity->MovingBlocks[i].ColNumber;
        
        if(RowNumber >= NewRowAmount || ColNumber >= NewColumnAmount)
        {
            GridEntityDeleteMovingBlock(GridEntity, i);
        }
    }
    
    for(u32 i = 0; i < LevelEntity->GridEntity->MovingBlocksAmount; ++i)
    {
        u32 RowNumber = LevelEntity->GridEntity->MovingBlocks[i].RowNumber;
        u32 ColNumber = LevelEntity->GridEntity->MovingBlocks[i].ColNumber;
        
        GridEntity->MovingBlocks[i].AreaQuad.w = ActiveBlockSize;
        GridEntity->MovingBlocks[i].AreaQuad.h = ActiveBlockSize;
        GridEntity->MovingBlocks[i].AreaQuad.x = GridEntity->GridArea.x + (ColNumber * ActiveBlockSize);
        GridEntity->MovingBlocks[i].AreaQuad.y = GridEntity->GridArea.y + (RowNumber * ActiveBlockSize);
    }
    
}

static void
LevelEditorUpdateAndRender(level_editor *LevelEditor, level_entity *LevelEntity, 
                           game_memory *Memory, game_offscreen_buffer *Buffer, game_input *Input)
{
    if(!LevelEntity->LevelPaused) return;
    
    game_rect GridArea   = LevelEntity->GridEntity->GridArea;
    game_rect FigureArea = LevelEntity->FigureEntity->FigureArea;
    
    u32 FigureAmount  = LevelEntity->FigureEntity->FigureAmount;
    u32 RowAmount     = LevelEntity->GridEntity->RowAmount;
    u32 ColAmount     = LevelEntity->GridEntity->ColumnAmount;
    s32 NewIndex      = LevelEditor->SelectedFigure;
    s32 CurrentLevel  = LevelEntity->LevelNumber;
    
    if(Input->Keyboard.Up.EndedDown)
    {
        NewIndex -= 1;
    }
    else if(Input->Keyboard.Down.EndedDown)
    {
        NewIndex += 1;
    }
    else if(Input->Keyboard.Left.EndedDown)
    {
        NewIndex -= 2;
    }
    else if(Input->Keyboard.Right.EndedDown)
    {
        NewIndex += 2;
    }
    
    if(Input->Keyboard.Q_Button.EndedDown)
    {
        //printf("Q_Button is down\n");
        
        s32 PrevLevelNumber = CurrentLevel - 1;
        if(PrevLevelNumber >= 0)
        {
            u32 RowAmount = Memory->LevelEntity.GridEntity->RowAmount;
            u32 ColAmount = Memory->LevelEntity.GridEntity->ColumnAmount;
            
            LevelEntityUpdateLevelEntityFromMemory(&Memory->LevelEntity, 
                                                   PrevLevelNumber, true,
                                                   Memory, Buffer);
            
            LevelEditorChangeGridCounters(Memory->LevelEditor, 
                                          Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount, 
                                          RowAmount, ColAmount,
                                          Buffer);
            
        }
        
        LevelEditor->PrevLevelQuad.x -= LevelEditor->PrevLevelQuad.w/2;
        LevelEditor->PrevLevelQuad.y -= LevelEditor->PrevLevelQuad.h/2;
        LevelEditor->PrevLevelQuad.w *= 2;
        LevelEditor->PrevLevelQuad.h *= 2;
        
        LevelEditor->Q_Pressed += 1;
    }
    if(Input->Keyboard.E_Button.EndedDown)
    {
        //printf("E_Button is down\n");
        s32 NextLevelNumber = CurrentLevel + 1;
        if(NextLevelNumber < Memory->LevelMemoryAmount)
        {
            u32 RowAmount = Memory->LevelEntity.GridEntity->RowAmount;
            u32 ColAmount = Memory->LevelEntity.GridEntity->ColumnAmount;
            LevelEntityUpdateLevelEntityFromMemory(&Memory->LevelEntity, 
                                                   NextLevelNumber,true,
                                                   Memory, Buffer);
            LevelEditorChangeGridCounters(Memory->LevelEditor, 
                                          Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount, 
                                          RowAmount, ColAmount,
                                          Buffer);
        }
        
        LevelEditor->NextLevelQuad.x -= LevelEditor->NextLevelQuad.w/2;
        LevelEditor->NextLevelQuad.y -= LevelEditor->NextLevelQuad.h/2;
        LevelEditor->NextLevelQuad.w *= 2;
        LevelEditor->NextLevelQuad.h *= 2;
        
        LevelEditor->E_Pressed += 1;
    }
    else if(Input->Keyboard.BackQuote.EndedDown)
    {
        //LevelEntity->LevelPaused = false;
        //Input->Keyboard.BackQuote.EndedDown  = false;
    }
    else if(Input->MouseButtons[0].EndedDown)
    {
        LevelEditor->ButtonPressed = true;
        
        if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->GridButtonLayer))
        {
            /* Grid layer*/
            
            /* Plus row */
            if (IsPointInsideRect(Input->MouseX, Input->MouseY,
                                  &LevelEditor->GridButton[0]))
            {
                printf("Plus row!\n");
                GridEntityNewGrid(Buffer, LevelEntity, RowAmount+1, ColAmount, LevelEditor);
                
                LevelEditor->ActiveButton = LevelEditor->GridButton[0];
            }
            /* Minus row */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->GridButton[2]))
            {
                printf("Minus row!\n");
                GridEntityNewGrid(Buffer, LevelEntity, RowAmount-1, ColAmount, LevelEditor);
                LevelEditor->ActiveButton = LevelEditor->GridButton[2];
            }
            /* Plus column */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->GridButton[3]))
            {
                printf("Plus column!\n");
                GridEntityNewGrid(Buffer, LevelEntity, RowAmount, ColAmount+1, LevelEditor);
                LevelEditor->ActiveButton = LevelEditor->GridButton[3];
            }
            /* Minus column */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->GridButton[5]))
            {
                printf("Minus column!\n");
                GridEntityNewGrid(Buffer, LevelEntity, RowAmount, ColAmount-1, LevelEditor);
                
                LevelEditor->ActiveButton = LevelEditor->GridButton[5];
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->FigureButtonLayer))
        {
            /* Figure Layer*/ 
            
            /* Add figure*/ 
            if (IsPointInsideRect(Input->MouseX, Input->MouseY,
                                  &LevelEditor->FigureButton[0]))
            {
                FigureUnitAddNewFigure(LevelEntity->FigureEntity, O_figure, classic, 0.0f, Memory, Buffer);
                FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->InActiveBlockSize);
                GridEntityUpdateStickUnits(LevelEntity->GridEntity, LevelEntity->FigureEntity->FigureAmount);
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[0];
            }
            /* Delete figure */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->FigureButton[1]))
            {
                FigureUnitDeleteFigure(LevelEntity->FigureEntity, LevelEntity->FigureEntity->FigureAmount - 1);
                FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->InActiveBlockSize);
                GridEntityUpdateStickUnits(LevelEntity->GridEntity, LevelEntity->FigureEntity->FigureAmount);
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[1];
            }
            /* Rotate figure */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->FigureButton[2]))
            {
                FigureUnitRotateShellBy(&LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure], 90);
                LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Angle += 90.0f;
                FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->InActiveBlockSize);
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[2];
            }
            /* Flip figure */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->FigureButton[3]))
            {
                FigureUnitFlipHorizontally(&LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure]);
                FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->InActiveBlockSize);
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[3];
            }
            /* Change figure form */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->FigureButton[4]))
            {
                if(LevelEditor->SelectedFigure >= 0)
                {
                    FreeTexture(LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Texture);
                    
                    figure_form Form = LevelEditorGetNextFigureForm(LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Form);
                    
                    figure_type Type = LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Type;
                    
                    FigureUnitInitFigure(&LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure], Form,Type, 0.0f,  Memory, Buffer);
                    
                    FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->InActiveBlockSize);
                }
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[4];
            }
            /* Change figure type */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->FigureButton[5]))
            {
                if(LevelEditor->SelectedFigure >= 0)
                {
                    FreeTexture(LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Texture);
                    
                    figure_type Type = LevelEditorGetNextFigureType(LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Type);
                    
                    figure_form Form = LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Form;
                    
                    FigureUnitInitFigure(&LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure], Form,Type, 0.0f, Memory, Buffer);
                    
                    FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->InActiveBlockSize);
                }
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[5];
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY,
                                  &LevelEditor->SaveButtonLayer))
        {
            printf("Save!\n");
            printf("LevelNumber = %d\n", LevelEntity->LevelNumber);
            
            SaveLevelToMemory(Memory, LevelEntity, LevelEntity->LevelNumber);
            LevelEditor->ActiveButton = LevelEditor->SaveButtonLayer;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                  &LevelEditor->LoadButtonLayer))
        {
            printf("Load!\n");
            LevelEntityUpdateLevelEntityFromMemory(LevelEntity, 
                                                   LevelEntity->LevelNumber,
                                                   false, Memory, Buffer);
            LevelEditorChangeGridCounters(LevelEditor, 
                                          LevelEntity->GridEntity->RowAmount, LevelEntity->GridEntity->ColumnAmount, 
                                          RowAmount, ColAmount,
                                          Buffer);
            
            LevelEditor->ActiveButton = LevelEditor->LoadButtonLayer;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &FigureArea))
        {
            for(u32 i = 0; i < FigureAmount; ++i)
            {
                game_rect AreaQuad = FigureUnitGetArea(&LevelEntity->FigureEntity->FigureUnit[i]);
                if(IsPointInsideRect(Input->MouseX, Input->MouseY, &AreaQuad))
                {
                    NewIndex = i;
                }
            }
            
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->PrevLevelQuad))
            {
                s32 PrevLevelNumber = LevelEntity->LevelNumber - 1;
                if(PrevLevelNumber >= 0)
                {
                    LevelEntityUpdateLevelEntityFromMemory(&Memory->LevelEntity, 
                                                           PrevLevelNumber,
                                                           true, Memory, Buffer);
                    
                    u32 RowAmount = Memory->LevelEntity.GridEntity->RowAmount;
                    u32 ColAmount = Memory->LevelEntity.GridEntity->ColumnAmount;
                    
                    LevelEditorChangeGridCounters(Memory->LevelEditor, 
                                                  Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount, 
                                                  RowAmount, ColAmount,
                                                  Buffer);
                }
                
                LevelEditor->ActiveButton = LevelEditor->PrevLevelQuad;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->NextLevelQuad))
            {
                s32 NextLevelNumber = LevelEntity->LevelNumber + 1;
                if(NextLevelNumber < Memory->LevelMemoryAmount)
                {
                    LevelEntityUpdateLevelEntityFromMemory(&Memory->LevelEntity, 
                                                           NextLevelNumber,
                                                           true, Memory, Buffer);
                    
                    u32 RowAmount = Memory->LevelEntity.GridEntity->RowAmount;
                    u32 ColAmount = Memory->LevelEntity.GridEntity->ColumnAmount;
                    
                    LevelEditorChangeGridCounters(Memory->LevelEditor, 
                                                  Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount, 
                                                  RowAmount, ColAmount,
                                                  Buffer);
                }
                
                LevelEditor->ActiveButton = LevelEditor->NextLevelQuad;
            }
            
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &GridArea))
        {
            printf("GridArea click !\n");
            
            game_rect AreaQuad = { 0, 0, (s32)LevelEntity->ActiveBlockSize, (s32)LevelEntity->ActiveBlockSize };
            
            u32 StartX = 0;
            u32 StartY = 0;
            
            for(u32 i = 0; i < RowAmount; ++i)
            {
                StartY = GridArea.y + (LevelEntity->ActiveBlockSize * i);
                
                for(u32 j = 0; j < ColAmount; ++j)
                {
                    StartX = GridArea.x + (LevelEntity->ActiveBlockSize * j);
                    
                    AreaQuad.x = StartX;
                    AreaQuad.y = StartY;
                    
                    if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                         &AreaQuad))
                    {
                        u32 GridUnit = LevelEntity->GridEntity->UnitField[i][j];
                        if(GridUnit == 0)
                        {
                            LevelEntity->GridEntity->UnitField[i][j] = 1;
                        }
                        else
                        {
                            s32 Index = -1;
                            for(u32 m = 0; m < LevelEntity->GridEntity->MovingBlocksAmount; ++m)
                            {
                                u32 RowNumber = LevelEntity->GridEntity->MovingBlocks[m].RowNumber;
                                u32 ColNumber = LevelEntity->GridEntity->MovingBlocks[m].ColNumber;
                                
                                if((i == RowNumber) && (j == ColNumber))
                                {
                                    Index = m;
                                    break;
                                }
                            }
                            
                            if(Index >= 0)
                            {
                                
                                if(!LevelEntity->GridEntity->MovingBlocks[Index].MoveSwitch)
                                {
                                    LevelEntity->GridEntity->MovingBlocks[Index].MoveSwitch = true;
                                }
                                else if(!LevelEntity->GridEntity->MovingBlocks[Index].IsVertical)
                                {
                                    LevelEntity->GridEntity->MovingBlocks[Index].IsVertical = true;
                                    LevelEntity->GridEntity->MovingBlocks[Index].MoveSwitch = false;
                                }
                                else
                                {
                                    GridEntityDeleteMovingBlock(LevelEntity->GridEntity, Index);
                                    LevelEntity->GridEntity->UnitField[i][j] = 0;
                                }
                            }
                            else
                            {
                                GridEntityAddMovingBlock(LevelEntity->GridEntity, i, j, false, false, LevelEntity->ActiveBlockSize);
                            }
                        }
                    }
                }
            }
        }
    }
    else if(Input->MouseButtons[0].EndedUp)
    {
        LevelEditor->ButtonPressed = false;
        LevelEditor->ActiveButton.y = -100;
    }
    
    if(Input->Keyboard.Q_Button.EndedUp)
    {
        LevelEditor->PrevLevelQuad.w /= 2;
        LevelEditor->PrevLevelQuad.h /= 2;
        LevelEditor->PrevLevelQuad.x += LevelEditor->PrevLevelQuad.w/2;
        LevelEditor->PrevLevelQuad.y += LevelEditor->PrevLevelQuad.h/2;
        
        LevelEditor->Q_Pressed -= 1;
    }
    if(Input->Keyboard.E_Button.EndedUp)
    {
        LevelEditor->NextLevelQuad.w /= 2;
        LevelEditor->NextLevelQuad.h /= 2;
        LevelEditor->NextLevelQuad.x += LevelEditor->NextLevelQuad.w/2;
        LevelEditor->NextLevelQuad.y += LevelEditor->NextLevelQuad.h/2;
        
        LevelEditor->E_Pressed -= 1;
    }
    
    if(NewIndex < 0)
    {
        NewIndex = LevelEntity->FigureEntity->FigureAmount - 1;
    }
    
    if(NewIndex >= LevelEntity->FigureEntity->FigureAmount)
    {
        NewIndex = 0;
    }
    
    LevelEditor->SelectedFigure = NewIndex;
    
    if(LevelEntity->LevelStarted)
    {
        for(u32 i = 0; i < LevelEntity->FigureEntity->FigureAmount; ++i)
        {
            DEBUGRenderFigureShell(Buffer, &LevelEntity->FigureEntity->FigureUnit[i], LevelEntity->InActiveBlockSize / 4, {0, 0, 255}, 255);
            DEBUGRenderQuad(Buffer, &LevelEntity->FigureEntity->FigureUnit[i].AreaQuad, {255, 0, 0}, 255);
        }
        
        DEBUGRenderFigureShell(Buffer, &LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure], LevelEntity->InActiveBlockSize, {255, 255, 255}, 100);
    }
    
    game_rect ButtonQuad = 
    {
        ButtonQuad.x = LevelEditor->GridButtonLayer.x,
        ButtonQuad.y = LevelEditor->GridButtonLayer.y,
        ButtonQuad.w = LevelEntity->InActiveBlockSize * 2, 
        ButtonQuad.h = LevelEntity->InActiveBlockSize * 2
    };
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->GridButtonLayer, {0, 0, 255}, 100);
    
    DEBUGRenderQuad(Buffer, &ButtonQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PlusTexture, &LevelEditor->GridButtonQuad[0]);
    
    ButtonQuad.y += ButtonQuad.h;
    
    DEBUGRenderQuad(Buffer, &ButtonQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->RowTexture, &LevelEditor->GridButtonQuad[1]);
    
    ButtonQuad.y += ButtonQuad.h;
    
    DEBUGRenderQuad(Buffer, &ButtonQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->MinusTexture,  &LevelEditor->GridButtonQuad[2]);
    
    ButtonQuad.y += ButtonQuad.h;
    
    DEBUGRenderQuad(Buffer, &ButtonQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PlusTexture, &LevelEditor->GridButtonQuad[3]);
    
    ButtonQuad.y += ButtonQuad.h;
    
    DEBUGRenderQuad(Buffer, &ButtonQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->ColumnTexture, &LevelEditor->GridButtonQuad[4]);
    
    ButtonQuad.y += ButtonQuad.h;
    
    DEBUGRenderQuad(Buffer, &ButtonQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->MinusTexture, &LevelEditor->GridButtonQuad[5]);
    
    ButtonQuad.x = LevelEditor->FigureButtonLayer.x;
    ButtonQuad.y = LevelEditor->FigureButtonLayer.y;
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->FigureButtonLayer, {0, 255, 0}, 100);
    
    DEBUGRenderQuad(Buffer, &ButtonQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PlusTexture, 
                             &LevelEditor->FigureButtonQuad[0]);
    
    ButtonQuad.y += ButtonQuad.h;
    
    DEBUGRenderQuad(Buffer, &ButtonQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->MinusTexture, 
                             &LevelEditor->FigureButtonQuad[1]);
    
    ButtonQuad.y += ButtonQuad.h;
    
    DEBUGRenderQuad(Buffer, &ButtonQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->RotateTexture, 
                             &LevelEditor->FigureButtonQuad[2]);
    
    ButtonQuad.y += ButtonQuad.h;
    
    DEBUGRenderQuad(Buffer, &ButtonQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->FlipTexture, 
                             &LevelEditor->FigureButtonQuad[3]);
    
    ButtonQuad.y += ButtonQuad.h;
    
    DEBUGRenderQuad(Buffer, &ButtonQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->FormTexture, 
                             &LevelEditor->FigureButtonQuad[4]);
    
    ButtonQuad.y += ButtonQuad.h;
    
    DEBUGRenderQuad(Buffer, &ButtonQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->TypeTexture, 
                             &LevelEditor->FigureButtonQuad[5]);
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->SaveButtonLayer, {255, 0, 0}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->SaveButtonLayer, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->SaveTexture, &LevelEditor->SaveButtonQuad);
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->LoadButtonLayer, {255, 0, 0}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->LoadButtonLayer, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->LoadTexture, &LevelEditor->LoadButtonQuad);
    //
    // Prev/Next level buttons rendering
    //
    
    CurrentLevel = LevelEntity->LevelNumber;
    
    if(CurrentLevel > 0)
    {
        GameRenderBitmapToBuffer(Buffer, LevelEditor->PrevLevelTexture, &LevelEditor->PrevLevelQuad);
    }
    
    if(CurrentLevel < Memory->LevelMemoryAmount-1)
    {
        GameRenderBitmapToBuffer(Buffer, LevelEditor->NextLevelTexture, &LevelEditor->NextLevelQuad);
    }
    
    
    if(LevelEditor->ButtonPressed)
    {
        DEBUGRenderQuadFill(Buffer, &LevelEditor->ActiveButton, {255, 0, 0}, 150);
    }
}
