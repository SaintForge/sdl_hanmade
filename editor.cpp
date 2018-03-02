// game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

#include "editor.h"

static void
LevelEditorInit(level_entity *LevelEntity, game_memory *Memory, game_offscreen_buffer *Buffer)
{
    Memory->LevelEditor = (level_editor*)malloc(sizeof(level_editor));
    Assert(Memory->LevelEditor);
    
    *Memory->LevelEditor = {};
    
    Memory->LevelEditor->LevelNumberSelected = false;
    
    Memory->LevelEditor->ActiveButton.x = 0;
    Memory->LevelEditor->ActiveButton.y = 0;
    Memory->LevelEditor->ActiveButton.w = 0;
    Memory->LevelEditor->ActiveButton.h = 0;
    
    Memory->LevelEditor->SelectedFigure = 0;
    Memory->LevelEditor->ButtonPressed  = false;
    
    s32 ButtonSize   = LevelEntity->Configuration.InActiveBlockSize * 2;
    s32 FontSize     = LevelEntity->Configuration.InActiveBlockSize * 2;
    u32 RowAmount    = LevelEntity->GridEntity->RowAmount;
    u32 ColumnAmount = LevelEntity->GridEntity->ColumnAmount;
    
    char RowString[2] = {0};
    char ColString[2] = {0};
    sprintf(RowString, "%d", RowAmount);
    sprintf(ColString, "%d", ColumnAmount);
    
    level_editor *&LevelEditor = Memory->LevelEditor;
    LevelEditor->GridButtonLayer.w = ButtonSize;
    LevelEditor->GridButtonLayer.h = ButtonSize * 6;
    
    LevelEditor->GridButtonLayer.x = 0;
    LevelEditor->GridButtonLayer.y = 0;
    
    LevelEditor->Font = TTF_OpenFont("..\\data\\Karmina-Bold.otf", FontSize);
    Assert(LevelEditor->Font);
    
    LevelEditor->StatsFont = TTF_OpenFont("..\\data\\Karmina-Bold.otf", ButtonSize / 3);
    Assert(LevelEditor->StatsFont);
    
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
GridEntityDeleteMovingBlock(grid_entity *GridEntity, u32 Index)
{
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
LevelEditorChangeGridCounters(level_editor *LevelEditor, 
                              u32 NewRowAmount, u32 NewColumnAmount,
                              game_offscreen_buffer *Buffer)
{
    game_surface *Surface = 0;
    
    char RowString[3]   = {};
    char ColString[3]   = {};
    
    sprintf(RowString, "%d", NewRowAmount);
    sprintf(ColString, "%d", NewColumnAmount);
    
    if(LevelEditor->RowTexture)
    {
        FreeTexture(LevelEditor->RowTexture);
    }
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, RowString, { 0, 0, 0 });
    
    LevelEditor->RowTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->RowTexture, 0, 0, &LevelEditor->GridButtonQuad[1].w, &LevelEditor->GridButtonQuad[1].h);
    
    LevelEditor->GridButtonQuad[1].x = LevelEditor->GridButton[1].x + (LevelEditor->GridButton[1].w /2) - (LevelEditor->GridButtonQuad[1].w / 2);
    LevelEditor->GridButtonQuad[1].y = LevelEditor->GridButton[1].y + (LevelEditor->GridButton[4].h /2) - (LevelEditor->GridButtonQuad[1].h / 2);
    
    SDL_FreeSurface(Surface);
    
    if(LevelEditor->ColumnTexture)
    {
        FreeTexture(LevelEditor->ColumnTexture);
    }
    
    Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, ColString, { 0, 0, 0 });
    LevelEditor->ColumnTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    SDL_QueryTexture(LevelEditor->ColumnTexture, 0, 0, &LevelEditor->GridButtonQuad[4].w, &LevelEditor->GridButtonQuad[4].h);
    
    LevelEditor->GridButtonQuad[4].x = LevelEditor->GridButton[4].x + (LevelEditor->GridButton[4].w /2) - (LevelEditor->GridButtonQuad[4].w / 2);
    LevelEditor->GridButtonQuad[4].y = LevelEditor->GridButton[4].y + (LevelEditor->GridButton[4].h /2) - (LevelEditor->GridButtonQuad[4].h / 2);
    
    SDL_FreeSurface(Surface);
}


