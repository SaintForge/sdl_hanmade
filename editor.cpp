// editor.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

struct label_button
{
    game_rect InfoQuad;
    game_rect InfoTextureQuad;
    game_texture *InfoTexture;
    
    game_rect MinusQuad;
    game_rect MinusTextureQuad;
    game_texture *MinusTexture;
    
    game_rect NumberQuad;
    game_rect NumberTextureQuad;
    game_texture *NumberTexture;
    
    game_rect PlusQuad;
    game_rect PlusTextureQuad;
    game_texture *PlusTexture;
};

enum cursor_type
{
    ARROW,
    
    SIZE_ALL,
    
    SIZE_WEST,
    SIZE_EAST,
    SIZE_NORTH,
    SIZE_SOUTH,
    
    SIZE_WN,
    SIZE_ES,
    SIZE_NE,
    SIZE_WS
};

enum coordinate_type
{
    PIXEL_AREA,
    GAME_AREA,
    SCREEN_AREA
};

struct position_panel
{
    coordinate_type CoordType;
    
    game_rect HeaderQuad;
    game_rect HeaderTextureQuad;
    game_texture *HeaderTexture;
    
    game_rect SwitchNameQuad;
    game_rect SwitchNameTextureQuad;
    game_texture *SwitchNameTexture;
    
    game_rect LeftArrowQuad;
    game_rect LeftArrowTextureQuad;
    game_texture *LeftArrowTexture;
    
    game_rect RightArrowQuad;
    game_rect RightArrowTextureQuad;
    game_texture *RightArrowTexture;
    
    game_rect FirstNumberNameQuad;
    game_rect FirstNumberNameTextureQuad;
    game_texture *FirstNumberNameTexture;
    
    game_rect FirstNumberQuad;
    game_rect FirstNumberTextureQuad;
    game_texture *FirstNumberTexture;
    
    game_rect SecondNumberNameQuad;
    game_rect SecondNumberNameTextureQuad;
    game_texture *SecondNumberNameTexture;
    
    game_rect SecondNumberQuad;
    game_rect SecondNumberTextureQuad;
    game_texture *SecondNumberTexture;
    
    game_rect ThirdNumberNameQuad;
    game_rect ThirdNumberNameTextureQuad;
    game_texture *ThirdNumberNameTexture;
    
    game_rect ThirdNumberQuad;
    game_rect ThirdNumberTextureQuad;
    game_texture *ThirdNumberTexture;
    
    game_rect FourthNumberNameQuad;
    game_rect FourthNumberNameTextureQuad;
    game_texture *FourthNumberNameTexture;
    
    game_rect FourthNumberQuad;
    game_rect FourthNumberTextureQuad;
    game_texture *FourthNumberTexture;
};

struct level_editor
{
    /* For switching to editor mode*/
    bool EditorMode;
    
    /* For creating string textures*/
    game_font *Font;
    
    /* Data for selecting figures */
    s32 CurrentFigureIndex;
    s32 CurrentLevelIndex;
    
    /* Data for changing level number */
    s32 OldLevelNumber;
    s32 LevelNumberBufferIndex;
    char LevelNumberBuffer[4];
    bool LevelNumberSelected;
    
    /* Current level index data */
    game_rect LevelIndexQuad;
    game_texture *LevelIndexTexture;
    
    /* Current level number data */
    game_rect LevelNumberQuad;
    game_texture *LevelNumberTexture;
    
    /* Next and Previous button data*/
    game_rect PrevLevelQuad;
    game_texture *PrevLevelTexture;
    
    game_rect NextLevelQuad;
    game_texture *NextLevelTexture;
    
    /* For highlightning the button whenever pressed */
    bool ButtonSelected;
    game_rect HighlightButtonQuad;
    
    /* Level properties layer quad */
    game_rect LevelPropertiesQuad;
    
    game_rect LevelConfigQuad;
    game_rect LevelConfigTextureQuad;
    game_texture *LevelConfigTexture;
    
    label_button RowLabel;
    label_button ColumnLabel;
    label_button FigureLabel;
    
    /* Figure properties layer quad */
    
    game_rect FigurePropertiesQuad;
    
    game_rect FigureConfigQuad;
    game_rect FigureConfigTextureQuad;
    game_texture *FigureConfigTexture;
    
    game_rect RotateFigureQuad;
    game_rect RotateFigureTextureQuad;
    game_texture *RotateFigureTexture;
    
    game_rect FlipFigureQuad;
    game_rect FlipFigureTextureQuad;
    game_texture *FlipFigureTexture;
    
    game_rect TypeFigureQuad;
    game_rect TypeFigureTextureQuad;
    game_texture *TypeFigureTexture;
    
    game_rect FormFigureQuad;
    game_rect FormFigureTextureQuad;
    game_texture *FormFigureTexture;
    
    /* IOproperties layer quad */
    
    game_rect IOPropertiesQuad;
    
    game_rect IOConfigQuad;
    game_rect IOConfigTextureQuad;
    game_texture *IOConfigTexture;
    
    game_rect SaveLevelQuad;
    game_rect SaveLevelTextureQuad;
    game_texture *SaveLevelTexture;
    
    game_rect LoadLevelQuad;
    game_rect LoadLevelTextureQuad;
    game_texture *LoadLevelTexture;
    
    
    /* Resize/Positioning routine */
    bool ShiftKeyPressed;
    cursor_type CursorType;
    
    bool AreaIsMoving;
    
    /* Position panel routine*/
    
    game_rect PosPanelQuad;
    position_panel PosPanel;
};

static void
LevelEditorUpdateTextOnButton(game_offscreen_buffer *Buffer, game_font *&Font, char *TextBuffer, game_texture *&Texture, game_rect *TextureQuad, game_rect *AreaQuad, game_color Color)
{
    GameMakeTextureFromString(Texture, TextBuffer, TextureQuad, Font, Color, Buffer);
    
    TextureQuad->x = AreaQuad->x + (AreaQuad->w / 2) - (TextureQuad->w / 2);
    TextureQuad->y = AreaQuad->y + (AreaQuad->h / 2) - (TextureQuad->h / 2);
}


static void
LevelEditorUpdateCoordinates(game_offscreen_buffer *Buffer, game_font *Font, position_panel *PosPanel, game_rect Rectangle, 
                             game_rect GameArea, game_rect ScreenArea)
{
    char FirstNumberBuffer[8]  = {};
    char SecondNumberBuffer[8] = {};
    char ThirdNumberBuffer[8]  = {};
    char FourthNumberBuffer[8] = {};
    
    if(PosPanel->CoordType == PIXEL_AREA)
    {
        sprintf(FirstNumberBuffer,  "%d", Rectangle.x);
        sprintf(SecondNumberBuffer, "%d", Rectangle.y);
        sprintf(ThirdNumberBuffer,  "%d", Rectangle.w);
        sprintf(FourthNumberBuffer, "%d", Rectangle.h);
        
    }
    else if(PosPanel->CoordType == GAME_AREA)
    {
        math_rect MathRect = {
            (r32)Rectangle.x, (r32)Rectangle.y, 
            (r32)Rectangle.x + (r32)Rectangle.w, (r32)Rectangle.y + (r32)Rectangle.h};
        
        math_rect RelMathRect = {(r32)GameArea.x, (r32)GameArea.y, (r32)GameArea.x + (r32)GameArea.w, (r32)GameArea.y + (r32)GameArea.h};
        
        MathRect = NormalizeRectangle(MathRect, RelMathRect);
        
        sprintf(FirstNumberBuffer,  "%.3f", MathRect.Left);
        sprintf(SecondNumberBuffer, "%.3f", MathRect.Top);
        sprintf(ThirdNumberBuffer,  "%.3f", MathRect.Right);
        sprintf(FourthNumberBuffer, "%.3f", MathRect.Bottom);
    }
    else if(PosPanel->CoordType == SCREEN_AREA)
    {
        math_rect MathRect = {
            (r32)Rectangle.x, (r32)Rectangle.y, 
            (r32)Rectangle.x + (r32)Rectangle.w, (r32)Rectangle.y + (r32)Rectangle.h};
        
        math_rect RelMathRect = {(r32)ScreenArea.x, (r32)ScreenArea.y, (r32)ScreenArea.x + (r32)ScreenArea.w, (r32)ScreenArea.y + (r32)ScreenArea.h};
        
        MathRect = NormalizeRectangle(MathRect, RelMathRect);
        
        sprintf(FirstNumberBuffer,  "%.3f", MathRect.Left);
        sprintf(SecondNumberBuffer, "%.3f", MathRect.Top);
        sprintf(ThirdNumberBuffer,  "%.3f", MathRect.Right);
        sprintf(FourthNumberBuffer, "%.3f", MathRect.Bottom);
    }
    
    LevelEditorUpdateTextOnButton(Buffer, Font, FirstNumberBuffer, PosPanel->FirstNumberTexture, &PosPanel->FirstNumberTextureQuad, &PosPanel->FirstNumberQuad, {255, 255, 255});
    
    LevelEditorUpdateTextOnButton(Buffer, Font, SecondNumberBuffer, PosPanel->SecondNumberTexture, &PosPanel->SecondNumberTextureQuad, &PosPanel->SecondNumberQuad, {255, 255, 255});
    
    LevelEditorUpdateTextOnButton(Buffer, Font, ThirdNumberBuffer, PosPanel->ThirdNumberTexture, &PosPanel->ThirdNumberTextureQuad, &PosPanel->ThirdNumberQuad, {255, 255, 255});
    
    LevelEditorUpdateTextOnButton(Buffer, Font, FourthNumberBuffer, PosPanel->FourthNumberTexture, &PosPanel->FourthNumberTextureQuad, &PosPanel->FourthNumberQuad, {255, 255, 255});
}

