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
    
    Result.Min.x = Rectangle.Min.x + (RectangleDim.w * 0.5f) - (TextureDim.w * 0.5f);
    Result.Min.y = Rectangle.Min.y + (RectangleDim.h * 0.5f) - (TextureDim.h * 0.5f);
    SetDim(&Result, TextureDim);
    
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
}


static menu_result_option
PlaygroundMenuUpdateAndRender(playground_menu *PlaygroundMenu, 
                              playground_data *PlaygroundData, 
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
            case DIFFICULTY_PAGE:
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
                        if (Page == menu_page::QUIT_PAGE)
                        {
                            Result.QuitGame = true;
                        }
                        
                        PlaygroundMenu->MenuPage    = Page;
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
                        PlaygroundMenu->MenuPage = MAIN_PAGE;
                    }
                }
            }
            
        } break;
        
        case DIFFICULTY_PAGE:
        {
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
                        PlaygroundMenu->DiffMode = DiffMode;
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
                            if (PlaygroundData[PlaygroundIndex].IsUnlocked)
                            {
                                Result.SwitchToPlayground = true;
                                Result.PlaygroundIndex = PlaygroundIndex;
                                printf("LevelIndex: %d\n", Result.PlaygroundIndex);
                                
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
                    PlaygroundMenu->ButtonIndex = 0;
                    PlaygroundMenu->MenuPage = MAIN_PAGE;
                }
            }
            
        } break;
    }
    
    //
    // NOTE(msokolov): Rendering
    //
    
    //Clear(RenderGroup, {42, 6, 21, 255});
    ClearScreen(RenderGroup, {0, 0, 0, 255});
    //Clear(RenderGroup, {51, 8, 23, 255});
    
    switch(PlaygroundMenu->MenuPage)
    {
        case MAIN_PAGE:
        {
            rectangle2 BackgroundRectangle = {};
            BackgroundRectangle.Min.x = 710.0f;
            BackgroundRectangle.Min.y = 0.0f;
            BackgroundRectangle.Max.x = 1210.0f;
            BackgroundRectangle.Max.y = VIRTUAL_GAME_HEIGHT;
            PushRectangle(RenderGroup, BackgroundRectangle, {51, 8, 23, 255});
            
            v2 CornerDim = {200.0f, 200.0f};
            DrawCornersOnButton(RenderGroup, PlaygroundMenu->CornerTexture, BackgroundRectangle, CornerDim);
            
            rectangle2 ButtonRectangle = {};
            for (u32 Index = 0;
                 Index < 3;
                 ++Index)
            {
                ButtonRectangle.Min.x = MainMenuPosition.x;
                ButtonRectangle.Min.y = MainMenuPosition.y + (Index * MainMenuButtonSize.h);
                SetDim(&ButtonRectangle, MainMenuButtonSize.w, MainMenuButtonSize.h - 10.0f);
                
                v4 Color = {128, 128, 128, 255};
                
                rectangle2 TextRectangle = {};
                v2 Dim = QueryTextureDim(PlaygroundMenu->MainMenuTexture[Index]);
                TextRectangle.Min.x = ButtonRectangle.Min.x + (GetDim(ButtonRectangle).w / 2.0f) - (Dim.w / 2.0f);
                TextRectangle.Min.y = ButtonRectangle.Min.y + (GetDim(ButtonRectangle).h / 2.0f) - (Dim.h / 2.0f);
                SetDim(&TextRectangle, Dim);
                
                rectangle2 ShadowRectangle = TextRectangle;
                ShadowRectangle.Min += 5.0f;
                ShadowRectangle.Max += 5.0f;
                
                PushBitmap(RenderGroup, PlaygroundMenu->MainMenuShadowTexture[Index], ShadowRectangle);
                PushBitmap(RenderGroup, PlaygroundMenu->MainMenuTexture[Index], TextRectangle);
                
                if (PlaygroundMenu->ButtonIndex == Index)
                {
                    Dim = QueryTextureDim(PlaygroundMenu->HorizontalLineTexture);
                    
                    v2 LineDim = {200.0f, 33.0f};
                    HighlightButtonWithLine(RenderGroup, PlaygroundMenu->HorizontalLineTexture, 
                                            ButtonRectangle, LineDim);
                }
            }
            
        } break;
        
        case SETTINGS_PAGE:
        {
            // NOTE(msokolov): Background
            rectangle2 BackgroundRectangle = {};
            BackgroundRectangle.Min = V2(610.0f, 0.0f);
            BackgroundRectangle.Max = V2(1310.0f, VIRTUAL_GAME_HEIGHT);
            PushRectangle(RenderGroup, BackgroundRectangle, {51, 8, 23, 255});
            
            v2 CornerDim = {200.0f, 200.0f};
            DrawCornersOnButton(RenderGroup, PlaygroundMenu->CornerTexture, BackgroundRectangle, CornerDim);
            
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
                //PushRectangleOutline(RenderGroup, OptionRectangle, V4(128, 128, 128, 255));
                
                ValueOneRectangle.Min.x = SettingsButtonPosition.x + (SettingsButtonSize.w);
                ValueOneRectangle.Min.y = SettingsButtonPosition.y + (Index * (SettingsButtonSize.h));
                SetDim(&ValueOneRectangle, ValueButtonSize);
                //PushRectangleOutline(RenderGroup, ValueOneRectangle, V4(128, 128, 128, 255));
                
                ValueTwoRectangle.Min.x = ValueOneRectangle.Min.x + ValueButtonSize.w;
                ValueTwoRectangle.Min.y = ValueOneRectangle.Min.y;
                SetDim(&ValueTwoRectangle, ValueButtonSize);
                //PushRectangleOutline(RenderGroup, ValueTwoRectangle, V4(128, 128, 128, 255));
                
                // NOTE(msokolov): Sound settings
                if (Index == 0)
                {
                    TextRectangle = GetTextOnTheCenterOfRectangle(OptionRectangle, PlaygroundMenu->SoundNameTexture);
                    
                    TextShadowRectangle = TextRectangle;
                    TextShadowRectangle.Min += 5.0f;
                    TextShadowRectangle.Max += 5.0f;
                    
                    PushBitmap(RenderGroup, PlaygroundMenu->SoundNameShadowTexture, TextShadowRectangle);
                    PushBitmap(RenderGroup, PlaygroundMenu->SoundNameTexture, TextRectangle);
                    
                    // NOTE(msokolov): On/Off for sound
                    
                    // NOTE(msokolov): On value
                    rectangle2 OnOffRectangle = GetTextOnTheCenterOfRectangle(ValueOneRectangle, PlaygroundMenu->SoundOnTexture);
                    rectangle2 OnOffShadowRectangle = OnOffRectangle;
                    OnOffShadowRectangle.Min += 5.0f;
                    OnOffShadowRectangle.Max += 5.0f;
                    PushBitmap(RenderGroup, PlaygroundMenu->SoundOnShadowTexture, OnOffShadowRectangle);
                    PushBitmap(RenderGroup, PlaygroundMenu->SoundOnTexture, OnOffRectangle);
                    
                    // NOTE(msokolov): Off value
                    OnOffRectangle = GetTextOnTheCenterOfRectangle(ValueTwoRectangle, PlaygroundMenu->SoundOffTexture);
                    OnOffShadowRectangle = OnOffRectangle;
                    OnOffShadowRectangle.Min += 5.0f;
                    OnOffShadowRectangle.Max += 5.0f;
                    
                    PushBitmap(RenderGroup, PlaygroundMenu->SoundOffShadowTexture, OnOffShadowRectangle);
                    PushBitmap(RenderGroup, PlaygroundMenu->SoundOffTexture, OnOffRectangle);
                    
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
                    
                    PushBitmap(RenderGroup, PlaygroundMenu->MusicNameShadowTexture, TextShadowRectangle);
                    PushBitmap(RenderGroup, PlaygroundMenu->MusicNameTexture, TextRectangle);
                    
                    // NOTE(msokolov): On/Off for sound
                    
                    // NOTE(msokolov): On value
                    rectangle2 OnOffRectangle = GetTextOnTheCenterOfRectangle(ValueOneRectangle, PlaygroundMenu->SoundOnTexture);
                    rectangle2 OnOffShadowRectangle = OnOffRectangle;
                    OnOffShadowRectangle.Min += 5.0f;
                    OnOffShadowRectangle.Max += 5.0f;
                    PushBitmap(RenderGroup, PlaygroundMenu->SoundOnShadowTexture, OnOffShadowRectangle);
                    PushBitmap(RenderGroup, PlaygroundMenu->SoundOnTexture, OnOffRectangle);
                    
                    
                    // NOTE(msokolov): Off value
                    OnOffRectangle = GetTextOnTheCenterOfRectangle(ValueTwoRectangle, PlaygroundMenu->SoundOffTexture);
                    OnOffShadowRectangle = OnOffRectangle;
                    OnOffShadowRectangle.Min += 5.0f;
                    OnOffShadowRectangle.Max += 5.0f;
                    
                    PushBitmap(RenderGroup, PlaygroundMenu->SoundOffShadowTexture, OnOffShadowRectangle);
                    PushBitmap(RenderGroup, PlaygroundMenu->SoundOffTexture, OnOffRectangle);
                    
                    v2 LineDim = {130.0f, 21.45f};
                    HighlightButtonWithLine(RenderGroup, PlaygroundMenu->HorizontalLineTexture, 
                                            PlaygroundMenu->MusicOn ? ValueOneRectangle : ValueTwoRectangle, LineDim);
                }
            }
            
            /* Back Button */
            rectangle2 ButtonRectangle = {};
            ButtonRectangle.Min.x = VIRTUAL_GAME_WIDTH * 0.5f - (ValueButtonSize.w * 0.5f);
            ButtonRectangle.Min.y = VIRTUAL_GAME_HEIGHT - (ValueButtonSize.h + 50.0f);
            SetDim(&ButtonRectangle, ValueButtonSize);
            //PushRectangleOutline(RenderGroup, ButtonRectangle, V4(128, 128, 128, 255));
            
            TextRectangle = GetTextOnTheCenterOfRectangle(ButtonRectangle, PlaygroundMenu->BackTexture);
            
            TextShadowRectangle = TextRectangle;
            TextShadowRectangle.Min += 5.0f;
            TextShadowRectangle.Max += 5.0f;
            
            PushBitmap(RenderGroup, PlaygroundMenu->BackShadowTexture, TextShadowRectangle);
            PushBitmap(RenderGroup, PlaygroundMenu->BackTexture, TextRectangle);
            
            if (PlaygroundMenu->ButtonIndex == 4)
            {
                v2 LineDim = {200.0f, 33.0f};
                HighlightButtonWithLine(RenderGroup, PlaygroundMenu->HorizontalLineTexture, 
                                        ButtonRectangle, LineDim);
            }
        } break;
        
        case DIFFICULTY_PAGE:
        {
            rectangle2 BackgroundRectangle = {};
            BackgroundRectangle.Min = V2(300.0f, 0.0f);
            BackgroundRectangle.Max = V2(1620.0f, VIRTUAL_GAME_HEIGHT);
            PushRectangle(RenderGroup, BackgroundRectangle, {51, 8, 23, 255});
            
            v2 CornerDim = {200.0f, 200.0f};
            DrawCornersOnButton(RenderGroup, PlaygroundMenu->CornerTexture, BackgroundRectangle, CornerDim);
            
            rectangle2 DifficultyRectangle = {};
            for (u32 Index = 0;
                 Index < 3;
                 ++Index)
            {
                DifficultyRectangle.Min.x = DifficultyPosition.x;
                DifficultyRectangle.Min.y = DifficultyPosition.y + ((Index * DifficultyButtonSize.h));
                SetDim(&DifficultyRectangle, 400, 150);
                
                rectangle2 TextRectangle = GetTextOnTheCenterOfRectangle(DifficultyRectangle, PlaygroundMenu->DifficultyTexture[Index]);
                
                rectangle2 TextShadowRectangle = TextRectangle;
                TextShadowRectangle.Min += 5.0f;
                TextShadowRectangle.Max += 5.0f;
                
                PushBitmap(RenderGroup, PlaygroundMenu->DifficultyShadowTexture[Index], TextShadowRectangle);
                PushBitmap(RenderGroup, PlaygroundMenu->DifficultyTexture[Index], TextRectangle);
                
                if (Index == PlaygroundMenu->ButtonIndex)
                {
                    /* Hightlight Line*/
                    v2 LineDim = {200.0f, 33.0f};
                    HighlightButtonWithLine(RenderGroup, PlaygroundMenu->HorizontalLineTexture, 
                                            DifficultyRectangle, LineDim);
                }
                
                if (Index == (s32)PlaygroundMenu->DiffMode)
                {
                    v2 CornerDim = V2(150.0f * 0.4f, 150.0f * 0.4f);
                    DrawCornersOnButton(RenderGroup, PlaygroundMenu->LevelCornerTexture, DifficultyRectangle, CornerDim);
                }
            }
            
            u32 InitialLevelIndex = 0;
            if (PlaygroundMenu->DiffMode == difficulty::MEDIUM)
                InitialLevelIndex = 32;
            else if (PlaygroundMenu->DiffMode == difficulty::HARD)
                InitialLevelIndex = 64;
            
            v2 LevelPosition = {};
            LevelPosition.x = VIRTUAL_GAME_WIDTH * 0.5f - (((ColumnAmount * 150.0f) + 30.0f) * 0.5f);
            LevelPosition.y = VIRTUAL_GAME_HEIGHT * 0.5f - ((RowAmount * 100.0f) * 0.5f) + (VIRTUAL_GAME_HEIGHT * 0.2f);
            
            v2 ButtonDim = V2(150.0f, 86.0f);
            
            rectangle2 ButtonRectangle = {};
            for (u32 Row = 0; Row < RowAmount; ++Row)
            {
                rectangle2 ColorRectangle = {};
                ColorRectangle.Min.x = LevelPosition.x;
                ColorRectangle.Min.y = LevelPosition.y + (MenuButtonSize.h * Row);
                SetDim(&ColorRectangle, 30.0f, MenuButtonSize.h);
                
                PushBitmap(RenderGroup, PlaygroundMenu->ColorBarTexture[Row], ColorRectangle);
                
                ButtonRectangle.Min.y = LevelPosition.y + (MenuButtonSize.h * Row);
                
                for (u32 Col = 0; Col < ColumnAmount; ++Col)
                {
                    ButtonRectangle.Min.x = LevelPosition.x + (GetDim(ColorRectangle).w) + (150.0f * Col) + 10.0f;
                    
                    ButtonRectangle.Max.x = ButtonRectangle.Min.x + 150.0f;
                    ButtonRectangle.Max.y = ButtonRectangle.Min.y + 100.0f;
                    
                    u32 LevelIndex = InitialLevelIndex + (Row * ColumnAmount) + Col;
                    b32 IsUnlocked = PlaygroundData[LevelIndex].IsUnlocked;
                    
                    rectangle2 LevelRectangle = {}; 
                    if (IsUnlocked)
                    {
                        LevelRectangle.Min.x = ButtonRectangle.Min.x + (GetDim(ButtonRectangle).w * 0.5f) - (ButtonDim.w * 0.5f);
                        LevelRectangle.Min.y = ButtonRectangle.Min.y + (GetDim(ButtonRectangle).h * 0.5f) - (ButtonDim.h * 0.5f);
                        SetDim(&LevelRectangle, ButtonDim);
                        PushBitmap(RenderGroup, PlaygroundMenu->SquareFrameUnlocked, LevelRectangle);
                        
                        LevelRectangle = GetTextOnTheCenterOfRectangle(ButtonRectangle, PlaygroundMenu->LevelNumberTexture[LevelIndex]);
                        
                        LevelRectangle.Min += V2(5.0f, 5.0f);
                        LevelRectangle.Max += V2(5.0f, 5.0f);
                        
                        PushBitmap(RenderGroup, PlaygroundMenu->LevelNumberShadowTexture[LevelIndex], LevelRectangle);
                        
                        LevelRectangle.Min -= V2(5.0f, 5.0f);
                        LevelRectangle.Max -= V2(5.0f, 5.0f);
                        
                        PushBitmap(RenderGroup, PlaygroundMenu->LevelNumberTexture[LevelIndex], LevelRectangle);
                    }
                    else
                    {
                        LevelRectangle.Min.x = ButtonRectangle.Min.x + (GetDim(ButtonRectangle).w * 0.5f) - (ButtonDim.w * 0.5f);
                        LevelRectangle.Min.y = ButtonRectangle.Min.y + (GetDim(ButtonRectangle).h * 0.5f) - (ButtonDim.h * 0.5f);
                        SetDim(&LevelRectangle, ButtonDim);
                        PushBitmap(RenderGroup, PlaygroundMenu->SquareFrameLocked, LevelRectangle);
                    }
                }
            }
            
            /* Back Button */
            ButtonRectangle.Min.x = 350.0f;
            ButtonRectangle.Min.y = VIRTUAL_GAME_HEIGHT - (120.0f);
            SetDim(&ButtonRectangle, 200.0f, 80.0f);
            
            rectangle2 TextRectangle = GetTextOnTheCenterOfRectangle(ButtonRectangle, PlaygroundMenu->BackTexture);
            
            rectangle2 TextShadowRectangle = TextRectangle;
            TextShadowRectangle.Min += 5.0f;
            TextShadowRectangle.Max += 5.0f;
            
            PushBitmap(RenderGroup, PlaygroundMenu->BackShadowTexture, TextShadowRectangle);
            PushBitmap(RenderGroup, PlaygroundMenu->BackTexture, TextRectangle);
            
            //PushRectangleOutline(RenderGroup, ButtonRectangle, V4(255, 255, 255, 255));
            
            if (PlaygroundMenu->ButtonIndex == 3)
            {
                /* Hightlight Line*/
                v2 LineDim = {200.0f, 33.0f};
                HighlightButtonWithLine(RenderGroup, PlaygroundMenu->HorizontalLineTexture, 
                                        ButtonRectangle, LineDim);
            }
        } break;
    }
    
    return (Result);
}