static void 
LevelEditorUpdateLevelStats(level_editor *LevelEditor, 
                            s32 LevelNumber, s32 LevelIndex, game_offscreen_buffer *Buffer)
{
    game_surface *Surface = {};
    
    char LevelNumberString[3] = {};
    char LevelIndexString[3]  = {};
    
    sprintf(LevelNumberString, "%d", LevelNumber);
    sprintf(LevelIndexString, "%d", LevelIndex);
    
    /* Level Index Texture initialization */
    
    {
        char TmpBuffer[128] = {};
        strcpy(TmpBuffer, "level index      = ");
        strcat(TmpBuffer, LevelIndexString);
        
        if(LevelEditor->LevelIndexTexture)
        {
            SDL_DestroyTexture(LevelEditor->LevelIndexTexture);
        }
        
        Surface = TTF_RenderUTF8_Blended(LevelEditor->StatsFont, TmpBuffer, {255, 255, 255});
        Assert(Surface);
        
        LevelEditor->LevelIndexQuad.w = Surface->w;
        LevelEditor->LevelIndexQuad.h = Surface->h;
        LevelEditor->LevelIndexQuad.x = 0;
        LevelEditor->LevelIndexQuad.y = (LevelEditor->LoadButtonQuad.y + LevelEditor->LoadButtonQuad.h) + (LevelEditor->LevelIndexQuad.h / 2);
        
        LevelEditor->LevelIndexTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
        Assert(LevelEditor->LevelIndexTexture);
        
        SDL_FreeSurface(Surface);
        
    }
    
    /* Level Number Texture initialization */
    
    {
        char TmpBuffer[128] = {};
        strcpy(TmpBuffer, "level number = ");
        strcat(TmpBuffer, LevelNumberString);
        
        if(LevelEditor->LevelNumberTexture)
        {
            SDL_DestroyTexture(LevelEditor->LevelNumberTexture);
        }
        
        Surface = TTF_RenderUTF8_Blended(LevelEditor->StatsFont, TmpBuffer, {255, 255, 255});
        Assert(Surface);
        
        LevelEditor->LevelNumberQuad.w = Surface->w;
        LevelEditor->LevelNumberQuad.h = Surface->h;
        LevelEditor->LevelNumberQuad.x = 0;
        LevelEditor->LevelNumberQuad.y = LevelEditor->LevelIndexQuad.y + LevelEditor->LevelIndexQuad.h;
        
        LevelEditor->LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
        Assert(LevelEditor->LevelNumberTexture);
        
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


inline void
LevelEditorRenderButton(game_rect *ButtonQuad, game_rect *ImageQuad,
                        game_color ButtonColor, u8 Alpha, game_texture *ImageTexture,
                        s32 *Accumulator, s32 Offset,  game_offscreen_buffer *Buffer)
{
    DEBUGRenderQuad(Buffer, ButtonQuad, ButtonColor, Alpha);
    GameRenderBitmapToBuffer(Buffer, ImageTexture, ImageQuad);
    
    if(Accumulator)
    {
        *Accumulator += Offset;
    }
}

static void
GridEntityNewGrid(game_offscreen_buffer *Buffer, level_entity *LevelEntity, 
                  s32 NewRowAmount, s32 NewColumnAmount, level_editor *LevelEditor)
{
    if(NewRowAmount < 0 || NewColumnAmount < 0) return;
    
    grid_entity *&GridEntity = LevelEntity->GridEntity;
    
    s32 *UnitField = (s32*)calloc(NewRowAmount * NewColumnAmount, sizeof(s32));
    Assert(UnitField);
    for(u32 Row = 0; Row < NewRowAmount; ++Row){
        for(u32 Col = 0; Col < NewColumnAmount; Col++){
            UnitField[(Row * NewColumnAmount) + Col] = 0;
        }
    }
    
    u32 CurrentRowAmount = NewRowAmount < GridEntity->RowAmount ? NewRowAmount : GridEntity->RowAmount;
    u32 CurrentColumnAmount = NewColumnAmount < GridEntity->ColumnAmount ? NewColumnAmount : GridEntity->ColumnAmount;
    
    for(u32 Row = 0; Row < CurrentRowAmount; ++Row){
        for(u32 Col = 0; Col < CurrentColumnAmount; ++Col){
            s32 UnitIndex = (Row * CurrentColumnAmount) + Col;
            UnitField[UnitIndex] = GridEntity->UnitField[UnitIndex];
        }
    }
    
    free(GridEntity->UnitField);
    
    LevelEditorChangeGridCounters(LevelEditor, 
                                  NewRowAmount, NewColumnAmount, 
                                  Buffer);
    
    u32 DefaultBlocksInRow = 12;
    u32 DefaultBlocksInCol = 9;
    
    RescaleGameField(Buffer, NewRowAmount, NewColumnAmount,
                     LevelEntity->FigureEntity->FigureAmount, DefaultBlocksInRow, DefaultBlocksInCol, LevelEntity);
    
    GridEntity->UnitField    = UnitField;
    GridEntity->RowAmount    = NewRowAmount;
    GridEntity->ColumnAmount = NewColumnAmount;
    
    u32 ActiveBlockSize   = LevelEntity->Configuration.ActiveBlockSize;
    u32 InActiveBlockSize = LevelEntity->Configuration.InActiveBlockSize;
    
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
    if(Input->Keyboard.BackQuote.EndedDown)
    {
        if(!LevelEditor->EditorMode)
        {
            LevelEditor->EditorMode  = true;
            LevelEntity->LevelPaused = true;
            RestartLevelEntity(LevelEntity);
        }
        else
        {
            LevelEditor->EditorMode  = false;
            LevelEntity->LevelPaused = false;
        }
    }
    
    if(!LevelEditor->EditorMode) 
    {
        return;
    }
    
    game_rect GridArea   = LevelEntity->GridEntity->GridArea;
    game_rect FigureArea = LevelEntity->FigureEntity->FigureArea;
    
    u32 FigureAmount  = LevelEntity->FigureEntity->FigureAmount;
    u32 RowAmount     = LevelEntity->GridEntity->RowAmount;
    u32 ColAmount     = LevelEntity->GridEntity->ColumnAmount;
    s32 NewIndex      = LevelEditor->SelectedFigure;
    s32 CurrentLevelIndex = Memory->CurrentLevelIndex;
    
    if(LevelEditor->CurrentLevelIndex != CurrentLevelIndex)
    {
        LevelEditor->CurrentLevelIndex = CurrentLevelIndex;
        LevelEditorChangeGridCounters(LevelEditor, RowAmount, ColAmount, Buffer);
        LevelEditorUpdateLevelStats(Memory->LevelEditor, Memory->LevelEntity.LevelNumber, CurrentLevelIndex, Buffer);
        
    }
    
    if(LevelEditor->LevelNumberSelected)
    {
        char NextDigit  = '\n';
        s32  DigitIndex = LevelEditor->LevelNumberBufferIndex;
        
        if(Input->Keyboard.Zero.EndedDown)
        {
            NextDigit = '0';
        }
        else if(Input->Keyboard.One.EndedDown)
        {
            NextDigit = '1';
        }
        else if(Input->Keyboard.Two.EndedDown)
        {
            NextDigit = '2';
        }
        else if(Input->Keyboard.Three.EndedDown)
        {
            NextDigit = '3';
        }
        else if(Input->Keyboard.Four.EndedDown)
        {
            NextDigit = '4';
        }
        else if(Input->Keyboard.Five.EndedDown)
        {
            NextDigit = '5';
        }
        else if(Input->Keyboard.Six.EndedDown)
        {
            NextDigit = '6';
        }
        else if(Input->Keyboard.Seven.EndedDown)
        {
            NextDigit = '7';
        }
        else if(Input->Keyboard.Eight.EndedDown)
        {
            NextDigit = '8';
        }
        else if(Input->Keyboard.Nine.EndedDown)
        {
            NextDigit = '9';
        }
        
        if(DigitIndex <= 2 && NextDigit != '\n')
        {
            LevelEditor->LevelNumberBuffer[DigitIndex] = NextDigit;
            LevelEditor->LevelNumberBufferIndex = ++DigitIndex;
            
            GameMakeTextureFromString(LevelEntity->LevelNumberTexture, 
                                      LevelEditor->LevelNumberBuffer, 
                                      &LevelEntity->LevelNumberQuad, 
                                      Memory->LevelNumberFont, 
                                      {255, 255, 255}, 
                                      Buffer);
            
            GameMakeTextureFromString(LevelEntity->LevelNumberShadowTexture, 
                                      LevelEditor->LevelNumberBuffer, 
                                      &LevelEntity->LevelNumberShadowQuad, 
                                      Memory->LevelNumberFont, 
                                      {0, 0, 0}, 
                                      Buffer);
            
            LevelEntity->LevelNumberQuad.x = (Buffer->Width - LevelEntity->LevelNumberQuad.w) - (LevelEntity->LevelNumberQuad.w / 2);
            LevelEntity->LevelNumberQuad.y = 0;
            
            LevelEntity->LevelNumberShadowQuad.x = LevelEntity->LevelNumberQuad.x + 3;
            LevelEntity->LevelNumberShadowQuad.y = LevelEntity->LevelNumberQuad.y + 3;
        }
    }
    
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
    else if(Input->Keyboard.Q_Button.EndedDown)
    {
        s32 PrevLevelNumber = CurrentLevelIndex - 1;
        if(PrevLevelNumber >= 0)
        {
            Memory->CurrentLevelIndex = PrevLevelNumber;
            
            LevelEntityUpdateLevelEntityFromMemory(&Memory->LevelEntity, 
                                                   Memory->CurrentLevelIndex, true,
                                                   Memory, Buffer);
            
            LevelEditorChangeGridCounters(Memory->LevelEditor, 
                                          Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount,
                                          Buffer);
            
            LevelEditorUpdateLevelStats(Memory->LevelEditor, 
                                        Memory->LevelEntity.LevelNumber, Memory->CurrentLevelIndex, Buffer);
        }
    }
    else if(Input->Keyboard.E_Button.EndedDown)
    {
        s32 NextLevelNumber = CurrentLevelIndex + 1;
        if(NextLevelNumber < Memory->LevelMemoryAmount)
        {
            Memory->CurrentLevelIndex = NextLevelNumber;
            
            LevelEntityUpdateLevelEntityFromMemory(&Memory->LevelEntity, 
                                                   Memory->CurrentLevelIndex,true,
                                                   Memory, Buffer);
            
            LevelEditorChangeGridCounters(Memory->LevelEditor, 
                                          Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount,
                                          Buffer);
            
            LevelEditorUpdateLevelStats(Memory->LevelEditor, 
                                        Memory->LevelEntity.LevelNumber, Memory->CurrentLevelIndex, Buffer);
        }
    }
    else if(Input->Keyboard.Enter.EndedDown)
    {
        if(LevelEditor->LevelNumberSelected)
        {
            LevelEntity->LevelNumber = strtol(LevelEditor->LevelNumberBuffer, 0, 10);
            LevelEditor->LevelNumberSelected = false;
            MenuChangeButtonText(Memory->LevelNumberFont, LevelEditor->LevelNumberBuffer, Memory->MenuEntity, 
                                 &Memory->MenuEntity->Buttons[Memory->CurrentLevelIndex], {255, 255, 255}, Buffer);
        }
    }
    
    if(Input->MouseButtons[0].EndedDown)
    {
        LevelEditor->ButtonPressed = true;
        
        if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEntity->LevelNumberQuad))
        {
            LevelEditor->LevelNumberSelected = true;
            LevelEditor->LevelNumberBufferIndex = 0;
            LevelEditor->OldLevelNumber = LevelEntity->LevelNumber;
            
            LevelEditor->LevelNumberBuffer[0] = '\0';
            LevelEditor->LevelNumberBuffer[1] = '\0';
            LevelEditor->LevelNumberBuffer[2] = '\0';
            LevelEditor->LevelNumberBuffer[3] = '\0';
        }
        else
        {
            if(LevelEditor->LevelNumberSelected)
            {
                LevelEditor->LevelNumberSelected = false;
                
                char StringBuffer[4] = {};
                sprintf(StringBuffer, "%d", LevelEntity->LevelNumber);
                
                GameMakeTextureFromString(LevelEntity->LevelNumberTexture, 
                                          StringBuffer, 
                                          &LevelEntity->LevelNumberQuad, 
                                          Memory->LevelNumberFont, 
                                          {255, 255, 255}, 
                                          Buffer);
                
                GameMakeTextureFromString(LevelEntity->LevelNumberShadowTexture, 
                                          StringBuffer, 
                                          &LevelEntity->LevelNumberShadowQuad, 
                                          Memory->LevelNumberFont, 
                                          {0, 0, 0}, 
                                          Buffer);
                
                
                LevelEntity->LevelNumberQuad.x = (Buffer->Width - LevelEntity->LevelNumberQuad.w) - (LevelEntity->LevelNumberQuad.w / 2);
                LevelEntity->LevelNumberQuad.y = 0;
                
                LevelEntity->LevelNumberShadowQuad.x = LevelEntity->LevelNumberQuad.x + 3;
                LevelEntity->LevelNumberShadowQuad.y = LevelEntity->LevelNumberQuad.y + 3;
            }
        }
        
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
                FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                GridEntityUpdateStickUnits(LevelEntity->GridEntity, LevelEntity->FigureEntity->FigureAmount);
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[0];
            }
            /* Delete figure */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->FigureButton[1]))
            {
                FigureUnitDeleteFigure(LevelEntity->FigureEntity, LevelEntity->FigureEntity->FigureAmount - 1);
                FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                GridEntityUpdateStickUnits(LevelEntity->GridEntity, LevelEntity->FigureEntity->FigureAmount);
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[1];
            }
            /* Rotate figure */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->FigureButton[2]))
            {
                FigureUnitRotateShellBy(&LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure], 90);
                LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure].Angle += 90.0f;
                FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[2];
            }
            /* Flip figure */
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                      &LevelEditor->FigureButton[3]))
            {
                FigureUnitFlipHorizontally(&LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure]);
                FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                
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
                    
                    FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
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
                    
                    FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                }
                
                LevelEditor->ActiveButton = LevelEditor->FigureButton[5];
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY,
                                  &LevelEditor->SaveButtonLayer))
        {
            printf("Save!\n");
            printf("LevelNumber = %d\n", LevelEntity->LevelNumber);
            
            SaveLevelToMemory(Memory, LevelEntity, Memory->CurrentLevelIndex);
            LevelEditor->ActiveButton = LevelEditor->SaveButtonLayer;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                  &LevelEditor->LoadButtonLayer))
        {
            LevelEntityUpdateLevelEntityFromMemory(LevelEntity, 
                                                   Memory->CurrentLevelIndex,
                                                   false, Memory, Buffer);
            LevelEditorChangeGridCounters(LevelEditor, 
                                          LevelEntity->GridEntity->RowAmount, LevelEntity->GridEntity->ColumnAmount, 
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
                    
                    
                    LevelEditorChangeGridCounters(Memory->LevelEditor, 
                                                  Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount, 
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
                    
                    LevelEditorChangeGridCounters(Memory->LevelEditor, 
                                                  Memory->LevelEntity.GridEntity->RowAmount, Memory->LevelEntity.GridEntity->ColumnAmount, Buffer);
                    
                }
                
                LevelEditor->ActiveButton = LevelEditor->NextLevelQuad;
            }
            
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &GridArea))
        {
            game_rect AreaQuad = { 0, 0, (s32)LevelEntity->Configuration.ActiveBlockSize, (s32)LevelEntity->Configuration.ActiveBlockSize };
            
            u32 StartX = 0;
            u32 StartY = 0;
            
            for(u32 Row = 0; Row < RowAmount; ++Row)
            {
                StartY = GridArea.y + (LevelEntity->Configuration.ActiveBlockSize * Row);
                
                for(u32 Col = 0; Col < ColAmount; ++Col)
                {
                    StartX = GridArea.x + (LevelEntity->Configuration.ActiveBlockSize * Col);
                    
                    AreaQuad.x = StartX;
                    AreaQuad.y = StartY;
                    
                    if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                         &AreaQuad))
                    {
                        s32 UnitIndex = (Row * ColAmount) + Col;
                        u32 GridUnit = LevelEntity->GridEntity->UnitField[UnitIndex];
                        if(GridUnit == 0)
                        {
                            LevelEntity->GridEntity->UnitField[UnitIndex] = 1;
                        }
                        else
                        {
                            s32 Index = -1;
                            for(u32 m = 0; m < LevelEntity->GridEntity->MovingBlocksAmount; ++m)
                            {
                                u32 RowNumber = LevelEntity->GridEntity->MovingBlocks[m].RowNumber;
                                u32 ColNumber = LevelEntity->GridEntity->MovingBlocks[m].ColNumber;
                                
                                if((Row == RowNumber) && (Col == ColNumber))
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
                                    LevelEntity->GridEntity->UnitField[UnitIndex] = 0;
                                }
                            }
                            else
                            {
                                GridEntityAddMovingBlock(LevelEntity->GridEntity, Row, Col, false, false, LevelEntity->Configuration.ActiveBlockSize);
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
            DEBUGRenderFigureShell(Buffer, &LevelEntity->FigureEntity->FigureUnit[i], LevelEntity->Configuration.InActiveBlockSize / 4, {0, 0, 255}, 255);
            DEBUGRenderQuad(Buffer, &LevelEntity->FigureEntity->FigureUnit[i].AreaQuad, {255, 0, 0}, 255);
        }
        
        if(LevelEntity->FigureEntity->FigureAmount > 0)
        {
            DEBUGRenderFigureShell(Buffer, &LevelEntity->FigureEntity->FigureUnit[LevelEditor->SelectedFigure], LevelEntity->Configuration.InActiveBlockSize, {255, 255, 255}, 100);
        }
        
    }
    
    game_rect ButtonQuad = 
    {
        ButtonQuad.x = LevelEditor->GridButtonLayer.x,
        ButtonQuad.y = LevelEditor->GridButtonLayer.y,
        ButtonQuad.w = LevelEntity->Configuration.InActiveBlockSize * 2, 
        ButtonQuad.h = LevelEntity->Configuration.InActiveBlockSize * 2
    };
    
    
    /* Grid buttons rendering*/ 
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->GridButtonLayer, {0, 0, 255}, 100);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->GridButtonQuad[0], {0, 0, 0}, 255, LevelEditor->PlusTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->GridButtonQuad[1], {0, 0, 0}, 255, LevelEditor->RowTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->GridButtonQuad[2], {0, 0, 0}, 255, LevelEditor->MinusTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->GridButtonQuad[3], {0, 0, 0}, 255, LevelEditor->PlusTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->GridButtonQuad[4], {0, 0, 0}, 255, LevelEditor->ColumnTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->GridButtonQuad[5], {0, 0, 0}, 255, LevelEditor->MinusTexture, 0, 0, Buffer);
    
    ButtonQuad.x = LevelEditor->FigureButtonLayer.x;
    ButtonQuad.y = LevelEditor->FigureButtonLayer.y;
    
    
    /* Figure buttons rendering*/ 
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->FigureButtonLayer, {0, 255, 0}, 100);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->FigureButtonQuad[0], {0, 0, 0}, 255, LevelEditor->PlusTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->FigureButtonQuad[1], {0, 0, 0}, 255, LevelEditor->MinusTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->FigureButtonQuad[2], {0, 0, 0}, 255, LevelEditor->RotateTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->FigureButtonQuad[3], {0, 0, 0}, 255, LevelEditor->FlipTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->FigureButtonQuad[4], {0, 0, 0}, 255, LevelEditor->FormTexture, &ButtonQuad.y, ButtonQuad.h, Buffer);
    
    LevelEditorRenderButton(&ButtonQuad, &LevelEditor->FigureButtonQuad[5], {0, 0, 0}, 255, LevelEditor->TypeTexture, 0, 0, Buffer);
    
    
    /* Save button rendering*/ 
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->SaveButtonLayer, {255, 0, 0}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->SaveButtonLayer, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->SaveTexture, &LevelEditor->SaveButtonQuad);
    
    
    /* Load buttons rendering*/ 
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->LoadButtonLayer, {255, 0, 0}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->LoadButtonLayer, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->LoadTexture, &LevelEditor->LoadButtonQuad);
    //
    // Prev/Next level buttons rendering
    //
    
    if(CurrentLevelIndex > 0)
    {
        GameRenderBitmapToBuffer(Buffer, LevelEditor->PrevLevelTexture, &LevelEditor->PrevLevelQuad);
    }
    
    if(CurrentLevelIndex < (s32)Memory->LevelMemoryAmount-1)
    {
        GameRenderBitmapToBuffer(Buffer, LevelEditor->NextLevelTexture, &LevelEditor->NextLevelQuad);
    }
    
    if(LevelEditor->ButtonPressed)
    {
        DEBUGRenderQuadFill(Buffer, &LevelEditor->ActiveButton, {255, 0, 0}, 150);
    }
    
    GameRenderBitmapToBuffer(Buffer, LevelEditor->LevelIndexTexture, &LevelEditor->LevelIndexQuad);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->LevelNumberTexture, &LevelEditor->LevelNumberQuad);
    
    /* Level number is selected */ 
    if(LevelEditor->LevelNumberSelected)
    {
        DEBUGRenderQuadFill(Buffer, &LevelEntity->LevelNumberQuad, {255, 0, 0}, 150);
    }
    
}



