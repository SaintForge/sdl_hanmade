/* ========================================= */
//     $File: tetroman_menu.cpp
//     $Date: October 10th 2017 10:32 pm 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */

static u32
PlaygroundMenuUpdateAndRender(playground_menu *PlaygroundMenu, game_input *Input, render_group *RenderGroup)
{
    s32 Result = -1;
    
    //r32 MenuButtonSize = 100.0f;
    v2 MenuButtonSize = {};
    MenuButtonSize.w  = 200.0f;
    MenuButtonSize.h  = 100.0f;
    
    u32 RowAmount = 4;
    u32 ColumnAmount = 8;
    
    v2 InitialPosition = {};
    InitialPosition.x = LOGICAL_GAME_WIDTH / 2.0f - ((ColumnAmount * MenuButtonSize.w) / 2.0f) - ((10.0f * (ColumnAmount - 1)) / 2.0f);
    InitialPosition.y = LOGICAL_GAME_HEIGHT / 2.0f - ((RowAmount * MenuButtonSize.h) / 2.0f) + (LOGICAL_GAME_HEIGHT / 4.0f) - ((10.0f * RowAmount) / 2.0f);
    
    if (Input->MouseButtons[0].EndedDown)
    {
        v2 MousePos = {};
        MousePos.x = Input->MouseX;
        MousePos.y = Input->MouseY;
        
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
                    Result = (Row * ColumnAmount) + Column;
                    printf("LevelIndex: %d\n", Result);
                    
                    FoundLevel = true;
                    break;
                }
            }
            
            if (FoundLevel) break;
        }
    }
    
    
    //v4 Color = {0, 0, 0, 255};
    //PushRectangle(RenderGroup, {0.0f, 0.0f, LOGICAL_GAME_WIDTH, LOGICAL_GAME_HEIGHT}, Color);
    Clear(RenderGroup, {42, 6, 21, 255});
    
    rectangle2 ButtonRectangle = {};
    for (u32 Row = 0; Row < RowAmount; ++Row)
    {
        ButtonRectangle.Min.y = InitialPosition.y + (MenuButtonSize.h * Row);
        
        for (u32 Col = 0; Col < ColumnAmount; ++Col)
        {
            ButtonRectangle.Min.x = InitialPosition.x + (MenuButtonSize.w * Col);
            
            ButtonRectangle.Max.x = ButtonRectangle.Min.x + MenuButtonSize.w - 10.0f;
            ButtonRectangle.Max.y = ButtonRectangle.Min.y + MenuButtonSize.h - 10.0f;
            
            //PushBitmap(RenderGroup, PlaygroundMenu->LevelButtonTexture, ButtonRectangle);
            v4 Color = {128, 128, 128, 255};
            PushRectangle(RenderGroup, ButtonRectangle, Color);
            Color = {255, 255, 255, 255};
            PushRectangleOutline(RenderGroup, ButtonRectangle, Color);
            
            u32 LevelIndex = (Row * ColumnAmount) + Col;
            
            rectangle2 LevelRectangle = {};
            v2 Dim = QueryTextureDim(PlaygroundMenu->LevelNumberTexture[LevelIndex]);
            LevelRectangle.Min.x = ButtonRectangle.Min.x + (GetDim(ButtonRectangle).w / 2.0f) - (Dim.w / 2.0f);
            LevelRectangle.Min.y = ButtonRectangle.Min.y + (GetDim(ButtonRectangle).h / 2.0f) - (Dim.h / 2.0f);
            SetDim(&LevelRectangle, Dim);
            
            PushBitmap(RenderGroup, PlaygroundMenu->LevelNumberTexture[LevelIndex], LevelRectangle);
            
            
        }
    }
    
    
    return (Result);
}