static void
LevelEditorUpdateCoordinateSwitch(level_editor *LevelEditor, position_panel *PosPanel, game_offscreen_buffer *Buffer)
{
    coordinate_type CoordType = PosPanel->CoordType;
    if(CoordType == PIXEL_AREA)
    {
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Pixels", LevelEditor->PosPanel.SwitchNameTexture, &LevelEditor->PosPanel.SwitchNameTextureQuad, &LevelEditor->PosPanel.SwitchNameQuad, {255, 255, 255});
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "X", LevelEditor->PosPanel.FirstNumberNameTexture, &LevelEditor->PosPanel.FirstNumberNameTextureQuad, &LevelEditor->PosPanel.FirstNumberNameQuad, {255, 255, 255});
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Y", LevelEditor->PosPanel.SecondNumberNameTexture, &LevelEditor->PosPanel.SecondNumberNameTextureQuad, &LevelEditor->PosPanel.SecondNumberNameQuad, {255, 255, 255});
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Width", LevelEditor->PosPanel.ThirdNumberNameTexture, &LevelEditor->PosPanel.ThirdNumberNameTextureQuad, &LevelEditor->PosPanel.ThirdNumberNameQuad, {255, 255, 255});
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Height", LevelEditor->PosPanel.FourthNumberNameTexture, &LevelEditor->PosPanel.FourthNumberNameTextureQuad, &LevelEditor->PosPanel.FourthNumberNameQuad, {255, 255, 255});
    }
    else if(CoordType == GAME_AREA || CoordType == SCREEN_AREA)
    {
        if(CoordType == GAME_AREA)
        {
            LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "% game_area", LevelEditor->PosPanel.SwitchNameTexture, &LevelEditor->PosPanel.SwitchNameTextureQuad, &LevelEditor->PosPanel.SwitchNameQuad, {255, 255, 255});
        }
        else
        {
            LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "% screen_area", LevelEditor->PosPanel.SwitchNameTexture, &LevelEditor->PosPanel.SwitchNameTextureQuad, &LevelEditor->PosPanel.SwitchNameQuad, {255, 255, 255});
        }
        
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Left", LevelEditor->PosPanel.FirstNumberNameTexture, &LevelEditor->PosPanel.FirstNumberNameTextureQuad, &LevelEditor->PosPanel.FirstNumberNameQuad, {255, 255, 255});
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Top", LevelEditor->PosPanel.SecondNumberNameTexture, &LevelEditor->PosPanel.SecondNumberNameTextureQuad, &LevelEditor->PosPanel.SecondNumberNameQuad, {255, 255, 255});
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Right", LevelEditor->PosPanel.ThirdNumberNameTexture, &LevelEditor->PosPanel.ThirdNumberNameTextureQuad, &LevelEditor->PosPanel.ThirdNumberNameQuad, {255, 255, 255});
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Bottom", LevelEditor->PosPanel.FourthNumberNameTexture, &LevelEditor->PosPanel.FourthNumberNameTextureQuad, &LevelEditor->PosPanel.FourthNumberNameQuad, {255, 255, 255});
    }
    
}


static void
LevelEditorSetCursorType(cursor_type CursorType)
{
    SDL_Cursor* Cursor;
    
    switch(CursorType)
    {
        case ARROW:
        {
            Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        } break;
        
        case SIZE_ALL:
        {
            Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
        } break;
        
        case SIZE_WEST:
        case SIZE_EAST:
        {
            Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
        } break;
        
        case SIZE_NORTH:
        case SIZE_SOUTH:
        {
            Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
        } break;
        
        case SIZE_WN:
        case SIZE_ES:
        {
            Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
        } break;
        
        case SIZE_NE:
        case SIZE_WS:
        {
            Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
        } break;
    }
    
    SDL_SetCursor(Cursor);
}

static void 
LevelEditorUpdateLevelStats(s32 X, s32 Y, level_editor *LevelEditor, 
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
        
        Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, TmpBuffer, {255, 255, 255});
        Assert(Surface);
        
        LevelEditor->LevelIndexQuad.w = Surface->w;
        LevelEditor->LevelIndexQuad.h = Surface->h;
        LevelEditor->LevelIndexQuad.x = X;
        LevelEditor->LevelIndexQuad.y = Y;
        
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
        
        Surface = TTF_RenderUTF8_Blended(LevelEditor->Font, TmpBuffer, {255, 255, 255});
        Assert(Surface);
        
        LevelEditor->LevelNumberQuad.w = Surface->w;
        LevelEditor->LevelNumberQuad.h = Surface->h;
        LevelEditor->LevelNumberQuad.x = X;
        LevelEditor->LevelNumberQuad.y =  LevelEditor->LevelIndexQuad.y + LevelEditor->LevelIndexQuad.h;
        
        LevelEditor->LevelNumberTexture = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
        Assert(LevelEditor->LevelNumberTexture);
        
        SDL_FreeSurface(Surface);
    }
}

static void
LevelEditorInitLabel(level_editor *LevelEditor, label_button *Label, char* Text,
                     s32 Number, s32 X, s32 Y, s32 LabelWidth, s32 LabelHeight, 
                     game_offscreen_buffer *Buffer)
{
    s32 InfoWidth = (r32)LabelWidth * 0.6f;
    s32 BoxWidth  = (LabelWidth - InfoWidth) / 3.0f;
    
    Label->InfoQuad = {X, Y, InfoWidth, LabelHeight};
    
    MenuMakeTextButton(Text, Label->InfoQuad.x, Label->InfoQuad.y, Label->InfoQuad.w,
                       Label->InfoQuad.h, &Label->InfoQuad, &Label->InfoTextureQuad,
                       Label->InfoTexture, LevelEditor->Font, {255, 255, 255}, Buffer);
    
    Label->MinusQuad = {Label->InfoQuad.x + InfoWidth, Y, BoxWidth, LabelHeight};
    MenuMakeTextButton("-", Label->MinusQuad.x, Label->MinusQuad.y,
                       Label->MinusQuad.w, Label->MinusQuad.h, &Label->MinusQuad,
                       &Label->MinusTextureQuad, Label->MinusTexture, LevelEditor->Font, {255, 255, 255}, Buffer);
    
    
    char NumberString[3] = {};
    sprintf(NumberString, "%d", Number);
    
    Label->NumberQuad = {Label->MinusQuad.x + BoxWidth, Y, BoxWidth, LabelHeight};
    MenuMakeTextButton(NumberString, Label->NumberQuad.x, Label->NumberQuad.y,
                       Label->NumberQuad.w, Label->NumberQuad.h, &Label->NumberQuad,
                       &Label->NumberTextureQuad, Label->NumberTexture, LevelEditor->Font, {255, 255, 255}, Buffer);
    
    Label->PlusQuad = {Label->NumberQuad.x + BoxWidth, Y, BoxWidth, LabelHeight};
    MenuMakeTextButton("+", Label->PlusQuad.x, Label->PlusQuad.y,
                       Label->PlusQuad.w, Label->PlusQuad.h, &Label->PlusQuad,
                       &Label->PlusTextureQuad, Label->PlusTexture, LevelEditor->Font, {255, 255, 255}, Buffer);
    
}

static void
LevelEditorRenderLabel(label_button *Label, game_offscreen_buffer *Buffer)
{
    /* Information quad */
    DEBUGRenderQuadFill(Buffer, &Label->InfoQuad, {255, 0, 0}, 100);
    DEBUGRenderQuad(Buffer, &Label->InfoQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, Label->InfoTexture, &Label->InfoTextureQuad);
    
    /* Minus quad */
    DEBUGRenderQuadFill(Buffer, &Label->MinusQuad, {0, 255, 0}, 100);
    DEBUGRenderQuad(Buffer, &Label->MinusQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, Label->MinusTexture, &Label->MinusTextureQuad);
    
    /* Number quad */
    DEBUGRenderQuadFill(Buffer, &Label->NumberQuad, {0, 255, 0}, 100);
    DEBUGRenderQuad(Buffer, &Label->NumberQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, Label->NumberTexture, &Label->NumberTextureQuad);
    
    /* Plus quad */
    DEBUGRenderQuadFill(Buffer, &Label->PlusQuad, {0, 255, 0}, 100);
    DEBUGRenderQuad(Buffer, &Label->PlusQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, Label->PlusTexture, &Label->PlusTextureQuad);
}

