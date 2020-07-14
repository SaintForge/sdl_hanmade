/* ========================================= */
//     $File: tetroman_menu.cpp
//     $Date: October 10th 2017 10:32 pm 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */

static menu_result_option
PlaygroundMenuUpdateAndRender(playground_menu *PlaygroundMenu, game_input *Input, render_group *RenderGroup)
{
    menu_result_option Result = {};
    
    v2 MenuButtonSize = {};
    MenuButtonSize.w  = 200.0f;
    MenuButtonSize.h  = 100.0f;
    
    v2 DifficultyButtonSize = {};
    DifficultyButtonSize.w = 500.0f;
    DifficultyButtonSize.h = 200.0f;
    
    v2 MainMenuButtonSize = DifficultyButtonSize;
    
    u32 RowAmount = 4;
    u32 ColumnAmount = 8;
    
    v2 InitialPosition = {};
    InitialPosition.x = VIRTUAL_GAME_WIDTH / 2.0f - ((ColumnAmount * MenuButtonSize.w) / 2.0f);
    InitialPosition.y = VIRTUAL_GAME_HEIGHT / 2.0f - ((RowAmount * MenuButtonSize.h) / 2.0f) + (VIRTUAL_GAME_HEIGHT / 4.0f);
    
    v2 DifficultyPosition = {};
    DifficultyPosition.x = VIRTUAL_GAME_WIDTH / 2.0f - ((DifficultyButtonSize.w * 3.0f) / 2.0f);
    DifficultyPosition.y = VIRTUAL_GAME_HEIGHT / 2.0f - ((DifficultyButtonSize.h) / 2.0f) - (VIRTUAL_GAME_HEIGHT / 4.0f);
    
    v2 MainMenuPosition = {};
    MainMenuPosition.x = VIRTUAL_GAME_WIDTH / 2.0f - (MainMenuButtonSize.w / 2.0f);
    MainMenuPosition.y = VIRTUAL_GAME_HEIGHT / 2.0f - ((MainMenuButtonSize.h * 3.0f) / 2.0f);
    
    if (Input->Keyboard.Escape.EndedDown)
    {
        switch(PlaygroundMenu->MenuPage)
        {
            case MAIN_PAGE:
            case QUIT_PAGE:
            {
                Result.QuitGame = true;
            } break;
            case DIFFICULTY_PAGE:
            case SETTINGS_PAGE:
            {
                PlaygroundMenu->MenuPage = MAIN_PAGE;
            } break;
            
        }
    }
    
    if (Input->MouseButtons[0].EndedDown)
    {
        v2 MousePos = {};
        MousePos.x = Input->MouseX;
        MousePos.y = Input->MouseY;
        
        switch(PlaygroundMenu->MenuPage)
        {
            case MAIN_PAGE:
            {
                rectangle2 ButtonRectangle = {};
                for (u32 Index = 0;
                     Index < 3;
                     ++Index)
                {
                    ButtonRectangle.Min.x = MainMenuPosition.x;
                    ButtonRectangle.Min.y = MainMenuPosition.y + (Index * MainMenuButtonSize.h);
                    SetDim(&ButtonRectangle, MainMenuButtonSize.w, MainMenuButtonSize.h - 10.0f);
                    
                    if (IsInRectangle(MousePos, ButtonRectangle))
                    {
                        menu_page Page = (menu_page)Index;
                        if (Page == menu_page::QUIT_PAGE)
                        {
                            Result.QuitGame = true;
                        }
                        
                        PlaygroundMenu->MenuPage = Page;
                    }
                }
            } break;
            
            case SETTINGS_PAGE:
            {
                rectangle2 ButtonRectangle = {};
                ButtonRectangle.Min.x = MainMenuPosition.x;
                ButtonRectangle.Min.y = MainMenuPosition.y;
                SetDim(&ButtonRectangle, MainMenuButtonSize.w, MainMenuButtonSize.h - 10.0f);
                
                if (IsInRectangle(MousePos, ButtonRectangle))
                {
                    resolution_standard Resolution = PlaygroundMenu->Resolution;
                    switch(Resolution)
                    {
                        case HD:
                        {
                            Resolution = resolution_standard::FULLHD;
                        } break;
                        case FULLHD:
                        {
                            Resolution = resolution_standard::QFULLHD;
                        } break;
                        case QFULLHD:
                        {
                            Resolution = resolution_standard::HD;
                        } break;
                        
                    }
                    
                    PlaygroundMenu->Resolution = Resolution;
                    Result.ChangeResolution = true;
                    Result.Resolution = Resolution;
                }
                
                ButtonRectangle.Min.x = MainMenuPosition.x;
                ButtonRectangle.Min.y = MainMenuPosition.y + MainMenuButtonSize.h + 10.0f;
                SetDim(&ButtonRectangle, MainMenuButtonSize.w, MainMenuButtonSize.h);
                
                if (IsInRectangle(MousePos, ButtonRectangle))
                {
                    PlaygroundMenu->IsFullScreen = PlaygroundMenu->IsFullScreen ? false : true;
                    Result.ToggleFullScreen = true;
                }
                
            } break;
            
            case DIFFICULTY_PAGE:
            {
                rectangle2 DifficultyRectangle = {};
                for (u32 Index = 0;
                     Index < 3;
                     ++Index)
                {
                    DifficultyRectangle.Min.x = DifficultyPosition.x + (Index * DifficultyButtonSize.w);
                    DifficultyRectangle.Min.y = DifficultyPosition.y;
                    SetDim(&DifficultyRectangle, DifficultyButtonSize.w - 10.0f, DifficultyButtonSize.h);
                    
                    if (IsInRectangle(MousePos, DifficultyRectangle))
                    {
                        difficulty_mode DiffMode = (difficulty_mode)Index;
                        PlaygroundMenu->DiffMode = DiffMode;
                    }
                }
                
                u32 InitialLevelIndex = 0;
                if (PlaygroundMenu->DiffMode == difficulty_mode::MEDIUM)
                    InitialLevelIndex = 32;
                else if (PlaygroundMenu->DiffMode == difficulty_mode::HARD)
                    InitialLevelIndex = 64;
                
                b32 FoundLevel = false;
                rectangle2 ButtonRectangle = {};
                for (u32 Row = 0; Row < RowAmount; ++Row)
                {
                    ButtonRectangle.Min.y = InitialPosition.y + (MenuButtonSize.h * Row);
                    for (u32 Column = 0; Column < ColumnAmount; ++Column)
                    {
                        ButtonRectangle.Min.x = InitialPosition.x + (MenuButtonSize.w * Column);
                        
                        ButtonRectangle.Max.x = ButtonRectangle.Min.x + MenuButtonSize.w - 10.0f;
                        ButtonRectangle.Max.y = ButtonRectangle.Min.y + MenuButtonSize.h - 10.0f;
                        
                        if (IsInRectangle(MousePos, ButtonRectangle))
                        {
                            Result.SwitchToPlayground = true;
                            Result.PlaygroundIndex = InitialLevelIndex + (Row * ColumnAmount) + Column;
                            printf("LevelIndex: %d\n", Result.PlaygroundIndex);
                            
                            FoundLevel = true;
                            break;
                        }
                    }
                    
                    if (FoundLevel) break;
                }
                
            } break;
        }
    }
    
    //
    // NOTE(msokolov): Rendering
    //
    
    Clear(RenderGroup, {42, 6, 21, 255});
    
    switch(PlaygroundMenu->MenuPage)
    {
        case MAIN_PAGE:
        {
            rectangle2 ButtonRectangle = {};
            for (u32 Index = 0;
                 Index < 3;
                 ++Index)
            {
                ButtonRectangle.Min.x = MainMenuPosition.x;
                ButtonRectangle.Min.y = MainMenuPosition.y + (Index * MainMenuButtonSize.h);
                SetDim(&ButtonRectangle, MainMenuButtonSize.w, MainMenuButtonSize.h - 10.0f);
                
                v4 Color = {128, 128, 128, 255};
                PushRectangleOutline(RenderGroup, ButtonRectangle, Color);
                
                rectangle2 TextRectangle = {};
                v2 Dim = QueryTextureDim(PlaygroundMenu->MainMenuTexture[Index]);
                TextRectangle.Min.x = ButtonRectangle.Min.x + (GetDim(ButtonRectangle).w / 2.0f) - (Dim.w / 2.0f);
                TextRectangle.Min.y = ButtonRectangle.Min.y + (GetDim(ButtonRectangle).h / 2.0f) - (Dim.h / 2.0f);
                SetDim(&TextRectangle, Dim);
                PushBitmap(RenderGroup, PlaygroundMenu->MainMenuTexture[Index], TextRectangle);
            }
            
        } break;
        
        case SETTINGS_PAGE:
        {
            rectangle2 ButtonRectangle = {};
            ButtonRectangle.Min.x = MainMenuPosition.x;
            ButtonRectangle.Min.y = MainMenuPosition.y;
            SetDim(&ButtonRectangle, MainMenuButtonSize.w, MainMenuButtonSize.h - 10.0f);
            
            v4 Color = {128, 128, 128, 255};
            PushRectangleOutline(RenderGroup, ButtonRectangle, Color);
            
            u32 Index = (u32)PlaygroundMenu->Resolution;
            
            rectangle2 TextRectangle = {};
            v2 Dim = QueryTextureDim(PlaygroundMenu->ResolutionTexture[Index]);
            TextRectangle.Min.x = ButtonRectangle.Min.x + (GetDim(ButtonRectangle).w / 2.0f) - (Dim.w / 2.0f);
            TextRectangle.Min.y = ButtonRectangle.Min.y + (GetDim(ButtonRectangle).h / 2.0f) - (Dim.h / 2.0f);
            SetDim(&TextRectangle, Dim);
            PushBitmap(RenderGroup, PlaygroundMenu->ResolutionTexture[Index], TextRectangle);
            
            ButtonRectangle.Min.x = MainMenuPosition.x;
            ButtonRectangle.Min.y = MainMenuPosition.y + (MainMenuButtonSize.h + 10.0f);
            SetDim(&ButtonRectangle, MainMenuButtonSize.w, MainMenuButtonSize.h - 10.0f);
            
            Color = {128, 128, 128, 255};
            PushRectangleOutline(RenderGroup, ButtonRectangle, Color);
            
            Index = (u32)PlaygroundMenu->IsFullScreen;
            
            TextRectangle = {};
            Dim = QueryTextureDim(PlaygroundMenu->FullScreenTexture[Index]);
            TextRectangle.Min.x = ButtonRectangle.Min.x + (GetDim(ButtonRectangle).w / 2.0f) - (Dim.w / 2.0f);
            TextRectangle.Min.y = ButtonRectangle.Min.y + (GetDim(ButtonRectangle).h / 2.0f) - (Dim.h / 2.0f);
            SetDim(&TextRectangle, Dim);
            PushBitmap(RenderGroup, PlaygroundMenu->FullScreenTexture[Index], TextRectangle);
            
        } break;
        
        case DIFFICULTY_PAGE:
        {
            rectangle2 DifficultyRectangle = {};
            for (u32 Index = 0;
                 Index < 3;
                 ++Index)
            {
                DifficultyRectangle.Min.x = DifficultyPosition.x + (Index * DifficultyButtonSize.w);
                DifficultyRectangle.Min.y = DifficultyPosition.y;
                SetDim(&DifficultyRectangle, DifficultyButtonSize.w - 10.0f, DifficultyButtonSize.h);
                
                v4 Color = {128, 128, 128, 255};
                PushRectangle(RenderGroup, DifficultyRectangle, Color);
                
                Color = {255, 255, 255, 255};
                PushRectangleOutline(RenderGroup, DifficultyRectangle, Color);
                
                rectangle2 TextRectangle = {};
                v2 Dim = QueryTextureDim(PlaygroundMenu->DifficultyTexture[Index]);
                TextRectangle.Min.x = DifficultyRectangle.Min.x + (GetDim(DifficultyRectangle).w / 2.0f) - (Dim.w / 2.0f);
                TextRectangle.Min.y = DifficultyRectangle.Min.y + (GetDim(DifficultyRectangle).h / 2.0f) - (Dim.h / 2.0f);
                SetDim(&TextRectangle, Dim);
                PushBitmap(RenderGroup, PlaygroundMenu->DifficultyTexture[Index], TextRectangle);
                
                if (Index == (s32)PlaygroundMenu->DiffMode)
                {
                    Color = {255, 255, 0, 128};
                    PushRectangle(RenderGroup, DifficultyRectangle, Color);
                }
            }
            
            u32 InitialLevelIndex = 0;
            if (PlaygroundMenu->DiffMode == difficulty_mode::MEDIUM)
                InitialLevelIndex = 32;
            else if (PlaygroundMenu->DiffMode == difficulty_mode::HARD)
                InitialLevelIndex = 64;
            
            rectangle2 ButtonRectangle = {};
            for (u32 Row = 0; Row < RowAmount; ++Row)
            {
                ButtonRectangle.Min.y = InitialPosition.y + (MenuButtonSize.h * Row);
                
                for (u32 Col = 0; Col < ColumnAmount; ++Col)
                {
                    ButtonRectangle.Min.x = InitialPosition.x + (MenuButtonSize.w * Col);
                    
                    ButtonRectangle.Max.x = ButtonRectangle.Min.x + MenuButtonSize.w - 10.0f;
                    ButtonRectangle.Max.y = ButtonRectangle.Min.y + MenuButtonSize.h - 10.0f;
                    
                    v4 Color = {128, 128, 128, 255};
                    PushRectangle(RenderGroup, ButtonRectangle, Color);
                    Color = {255, 255, 255, 255};
                    PushRectangleOutline(RenderGroup, ButtonRectangle, Color);
                    
                    u32 LevelIndex = InitialLevelIndex + (Row * ColumnAmount) + Col;
                    
                    rectangle2 LevelRectangle = {};
                    v2 Dim = QueryTextureDim(PlaygroundMenu->LevelNumberTexture[LevelIndex]);
                    LevelRectangle.Min.x = ButtonRectangle.Min.x + (GetDim(ButtonRectangle).w / 2.0f) - (Dim.w / 2.0f);
                    LevelRectangle.Min.y = ButtonRectangle.Min.y + (GetDim(ButtonRectangle).h / 2.0f) - (Dim.h / 2.0f);
                    SetDim(&LevelRectangle, Dim);
                    
                    PushBitmap(RenderGroup, PlaygroundMenu->LevelNumberTexture[LevelIndex], LevelRectangle);
                }
            }
        } break;
    }
    
    return (Result);
}