#if 0
static void
MenuMakeTextButton(const char* Text, s32 X, s32 Y, s32 Width, s32 Height,
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
MenuEntityAlignButtons(menu_entity *MenuEntity, 
                       u32 ScreenWidth, 
                       u32 ScreenHeight)
{
    u32 ButtonsPerRow       = 4;
    u32 ButtonsPerColumn    = 5;
    u32 SpaceBetweenButtons = MenuEntity->SpaceBetweenButtons;
    
    s32 XOffset = 0;
    s32 YOffset = 0;
    
    s32 XPosition = 0;
    s32 YPosition = 0;
    
    u32 ButtonWidth  = MenuEntity->ButtonSizeWidth;
    u32 ButtonHeight = MenuEntity->ButtonSizeHeight;
    
    s32 StartX = (ScreenWidth / 2)  - (((ButtonWidth * ButtonsPerRow) + ((ButtonsPerRow - 1) * SpaceBetweenButtons)) / 2);
    s32 StartY = (ScreenHeight / 2) - ((ButtonHeight * ButtonsPerColumn) / 2)- ((ButtonsPerColumn * SpaceBetweenButtons) / 2);
    
    for(u32 i = 0; i < 5; ++i)
    {
        MenuEntity->ButtonsArea[i].x = StartX + (i * ScreenWidth);
        MenuEntity->ButtonsArea[i].y = StartY;
    }
    
    for(u32 i = 0; i < MenuEntity->ButtonsAmountReserved; ++i)
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
        MenuEntity->Buttons[i].ButtonQuad.w = ButtonWidth;
        MenuEntity->Buttons[i].ButtonQuad.h = ButtonHeight;
        MenuEntity->Buttons[i].ButtonQuad.x = XPosition;
        MenuEntity->Buttons[i].ButtonQuad.y = YPosition;
        
        MenuEntity->Buttons[i].LevelNumberTextureQuad.x = XPosition + (MenuEntity->Buttons[i].ButtonQuad.w / 2) - (MenuEntity->Buttons[i].LevelNumberTextureQuad.w / 2);
        MenuEntity->Buttons[i].LevelNumberTextureQuad.y = YPosition + (MenuEntity->Buttons[i].ButtonQuad.h / 2) - (MenuEntity->Buttons[i].LevelNumberTextureQuad.h / 2);
    }
}

static void
MenuEntityUpdatePositionsLandscape(game_offscreen_buffer *Buffer, menu_entity *MenuEntity, game_memory *Memory)
{
    s32 ActualWidth  = Buffer->Width;
    s32 ActualHeight = Buffer->Height;
    
    s32 ReferenceWidth  = Buffer->ReferenceWidth;
    s32 ReferenceHeight = Buffer->ReferenceHeight;
    
    r32 ScaleByWidth = GetScale(ActualWidth, ActualHeight, ReferenceWidth, ReferenceHeight, 0.0f);
    
    r32 ScaleByHeight = GetScale(ActualWidth, ActualHeight, ReferenceWidth, ReferenceHeight, 1.0f);
    
    r32 ScaleByAll = GetScale(ActualWidth, ActualHeight, ReferenceWidth, ReferenceHeight, 0.5f);
    
    // Font for the level number buttons
    {
        s32 FontSize = 50;
        FontSize = roundf((r32)FontSize * ScaleByHeight);
        
        if(MenuEntity->LevelNumberFont)
        {
            TTF_CloseFont(MenuEntity->LevelNumberFont);
        }
        
        MenuEntity->LevelNumberFont = TTF_OpenFont(FontPath, FontSize);
        Assert(MenuEntity->LevelNumberFont);
    }
    
    // Menu level buttons area
    
    {
        
        s32 ButtonsPerRow = 4;
        s32 ButtonsPerColumn = 5;
        
        s32 SpaceBetweenButtons = 10;
        s32 ButtonSize = 100;
        
        SpaceBetweenButtons = roundf((r32)SpaceBetweenButtons * ScaleByWidth);
        ButtonSize = roundf((r32)ButtonSize * ScaleByHeight);
        
        game_rect ButtonsArea = {};
        ButtonsArea.w = (ButtonSize * ButtonsPerRow) + (SpaceBetweenButtons * (ButtonsPerRow - 1));
        ButtonsArea.h = (ButtonSize * ButtonsPerColumn) + (SpaceBetweenButtons * (ButtonsPerColumn - 1));
        
        MenuEntity->ButtonSizeWidth = ButtonSize;
        MenuEntity->ButtonSizeHeight = ButtonSize;
        MenuEntity->SpaceBetweenButtons = SpaceBetweenButtons;
        
        for(s32 i = 0; i < 5; ++i )
        {
            MenuEntity->ButtonsArea[i].w = ButtonsArea.w;
            MenuEntity->ButtonsArea[i].h = ButtonsArea.h;
        }
        
        //MenuLoadButtonsFromMemory(MenuEntity, Memory, Buffer);
        MenuEntityAlignButtons(MenuEntity, Buffer->Width, Buffer->Height);
    }
    
}