static void
MenuEditorInit(menu_entity *MenuEntity, game_memory *Memory, game_offscreen_buffer *Buffer)
{
    Memory->MenuEditor = (menu_editor *) malloc(sizeof(menu_editor));
    Assert(Memory->MenuEditor);
    
    menu_editor *MenuEditor = Memory->MenuEditor;
    MenuEditor->DeleteBarOn = false;
    MenuEditor->EditorMode  = false;
    
    MenuMakeTextButton("New", 0, 0, 100, 50,
                       &MenuEditor->NewLevelButtonQuad,
                       &MenuEditor->NewLevelTextureQuad,
                       MenuEditor->NewLevelTexture,
                       Memory->LevelNumberFont,
                       {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Load", 0, 50, 100, 50,
                       &MenuEditor->LoadButtonQuad,
                       &MenuEditor->LoadButtonTextureQuad,
                       MenuEditor->LoadButtonTexture,
                       Memory->LevelNumberFont,
                       {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Save", 0, 100, 100, 50,
                       &MenuEditor->SaveButtonQuad,
                       &MenuEditor->SaveButtonTextureQuad,
                       MenuEditor->SaveButtonTexture,
                       Memory->LevelNumberFont,
                       {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Sort", 0, 150, 100, 50,
                       &MenuEditor->SortButtonQuad,
                       &MenuEditor->SortButtonTextureQuad,
                       MenuEditor->SortButtonTexture,
                       Memory->LevelNumberFont,
                       {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("y", 0, 0, 30, 30,
                       &MenuEditor->DeleteButtonQuad,
                       &MenuEditor->DeleteButtonTextureQuad,
                       MenuEditor->DeleteButtonTexture,
                       Memory->LevelNumberFont,
                       {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("n", 0, 0, 30, 30,
                       &MenuEditor->CancelButtonQuad,
                       &MenuEditor->CancelButtonTextureQuad,
                       MenuEditor->CancelButtonTexture,
                       Memory->LevelNumberFont,
                       {255, 255, 255}, Buffer);
}

static void
MenuEditorUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
    menu_editor *&MenuEditor = Memory->MenuEditor;
    menu_entity *&MenuEntity = Memory->MenuEntity;
    
    if(Input->Keyboard.BackQuote.EndedDown)
    {
        if(!MenuEditor->EditorMode)
        {
            MenuEditor->EditorMode = true;
            MenuEntity->IsPaused = true;
        }
        else
        {
            MenuEditor->EditorMode = false;
            MenuEntity->IsPaused = false;
        }
    }
    
    if(!MenuEditor->EditorMode)
    {
        return;
    }
    
    if(Input->MouseButtons[0].EndedDown)
    {
        if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->NewLevelButtonQuad))
        {
            if(MenuEntity->ButtonsAmount < MenuEntity->ButtonsAmountReserved)
            {
                MenuEntity->ButtonsAmount += 1;
                Memory->LevelMemoryAmount += 1;
                
                MenuEntityAlignButtons(MenuEntity, Buffer->Width, Buffer->Height);
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->LoadButtonQuad))
        {
            LoadLevelMemoryFromFile("package2.bin", Memory);
            MenuLoadButtonsFromMemory(MenuEntity, Memory, Buffer);
        }
        if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->SaveButtonQuad))
        {
            SaveLevelMemoryToFile(Memory);
        }
        if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->SortButtonQuad))
        {
            MenuEntitySortButtons(MenuEntity, Memory);
            MenuLoadButtonsFromMemory(MenuEntity, Memory, Buffer);
        }
        
        if(MenuEditor->DeleteBarOn)
        {
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->DeleteButtonQuad))
            {
                MenuDeleteLevel(MenuEntity, MenuEntity->ButtonIndex,
                                Memory, Buffer);
                Memory->CurrentLevelIndex -= 1;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->CancelButtonQuad))
            {
            }
            
            MenuEditor->DeleteBarOn = false;
        }
    }
    if(Input->MouseButtons[1].EndedDown)
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
        
        if(Index >= 0)
        {
            MenuEditor->DeleteBarOn = true;
            
            MenuEditor->DeleteButtonQuad.x = MenuEntity->Buttons[Index].ButtonQuad.x;
            MenuEditor->DeleteButtonQuad.y = MenuEntity->Buttons[Index].ButtonQuad.y;
            
            MenuEditor->DeleteButtonTextureQuad.x = MenuEditor->DeleteButtonQuad.x + (MenuEditor->DeleteButtonQuad.w / 2) - (MenuEditor->DeleteButtonTextureQuad.w / 2);
            MenuEditor->DeleteButtonTextureQuad.y = MenuEditor->DeleteButtonQuad.y + (MenuEditor->DeleteButtonQuad.h / 2) - (MenuEditor->DeleteButtonTextureQuad.h / 2);
            
            MenuEditor->CancelButtonQuad.x = MenuEntity->Buttons[Index].ButtonQuad.x + 30;
            MenuEditor->CancelButtonQuad.y = MenuEntity->Buttons[Index].ButtonQuad.y;
            
            MenuEditor->CancelButtonTextureQuad.x = MenuEditor->CancelButtonQuad.x + (MenuEditor->CancelButtonQuad.w / 2) - (MenuEditor->CancelButtonTextureQuad.w / 2);
            MenuEditor->CancelButtonTextureQuad.y = MenuEditor->CancelButtonQuad.y + (MenuEditor->CancelButtonQuad.h / 2) - (MenuEditor->CancelButtonTextureQuad.h / 2);
        }
    }
    
    DEBUGRenderQuadFill(Buffer, &MenuEditor->NewLevelButtonQuad, {128, 128, 128}, 100);
    DEBUGRenderQuad(Buffer, &MenuEditor->NewLevelButtonQuad, {255, 255, 255}, 255);
    GameRenderBitmapToBuffer(Buffer, MenuEditor->NewLevelTexture, &MenuEditor->NewLevelTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &MenuEditor->LoadButtonQuad, {128, 128, 128}, 100);
    DEBUGRenderQuad(Buffer, &MenuEditor->LoadButtonQuad, {255, 255, 255}, 255);
    GameRenderBitmapToBuffer(Buffer, MenuEditor->LoadButtonTexture, &MenuEditor->LoadButtonTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &MenuEditor->SaveButtonQuad, {128, 128, 128}, 100);
    DEBUGRenderQuad(Buffer, &MenuEditor->SaveButtonQuad, {255, 255, 255}, 255);
    GameRenderBitmapToBuffer(Buffer, MenuEditor->SaveButtonTexture, &MenuEditor->SaveButtonTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &MenuEditor->SortButtonQuad, {128, 128, 128}, 100);
    DEBUGRenderQuad(Buffer, &MenuEditor->SortButtonQuad, {255, 255, 255}, 255);
    GameRenderBitmapToBuffer(Buffer, MenuEditor->SortButtonTexture, &MenuEditor->SortButtonTextureQuad);
    
    if(MenuEditor->DeleteBarOn)
    {
        DEBUGRenderQuadFill(Buffer, &MenuEditor->DeleteButtonQuad, {128, 128, 128}, 100);
        DEBUGRenderQuad(Buffer, &MenuEditor->DeleteButtonQuad, {255, 255, 255}, 255);
        GameRenderBitmapToBuffer(Buffer, MenuEditor->DeleteButtonTexture, &MenuEditor->DeleteButtonTextureQuad);
        
        DEBUGRenderQuadFill(Buffer, &MenuEditor->CancelButtonQuad, {128, 128, 128}, 100);
        DEBUGRenderQuad(Buffer, &MenuEditor->CancelButtonQuad, {255, 255, 255}, 255);
        GameRenderBitmapToBuffer(Buffer, MenuEditor->CancelButtonTexture, &MenuEditor->CancelButtonTextureQuad);
    }
}