static void
LevelEditorInit(level_editor *LevelEditor, level_entity *LevelEntity, game_memory *Memory, game_offscreen_buffer *Buffer)
{
    *LevelEditor = {};
    
    r32 FontRatio = 0.0146f;
    s32 FontSize  = roundf(FontRatio * Buffer->Width);
    
    LevelEditor->Font = TTF_OpenFont("..\\data\\Karmina-Bold.otf", FontSize);
    Assert(LevelEditor->Font);
    
    /* Next/Prev level buttons */
    
    LevelEditor->PrevLevelTexture = GetTexture(Memory, "left_arrow.png", Buffer->Renderer);
    Assert(LevelEditor->PrevLevelTexture);
    
    LevelEditor->PrevLevelQuad.w = 40;
    LevelEditor->PrevLevelQuad.h = 40;
    LevelEditor->PrevLevelQuad.x = LevelEditor->PrevLevelQuad.w;
    LevelEditor->PrevLevelQuad.y = Buffer->Height - (LevelEditor->PrevLevelQuad.h * 2);
    
    LevelEditor->NextLevelTexture = GetTexture(Memory, "right_arrow.png", Buffer->Renderer);
    Assert(LevelEditor->NextLevelTexture);
    
    LevelEditor->NextLevelQuad.w = 40;
    LevelEditor->NextLevelQuad.h = 40;
    LevelEditor->NextLevelQuad.x = Buffer->Width - (LevelEditor->NextLevelQuad.w * 2);
    LevelEditor->NextLevelQuad.y = Buffer->Height - (LevelEditor->NextLevelQuad.h * 2);
    
    //0.053333, 0.043924
    //s32 ButtonWidth  = 60;
    //s32 ButtonHeight = 40;
    
    r32 ButtonNormHeight = 0.053333f;
    r32 ButtonNormWidth  = 0.045;
    s32 ButtonWidth  = roundf(ButtonNormWidth * (r32)Buffer->Width);
    s32 ButtonHeight = roundf(ButtonNormHeight * (r32)Buffer->Height);
    
    math_rect ScreenArea = {0.0f, 0.0f, (r32)Buffer->Width, (r32)Buffer->Height};
    
    r32 NormalX = 0.0f;
    r32 NormalY = 1.0f;
    
    //r32 NormalHeight = 1.0f - ((r32)(ButtonHeight * 4) / (r32)(Buffer->Height));
    //r32 NormalWidth  = ((r32)(ButtonWidth * 4) / (r32)(Buffer->Width));
    r32 NormalHeight   = 1.0f - (ButtonNormHeight * 4.0f);
    r32 NormalWidth    = ButtonNormWidth * 4.0f;
    
    math_rect PropertiesQuad = CreateMathRect(NormalX, NormalY, NormalWidth, NormalHeight, ScreenArea);
    
    LevelEditor->LevelPropertiesQuad = ConvertMathRectToGameRect(PropertiesQuad);
    
    /* Level header name initialization */
    MenuMakeTextButton("Level Configuration", 
                       LevelEditor->LevelPropertiesQuad.x, LevelEditor->LevelPropertiesQuad.y, 
                       LevelEditor->LevelPropertiesQuad.w, ButtonHeight,
                       &LevelEditor->LevelConfigQuad,
                       &LevelEditor->LevelConfigTextureQuad,
                       LevelEditor->LevelConfigTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    /* Row label initialization */
    
    s32 RowAmount = LevelEntity->GridEntity->RowAmount;
    LevelEditorInitLabel(LevelEditor, &LevelEditor->RowLabel, "Row amount",
                         RowAmount, 0, ButtonHeight, LevelEditor->LevelPropertiesQuad.w, ButtonHeight, Buffer);
    
    /* Column label initialization */
    
    s32 ColumnAmount = LevelEntity->GridEntity->ColumnAmount;
    LevelEditorInitLabel(LevelEditor, &LevelEditor->ColumnLabel, "Column amount",
                         ColumnAmount, 0, ButtonHeight*2, LevelEditor->LevelPropertiesQuad.w, ButtonHeight, Buffer);
    
    /* Figure label initialization */
    
    s32 FigureAmount = LevelEntity->FigureEntity->FigureAmount;
    LevelEditorInitLabel(LevelEditor, &LevelEditor->FigureLabel, "Figure amount",
                         FigureAmount, 0, ButtonHeight*3, LevelEditor->LevelPropertiesQuad.w, ButtonHeight, Buffer);
    
    
    /* Figure header name initialization */
    
    ButtonNormHeight = 0.05f;
    //ButtonHeight = 30;
    ButtonHeight = roundf(ButtonNormHeight * (r32)Buffer->Height);
    
    NormalY      = NormalHeight;
    NormalHeight = NormalY - ((r32)(ButtonHeight * 5) / (r32)(Buffer->Height));
    
    PropertiesQuad = CreateMathRect(NormalX, NormalY, NormalWidth, NormalHeight, ScreenArea);
    
    LevelEditor->FigurePropertiesQuad = ConvertMathRectToGameRect(PropertiesQuad);
    
    MenuMakeTextButton("Figure Configuration",
                       LevelEditor->FigurePropertiesQuad.x, LevelEditor->FigurePropertiesQuad.y, 
                       LevelEditor->FigurePropertiesQuad.w, ButtonHeight,
                       &LevelEditor->FigureConfigQuad,
                       &LevelEditor->FigureConfigTextureQuad,
                       LevelEditor->FigureConfigTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Flip figure", 
                       LevelEditor->FigurePropertiesQuad.x, LevelEditor->FigurePropertiesQuad.y + ButtonHeight, 
                       LevelEditor->FigurePropertiesQuad.w, ButtonHeight,
                       &LevelEditor->FlipFigureQuad,
                       &LevelEditor->FlipFigureTextureQuad,
                       LevelEditor->FlipFigureTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Rotate figure", 
                       LevelEditor->FigurePropertiesQuad.x, LevelEditor->FigurePropertiesQuad.y + (ButtonHeight * 2), 
                       LevelEditor->FigurePropertiesQuad.w, ButtonHeight,
                       &LevelEditor->RotateFigureQuad,
                       &LevelEditor->RotateFigureTextureQuad,
                       LevelEditor->RotateFigureTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Change type", 
                       LevelEditor->FigurePropertiesQuad.x, LevelEditor->FigurePropertiesQuad.y + (ButtonHeight * 3), 
                       LevelEditor->FigurePropertiesQuad.w, ButtonHeight,
                       &LevelEditor->TypeFigureQuad,
                       &LevelEditor->TypeFigureTextureQuad,
                       LevelEditor->TypeFigureTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Change form", 
                       LevelEditor->FigurePropertiesQuad.x, LevelEditor->FigurePropertiesQuad.y + (ButtonHeight * 4), 
                       LevelEditor->FigurePropertiesQuad.w, ButtonHeight,
                       &LevelEditor->FormFigureQuad,
                       &LevelEditor->FormFigureTextureQuad,
                       LevelEditor->FormFigureTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    /* IOproperties header name initialization */
    
    NormalY      = NormalHeight;
    NormalHeight = NormalY - ((r32)(ButtonHeight * 3) / (r32)(Buffer->Height));
    
    PropertiesQuad = CreateMathRect(NormalX, NormalY, NormalWidth, NormalHeight, ScreenArea);
    
    LevelEditor->IOPropertiesQuad = ConvertMathRectToGameRect(PropertiesQuad);
    
    MenuMakeTextButton("Save/Load level",
                       LevelEditor->IOPropertiesQuad.x, LevelEditor->IOPropertiesQuad.y, 
                       LevelEditor->IOPropertiesQuad.w, ButtonHeight,
                       &LevelEditor->IOConfigQuad,
                       &LevelEditor->IOConfigTextureQuad,
                       LevelEditor->IOConfigTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Save to disk",
                       LevelEditor->IOPropertiesQuad.x, LevelEditor->IOPropertiesQuad.y + (ButtonHeight), 
                       LevelEditor->IOPropertiesQuad.w, ButtonHeight,
                       &LevelEditor->SaveLevelQuad,
                       &LevelEditor->SaveLevelTextureQuad,
                       LevelEditor->SaveLevelTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Load from disk",
                       LevelEditor->IOPropertiesQuad.x, LevelEditor->IOPropertiesQuad.y + (ButtonHeight * 2), 
                       LevelEditor->IOPropertiesQuad.w, ButtonHeight,
                       &LevelEditor->LoadLevelQuad,
                       &LevelEditor->LoadLevelTextureQuad,
                       LevelEditor->LoadLevelTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    
    /* Level stats initialization */
    LevelEditorUpdateLevelStats(LevelEditor->LevelPropertiesQuad.w + 10, 0, 
                                LevelEditor, LevelEntity->LevelNumber, 
                                Memory->CurrentLevelIndex, Buffer);
    
    
    /* Position panel initialization */
    
    LevelEditor->PosPanel.CoordType = PIXEL_AREA;
    
    ButtonHeight = 20;
    r32 PanelHeight = ButtonHeight * 6;
    r32 PanelWidth  = 150;
    
    r32 NormPanelWidth  = PanelWidth / (r32) Buffer->Width;
    r32 NormPanelHeight = PanelHeight / (r32) Buffer->Height;
    
    math_rect MathPanel = CreateMathRect(1.0f - NormPanelWidth, 0.5f + (NormPanelHeight / 2.0f), 1.0f, 0.5f - (NormPanelHeight / 2.0f), ScreenArea);
    
    LevelEditor->PosPanelQuad = ConvertMathRectToGameRect(MathPanel);
    
    MenuMakeTextButton("Position properties",
                       LevelEditor->PosPanelQuad.x, LevelEditor->PosPanelQuad.y, 
                       LevelEditor->PosPanelQuad.w, ButtonHeight,
                       &LevelEditor->PosPanel.HeaderQuad,
                       &LevelEditor->PosPanel.HeaderTextureQuad,
                       LevelEditor->PosPanel.HeaderTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    s32 ArrowWidth  = roundf((r32)PanelWidth * 0.2f);
    s32 SwitchWidth = roundf((r32)PanelWidth * 0.6f);
    MenuMakeTextButton("<",
                       LevelEditor->PosPanelQuad.x, LevelEditor->PosPanelQuad.y + ButtonHeight, 
                       ArrowWidth, ButtonHeight,
                       &LevelEditor->PosPanel.LeftArrowQuad,
                       &LevelEditor->PosPanel.LeftArrowTextureQuad,
                       LevelEditor->PosPanel.LeftArrowTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Pixels",
                       LevelEditor->PosPanelQuad.x + ArrowWidth, LevelEditor->PosPanelQuad.y + ButtonHeight, 
                       SwitchWidth, ButtonHeight,
                       &LevelEditor->PosPanel.SwitchNameQuad,
                       &LevelEditor->PosPanel.SwitchNameTextureQuad,
                       LevelEditor->PosPanel.SwitchNameTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    MenuMakeTextButton(">",
                       LevelEditor->PosPanelQuad.x + ArrowWidth + SwitchWidth, LevelEditor->PosPanelQuad.y + ButtonHeight, 
                       ArrowWidth, ButtonHeight,
                       &LevelEditor->PosPanel.RightArrowQuad,
                       &LevelEditor->PosPanel.RightArrowTextureQuad,
                       LevelEditor->PosPanel.RightArrowTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    s32 NameNumberWidth = roundf((r32)PanelWidth * 0.4f);
    s32 NumberWidth     = PanelWidth - NameNumberWidth;
    char NumberString[128] = {};
    
    MenuMakeTextButton("X",
                       LevelEditor->PosPanelQuad.x, LevelEditor->PosPanelQuad.y + (ButtonHeight * 2), 
                       NameNumberWidth, ButtonHeight,
                       &LevelEditor->PosPanel.FirstNumberNameQuad,
                       &LevelEditor->PosPanel.FirstNumberNameTextureQuad,
                       LevelEditor->PosPanel.FirstNumberNameTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    sprintf(NumberString, "%d", LevelEntity->GridEntity->GridArea.x);
    MenuMakeTextButton(NumberString,
                       LevelEditor->PosPanelQuad.x + NameNumberWidth, LevelEditor->PosPanelQuad.y + (ButtonHeight * 2), 
                       NumberWidth, ButtonHeight,
                       &LevelEditor->PosPanel.FirstNumberQuad,
                       &LevelEditor->PosPanel.FirstNumberTextureQuad,
                       LevelEditor->PosPanel.FirstNumberTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Y",
                       LevelEditor->PosPanelQuad.x, LevelEditor->PosPanelQuad.y + (ButtonHeight * 3), 
                       NameNumberWidth, ButtonHeight,
                       &LevelEditor->PosPanel.SecondNumberNameQuad,
                       &LevelEditor->PosPanel.SecondNumberNameTextureQuad,
                       LevelEditor->PosPanel.SecondNumberNameTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    sprintf(NumberString, "%d", LevelEntity->GridEntity->GridArea.y);
    MenuMakeTextButton(NumberString,
                       LevelEditor->PosPanelQuad.x + NameNumberWidth, LevelEditor->PosPanelQuad.y + (ButtonHeight * 3), 
                       NumberWidth, ButtonHeight,
                       &LevelEditor->PosPanel.SecondNumberQuad,
                       &LevelEditor->PosPanel.SecondNumberTextureQuad,
                       LevelEditor->PosPanel.SecondNumberTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    
    MenuMakeTextButton("Width",
                       LevelEditor->PosPanelQuad.x, LevelEditor->PosPanelQuad.y + (ButtonHeight * 4), 
                       NameNumberWidth, ButtonHeight,
                       &LevelEditor->PosPanel.ThirdNumberNameQuad,
                       &LevelEditor->PosPanel.ThirdNumberNameTextureQuad,
                       LevelEditor->PosPanel.ThirdNumberNameTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    sprintf(NumberString, "%d", LevelEntity->GridEntity->GridArea.w);
    MenuMakeTextButton(NumberString,
                       LevelEditor->PosPanelQuad.x + NameNumberWidth, LevelEditor->PosPanelQuad.y + (ButtonHeight * 4), 
                       NumberWidth, ButtonHeight,
                       &LevelEditor->PosPanel.ThirdNumberQuad,
                       &LevelEditor->PosPanel.ThirdNumberTextureQuad,
                       LevelEditor->PosPanel.ThirdNumberTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Height",
                       LevelEditor->PosPanelQuad.x, LevelEditor->PosPanelQuad.y + (ButtonHeight * 5), 
                       NameNumberWidth, ButtonHeight,
                       &LevelEditor->PosPanel.FourthNumberNameQuad,
                       &LevelEditor->PosPanel.FourthNumberNameTextureQuad,
                       LevelEditor->PosPanel.FourthNumberNameTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
    
    sprintf(NumberString, "%d", LevelEntity->GridEntity->GridArea.h);
    MenuMakeTextButton(NumberString,
                       LevelEditor->PosPanelQuad.x + NameNumberWidth, LevelEditor->PosPanelQuad.y + (ButtonHeight * 5), 
                       NumberWidth, ButtonHeight,
                       &LevelEditor->PosPanel.FourthNumberQuad,
                       &LevelEditor->PosPanel.FourthNumberTextureQuad,
                       LevelEditor->PosPanel.FourthNumberTexture, 
                       LevelEditor->Font, {255, 255, 255}, Buffer);
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
LevelEditorChangeFigureCounter(level_editor *LevelEditor, s32 NewFigureAmount,
                               game_offscreen_buffer *Buffer)
{
    if(NewFigureAmount < 0)
    {
        return;
    }
    
    char FigureString[3] = {};
    
    sprintf(FigureString, "%d", NewFigureAmount);
    
    if(LevelEditor->FigureLabel.NumberTexture)
    {
        FreeTexture(LevelEditor->FigureLabel.NumberTexture);
        LevelEditor->FigureLabel.NumberTexture = 0;
    }
    
    GameMakeTextureFromString(LevelEditor->FigureLabel.NumberTexture, FigureString,
                              &LevelEditor->FigureLabel.NumberTextureQuad, LevelEditor->Font, {255, 255, 255}, Buffer);
    
    LevelEditor->FigureLabel.NumberTextureQuad.x = LevelEditor->FigureLabel.NumberQuad.x + (LevelEditor->FigureLabel.NumberQuad.w / 2.0f)
        - (LevelEditor->FigureLabel.NumberTextureQuad.w / 2.0f);
    
    LevelEditor->FigureLabel.NumberTextureQuad.y = LevelEditor->FigureLabel.NumberQuad.y + (LevelEditor->FigureLabel.NumberQuad.h / 2.0f) - (LevelEditor->FigureLabel.NumberTextureQuad.h / 2.0f);
}

static void
LevelEditorChangeGridCounters(level_editor *LevelEditor, 
                              u32 NewRowAmount, u32 NewColumnAmount,
                              game_offscreen_buffer *Buffer)
{
    char RowString[3]   = {};
    char ColString[3]   = {};
    
    sprintf(RowString, "%d", NewRowAmount);
    sprintf(ColString, "%d", NewColumnAmount);
    
    if(LevelEditor->RowLabel.NumberTexture)
    {
        FreeTexture(LevelEditor->RowLabel.NumberTexture);
        LevelEditor->RowLabel.NumberTexture = 0;
    }
    
    GameMakeTextureFromString(LevelEditor->RowLabel.NumberTexture, RowString,
                              &LevelEditor->RowLabel.NumberTextureQuad, LevelEditor->Font, {255, 255, 255}, Buffer);
    
    LevelEditor->RowLabel.NumberTextureQuad.x = LevelEditor->RowLabel.NumberQuad.x + (LevelEditor->RowLabel.NumberQuad.w / 2.0f)
        - (LevelEditor->RowLabel.NumberTextureQuad.w / 2.0f);
    LevelEditor->RowLabel.NumberTextureQuad.y = LevelEditor->RowLabel.NumberQuad.y + (LevelEditor->RowLabel.NumberQuad.h / 2.0f) - (LevelEditor->RowLabel.NumberTextureQuad.h / 2.0f);
    
    if(LevelEditor->ColumnLabel.NumberTexture)
    {
        FreeTexture(LevelEditor->ColumnLabel.NumberTexture);
        LevelEditor->ColumnLabel.NumberTexture = 0;
    }
    
    GameMakeTextureFromString(LevelEditor->ColumnLabel.NumberTexture, ColString,
                              &LevelEditor->ColumnLabel.NumberTextureQuad, LevelEditor->Font, {255, 255, 255}, Buffer);
    
    LevelEditor->ColumnLabel.NumberTextureQuad.x = LevelEditor->ColumnLabel.NumberQuad.x + (LevelEditor->ColumnLabel.NumberQuad.w / 2.0f)
        - (LevelEditor->ColumnLabel.NumberTextureQuad.w / 2.0f);
    LevelEditor->ColumnLabel.NumberTextureQuad.y = LevelEditor->ColumnLabel.NumberQuad.y + (LevelEditor->ColumnLabel.NumberQuad.h / 2.0f) - (LevelEditor->ColumnLabel.NumberTextureQuad.h / 2.0f);
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
    
    s32 FigureAmount   = LevelEntity->FigureEntity->FigureAmount;
    s32 RowAmount      = LevelEntity->GridEntity->RowAmount;
    s32 ColAmount      = LevelEntity->GridEntity->ColumnAmount;
    
    s32 NewFigureIndex = LevelEditor->CurrentFigureIndex;
    
    s32 CurrentLevelIndex = Memory->CurrentLevelIndex;
    if(LevelEditor->CurrentLevelIndex != CurrentLevelIndex)
    {
        LevelEditor->CurrentLevelIndex = CurrentLevelIndex;
        LevelEditorChangeGridCounters(LevelEditor, RowAmount, ColAmount, Buffer);
        LevelEditorUpdateLevelStats(LevelEditor->LevelPropertiesQuad.w + 10, 0, 
                                    LevelEditor, LevelEntity->LevelNumber, 
                                    CurrentLevelIndex, Buffer);
        
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
    
    if(Input->Keyboard.LeftShift.EndedDown)
    {
        if(!LevelEditor->ShiftKeyPressed)
        {
            LevelEditor->ShiftKeyPressed = true;
        }
    }
    else if(Input->Keyboard.LeftShift.EndedUp)
    {
        if(LevelEditor->ShiftKeyPressed)
        {
            LevelEditor->ShiftKeyPressed = false;
            
            if(LevelEditor->CursorType != ARROW)
            {
                LevelEditor->CursorType = ARROW;
                LevelEditorSetCursorType(ARROW);
                
                if(LevelEditor->AreaIsMoving)
                {
                    LevelEditor->AreaIsMoving = false;
                }
            }
            
        }
    }
    
    
    if(LevelEditor->ShiftKeyPressed)
    {
        if(!LevelEditor->AreaIsMoving)
        {
            game_rect Corner[4] = {};
            
            Corner[0] = {GridArea.x - 10, GridArea.y - 10, 20, 20};
            Corner[1] = {(GridArea.x + GridArea.w) - 10, GridArea.y - 10, 20, 20};
            Corner[2] = {(GridArea.x + GridArea.w) - 10, (GridArea.y + GridArea.h) - 10, 20, 20};
            Corner[3] = {GridArea.x - 10, (GridArea.y + GridArea.h), 20, 20};
            
            cursor_type CursorType = LevelEditor->CursorType;
            
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Corner[0]))
            {
                CursorType = SIZE_WN;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Corner[1]))
            {
                CursorType = SIZE_NE;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Corner[2]))
            {
                CursorType = SIZE_ES;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Corner[3]))
            {
                CursorType = SIZE_WS;
            }
            else
            {
                game_rect Border[4] = {};
                
                Border[0] = {GridArea.x, GridArea.y - 10, GridArea.w, 20};
                Border[1] = {(GridArea.x + GridArea.w) - 10, GridArea.y, 20, GridArea.h};
                Border[2] = {GridArea.x, (GridArea.y + GridArea.h) - 10, GridArea.w, 20};
                Border[3] = {GridArea.x - 10, GridArea.y, 20, GridArea.h};
                
                if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Border[0]))
                {
                    CursorType = SIZE_NORTH;
                }
                else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Border[1]))
                {
                    CursorType = SIZE_EAST;
                }
                else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Border[2]))
                {
                    CursorType = SIZE_SOUTH;
                }
                else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Border[3]))
                {
                    CursorType = SIZE_WEST;
                }
                else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &GridArea))
                {
                    CursorType = SIZE_ALL;
                }
            }
            
            if(CursorType != LevelEditor->CursorType)
            {
                LevelEditor->CursorType = CursorType;
                LevelEditorSetCursorType(CursorType);
                
                printf("Cursor set\n");
            }
        }
    }
    
    if(Input->Keyboard.Up.EndedDown)
    {
        NewFigureIndex -= 1;
    }
    else if(Input->Keyboard.Down.EndedDown)
    {
        NewFigureIndex += 1;
    }
    else if(Input->Keyboard.Left.EndedDown)
    {
        NewFigureIndex -= 2;
    }
    else if(Input->Keyboard.Right.EndedDown)
    {
        NewFigureIndex += 2;
    }
    else if(Input->Keyboard.Q_Button.EndedDown)
    {
        s32 PrevLevelNumber = CurrentLevelIndex - 1;
        if(PrevLevelNumber >= 0)
        {
            Memory->CurrentLevelIndex = PrevLevelNumber;
            
            LevelEntityUpdateLevelEntityFromMemory(Memory, Memory->CurrentLevelIndex, true, Buffer);
            
            LevelEditorChangeGridCounters(LevelEditor, LevelEntity->GridEntity->RowAmount,
                                          LevelEntity->GridEntity->ColumnAmount,
                                          Buffer);
            
            LevelEditorUpdateLevelStats(LevelEditor->LevelPropertiesQuad.w + 10, 0, 
                                        LevelEditor, LevelEntity->LevelNumber, 
                                        Memory->CurrentLevelIndex, Buffer);
            
            LevelEditorChangeFigureCounter(LevelEditor, LevelEntity->FigureEntity->FigureAmount, Buffer);
        }
    }
    else if(Input->Keyboard.E_Button.EndedDown)
    {
        s32 NextLevelNumber = CurrentLevelIndex + 1;
        if(NextLevelNumber < Memory->LevelMemoryAmount)
        {
            Memory->CurrentLevelIndex = NextLevelNumber;
            
            LevelEntityUpdateLevelEntityFromMemory(Memory, Memory->CurrentLevelIndex, true, Buffer);
            LevelEditorChangeGridCounters(LevelEditor, LevelEntity->GridEntity->RowAmount,
                                          LevelEntity->GridEntity->ColumnAmount,
                                          Buffer);
            
            LevelEditorUpdateLevelStats(LevelEditor->LevelPropertiesQuad.w + 10, 0, 
                                        LevelEditor, LevelEntity->LevelNumber, 
                                        Memory->CurrentLevelIndex, Buffer);
            
            LevelEditorChangeFigureCounter(LevelEditor, LevelEntity->FigureEntity->FigureAmount, Buffer);
        }
    }
    else if(Input->Keyboard.Enter.EndedDown)
    {
        if(LevelEditor->LevelNumberSelected)
        {
            LevelEntity->LevelNumber = strtol(LevelEditor->LevelNumberBuffer, 0, 10);
            LevelEditor->LevelNumberSelected = false;
            
            menu_entity* MenuEntity = 
                (menu_entity*) (((char*)Memory->LocalMemoryStorage) + (sizeof(level_entity)));
            
            MenuChangeButtonText(Memory->LevelNumberFont, LevelEditor->LevelNumberBuffer, MenuEntity, 
                                 &MenuEntity->Buttons[Memory->CurrentLevelIndex], {255, 255, 255}, Buffer);
        }
    }
    
    if(Input->MouseButtons[0].EndedDown)
    {
        
        if (IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEntity->LevelNumberQuad))
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
        
        if(LevelEditor->ShiftKeyPressed)
        {
            if(LevelEditor->CursorType != ARROW)
            {
                if(!LevelEditor->AreaIsMoving)
                {
                    LevelEditor->AreaIsMoving = true;
                }
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->PosPanel.LeftArrowQuad))
        {
            if(LevelEditor->PosPanel.CoordType == PIXEL_AREA)
            {
                LevelEditor->PosPanel.CoordType = SCREEN_AREA;
                
            }
            else if(LevelEditor->PosPanel.CoordType == SCREEN_AREA)
            {
                LevelEditor->PosPanel.CoordType = GAME_AREA;
            }
            else
            {
                LevelEditor->PosPanel.CoordType = PIXEL_AREA;
            }
            
            LevelEditorUpdateCoordinates(Buffer, LevelEditor->Font, &LevelEditor->PosPanel, LevelEntity->GridEntity->GridArea, 
                                         Memory->PadRect, {0, 0, Buffer->Width, Buffer->Height});
            LevelEditorUpdateCoordinateSwitch(LevelEditor, &LevelEditor->PosPanel, Buffer);
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->PosPanel.RightArrowQuad))
        {
            if(LevelEditor->PosPanel.CoordType == PIXEL_AREA)
            {
                LevelEditor->PosPanel.CoordType = GAME_AREA;
                
            }
            else if(LevelEditor->PosPanel.CoordType == GAME_AREA)
            {
                LevelEditor->PosPanel.CoordType = SCREEN_AREA;
            }
            else
            {
                LevelEditor->PosPanel.CoordType = PIXEL_AREA;
            }
            
            LevelEditorUpdateCoordinates(Buffer, LevelEditor->Font, &LevelEditor->PosPanel, LevelEntity->GridEntity->GridArea, 
                                         Memory->PadRect, {0, 0, Buffer->Width, Buffer->Height});
            LevelEditorUpdateCoordinateSwitch(LevelEditor, &LevelEditor->PosPanel, Buffer);
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->LevelPropertiesQuad))
        {
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->RowLabel.MinusQuad))
            {
                printf("minus row!\n");
                
                GridEntityNewGrid(Buffer, LevelEntity, RowAmount-1, ColAmount, LevelEditor);
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->RowLabel.MinusQuad;
            }
            
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->RowLabel.PlusQuad))
            {
                printf("plus row!\n");
                
                GridEntityNewGrid(Buffer, LevelEntity, RowAmount+1, ColAmount, LevelEditor);
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->RowLabel.PlusQuad;
            }
            
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->ColumnLabel.MinusQuad))
            {
                printf("minus column!\n");
                
                GridEntityNewGrid(Buffer, LevelEntity, RowAmount, ColAmount-1, LevelEditor);
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->ColumnLabel.MinusQuad;
            }
            
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->ColumnLabel.PlusQuad))
            {
                printf("plus column!\n");
                
                GridEntityNewGrid(Buffer, LevelEntity, RowAmount, ColAmount+1, LevelEditor);
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->ColumnLabel.PlusQuad;
            }
            
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->FigureLabel.MinusQuad))
            {
                printf("minus figure!\n");
                
                if(FigureAmount > 0)
                {
                    FigureUnitDeleteFigure(LevelEntity->FigureEntity, LevelEntity->FigureEntity->FigureAmount - 1);
                    FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                    GridEntityUpdateStickUnits(LevelEntity->GridEntity, LevelEntity->FigureEntity->FigureAmount);
                    
                    LevelEditorChangeFigureCounter(LevelEditor, FigureAmount - 1, Buffer);
                }
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->FigureLabel.MinusQuad;
            }
            
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->FigureLabel.PlusQuad))
            {
                printf("plus figure!\n");
                
                if(FigureAmount < LevelEntity->FigureEntity->FigureAmountReserved)
                {
                    FigureUnitAddNewFigure(LevelEntity->FigureEntity, O_figure, classic, 0.0f, Memory, Buffer);
                    FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                    GridEntityUpdateStickUnits(LevelEntity->GridEntity, LevelEntity->FigureEntity->FigureAmount);
                    
                    LevelEditorChangeFigureCounter(LevelEditor, FigureAmount + 1, Buffer);
                }
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->FigureLabel.PlusQuad;
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->FigurePropertiesQuad))
        {
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->FlipFigureQuad))
            {
                printf("flip!\n");
                
                FigureUnitFlipHorizontally(&LevelEntity->FigureEntity->FigureUnit[LevelEditor->CurrentFigureIndex]);
                FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->FlipFigureQuad;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->RotateFigureQuad))
            {
                printf("rotate!\n");
                
                FigureUnitRotateShellBy(&LevelEntity->FigureEntity->FigureUnit[LevelEditor->CurrentFigureIndex], 90);
                LevelEntity->FigureEntity->FigureUnit[LevelEditor->CurrentFigureIndex].Angle += 90.0f;
                FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->RotateFigureQuad;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->TypeFigureQuad))
            {
                printf("type!\n");
                
                if(LevelEditor->CurrentFigureIndex >= 0)
                {
                    FreeTexture(LevelEntity->FigureEntity->FigureUnit[LevelEditor->CurrentFigureIndex].Texture);
                    
                    figure_type Type = LevelEditorGetNextFigureType(LevelEntity->FigureEntity->FigureUnit[LevelEditor->CurrentFigureIndex].Type);
                    
                    figure_form Form = LevelEntity->FigureEntity->FigureUnit[LevelEditor->CurrentFigureIndex].Form;
                    
                    FigureUnitInitFigure(&LevelEntity->FigureEntity->FigureUnit[LevelEditor->CurrentFigureIndex], Form,Type, 0.0f, Memory, Buffer);
                    
                    FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                }
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->TypeFigureQuad;
            }
            
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->FormFigureQuad))
            {
                printf("form!\n");
                
                if(LevelEditor->CurrentFigureIndex >= 0)
                {
                    FreeTexture(LevelEntity->FigureEntity->FigureUnit[LevelEditor->CurrentFigureIndex].Texture);
                    
                    figure_form Form = LevelEditorGetNextFigureForm(LevelEntity->FigureEntity->FigureUnit[LevelEditor->CurrentFigureIndex].Form);
                    
                    figure_type Type = LevelEntity->FigureEntity->FigureUnit[LevelEditor->CurrentFigureIndex].Type;
                    
                    FigureUnitInitFigure(&LevelEntity->FigureEntity->FigureUnit[LevelEditor->CurrentFigureIndex], Form,Type, 0.0f,  Memory, Buffer);
                    
                    FigureEntityAlignHorizontally(LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                }
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->FormFigureQuad;
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->IOPropertiesQuad))
        {
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->SaveLevelQuad))
            {
                printf("save!\n");
                
                SaveLevelToMemory(Memory, LevelEntity, Memory->CurrentLevelIndex);
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->SaveLevelQuad;
            }
            
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->LoadLevelQuad))
            {
                printf("load!\n");
                
                LevelEntityUpdateLevelEntityFromMemory(Memory, Memory->CurrentLevelIndex,
                                                       false, Buffer);
                LevelEditorChangeGridCounters(LevelEditor, 
                                              LevelEntity->GridEntity->RowAmount, LevelEntity->GridEntity->ColumnAmount, 
                                              Buffer);
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->LoadLevelQuad;
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &FigureArea))
        {
            for(u32 i = 0; i < FigureAmount; ++i)
            {
                game_rect AreaQuad = FigureUnitGetArea(&LevelEntity->FigureEntity->FigureUnit[i]);
                if(IsPointInsideRect(Input->MouseX, Input->MouseY, &AreaQuad))
                {
                    NewFigureIndex = i;
                }
            }
            
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->PrevLevelQuad))
            {
                s32 PrevLevelIndex = Memory->CurrentLevelIndex - 1;
                if(PrevLevelIndex >= 0)
                {
                    Memory->CurrentLevelIndex = PrevLevelIndex;
                    
                    LevelEntityUpdateLevelEntityFromMemory(Memory, PrevLevelIndex, true, Buffer);
                    
                    LevelEditorChangeGridCounters(LevelEditor, LevelEntity->GridEntity->RowAmount,
                                                  LevelEntity->GridEntity->ColumnAmount,Buffer);
                    
                    LevelEditorUpdateLevelStats(LevelEditor->LevelPropertiesQuad.w + 10, 0, LevelEditor, LevelEntity->LevelNumber, 
                                                Memory->CurrentLevelIndex, Buffer);
                    
                    LevelEditorChangeFigureCounter(LevelEditor, LevelEntity->FigureEntity->FigureAmount, Buffer);
                }
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->PrevLevelQuad;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->NextLevelQuad))
            {
                s32 NextLevelIndex = Memory->CurrentLevelIndex + 1;
                if(NextLevelIndex < Memory->LevelMemoryAmount)
                {
                    Memory->CurrentLevelIndex = NextLevelIndex;
                    
                    LevelEntityUpdateLevelEntityFromMemory(Memory, NextLevelIndex, true, Buffer);
                    
                    LevelEditorChangeGridCounters(LevelEditor, 
                                                  LevelEntity->GridEntity->RowAmount, LevelEntity->GridEntity->ColumnAmount, Buffer);
                    
                    LevelEditorUpdateLevelStats(LevelEditor->LevelPropertiesQuad.w + 10, 0, LevelEditor, LevelEntity->LevelNumber, 
                                                Memory->CurrentLevelIndex, Buffer);
                    
                    
                    LevelEditorChangeFigureCounter(LevelEditor, LevelEntity->FigureEntity->FigureAmount, Buffer);
                }
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->NextLevelQuad;
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
        LevelEditor->ButtonSelected      = true;
        LevelEditor->HighlightButtonQuad = {};
        
        if(LevelEditor->AreaIsMoving)
        {
            LevelEditor->AreaIsMoving = false;
        }
    }
    
    if(LevelEditor->AreaIsMoving)
    {
        s32 OffsetX = Input->MouseRelX;
        s32 OffsetY = Input->MouseRelY;
        
        math_rect AreaQuad = ConvertGameRectToMathRect(GridArea);
        
        switch(LevelEditor->CursorType)
        {
            case SIZE_ALL:
            {
                AreaQuad.Left   += OffsetX;
                AreaQuad.Top    += OffsetY;
                AreaQuad.Right  += OffsetX;
                AreaQuad.Bottom += OffsetY;
            } break;
            
            case SIZE_WEST:
            {
                AreaQuad.Left += OffsetX;
            } break;
            
            case SIZE_EAST:
            {
                AreaQuad.Right += OffsetX;
            } break;
            
            case SIZE_NORTH:
            {
                AreaQuad.Top += OffsetY;
            } break;
            
            case SIZE_SOUTH:
            {
                AreaQuad.Bottom += OffsetY;
            } break;
            
            case SIZE_WN:
            {
                AreaQuad.Left += OffsetX;
                AreaQuad.Top  += OffsetY;
            } break;
            
            case SIZE_ES:
            {
                AreaQuad.Right  += OffsetX;
                AreaQuad.Bottom += OffsetY;
            } break;
            
            case SIZE_NE:
            {
                AreaQuad.Right += OffsetX;
                AreaQuad.Top   += OffsetY;
            } break;
            
            case SIZE_WS:
            {
                AreaQuad.Left   += OffsetX;
                AreaQuad.Bottom += OffsetY;
            } break;
        }
        
        LevelEntity->GridEntity->GridArea = ConvertMathRectToGameRect(AreaQuad);
        
        LevelEntity->Configuration.GridBlockSize = CalculateGridBlockSize(RowAmount, ColAmount, LevelEntity->GridEntity->GridArea.w, 
                                                                          LevelEntity->GridEntity->GridArea.h,
                                                                          LevelEntity->Configuration.DefaultBlocksInRow, LevelEntity->Configuration.DefaultBlocksInCol);
        
        LevelEditorUpdateCoordinates(Buffer, LevelEditor->Font, &LevelEditor->PosPanel, LevelEntity->GridEntity->GridArea, 
                                     Memory->PadRect, {0, 0, Buffer->Width, Buffer->Height});
        
    }
    
    if(NewFigureIndex < 0)
    {
        NewFigureIndex = FigureAmount - 1;
    }
    
    if(NewFigureIndex > FigureAmount - 1)
    {
        NewFigureIndex = 0;
    }
    
    LevelEditor->CurrentFigureIndex = NewFigureIndex;
    
    if(LevelEntity->LevelStarted)
    {
        for(u32 i = 0; i < FigureAmount; ++i)
        {
            DEBUGRenderFigureShell(Buffer, &LevelEntity->FigureEntity->FigureUnit[i], LevelEntity->Configuration.InActiveBlockSize / 4, {0, 0, 255}, 255);
            DEBUGRenderQuad(Buffer, &LevelEntity->FigureEntity->FigureUnit[i].AreaQuad, {255, 0, 0}, 255);
        }
        
        if(LevelEntity->FigureEntity->FigureAmount > 0)
        {
            DEBUGRenderFigureShell(Buffer, &LevelEntity->FigureEntity->FigureUnit[NewFigureIndex],LevelEntity->Configuration.InActiveBlockSize, {255,
                                   255, 255}, 100);
        }
    }
    
    
    /* Level number is selected */ 
    
    if(LevelEditor->LevelNumberSelected)
    {
        DEBUGRenderQuadFill(Buffer, &LevelEntity->LevelNumberQuad, {255, 0, 0}, 150);
    }
    
    /* Level header name rendering */ 
    DEBUGRenderQuadFill(Buffer, &LevelEditor->LevelConfigQuad, {128, 128, 128}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->LevelConfigQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->LevelConfigTexture, &LevelEditor->LevelConfigTextureQuad);
    
    LevelEditorRenderLabel(&LevelEditor->RowLabel,    Buffer);
    LevelEditorRenderLabel(&LevelEditor->ColumnLabel, Buffer);
    LevelEditorRenderLabel(&LevelEditor->FigureLabel, Buffer);
    
    /* Figure header name rendering */ 
    DEBUGRenderQuadFill(Buffer, &LevelEditor->FigureConfigQuad, {128, 128, 128}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->FigureConfigQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->FigureConfigTexture, &LevelEditor->FigureConfigTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->FlipFigureQuad, {0, 0, 255}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->FlipFigureQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->FlipFigureTexture, &LevelEditor->FlipFigureTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->RotateFigureQuad, {0, 0, 255}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->RotateFigureQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->RotateFigureTexture, &LevelEditor->RotateFigureTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->TypeFigureQuad, {0, 0, 255}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->TypeFigureQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->TypeFigureTexture, &LevelEditor->TypeFigureTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->FormFigureQuad, {0, 0, 255}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->FormFigureQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->FormFigureTexture, &LevelEditor->FormFigureTextureQuad);
    
    /* IO header name rendering */ 
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->IOConfigQuad, {128, 128, 128}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->IOConfigQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->IOConfigTexture, &LevelEditor->IOConfigTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->SaveLevelQuad, {0, 255, 255}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->SaveLevelQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->SaveLevelTexture, &LevelEditor->SaveLevelTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->LoadLevelQuad, {0, 255, 255}, 100);
    DEBUGRenderQuad(Buffer, &LevelEditor->LoadLevelQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->LoadLevelTexture, &LevelEditor->LoadLevelTextureQuad);
    
    /* Left/Right arrows rendering */ 
    
    
    if(CurrentLevelIndex > 0)
    {
        GameRenderBitmapToBuffer(Buffer, LevelEditor->PrevLevelTexture, &LevelEditor->PrevLevelQuad);
    }
    
    if(CurrentLevelIndex < (s32)Memory->LevelMemoryAmount-1)
    {
        GameRenderBitmapToBuffer(Buffer, LevelEditor->NextLevelTexture, &LevelEditor->NextLevelQuad);
    }
    
    /* Prev/Next level buttons rendering */
    
    GameRenderBitmapToBuffer(Buffer, LevelEditor->LevelIndexTexture, &LevelEditor->LevelIndexQuad);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->LevelNumberTexture, &LevelEditor->LevelNumberQuad);
    
    
    if(LevelEditor->ButtonSelected)
    {
        DEBUGRenderQuadFill(Buffer, &LevelEditor->HighlightButtonQuad, {255, 255, 0}, 150);
    }
    
    DEBUGRenderQuad(Buffer, &LevelEntity->GridEntity->GridArea, { 0, 255, 255 }, 255);
    
    
    //DEBUGRenderQuad(Buffer, &LevelEditor->PosPanelQuad, { 0, 255, 255 }, 255);
    
    /* Position panel rendering */
    /* Position header rendering */
    DEBUGRenderQuadFill(Buffer, &LevelEditor->PosPanel.HeaderQuad, {128, 128, 128}, 255);
    DEBUGRenderQuad(Buffer, &LevelEditor->PosPanel.HeaderQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PosPanel.HeaderTexture, &LevelEditor->PosPanel.HeaderTextureQuad);
    
    /* Switch buttons rendering */
    DEBUGRenderQuadFill(Buffer, &LevelEditor->PosPanel.LeftArrowQuad, {0, 128, 0}, 255);
    DEBUGRenderQuad(Buffer, &LevelEditor->PosPanel.LeftArrowQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PosPanel.LeftArrowTexture, &LevelEditor->PosPanel.LeftArrowTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->PosPanel.SwitchNameQuad, {0, 128, 0}, 255);
    DEBUGRenderQuad(Buffer, &LevelEditor->PosPanel.SwitchNameQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PosPanel.SwitchNameTexture, &LevelEditor->PosPanel.SwitchNameTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->PosPanel.RightArrowQuad, {0, 128, 0}, 255);
    DEBUGRenderQuad(Buffer, &LevelEditor->PosPanel.RightArrowQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PosPanel.RightArrowTexture, &LevelEditor->PosPanel.RightArrowTextureQuad);
    
    /* First coordinate buttons rendering */
    DEBUGRenderQuadFill(Buffer, &LevelEditor->PosPanel.FirstNumberNameQuad, {0, 0, 128}, 255);
    DEBUGRenderQuad(Buffer, &LevelEditor->PosPanel.FirstNumberNameQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PosPanel.FirstNumberNameTexture, &LevelEditor->PosPanel.FirstNumberNameTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->PosPanel.FirstNumberQuad, {0, 0, 128}, 255);
    DEBUGRenderQuad(Buffer, &LevelEditor->PosPanel.FirstNumberQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PosPanel.FirstNumberTexture, &LevelEditor->PosPanel.FirstNumberTextureQuad);
    
    /* Second coordinate buttons rendering */
    DEBUGRenderQuadFill(Buffer, &LevelEditor->PosPanel.SecondNumberNameQuad, {0, 0, 128}, 255);
    DEBUGRenderQuad(Buffer, &LevelEditor->PosPanel.SecondNumberNameQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PosPanel.SecondNumberNameTexture, &LevelEditor->PosPanel.SecondNumberNameTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->PosPanel.SecondNumberQuad, {0, 0, 128}, 255);
    DEBUGRenderQuad(Buffer, &LevelEditor->PosPanel.SecondNumberQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PosPanel.SecondNumberTexture, &LevelEditor->PosPanel.SecondNumberTextureQuad);
    
    /*Third coordinate buttons rendering */
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->PosPanel.ThirdNumberNameQuad, {0, 0, 128}, 255);
    DEBUGRenderQuad(Buffer, &LevelEditor->PosPanel.ThirdNumberNameQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PosPanel.ThirdNumberNameTexture, &LevelEditor->PosPanel.ThirdNumberNameTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->PosPanel.ThirdNumberQuad, {0, 0, 128}, 255);
    DEBUGRenderQuad(Buffer, &LevelEditor->PosPanel.ThirdNumberQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PosPanel.ThirdNumberTexture, &LevelEditor->PosPanel.ThirdNumberTextureQuad);
    
    
    /*Fourth coordinate buttons rendering*/
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->PosPanel.FourthNumberNameQuad, {0, 0, 128}, 255);
    DEBUGRenderQuad(Buffer, &LevelEditor->PosPanel.FourthNumberNameQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PosPanel.FourthNumberNameTexture, &LevelEditor->PosPanel.FourthNumberNameTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &LevelEditor->PosPanel.FourthNumberQuad, {0, 0, 128}, 255);
    DEBUGRenderQuad(Buffer, &LevelEditor->PosPanel.FourthNumberQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, LevelEditor->PosPanel.FourthNumberTexture, &LevelEditor->PosPanel.FourthNumberTextureQuad);
    
}

