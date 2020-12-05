/* ========================================= */
//     $File: tetroman_menu.cpp
//     $Date: October 10th 2017 10:32 pm 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */

static inline rectangle2
GetTextOnTheCenterOfRectangle(rectangle2 Rectangle, game_texture *Texture)
{
    rectangle2 Result = {};
    
    v2 RectangleDim = GetDim(Rectangle);
    v2 TextureDim = QueryTextureDim(Texture);
    
    Result.Min.x = Rectangle.Min.x + (RectangleDim.w * 0.5f);
    Result.Min.y = Rectangle.Min.y + (RectangleDim.h * 0.5f);
    SetDim(&Result, TextureDim);
    
    return (Result);
}

static rectangle2
GetBackgroundMenuArea(menu_page Page) {
    
    rectangle2 Result = {V2(0.0f, 0.0f), V2(VIRTUAL_GAME_WIDTH, VIRTUAL_GAME_HEIGHT)};
    
    switch (Page) {
        case menu_page::MAIN_PAGE: {
            Result.Min = V2(710.0f, 0.0f);
            Result.Max = V2(1210.0f, VIRTUAL_GAME_HEIGHT);
        } break;
        
        case menu_page::DIFFICULTY_PAGE: {
            Result.Min = V2(300.0f, 0.0f);
            Result.Max = V2(1620.0f, VIRTUAL_GAME_HEIGHT);
        } break;
        
        case menu_page::SETTINGS_PAGE: {
            Result.Min = V2(610.0f, 0.0f);
            Result.Max = V2(1310.0f, VIRTUAL_GAME_HEIGHT);
        } break;
    }
    
    return (Result);
}

static inline void
DrawCornersOnButton(render_group *RenderGroup, game_texture *CornerTexture[4], 
                    rectangle2 ButtonRectangle, v2 CornerDim)
{
    rectangle2 CornerRectangle = {};
    
    // top left
    CornerRectangle.Min.x = ButtonRectangle.Min.x;
    CornerRectangle.Min.y = ButtonRectangle.Min.y;
    SetDim(&CornerRectangle, CornerDim);
    PushBitmap(RenderGroup, CornerTexture[0], CornerRectangle);
    
    // bottom left
    CornerRectangle.Min.x = ButtonRectangle.Min.x;
    CornerRectangle.Min.y = ButtonRectangle.Max.y - CornerDim.h;
    SetDim(&CornerRectangle, CornerDim);
    PushBitmap(RenderGroup, CornerTexture[1], CornerRectangle);
    
    // top right
    CornerRectangle.Min.x = ButtonRectangle.Max.x - CornerDim.w;
    CornerRectangle.Min.y = ButtonRectangle.Min.y;
    SetDim(&CornerRectangle, CornerDim);
    PushBitmap(RenderGroup, CornerTexture[2], CornerRectangle);
    
    // bottom right
    CornerRectangle.Min.y = ButtonRectangle.Max.y - (CornerDim.h);
    SetDim(&CornerRectangle, CornerDim);
    PushBitmap(RenderGroup, CornerTexture[3], CornerRectangle);
}

static void
UpdateTextureForLevels(playground_menu *PlaygroundMenu, player_data *PlayerData, render_group *RenderGroup) {
    
    u32 RowAmount = 4;
    u32 ColumnAmount = 8;
    
    u32 InitialLevelIndex = 0;
    if (PlaygroundMenu->DiffMode == difficulty::MEDIUM)
        InitialLevelIndex = 32;
    else if (PlaygroundMenu->DiffMode == difficulty::HARD)
        InitialLevelIndex = 64;
    
    v2 MenuButtonSize = V2(200.0f, 100.0f);
    v2 ButtonDim = V2(150.0f, 86.0f);
    
    rectangle2 BackgroundRectangle = {};
    SetDim(&BackgroundRectangle, (ColumnAmount * 150.0f) + 30.0f + 10.0f, (RowAmount * 100.0f));
    
    PushBitmapOnBitmap(RenderGroup, PlaygroundMenu->LevelsCanvasTexture, PlaygroundMenu->BackgroundTexture,
                       BackgroundRectangle);
    
    
    b32 NextLevelFound = false;
    rectangle2 ButtonRectangle = {};
    for (u32 Row = 0; Row < RowAmount; ++Row)
    {
        rectangle2 ColorRectangle = {};
        ColorRectangle.Min.x = 0.0f;
        ColorRectangle.Min.y = (MenuButtonSize.h * Row);
        SetDim(&ColorRectangle, 30.0f, MenuButtonSize.h);
        
        PushBitmapOnBitmap(RenderGroup, PlaygroundMenu->LevelsCanvasTexture, PlaygroundMenu->ColorBarTexture[Row], ColorRectangle);
        
        ButtonRectangle.Min.y = (MenuButtonSize.h * Row);
        
        for (u32 Col = 0; Col < ColumnAmount; ++Col)
        {
            ButtonRectangle.Min.x = (GetDim(ColorRectangle).w) + (150.0f * Col) + 10.0f;
            
            ButtonRectangle.Max.x = ButtonRectangle.Min.x + 150.0f;
            ButtonRectangle.Max.y = ButtonRectangle.Min.y + 100.0f;
            
            u32 LevelIndex = InitialLevelIndex + (Row * ColumnAmount) + Col;
            b32 IsUnlocked = PlayerData->PlaygroundUnlocked[LevelIndex];
            
            rectangle2 LevelRectangle = {}; 
            if (IsUnlocked)
            {
                LevelRectangle.Min.x = ButtonRectangle.Min.x + (GetDim(ButtonRectangle).w * 0.5f) - (ButtonDim.w * 0.5f);
                LevelRectangle.Min.y = ButtonRectangle.Min.y + (GetDim(ButtonRectangle).h * 0.5f) - (ButtonDim.h * 0.5f);
                SetDim(&LevelRectangle, ButtonDim);
                PushBitmapOnBitmap(RenderGroup, PlaygroundMenu->LevelsCanvasTexture, PlaygroundMenu->SquareFrameUnlocked, LevelRectangle);
                
                u32 ForwardLevelIndex = LevelIndex + 1;
                if (!NextLevelFound && !PlayerData->PlaygroundUnlocked[ForwardLevelIndex]) {
                    
                    PushBitmapOnBitmap(RenderGroup, PlaygroundMenu->LevelsCanvasTexture, PlaygroundMenu->SquareFrameUnlocked, LevelRectangle);
                    
                    v2 IndicatorSize = V2(50.0f, 50.0f);
                    rectangle2 Rectangle = {};
                    Rectangle.Min = LevelRectangle.Min + (ButtonDim * 0.5f) - (IndicatorSize * 0.5f);
                    Rectangle.Max = Rectangle.Min + IndicatorSize;
                    
                    PushBitmapOnBitmap(RenderGroup, PlaygroundMenu->LevelsCanvasTexture, PlaygroundMenu->NextLevelTexture, Rectangle);
                    NextLevelFound = true;
                }
                else {
                    PushBitmapOnBitmap(RenderGroup, PlaygroundMenu->LevelsCanvasTexture, PlaygroundMenu->NextLevelBackgroundTexture, LevelRectangle);
                    
                    LevelRectangle = GetTextOnTheCenterOfRectangle(ButtonRectangle, PlaygroundMenu->LevelTimeTexture[LevelIndex]);
                    
                    LevelRectangle.Min += V2(5.0f, 5.0f);
                    LevelRectangle.Max += V2(5.0f, 5.0f);
                    
                    PushFontBitmapOnBitmap(RenderGroup, PlaygroundMenu->LevelsCanvasTexture, PlaygroundMenu->LevelTimeShadowTexture[LevelIndex], LevelRectangle);
                    
                    LevelRectangle.Min -= V2(5.0f, 5.0f);
                    LevelRectangle.Max -= V2(5.0f, 5.0f);
                    
                    PushFontBitmapOnBitmap(RenderGroup, PlaygroundMenu->LevelsCanvasTexture, PlaygroundMenu->LevelTimeTexture[LevelIndex], LevelRectangle);
                }
            }
            else
            {
                LevelRectangle.Min.x = ButtonRectangle.Min.x + (GetDim(ButtonRectangle).w * 0.5f) - (ButtonDim.w * 0.5f);
                LevelRectangle.Min.y = ButtonRectangle.Min.y + (GetDim(ButtonRectangle).h * 0.5f) - (ButtonDim.h * 0.5f);
                SetDim(&LevelRectangle, ButtonDim);
                PushBitmapOnBitmap(RenderGroup, PlaygroundMenu->LevelsCanvasTexture, PlaygroundMenu->SquareFrameLocked, LevelRectangle);
            }
        }
    }
}