static void
MenuEntityUpdatePositionsPortrait(game_offscreen_buffer *Buffer, menu_entity *MenuEntity, game_memory *Memory)
{
    s32 ActualWidth  = Buffer->Width;
    s32 ActualHeight = Buffer->Height;
    
    s32 ReferenceWidth  = Buffer->ReferenceWidth;
    s32 ReferenceHeight = Buffer->ReferenceHeight;
    
    r32 ScaleByWidth = GetScale(ActualWidth, ActualHeight, 600, 800, 0.0f);
    
    r32 ScaleByHeight = GetScale(ActualWidth, ActualHeight, 600, 800, 1.0f);
    
    r32 ScaleByAll = GetScale(ActualWidth, ActualHeight, 600, 800, 0.5f);
    
    // Font for the level number buttons
    {
        s32 FontSize = 50;
        FontSize = roundf((r32)FontSize * ScaleByWidth);
        
        if(MenuEntity->LevelNumberFont)
        {
            TTF_CloseFont(MenuEntity->LevelNumberFont);
        }
        
        MenuEntity->LevelNumberFont = TTF_OpenFont(FontPath, FontSize);
        Assert(MenuEntity->LevelNumberFont);
        
    }
    
    
    // Menu level buttons area
    {
        s32 ButtonsPerRow = 4;
        s32 ButtonsPerColumn = 5;
        
        s32 SpaceBetweenButtons = 10;
        s32 ButtonSize = 100;
        
        SpaceBetweenButtons = roundf((r32)SpaceBetweenButtons * ScaleByWidth);
        ButtonSize = roundf((r32)ButtonSize * ScaleByWidth);
        
        game_rect ButtonsArea = {};
        ButtonsArea.w = (ButtonSize * ButtonsPerRow) + (SpaceBetweenButtons * (ButtonsPerRow - 1));
        ButtonsArea.h = (ButtonSize * ButtonsPerColumn) + (SpaceBetweenButtons * (ButtonsPerColumn - 1));
        
        MenuEntity->ButtonSizeWidth = ButtonSize;
        MenuEntity->ButtonSizeHeight = ButtonSize;
        MenuEntity->SpaceBetweenButtons = SpaceBetweenButtons;
        
        for(s32 i = 0; i < 5; ++i )
        {
            MenuEntity->ButtonsArea[i] = ButtonsArea;
        }
        
        MenuEntityAlignButtons(MenuEntity, ActualWidth, ActualHeight);
    }
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
MenuUpdateAndRender(game_state *GameState, menu_entity *MenuEntity, game_memory *Memory, 
                    game_input *Input, game_offscreen_buffer *Buffer)
{
    if(!MenuEntity->IsPaused)
    {
        /* Mouse Left button pressed */
        if(Input->MouseButtons[0].EndedDown)
        {
            MenuEntity->IsMoving    = true;
            MenuEntity->IsAnimating = false;
            
            MenuEntity->ScrollingTicks = SDL_GetTicks();
            
            MenuEntity->ButtonIndex   = -1;
            MenuEntity->MouseOffsetX  = 0;
            MenuEntity->MouseOffsetY  = 0;
            MenuEntity->Velocity.x    = 0;
            
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
            
        }
        /* Mouse Left button relased */
        else if(Input->MouseButtons[0].EndedUp)
        {
            MenuEntity->IsMoving    = false;
            MenuEntity->IsAnimating = true;
            MenuEntity->ScrollingTicks = SDL_GetTicks() - MenuEntity->ScrollingTicks;
            MenuEntity->MaxVelocity = 20.0f;
            
            u32 ButtonsAreaAmount = (MenuEntity->ButtonsAmount / 20) + 1;
            
            /* If mouse was moved far enough from the selecting the level  */
            if(abs(MenuEntity->MouseOffsetX) >= MenuEntity->ButtonSizeWidth * 0.5f)
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
                        s32 TargetOffset = MenuEntity->MouseOffsetX;
                        if(abs(TargetOffset) < MenuEntity->ButtonsArea[0].w)
                        {
                            TargetOffset = TargetOffset > 0
                                ? Buffer->Width
                                : -Buffer->Width;
                            
                            MenuEntity->MaxVelocity   *= 2;
                            MenuEntity->MouseOffsetX = TargetOffset;
                            MenuEntity->ButtonIndex    = -1;
                        }
                    }
                }
                
                if(MenuEntity->MouseOffsetX > 0)
                {
                    Center_x = MenuEntity->ButtonsArea[MenuEntity->TargetIndex].x + (MenuEntity->ButtonsArea[MenuEntity->TargetIndex].w * 0.2f);
                }
                else
                {
                    Center_x = MenuEntity->ButtonsArea[MenuEntity->TargetIndex].x + (MenuEntity->ButtonsArea[MenuEntity->TargetIndex].w * 0.8f);
                }
                
                CenterOffset = Center_x - MenuEntity->MouseOffsetX;
                
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
                
                MenuEntity->MouseOffsetX = Buffer->Width / 2;
            }
            /* When a player clicked on the level button */
            else
            {
                MenuEntity->MouseOffsetX = Buffer->Width / 2;
                
                s32 Index = MenuEntity->ButtonIndex;
                if(Index >= 0)
                {
                    //LevelEntityUpdateLevelEntityFromMemory(Memory, Index, false,Buffer);
                    
                    GameState->CurrentMode = LEVEL;
                    //Memory->CurrentLevelIndex = Index;
                    
                    MenuEntity->MouseOffsetX = 0;
                    MenuEntity->MouseOffsetY = 0;
                    MenuEntity->IsMoving = false;
                    MenuEntity->IsAnimating = false;
                }
                
                MenuEntity->ButtonIndex = -1;
            }
            
        }
    }
    
    // TODO(Sierra): This is weird, maybe there is a way i can fix that!!
    s32 OffsetX = 0;
    if(MenuEntity->IsMoving)
    {
        OffsetX += Input->MouseRelX;
        
        MenuEntity->MouseOffsetX += Input->MouseRelX;
        MenuEntity->MouseOffsetY += Input->MouseRelY;
        
        printf("MenuEntity->MouseOffsetX = %d\n",MenuEntity->MouseOffsetX);
        
        r32 MouseDistance = sqrt((MenuEntity->MouseOffsetX * MenuEntity->MouseOffsetX) + (MenuEntity->MouseOffsetY * MenuEntity->MouseOffsetY));
        
        if(MouseDistance >= (r32)MenuEntity->ButtonSizeWidth * 0.2f)
        {
            MenuEntity->ButtonIndex = -1;
        }
    }
    
    if(MenuEntity->IsAnimating)
    {
        s32 RelativeCenter = MenuEntity->ButtonsArea[MenuEntity->TargetIndex].x + (MenuEntity->ButtonsArea[MenuEntity->TargetIndex].w/2);
        
        vector2 Vector = { (r32)MenuEntity->MouseOffsetX - RelativeCenter, 0 };
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
                MenuEntity->MouseOffsetX   = 0;
                MenuEntity->MouseOffsetY   = 0;
                MenuEntity->IsAnimating    = false;
                
                u32 ButtonsAreaAmount = (MenuEntity->ButtonsAmount / 20) + 1;
                for(u32 i = 0; i < ButtonsAreaAmount; ++i)
                {
                    MenuEntity->ButtonsArea[i].x += Offset;
                }
                
                for(u32 i = 0; i < MenuEntity->ButtonsAmountReserved; ++i)
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
    
    /* Menu Rendering */
    
    game_rect ScreenRect = {0, 0, Buffer->Width, Buffer->Height};
    DEBUGRenderQuadFill(Buffer, &ScreenRect, {0, 128, 255}, 100);
    
    for(u32 i = 0; i < 5; ++i)
    {
        if(MenuEntity->IsMoving)
        {
            MenuEntity->ButtonsArea[i].x += OffsetX;
            
            game_rect TargetArea = {};
            TargetArea.x = MenuEntity->ButtonsArea[i].x + (MenuEntity->ButtonsArea[i].w / 2) - (Buffer->Width / 2);
            TargetArea.y = 0;
            TargetArea.w = Buffer->Width;
            TargetArea.h = Buffer->Height;
        }
        
        if(MenuEntity->IsAnimating)
        {
            MenuEntity->ButtonsArea[i].x += roundf(MenuEntity->Velocity.x);
        }
        
        DEBUGRenderQuad(Buffer, &MenuEntity->ButtonsArea[i], {255, 255, 255}, 100);
    }
    
    s32 ButtonsAmount = MenuEntity->ButtonsAmount;
    s32 ButtonsAmountReserved = MenuEntity->ButtonsAmountReserved;
    
    for(u32 i = 0; i < ButtonsAmountReserved; ++i)
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
        
        if(MenuEntity->ButtonIndex == i && i < ButtonsAmount)
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
            if(i < ButtonsAmount)
            {
                
                GameRenderBitmapToBuffer(Buffer, MenuEntity->BackTexture, &MenuEntity->Buttons[i].ButtonQuad);
                GameRenderBitmapToBuffer(Buffer, MenuEntity->Buttons[i].LevelNumberTexture,
                                         &MenuEntity->Buttons[i].LevelNumberTextureQuad);
                
                if(MenuEntity->Buttons[i].IsLocked)
                {
                    DEBUGRenderQuadFill(Buffer, &MenuEntity->Buttons[i].ButtonQuad, {0, 0, 0}, 150);
                }
                
            }
            
        }
    }
}
#endif