struct selection_panel
{
    bool IsSelected;
    game_rect SelectQuad;
};

struct menu_editor
{
    /* For showing gui */
    bool EditorMode;
    bool ButtonsSelected;
    
    /* Button data for adding a new level in memory */
    game_rect NewLevelButtonQuad;
    game_rect NewLevelTextureQuad;
    game_texture *NewLevelTexture;
    
    /* Button data for loading levels from memory*/
    game_rect LoadButtonQuad;
    game_rect LoadButtonTextureQuad;
    game_texture *LoadButtonTexture;
    
    /* Button data for saving levels on the disk */
    game_rect SaveButtonQuad;
    game_rect SaveButtonTextureQuad;
    game_texture *SaveButtonTexture;
    
    /* Button data for sorting levels in memory by their level number*/
    game_rect SortButtonQuad;
    game_rect SortButtonTextureQuad;
    game_texture *SortButtonTexture;
    
    /* Button data for confirming deletion of a level */
    game_rect DeleteButtonQuad;
    game_rect DeleteButtonTextureQuad;
    game_texture *DeleteButtonTexture;
    
    /* Button data for canceling deletion of a level */
    game_rect CancelButtonQuad;
    game_rect CancelButtonTextureQuad;
    game_texture *CancelButtonTexture;
    