static inline void
HighlightButtonWithLine(render_group *RenderGroup, game_texture *LineTexture, 
                        rectangle2 ButtonRectangle, v2 LineDim) 
{
    v2 Dim = QueryTextureDim(LineTexture);
    
    rectangle2 Rectangle 
    {
        ButtonRectangle.Min.x + (GetDim(ButtonRectangle).w * 0.5f) - (LineDim.w * 0.5f),
        ButtonRectangle.Min.y + (GetDim(ButtonRectangle).h) - (LineDim.h * 0.8f)
    };
    
    SetDim(&Rectangle, LineDim);
    PushBitmap(RenderGroup, LineTexture, Rectangle);
    //PushRectangle(RenderGroup, Rectangle, V4(0.0f, 255.0f, 0.0f, 255.0f));
}


static menu_result_option
PlaygroundMenuUpdateAndRender(playground_menu *PlaygroundMenu, 
                              player_data *PlayerData,
                              game_settings *Settings, game_input *Input,
                              render_group *RenderGroup)
{
    menu_result_option Result = {};
    
    v2 MenuButtonSize = {};
    MenuButtonSize.w  = 200.0f;
    MenuButtonSize.h  = 100.0f;
    
    v2 DifficultyButtonSize = {};
    DifficultyButtonSize.w = 400.0f;
    DifficultyButtonSize.h = 150.0f;
    
    v2 MainMenuButtonSize = DifficultyButtonSize;
    MainMenuButtonSize.w = 500.0f;
    MainMenuButtonSize.h = 100.0f;
    
    u32 RowAmount = 4;
    u32 ColumnAmount = 8;
    
    v2 InitialPosition = {};
    InitialPosition.x = VIRTUAL_GAME_WIDTH / 2.0f - ((ColumnAmount * MenuButtonSize.w) / 2.0f);
    InitialPosition.y = VIRTUAL_GAME_HEIGHT / 2.0f - ((RowAmount * MenuButtonSize.h) / 2.0f) + (VIRTUAL_GAME_HEIGHT / 4.0f);
    
    v2 DifficultyPosition = {};
    DifficultyPosition.x = (VIRTUAL_GAME_WIDTH * 0.5f) - (DifficultyButtonSize.w * 0.5f);
    DifficultyPosition.y = (VIRTUAL_GAME_HEIGHT * 0.5f) - (DifficultyButtonSize.h * 0.5f) - (VIRTUAL_GAME_HEIGHT * 0.4f);
    
    v2 MainMenuPosition = {};
    MainMenuPosition.x = VIRTUAL_GAME_WIDTH / 2.0f - (MainMenuButtonSize.w / 2.0f);
    MainMenuPosition.y = VIRTUAL_GAME_HEIGHT / 2.0f - ((MainMenuButtonSize.h * 3.0f) / 2.0f);
    
    if (!PlaygroundMenu->AnimationFinished) {
        PlaygroundMenu->InterpPoint += Input->dtForFrame / PlaygroundMenu->TimeMax;
        if (PlaygroundMenu->InterpPoint > 1.0f)
            PlaygroundMenu->InterpPoint = 1.0f;
    }
    
    // NOTE(msokolov): input events and logic
    if (Input->Keyboard.Escape.EndedDown)
    {
        switch(PlaygroundMenu->MenuPage)
        {
            case MAIN_PAGE:
            case QUIT_PAGE:
            {
                Result.QuitGame = true;
            } break;
            case DIFFICULTY_PAGE: {
                PlaygroundMenu->AnimationFinished = false;
                PlaygroundMenu->AnimationFinishedHalf = true;
                PlaygroundMenu->InterpPoint = 0.0f;
                PlaygroundMenu->InterpPointDiff = 1.0f;
                PlaygroundMenu->MenuPage = MAIN_PAGE;
            } break;
            case SETTINGS_PAGE:
            {
                PlaygroundMenu->ButtonIndex = 0;
                if (PlaygroundMenu->PlaygroundSwitch)
                {
                    Result.SwitchToPlayground = true;
                    return (Result);
                }
                else
                {
                    PlaygroundMenu->AnimationFinished = false;
                    PlaygroundMenu->InterpPoint = 0.0f;
                    PlaygroundMenu->MenuPage = MAIN_PAGE;
                }
                
            } break;
        }
    }
    
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
                    PlaygroundMenu->ButtonIndex = Index;
                    
                    menu_page Page = (menu_page)Index;
                    if (Input->MouseButtons[0].EndedDown)
                    {
                        if (Page == menu_page::QUIT_PAGE) {
                            Result.QuitGame = true;
                        }
                        
                        if (Page != PlaygroundMenu->MenuPage) {
                            PlaygroundMenu->MenuPage = Page;
                            PlaygroundMenu->AnimationFinished = false;
                            PlaygroundMenu->InterpPoint = 0.0f;
                            
                            
                            if (Page == menu_page::DIFFICULTY_PAGE) {
                                UpdateTextureForLevels(PlaygroundMenu, PlayerData, RenderGroup);
                                PlaygroundMenu->AnimationFinishedHalf = true;
                                PlaygroundMenu->ForwardAnimation = false;
                                PlaygroundMenu->InterpPointNext  = 1.0f;
                            }
                        }
                        
                        PlaygroundMenu->ButtonIndex = 0;
                    }
                }
            }
        } break;
        
        case SETTINGS_PAGE:
        {
            v2 SettingsButtonSize = V2(200.0f, 100.0f);
            v2 ValueButtonSize = V2(130.0f, 80.0f);
            
            v2 SettingsButtonPosition = 
            {
                VIRTUAL_GAME_WIDTH * 0.5f - ((SettingsButtonSize.w + ValueButtonSize.w * 2)* 0.5f),
                VIRTUAL_GAME_HEIGHT * 0.5f - ((SettingsButtonSize.h * 2) * 0.5f)
            };
            
            rectangle2 ButtonRectangle = {};
            for (u32 Index = 0;
                 Index < 2;
                 Index ++)
            {
                ButtonRectangle.Min.x = SettingsButtonPosition.x + (SettingsButtonSize.w);
                ButtonRectangle.Min.y = SettingsButtonPosition.y + (Index * (SettingsButtonSize.h));
                SetDim(&ButtonRectangle, ValueButtonSize.w * 2.0f, ValueButtonSize.h);
                
                if (IsInRectangle(MousePos, ButtonRectangle))
                {
                    rectangle2 ValueOneRectangle = {};
                    ValueOneRectangle.Min.x = SettingsButtonPosition.x + (SettingsButtonSize.w);
                    ValueOneRectangle.Min.y = SettingsButtonPosition.y + (Index * (SettingsButtonSize.h));
                    SetDim(&ValueOneRectangle, ValueButtonSize);
                    
                    rectangle2 ValueTwoRectangle = {};
                    ValueTwoRectangle.Min.x = ValueOneRectangle.Min.x + ValueButtonSize.w;
                    ValueTwoRectangle.Min.y = ValueOneRectangle.Min.y;
                    SetDim(&ValueTwoRectangle, ValueButtonSize);
                    
                    // NOTE(msokolov): Sound settings
                    if (Index == 0) 
                    {
                        if (IsInRectangle(MousePos, ValueOneRectangle)) 
                        {
                            if (Input->MouseButtons[0].EndedDown) {
                                PlaygroundMenu->SoundOn = true;
                                Settings->SoundIsOn = true;
                                Result.SettingsChanged = true;
                            }
                        }
                        else if (IsInRectangle(MousePos, ValueTwoRectangle)) 
                        {
                            if (Input->MouseButtons[0].EndedDown)  {
                                PlaygroundMenu->SoundOn = false;
                                Settings->SoundIsOn = false;
                                Result.SettingsChanged = true;
                            }
                        }
                    }
                    else if (Index == 1) 
                    {
                        if (IsInRectangle(MousePos, ValueOneRectangle)) 
                        {
                            if (Input->MouseButtons[0].EndedDown) {
                                PlaygroundMenu->MusicOn = true;
                                Settings->MusicIsOn = true;
                                Result.SettingsChanged = true;
                            }
                        }
                        else if (IsInRectangle(MousePos, ValueTwoRectangle)) 
                        {
                            if (Input->MouseButtons[0].EndedDown) {
                                PlaygroundMenu->MusicOn = false;
                                Settings->MusicIsOn = false;
                                Result.SettingsChanged = true;
                            }
                        }
                    }
                }
            }
            
            ButtonRectangle.Min.x = VIRTUAL_GAME_WIDTH * 0.5f - (ValueButtonSize.w * 0.5f);
            ButtonRectangle.Min.y = VIRTUAL_GAME_HEIGHT - (ValueButtonSize.h + 50.0f);
            SetDim(&ButtonRectangle, ValueButtonSize);
            
            PlaygroundMenu->ButtonIndex = 0;
            if (IsInRectangle(MousePos, ButtonRectangle))
            {
                PlaygroundMenu->ButtonIndex = 4;
                
                if (Input->MouseButtons[0].EndedDown)
                {
                    PlaygroundMenu->ButtonIndex = 0;
                    
                    if (PlaygroundMenu->PlaygroundSwitch)
                    {
                        Result.SwitchToPlayground = true;
                        return (Result);
                    }
                    else
                    {
                        PlaygroundMenu->AnimationFinished = false;
                        PlaygroundMenu->InterpPoint = 0.0f;
                        PlaygroundMenu->MenuPage = MAIN_PAGE;
                    }
                }
            }
            
        } break;
        
        case DIFFICULTY_PAGE:
        {
            PlaygroundMenu->InterpPointDiff += Input->dtForFrame * 2.0f;
            if (PlaygroundMenu->InterpPointDiff >= 1.0f) 
                PlaygroundMenu->InterpPointDiff = 1.0f;
            
            u32 ButtonIndex = 4;
            rectangle2 DifficultyRectangle = {};
            for (u32 Index = 0;
                 Index < 3;
                 ++Index)
            {
                DifficultyRectangle.Min.x = DifficultyPosition.x;
                DifficultyRectangle.Min.y = DifficultyPosition.y  + (Index * DifficultyButtonSize.h);
                SetDim(&DifficultyRectangle, 400, 150);
                
                if (IsInRectangle(MousePos, DifficultyRectangle))
                {
                    ButtonIndex = Index;
                    if (Input->MouseButtons[0].EndedDown)
                    {
                        difficulty DiffMode = (difficulty)Index;
                        if (PlaygroundMenu->DiffMode != DiffMode) {
                            PlaygroundMenu->DiffMode = DiffMode;
                            
                            // Just for a small alpha blending animation
                            PlaygroundMenu->ForwardAnimation = false;
                            PlaygroundMenu->InterpPointNext  = 1.0f;
                            PlaygroundMenu->InterpPointDiff = 0.0f;
                            PlaygroundMenu->AnimationFinishedHalf = false;
                        }
                    }
                }
            }
            
            PlaygroundMenu->ButtonIndex = ButtonIndex;
            
            u32 InitialLevelIndex = 0;
            if (PlaygroundMenu->DiffMode == difficulty::MEDIUM)
                InitialLevelIndex = 32;
            else if (PlaygroundMenu->DiffMode == difficulty::HARD)
                InitialLevelIndex = 64;
            
            b32 FoundLevel = false;
            v2 LevelPosition = 
            {
                LevelPosition.x = VIRTUAL_GAME_WIDTH * 0.5f - (((ColumnAmount * 150.0f) + 30.0f) * 0.5f),
                LevelPosition.y = VIRTUAL_GAME_HEIGHT * 0.5f - ((RowAmount * 100.0f) * 0.5f) + (VIRTUAL_GAME_HEIGHT * 0.2f)
            };
            
            rectangle2 ButtonRectangle = {};
            for (u32 Row = 0; Row < RowAmount && !FoundLevel; ++Row)
            {
                ButtonRectangle.Min.y = LevelPosition.y + (MenuButtonSize.h * Row);
                
                for (u32 Column = 0; Column < ColumnAmount; ++Column)
                {
                    ButtonRectangle.Min.x = LevelPosition.x + (30.0f) + (150.0f * Column) + 10.0f;
                    
                    ButtonRectangle.Max.x = ButtonRectangle.Min.x + 150.0f;
                    ButtonRectangle.Max.y = ButtonRectangle.Min.y + 100.0f;
                    
                    if (IsInRectangle(MousePos, ButtonRectangle))
                    {
                        if (Input->MouseButtons[0].EndedDown) {
                            
                            u32 PlaygroundIndex = InitialLevelIndex + (Row * ColumnAmount) + Column;
                            if (PlayerData->PlaygroundUnlocked[PlaygroundIndex])
                            {
                                Result.SwitchToPlayground = true;
                                Result.PlaygroundIndex = PlaygroundIndex;
                                printf("LevelIndex: %d\n", Result.PlaygroundIndex);
                                
                                PlaygroundMenu->BackgroundArea = {V2(0.0f, 0.0f), V2(VIRTUAL_GAME_WIDTH, VIRTUAL_GAME_HEIGHT)};
                                FoundLevel = true;
                                break;
                            }
                        }
                    }
                }
            }
            
            /* Back Button */
            ButtonRectangle.Min.x = 320.0f;
            ButtonRectangle.Min.y = VIRTUAL_GAME_HEIGHT - (120.0f);
            SetDim(&ButtonRectangle, 200.0f, 100.0f);
            
            if (IsInRectangle(MousePos, ButtonRectangle))
            {
                PlaygroundMenu->ButtonIndex = 3;
                
                if (Input->MouseButtons[0].EndedDown)
                {
                    PlaygroundMenu->AnimationFinished = false;
                    PlaygroundMenu->InterpPoint = 0.0f;
                    PlaygroundMenu->ButtonIndex = 0;
                    PlaygroundMenu->MenuPage = MAIN_PAGE;
                }
            }
            
        } break;
    }
    
    //
    // NOTE(msokolov): Rendering
    //
    
    ClearScreen(RenderGroup, {0.0f, 0.0f, 0.0f, 255.0f});
    
    switch(PlaygroundMenu->MenuPage)
    {
        case MAIN_PAGE:
        {
            rectangle2 BackgroundRectangle = GetBackgroundMenuArea(PlaygroundMenu->MenuPage);
            rectangle2 FinalBackgroundRectangle = BackgroundRectangle;
            
            if (!PlaygroundMenu->AnimationFinished) {
                
                rectangle2 AnimationBackground = {};
                AnimationBackground.Min.y = 0.0f;
                AnimationBackground.Max.y = BackgroundRectangle.Max.y;
                AnimationBackground.Min.x = Lerp1(PlaygroundMenu->BackgroundArea.Min.x, BackgroundRectangle.Min.x, PlaygroundMenu->InterpPoint);
                AnimationBackground.Max.x = Lerp1(PlaygroundMenu->BackgroundArea.Max.x, BackgroundRectangle.Max.x, PlaygroundMenu->InterpPoint);
                
                PushBitmap(RenderGroup, PlaygroundMenu->BackgroundTexture, AnimationBackground);
                
                FinalBackgroundRectangle = AnimationBackground;
            }
            else {
                PlaygroundMenu->BackgroundArea = BackgroundRectangle;
                PushBitmap(RenderGroup, PlaygroundMenu->BackgroundTexture, BackgroundRectangle);
            }
            
            DrawCornersOnButton(RenderGroup, PlaygroundMenu->CornerTexture, FinalBackgroundRectangle, V2(200.0f, 200.0f));
            
            rectangle2 ButtonRectangle = {};
            for (u32 Index = 0;
                 Index < 3;
                 ++Index)
            {
                ButtonRectangle.Min.x = MainMenuPosition.x;
                ButtonRectangle.Min.y = MainMenuPosition.y + (Index * MainMenuButtonSize.h);
                SetDim(&ButtonRectangle, MainMenuButtonSize.w, MainMenuButtonSize.h - 10.0f);
                
                rectangle2 TextRectangle = {};
                v2 Dim = QueryTextureDim(PlaygroundMenu->MainMenuTexture[Index]);
                TextRectangle.Min.x = ButtonRectangle.Min.x + (GetDim(ButtonRectangle).w * 0.5f);
                TextRectangle.Min.y = ButtonRectangle.Min.y + (GetDim(ButtonRectangle).h * 0.5f);
                SetDim(&TextRectangle, Dim);
                
                rectangle2 ShadowRectangle = TextRectangle;
                ShadowRectangle.Min += 5.0f;
                ShadowRectangle.Max += 5.0f;
                
                PushFontBitmap(RenderGroup, PlaygroundMenu->MainMenuShadowTexture[Index], ShadowRectangle);
                PushFontBitmap(RenderGroup, PlaygroundMenu->MainMenuTexture[Index], TextRectangle);
                
                if (!PlaygroundMenu->AnimationFinished) {
                    r32 AlphaChannel = (1.0f - PlaygroundMenu->InterpPoint) * 255.0f;
                    
                    ButtonRectangle.Min.x = FinalBackgroundRectangle.Min.x;
                    ButtonRectangle.Max.x = FinalBackgroundRectangle.Max.x;
                    PushRectangle(RenderGroup, ButtonRectangle, V4(51.0f, 8.0f, 23.0f, AlphaChannel));
                    
                    rectangle2 ClipRectangle = {};
                    ClipRectangle.Min = V2(0.0f, ButtonRectangle.Min.y);
                    ClipRectangle.Max = V2(ButtonRectangle.Min.x, ButtonRectangle.Max.y);
                    PushRectangle(RenderGroup, ClipRectangle, V4(0.0f, 0.0f, 0.0f, 255.0f));
                    
                    ClipRectangle.Min = V2(ButtonRectangle.Max.x, ButtonRectangle.Min.y);
                    ClipRectangle.Max = V2(VIRTUAL_GAME_WIDTH, ButtonRectangle.Max.y);
                    PushRectangle(RenderGroup, ClipRectangle, V4(0.0f, 0.0f, 0.0f, 255.0f));
                }
                
                if (PlaygroundMenu->AnimationFinished) {
                    if (PlaygroundMenu->ButtonIndex == Index)
                    {
                        Dim = QueryTextureDim(PlaygroundMenu->HorizontalLineTexture);
                        
                        v2 LineDim = V2(200.0f, 33.0f);
                        HighlightButtonWithLine(RenderGroup, PlaygroundMenu->HorizontalLineTexture, 
                                                ButtonRectangle, LineDim);
                    }
                }
            }
            
        } break;
        
        case SETTINGS_PAGE:
        {
            rectangle2 BackgroundRectangle = GetBackgroundMenuArea(PlaygroundMenu->MenuPage);
            rectangle2 FinalBackgroundRectangle = BackgroundRectangle;
            
            if (!PlaygroundMenu->AnimationFinished) {
                
                rectangle2 AnimationBackground = {};
                AnimationBackground.Min.y = 0.0f;
                AnimationBackground.Max.y = BackgroundRectangle.Max.y;
                AnimationBackground.Min.x = Lerp1(PlaygroundMenu->BackgroundArea.Min.x, BackgroundRectangle.Min.x, PlaygroundMenu->InterpPoint);
                AnimationBackground.Max.x = Lerp1(PlaygroundMenu->BackgroundArea.Max.x, BackgroundRectangle.Max.x, PlaygroundMenu->InterpPoint);
                
                PushBitmap(RenderGroup, PlaygroundMenu->BackgroundTexture, AnimationBackground);
                
                FinalBackgroundRectangle = AnimationBackground;
            }
            else {
                PlaygroundMenu->BackgroundArea = BackgroundRectangle;
                PushBitmap(RenderGroup, PlaygroundMenu->BackgroundTexture, BackgroundRectangle);
            }
            
            DrawCornersOnButton(RenderGroup, PlaygroundMenu->CornerTexture, FinalBackgroundRectangle, V2(200.0f, 200.0f));
            
            // NOTE(msokolov): Settings buttons
            v2 SettingsButtonSize = V2(200.0f, 100.0f);
            v2 ValueButtonSize    = V2(130.0f, 80.0f);
            
            v2 SettingsButtonPosition = 
            {
                VIRTUAL_GAME_WIDTH * 0.5f - ((SettingsButtonSize.w + ValueButtonSize.w * 2)* 0.5f),
                VIRTUAL_GAME_HEIGHT * 0.5f - ((SettingsButtonSize.h * 2) * 0.5f)
            };
            
            rectangle2 OptionRectangle{};
            rectangle2 ValueOneRectangle = {};
            rectangle2 ValueTwoRectangle = {};
            rectangle2 TextRectangle = {};
            rectangle2 TextShadowRectangle = {};
            
            for (u32 Index = 0;
                 Index < 2;
                 Index ++)
            {
                OptionRectangle.Min.x = SettingsButtonPosition.x;
                OptionRectangle.Min.y = SettingsButtonPosition.y + (Index * (SettingsButtonSize.h));
                SetDim(&OptionRectangle, SettingsButtonSize);
                
                ValueOneRectangle.Min.x = SettingsButtonPosition.x + (SettingsButtonSize.w);
                ValueOneRectangle.Min.y = SettingsButtonPosition.y + (Index * (SettingsButtonSize.h));
                SetDim(&ValueOneRectangle, ValueButtonSize);
                
                ValueTwoRectangle.Min.x = ValueOneRectangle.Min.x + ValueButtonSize.w;
                ValueTwoRectangle.Min.y = ValueOneRectangle.Min.y;
                SetDim(&ValueTwoRectangle, ValueButtonSize);
                
                // NOTE(msokolov): Sound settings
                if (Index == 0)
                {
                    TextRectangle = GetTextOnTheCenterOfRectangle(OptionRectangle, PlaygroundMenu->SoundNameTexture);
                    
                    TextShadowRectangle = TextRectangle;
                    TextShadowRectangle.Min += 5.0f;
                    TextShadowRectangle.Max += 5.0f;
                    
                    PushFontBitmap(RenderGroup, PlaygroundMenu->SoundNameShadowTexture, TextShadowRectangle);
                    PushFontBitmap(RenderGroup, PlaygroundMenu->SoundNameTexture, TextRectangle);
                    
                    // NOTE(msokolov): On/Off for sound
                    
                    // NOTE(msokolov): On value
                    rectangle2 OnOffRectangle = GetTextOnTheCenterOfRectangle(ValueOneRectangle, PlaygroundMenu->SoundOnTexture);
                    rectangle2 OnOffShadowRectangle = OnOffRectangle;
                    OnOffShadowRectangle.Min += 5.0f;
                    OnOffShadowRectangle.Max += 5.0f;
                    PushFontBitmap(RenderGroup, PlaygroundMenu->SoundOnShadowTexture, OnOffShadowRectangle);
                    PushFontBitmap(RenderGroup, PlaygroundMenu->SoundOnTexture, OnOffRectangle);
                    
                    // NOTE(msokolov): Off value
                    OnOffRectangle = GetTextOnTheCenterOfRectangle(ValueTwoRectangle, PlaygroundMenu->SoundOffTexture);
                    OnOffShadowRectangle = OnOffRectangle;
                    OnOffShadowRectangle.Min += 5.0f;
                    OnOffShadowRectangle.Max += 5.0f;
                    
                    PushFontBitmap(RenderGroup, PlaygroundMenu->SoundOffShadowTexture, OnOffShadowRectangle);
                    PushFontBitmap(RenderGroup, PlaygroundMenu->SoundOffTexture, OnOffRectangle);
                    
                    v2 LineDim = {130.0f, 21.45f};
                    HighlightButtonWithLine(RenderGroup, PlaygroundMenu->HorizontalLineTexture, 
                                            PlaygroundMenu->SoundOn ? ValueOneRectangle : ValueTwoRectangle, LineDim);
                }
                // NOTE(msokolov): Music settings
                else if (Index == 1)
                {
                    TextRectangle = GetTextOnTheCenterOfRectangle(OptionRectangle, PlaygroundMenu->MusicNameTexture);
                    
                    TextShadowRectangle = TextRectangle;
                    TextShadowRectangle.Min += 5.0f;
                    TextShadowRectangle.Max += 5.0f;
                    
                    PushFontBitmap(RenderGroup, PlaygroundMenu->MusicNameShadowTexture, TextShadowRectangle);
                    PushFontBitmap(RenderGroup, PlaygroundMenu->MusicNameTexture, TextRectangle);
                    
                    // NOTE(msokolov): On/Off for sound
                    
                    // NOTE(msokolov): On value
                    rectangle2 OnOffRectangle = GetTextOnTheCenterOfRectangle(ValueOneRectangle, PlaygroundMenu->SoundOnTexture);
                    rectangle2 OnOffShadowRectangle = OnOffRectangle;
                    OnOffShadowRectangle.Min += 5.0f;
                    OnOffShadowRectangle.Max += 5.0f;
                    PushFontBitmap(RenderGroup, PlaygroundMenu->SoundOnShadowTexture, OnOffShadowRectangle);
                    PushFontBitmap(RenderGroup, PlaygroundMenu->SoundOnTexture, OnOffRectangle);
                    
                    
                    // NOTE(msokolov): Off value
                    OnOffRectangle = GetTextOnTheCenterOfRectangle(ValueTwoRectangle, PlaygroundMenu->SoundOffTexture);
                    OnOffShadowRectangle = OnOffRectangle;
                    OnOffShadowRectangle.Min += 5.0f;
                    OnOffShadowRectangle.Max += 5.0f;
                    
                    PushFontBitmap(RenderGroup, PlaygroundMenu->SoundOffShadowTexture, OnOffShadowRectangle);
                    PushFontBitmap(RenderGroup, PlaygroundMenu->SoundOffTexture, OnOffRectangle);
                    
                    v2 LineDim = {130.0f, 21.45f};
                    HighlightButtonWithLine(RenderGroup, PlaygroundMenu->HorizontalLineTexture, 
                                            PlaygroundMenu->MusicOn ? ValueOneRectangle : ValueTwoRectangle, LineDim);
                }
                
                if (!PlaygroundMenu->AnimationFinished) {
                    
                    r32 AlphaChannel = (1.0f - PlaygroundMenu->InterpPoint) * 255.0f;
                    PushRectangle(RenderGroup, OptionRectangle, V4(51.0f, 8.0f, 23.0f, AlphaChannel));
                    
                    ValueOneRectangle.Max.y += 5.0f;
                    ValueTwoRectangle.Max.y += 5.0f;
                    PushRectangle(RenderGroup, ValueOneRectangle, V4(51.0f, 8.0f, 23.0f, AlphaChannel));
                    PushRectangle(RenderGroup, ValueTwoRectangle, V4(51.0f, 8.0f, 23.0f, AlphaChannel));
                }
            }
            
            /* Back Button */
            rectangle2 ButtonRectangle = {};
            ButtonRectangle.Min.x = VIRTUAL_GAME_WIDTH * 0.5f - (ValueButtonSize.w * 0.5f);
            ButtonRectangle.Min.y = VIRTUAL_GAME_HEIGHT - (ValueButtonSize.h + 50.0f);
            SetDim(&ButtonRectangle, ValueButtonSize);
            
            TextRectangle = GetTextOnTheCenterOfRectangle(ButtonRectangle, PlaygroundMenu->BackTexture);
            
            TextShadowRectangle = TextRectangle;
            TextShadowRectangle.Min += 5.0f;
            TextShadowRectangle.Max += 5.0f;
            
            PushFontBitmap(RenderGroup, PlaygroundMenu->BackShadowTexture, TextShadowRectangle);
            PushFontBitmap(RenderGroup, PlaygroundMenu->BackTexture, TextRectangle);
            
            if (!PlaygroundMenu->AnimationFinished) {
                r32 AlphaChannel = (1.0f - PlaygroundMenu->InterpPoint) * 255.0f;
                PushRectangle(RenderGroup, ButtonRectangle, V4(51.0f, 8.0f, 23.0f, AlphaChannel));
            }
            
            if (PlaygroundMenu->ButtonIndex == 4)
            {
                v2 LineDim = {200.0f, 33.0f};
                HighlightButtonWithLine(RenderGroup, PlaygroundMenu->HorizontalLineTexture, 
                                        ButtonRectangle, LineDim);
            }
        } break;
        
        case DIFFICULTY_PAGE:
        {
            rectangle2 BackgroundRectangle = GetBackgroundMenuArea(PlaygroundMenu->MenuPage);
            rectangle2 FinalBackgroundRectangle = BackgroundRectangle;
            
            if (!PlaygroundMenu->AnimationFinished) {
                
                rectangle2 AnimationBackground = {};
                AnimationBackground.Min.y = 0.0f;
                AnimationBackground.Max.y = BackgroundRectangle.Max.y;
                AnimationBackground.Min.x = Lerp1(PlaygroundMenu->BackgroundArea.Min.x, BackgroundRectangle.Min.x, PlaygroundMenu->InterpPoint);
                AnimationBackground.Max.x = Lerp1(PlaygroundMenu->BackgroundArea.Max.x, BackgroundRectangle.Max.x, PlaygroundMenu->InterpPoint);
                
                PushBitmap(RenderGroup, PlaygroundMenu->BackgroundTexture, AnimationBackground);
                
                FinalBackgroundRectangle = AnimationBackground;
            }
            else {
                if (!Result.SwitchToPlayground)
                    PlaygroundMenu->BackgroundArea = BackgroundRectangle;
                
                PushBitmap(RenderGroup, PlaygroundMenu->BackgroundTexture, BackgroundRectangle);
            }
            
            DrawCornersOnButton(RenderGroup, PlaygroundMenu->CornerTexture, FinalBackgroundRectangle, V2(200.0f, 200.0f));
            
            rectangle2 DifficultyRectangle = {};
            for (u32 Index = 0;
                 Index < 3;
                 ++Index)
            {
                DifficultyRectangle.Min.x = DifficultyPosition.x;
                DifficultyRectangle.Min.y = DifficultyPosition.y + ((Index * DifficultyButtonSize.h));
                SetDim(&DifficultyRectangle, DifficultyButtonSize);
                
                rectangle2 TextRectangle = GetTextOnTheCenterOfRectangle(DifficultyRectangle, PlaygroundMenu->DifficultyTexture[Index]);
                
                rectangle2 TextShadowRectangle = TextRectangle;
                TextShadowRectangle.Min += 5.0f;
                TextShadowRectangle.Max += 5.0f;
                
                PushFontBitmap(RenderGroup, PlaygroundMenu->DifficultyShadowTexture[Index], TextShadowRectangle);
                PushFontBitmap(RenderGroup, PlaygroundMenu->DifficultyTexture[Index], TextRectangle);
                
                if (Index == (s32)PlaygroundMenu->DiffMode)
                {
                    v2 CornerDim = V2(150.0f * 0.4f, 150.0f * 0.4f);
                    DrawCornersOnButton(RenderGroup, PlaygroundMenu->LevelCornerTexture, DifficultyRectangle, CornerDim);
                }
                
                if (!PlaygroundMenu->AnimationFinished) {
                    r32 AlphaChannel = (1.0f - PlaygroundMenu->InterpPoint) * 255.0f;
                    PushRectangle(RenderGroup, DifficultyRectangle, V4(51.0f, 8.0f, 23.0f, AlphaChannel));
                }
            }
            
            u32 InitialLevelIndex = 0;
            if (PlaygroundMenu->DiffMode == difficulty::MEDIUM)
                InitialLevelIndex = 32;
            else if (PlaygroundMenu->DiffMode == difficulty::HARD)
                InitialLevelIndex = 64;
            
            v2 ButtonDim     = V2(150.0f, 86.0f);
            v2 LevelPosition = V2(VIRTUAL_GAME_WIDTH * 0.5f - (((ColumnAmount * 150.0f) + 30.0f) * 0.5f), 
                                  VIRTUAL_GAME_HEIGHT * 0.5f - ((RowAmount * 100.0f) * 0.5f) + (VIRTUAL_GAME_HEIGHT * 0.2f));
            
            rectangle2 WholeButtonArea = {};
            WholeButtonArea.Min = LevelPosition;
            SetDim(&WholeButtonArea, (ColumnAmount * 150.0f) + 30.0f + 10.0f, (RowAmount * 100.0f));
            PushBitmap(RenderGroup, PlaygroundMenu->LevelsCanvasTexture, WholeButtonArea);
            
            r32 AlphaChannel = (1.0f - PlaygroundMenu->InterpPoint) * 255.0f;
            if (PlaygroundMenu->AnimationFinished) {
                r32 InterpPoint = PlaygroundMenu->InterpPointDiff;
                if (PlaygroundMenu->InterpPointDiff >= 0.5f) {
                    InterpPoint -= 0.5f;
                    if (!PlaygroundMenu->AnimationFinishedHalf) {
                        PlaygroundMenu->AnimationFinishedHalf = true;
                        UpdateTextureForLevels(PlaygroundMenu, PlayerData, RenderGroup);
                    }
                }
                
                InterpPoint *= 2.0f;
                AlphaChannel = Lerp1(0.0f, 255.0f, PlaygroundMenu->InterpPointDiff >= 0.5f ? (1.0f - InterpPoint) : InterpPoint);
                
                PushRectangle(RenderGroup, WholeButtonArea, V4(51.0f, 8.0f, 23.0f, AlphaChannel));
            }
            
            if (PlaygroundMenu->AnimationFinished && PlaygroundMenu->InterpPointDiff >= 1.0f) {
                if (PlaygroundMenu->ForwardAnimation) {
                    PlaygroundMenu->InterpPointNext += Input->dtForFrame;
                    if (PlaygroundMenu->InterpPointNext >= 1.0f) {
                        PlaygroundMenu->InterpPointNext = 1.0f;
                        PlaygroundMenu->ForwardAnimation = false;
                    }
                }
                else {
                    PlaygroundMenu->InterpPointNext -= Input->dtForFrame;
                    if (PlaygroundMenu->InterpPointNext <= 0.0f) {
                        PlaygroundMenu->InterpPointNext = 0.0f;
                        PlaygroundMenu->ForwardAnimation = true;
                    }
                }
                
                for (u32 Index = InitialLevelIndex + 1; Index <= InitialLevelIndex + 32; ++Index) {
                    if (!PlayerData->PlaygroundUnlocked[Index]) {
                        u32 NextLevelIndex = Index - 1;
                        if (PlayerData->PlaygroundUnlocked[NextLevelIndex]) {
                            u32 Row    = (NextLevelIndex % 32) / 8;
                            u32 Column = (NextLevelIndex % 32) % 8;
                            
                            v2 IndicatorSize = V2(50.0f, 50.0f);
                            v2 ColorDim = V2(30.0f, 100.0f);
                            
                            rectangle2 ButtonRectangle;
                            ButtonRectangle.Min.y = WholeButtonArea.Min.y + (ColorDim.h * Row);
                            ButtonRectangle.Min.x = WholeButtonArea.Min.x + (30.0f) + (150.0f * Column) + 10.0f;
                            ButtonRectangle.Max.x = ButtonRectangle.Min.x + 150.0f;
                            ButtonRectangle.Max.y = ButtonRectangle.Min.y + 100.0f;
                            
                            rectangle2 LevelRectangle;
                            LevelRectangle.Min.x = ButtonRectangle.Min.x + (150.0f * 0.5f) - (150.0f * 0.5f);
                            LevelRectangle.Min.y = ButtonRectangle.Min.y + (100.0f * 0.5f) - (86.0f * 0.5f);
                            SetDim(&LevelRectangle, V2(150.0f, 86.0f));
                            
                            
                            v2 ButtonDim  = V2(150.0f, 86.0f);
                            rectangle2 Rectangle = {};
                            Rectangle.Min = LevelRectangle.Min + (ButtonDim * 0.5f) - (IndicatorSize * 0.5f);
                            Rectangle.Max = Rectangle.Min + IndicatorSize;
                            
                            AlphaChannel = (1.0f - PlaygroundMenu->InterpPointNext) * 255.0f;
                            PushRectangle(RenderGroup, Rectangle, V4(79.0f, 21.0f, 47.0f, AlphaChannel));
                            break;
                        }
                    }
                }
            }
            
            
            /* Back Button */
            rectangle2 ButtonRectangle = {};
            ButtonRectangle.Min.x = 370.0f;
            ButtonRectangle.Min.y = VIRTUAL_GAME_HEIGHT - (120.0f);
            SetDim(&ButtonRectangle, 200.0f, 80.0f);
            
            rectangle2 TextRectangle = GetTextOnTheCenterOfRectangle(ButtonRectangle, PlaygroundMenu->BackTexture);
            
            rectangle2 TextShadowRectangle = TextRectangle;
            TextShadowRectangle.Min += 5.0f;
            TextShadowRectangle.Max += 5.0f;
            
            PushFontBitmap(RenderGroup, PlaygroundMenu->BackShadowTexture, TextShadowRectangle);
            PushFontBitmap(RenderGroup, PlaygroundMenu->BackTexture, TextRectangle);
            
            if (!PlaygroundMenu->AnimationFinished) {
                
                AlphaChannel = (1.0f - PlaygroundMenu->InterpPoint) * 255;
                PushRectangle(RenderGroup, WholeButtonArea, V4(51.0f, 8.0f, 23.0f, AlphaChannel));
                PushRectangle(RenderGroup, ButtonRectangle, V4(51.0f, 8.0f, 23.0f, AlphaChannel));
                
                rectangle2 ClipRectangle = {};
                ClipRectangle.Min = V2(0.0f, 0.0f);
                ClipRectangle.Max = V2(FinalBackgroundRectangle.Min.x, FinalBackgroundRectangle.Max.y);
                PushRectangle(RenderGroup, ClipRectangle, V4(0.0f, 0.0f, 0.0f, 255.0f));
                
                ClipRectangle.Min = V2(FinalBackgroundRectangle.Max.x, 0.0f);
                ClipRectangle.Max = V2(VIRTUAL_GAME_WIDTH, FinalBackgroundRectangle.Max.y);
                PushRectangle(RenderGroup, ClipRectangle, V4(0.0f, 0.0f, 0.0f, 255.0f));
            }
            
            
            if (PlaygroundMenu->ButtonIndex == 3)
            {
                /* Hightlight Line*/
                v2 LineDim = {200.0f, 33.0f};
                HighlightButtonWithLine(RenderGroup, PlaygroundMenu->HorizontalLineTexture, 
                                        ButtonRectangle, LineDim);
            }
        } break;
    }
    
    if (!PlaygroundMenu->AnimationFinished && PlaygroundMenu->InterpPoint >= 1.0f)  PlaygroundMenu->AnimationFinished = true;
    
    return (Result);
}