    /* For highlightning the button whenever pressed */
    bool ButtonIsPressed;
    game_rect HighlightButtonQuad;
    
    /* For highlightning selected level buttons  */
    selection_panel SelectionPanel[100];
};

static void
MenuEditorInit(menu_editor *MenuEditor, menu_entity *MenuEntity, 
               game_memory *Memory, game_offscreen_buffer *Buffer)
{
    *MenuEditor = {};
    
    s32 ButtonWidth  = 150;
    s32 ButtonHeigth = 50;
    
    MenuMakeTextButton("New", 0, 0, ButtonWidth, ButtonHeigth,
                       &MenuEditor->NewLevelButtonQuad,
                       &MenuEditor->NewLevelTextureQuad,
                       MenuEditor->NewLevelTexture,
                       Memory->LevelNumberFont,
                       {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Load", 0, 50, ButtonWidth, ButtonHeigth,
                       &MenuEditor->LoadButtonQuad,
                       &MenuEditor->LoadButtonTextureQuad,
                       MenuEditor->LoadButtonTexture,
                       Memory->LevelNumberFont,
                       {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Save", 0, 100, ButtonWidth, ButtonHeigth,
                       &MenuEditor->SaveButtonQuad,
                       &MenuEditor->SaveButtonTextureQuad,
                       MenuEditor->SaveButtonTexture,
                       Memory->LevelNumberFont,
                       {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Sort", 0, 150, ButtonWidth, ButtonHeigth,
                       &MenuEditor->SortButtonQuad,
                       &MenuEditor->SortButtonTextureQuad,
                       MenuEditor->SortButtonTexture,
                       Memory->LevelNumberFont,
                       {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Delete", 0, 250, ButtonWidth, ButtonHeigth,
                       &MenuEditor->DeleteButtonQuad,
                       &MenuEditor->DeleteButtonTextureQuad,
                       MenuEditor->DeleteButtonTexture,
                       Memory->LevelNumberFont,
                       {255, 255, 255}, Buffer);
    
    MenuMakeTextButton("Cancel", 0, 300, ButtonWidth, ButtonHeigth,
                       &MenuEditor->CancelButtonQuad,
                       &MenuEditor->CancelButtonTextureQuad,
                       MenuEditor->CancelButtonTexture,
                       Memory->LevelNumberFont,
                       {255, 255, 255}, Buffer);
}

inline void
SwapLevelMemory(level_memory *LevelMemory, s32 IndexA, s32 IndexB)
{
    level_memory TempLevel = LevelMemory[IndexA];
    LevelMemory[IndexA] = LevelMemory[IndexB];
    LevelMemory[IndexB] = TempLevel;
}

static void
MenuEditorSortButtonsRange(s32 BeginIndex, s32 EndIndex, menu_entity *MenuEntity, game_memory *Memory)
{
    level_memory *LevelMemory = (level_memory*)Memory->GlobalMemoryStorage;
    
    s32 LevelAmount = Memory->LevelMemoryAmount;
    
    for(s32 i = BeginIndex; i < EndIndex - 1; ++i)
    {
        for(s32 j = BeginIndex; j < EndIndex - 1; ++j)
        {
            if(LevelMemory[j].LevelNumber > LevelMemory[j+1].LevelNumber)
            {
                SwapLevelMemory(LevelMemory, j, j+1);
            }
        }
    }
}

static void
MenuEditorUpdateAndRender(menu_editor *MenuEditor, menu_entity *MenuEntity, game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
    
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
            
            MenuEditor->ButtonIsPressed = true;
            MenuEditor->HighlightButtonQuad = MenuEditor->NewLevelButtonQuad;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->LoadButtonQuad))
        {
            LoadLevelMemoryFromFile("package2.bin", Memory);
            MenuLoadButtonsFromMemory(MenuEntity, Memory, Buffer);
            
            if(MenuEditor->ButtonsSelected)
            {
                MenuEditor->ButtonsSelected = false;
                
                for(s32 Index = 0; Index < 100; Index++)
                {
                    if(MenuEditor->SelectionPanel[Index].IsSelected)
                    {
                        MenuEditor->SelectionPanel[Index].IsSelected = false;
                    }
                }
            }
            
            MenuEditor->ButtonIsPressed = true;
            MenuEditor->HighlightButtonQuad = MenuEditor->LoadButtonQuad;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->SaveButtonQuad))
        {
            SaveLevelMemoryToFile(Memory);
            
            MenuEditor->ButtonIsPressed = true;
            MenuEditor->HighlightButtonQuad = MenuEditor->SaveButtonQuad;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->SortButtonQuad))
        {
            if(MenuEditor->ButtonsSelected) 
            {
                MenuEditor->ButtonsSelected = false;
            }
            
            bool BeginFound = false;
            
            s32 BeginIndex = 0;
            s32 EndIndex   = 99;
            
            for(s32 Index = 0; Index < 100; ++Index)
            {
                if(MenuEditor->SelectionPanel[Index].IsSelected)
                {
                    MenuEditor->SelectionPanel[Index].IsSelected = false;
                    
                    if(!BeginFound)
                    {
                        BeginFound = true;
                        BeginIndex = Index;
                        
                    }
                }
                else
                {
                    if(BeginFound)
                    {
                        EndIndex = Index;
                        break;
                    }
                }
            }
            
            if(EndIndex > MenuEntity->ButtonsAmount) 
            {
                EndIndex = MenuEntity->ButtonsAmount;
            }
            
            MenuEditorSortButtonsRange(BeginIndex, EndIndex, MenuEntity, Memory);
            MenuLoadButtonsFromMemory(MenuEntity, Memory, Buffer);
            
            MenuEditor->ButtonIsPressed = true;
            MenuEditor->HighlightButtonQuad = MenuEditor->SortButtonQuad;
        }
        
        if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->DeleteButtonQuad))
        {
            if(MenuEditor->ButtonsSelected)
            {
                s32 LevelCounter = 0;
                for(s32 Index = 100; Index > 0; Index--)
                {
                    if(MenuEditor->SelectionPanel[Index].IsSelected)
                    {
                        MenuEditor->SelectionPanel[Index].IsSelected = false;
                        MenuDeleteLevel(MenuEntity, Index, Memory, Buffer);
                        
                        ++LevelCounter;
                    }
                }
                
                Memory->CurrentLevelIndex -= LevelCounter;
                Memory->CurrentLevelIndex = (LevelCounter > Memory->CurrentLevelIndex)
                    ? Memory->CurrentLevelIndex = 0
                    : Memory->CurrentLevelIndex - LevelCounter;
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->CancelButtonQuad))
        {
            if(MenuEditor->ButtonsSelected)
            {
                MenuEditor->ButtonsSelected = false;
                
                for(s32 Index = 0; Index < 100; ++Index)
                {
                    if(MenuEditor->SelectionPanel[Index].IsSelected)
                    {
                        MenuEditor->SelectionPanel[Index].IsSelected = false;
                    }
                }
                
                MenuEditor->ButtonIsPressed = true;
                MenuEditor->HighlightButtonQuad = MenuEditor->CancelButtonQuad;
            }
        }
        
    }
    else if(Input->MouseButtons[0].EndedUp)
    {
        MenuEditor->ButtonIsPressed = false;
        MenuEditor->HighlightButtonQuad = {};
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
            MenuEditor->ButtonsSelected = true;
            
            if(!MenuEditor->SelectionPanel[Index].IsSelected)
            {
                MenuEditor->SelectionPanel[Index].IsSelected = true;
                MenuEditor->SelectionPanel[Index].SelectQuad = MenuEntity->Buttons[Index].ButtonQuad;
            }
            else
            {
                MenuEditor->SelectionPanel[Index].IsSelected = false;
            }
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
    
    DEBUGRenderQuadFill(Buffer, &MenuEditor->DeleteButtonQuad, {128, 128, 128}, 100);
    DEBUGRenderQuad(Buffer, &MenuEditor->DeleteButtonQuad, {255, 255, 255}, 255);
    GameRenderBitmapToBuffer(Buffer, MenuEditor->DeleteButtonTexture, &MenuEditor->DeleteButtonTextureQuad);
    
    DEBUGRenderQuadFill(Buffer, &MenuEditor->CancelButtonQuad, {128, 128, 128}, 100);
    DEBUGRenderQuad(Buffer, &MenuEditor->CancelButtonQuad, {255, 255, 255}, 255);
    GameRenderBitmapToBuffer(Buffer, MenuEditor->CancelButtonTexture, &MenuEditor->CancelButtonTextureQuad);
    
    if(MenuEditor->ButtonsSelected)
    {
        for(s32 i = 0; i < 100; ++i)
        {
            if(MenuEditor->SelectionPanel[i].IsSelected)
            {
                DEBUGRenderQuadFill(Buffer, &MenuEditor->SelectionPanel[i].SelectQuad, {0, 255, 255}, 100);
            }
        }
    }
    
    if(MenuEditor->ButtonIsPressed)
    {
        DEBUGRenderQuadFill(Buffer, &MenuEditor->HighlightButtonQuad, {0, 255, 255}, 200);
    }
    
    
}

