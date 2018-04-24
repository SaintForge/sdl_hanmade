// editor.h --- 
// 
// Filename: editor.h
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

#if !defined(EDITOR_H)


static char
GetNumberFromInput(s32 BufferIndex, game_input *Input)
{
    char Result = '\n';
    
    s32 DigitIndex = BufferIndex;
    
    if(Input->Keyboard.Zero.EndedDown)
    {
        Result = '0';
    }
    else if(Input->Keyboard.One.EndedDown)
    {
        Result = '1';
    }
    else if(Input->Keyboard.Two.EndedDown)
    {
        Result = '2';
    }
    else if(Input->Keyboard.Three.EndedDown)
    {
        Result = '3';
    }
    else if(Input->Keyboard.Four.EndedDown)
    {
        Result = '4';
    }
    else if(Input->Keyboard.Five.EndedDown)
    {
        Result = '5';
    }
    else if(Input->Keyboard.Six.EndedDown)
    {
        Result = '6';
    }
    else if(Input->Keyboard.Seven.EndedDown)
    {
        Result = '7';
    }
    else if(Input->Keyboard.Eight.EndedDown)
    {
        Result = '8';
    }
    else if(Input->Keyboard.Nine.EndedDown)
    {
        Result = '9';
    }
    
    return (Result);
}



//
// button_quad start
//


struct button_quad
{
    game_rect Quad;
    game_rect TextureQuad;
    game_texture *Texture;
};

static void
EditorMakeTextButton(game_offscreen_buffer *Buffer, game_font *Font, char* Text,
                     s32 X, s32 Y, s32 Width, s32 Height, 
                     button_quad *ButtonQuad, u8 Red, u8 Green, u8 Blue)
{
    ButtonQuad->Quad.w  = Width;
    ButtonQuad->Quad.h  = Height;
    ButtonQuad->Quad.x  = X;
    ButtonQuad->Quad.y  = Y;
    
    GameMakeTextureFromString(ButtonQuad->Texture, Text, &ButtonQuad->TextureQuad, Font, {Red, Green, Blue}, Buffer);
    
    ButtonQuad->TextureQuad.w = (ButtonQuad->TextureQuad.w < ButtonQuad->Quad.w) ? ButtonQuad->TextureQuad.w : ButtonQuad->Quad.w;
    ButtonQuad->TextureQuad.h = (ButtonQuad->TextureQuad.h < ButtonQuad->Quad.h) ? ButtonQuad->TextureQuad.h : ButtonQuad->Quad.h;
    
    ButtonQuad->TextureQuad.x = ButtonQuad->Quad.x + (ButtonQuad->Quad.w / 2) - (ButtonQuad->TextureQuad.w / 2);
    ButtonQuad->TextureQuad.y = ButtonQuad->Quad.y + (ButtonQuad->Quad.h / 2) - (ButtonQuad->TextureQuad.h / 2);
}


static void
ButtonQuadUpdateTextureOnButton(game_offscreen_buffer *Buffer, game_font *Font,
                                char *Text, button_quad *ButtonQuad, 
                                u8 Red, u8 Green, u8 Blue)
{
    GameMakeTextureFromString(ButtonQuad->Texture, Text, &ButtonQuad->TextureQuad, Font,{Red, Green, Blue}, Buffer);
    
    ButtonQuad->TextureQuad.w = (ButtonQuad->TextureQuad.w < ButtonQuad->Quad.w) ? ButtonQuad->TextureQuad.w : ButtonQuad->Quad.w;
    ButtonQuad->TextureQuad.h = (ButtonQuad->TextureQuad.h < ButtonQuad->Quad.h) ? ButtonQuad->TextureQuad.h : ButtonQuad->Quad.h;
    
    ButtonQuad->TextureQuad.x = ButtonQuad->Quad.x + (ButtonQuad->Quad.w / 2) - (ButtonQuad->TextureQuad.w / 2);
    ButtonQuad->TextureQuad.y = ButtonQuad->Quad.y + (ButtonQuad->Quad.h / 2) - (ButtonQuad->TextureQuad.h / 2);
    
}

static void
RenderButtonQuad(game_offscreen_buffer *Buffer, button_quad *ButtonQuad, 
                 u8 Red, u8 Green, u8 Blue, u8 Alpha)
{
    DEBUGRenderQuadFill(Buffer, &ButtonQuad->Quad, {Red, Green, Blue}, Alpha);
    DEBUGRenderQuad(Buffer, &ButtonQuad->Quad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, ButtonQuad->Texture, &ButtonQuad->TextureQuad);
}

//
// button_quad end
//


//
// label_button start
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

static void
InitLabel(game_font *Font, label_button *Label, char* Text,
          s32 Number, s32 X, s32 Y, s32 LabelWidth, s32 LabelHeight, 
          s32 InfoWidth, s32 BoxWidth, game_offscreen_buffer *Buffer)

{
    Label->InfoQuad = {X, Y, InfoWidth, LabelHeight};
    
    MenuMakeTextButton(Text, Label->InfoQuad.x, Label->InfoQuad.y, Label->InfoQuad.w,
                       Label->InfoQuad.h, &Label->InfoQuad, &Label->InfoTextureQuad,
                       Label->InfoTexture, Font, {255, 255, 255}, Buffer);
    
    Label->MinusQuad = {Label->InfoQuad.x + InfoWidth, Y, BoxWidth, LabelHeight};
    MenuMakeTextButton("-", Label->MinusQuad.x, Label->MinusQuad.y,
                       Label->MinusQuad.w, Label->MinusQuad.h, &Label->MinusQuad,
                       &Label->MinusTextureQuad, Label->MinusTexture, Font, {255, 255, 255}, Buffer);
    
    
    char NumberString[3] = {};
    sprintf(NumberString, "%d", Number);
    
    Label->NumberQuad = {Label->MinusQuad.x + BoxWidth, Y, BoxWidth, LabelHeight};
    MenuMakeTextButton(NumberString, Label->NumberQuad.x, Label->NumberQuad.y,
                       Label->NumberQuad.w, Label->NumberQuad.h, &Label->NumberQuad,
                       &Label->NumberTextureQuad, Label->NumberTexture, Font, {255, 255, 255}, Buffer);
    
    Label->PlusQuad = {Label->NumberQuad.x + BoxWidth, Y, BoxWidth, LabelHeight};
    MenuMakeTextButton("+", Label->PlusQuad.x, Label->PlusQuad.y,
                       Label->PlusQuad.w, Label->PlusQuad.h, &Label->PlusQuad,
                       &Label->PlusTextureQuad, Label->PlusTexture, Font, {255, 255, 255}, Buffer);
    
}


static void
UpdateLabelNumber(game_offscreen_buffer *Buffer, game_font *Font, label_button *Label, s32 Number)
{
    char NumberString[5] = {};
    sprintf(NumberString, "%d", Number);
    
    if(Label->NumberTexture)
    {
        FreeTexture(Label->NumberTexture);
    }
    
    GameMakeTextureFromString(Label->NumberTexture, NumberString, &Label->NumberTextureQuad, Font, {255, 255, 255}, Buffer);
    
    Label->NumberTextureQuad.x = Label->NumberQuad.x + (Label->NumberQuad.w / 2.0f) - (Label->NumberTextureQuad.w / 2.0f);
    
    Label->NumberTextureQuad.y = Label->NumberQuad.y + (Label->NumberQuad.h / 2.0f) - (Label->NumberTextureQuad.h / 2.0f);
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
LevelEditorRenderLabel(label_button *Label, game_offscreen_buffer *Buffer, 
                       u8 LeftR, u8 LeftG, u8 LeftB, u8 RightR, u8 RightG, u8 RightB, u8 Alpha)
{
    /* Information quad */
    DEBUGRenderQuadFill(Buffer, &Label->InfoQuad, {LeftR, LeftG, LeftB}, Alpha);
    DEBUGRenderQuad(Buffer, &Label->InfoQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, Label->InfoTexture, &Label->InfoTextureQuad);
    
    /* Minus quad */
    DEBUGRenderQuadFill(Buffer, &Label->MinusQuad, {RightR, RightG, RightB}, Alpha);
    DEBUGRenderQuad(Buffer, &Label->MinusQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, Label->MinusTexture, &Label->MinusTextureQuad);
    
    /* Number quad */
    DEBUGRenderQuadFill(Buffer, &Label->NumberQuad, {RightR, RightG, RightB}, Alpha);
    DEBUGRenderQuad(Buffer, &Label->NumberQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, Label->NumberTexture, &Label->NumberTextureQuad);
    
    /* Plus quad */
    DEBUGRenderQuadFill(Buffer, &Label->PlusQuad, {RightR, RightG, RightB}, Alpha);
    DEBUGRenderQuad(Buffer, &Label->PlusQuad, {0, 0, 0}, 255);
    GameRenderBitmapToBuffer(Buffer, Label->PlusTexture, &Label->PlusTextureQuad);
}

//
// label_button end
//

//
// level_editor start
//

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
    
    button_quad HeaderButton;
    
    button_quad LeftArrowButton;
    button_quad SwitchNameButton;
    button_quad RightArrowButton;
    
    button_quad FirstNumberNameButton;
    button_quad FirstNumberButton;
    
    button_quad SecondNumberNameButton;
    button_quad SecondNumberButton;
    
    button_quad ThirdNumberNameButton;
    button_quad ThirdNumberButton;
    
    button_quad FourthNumberNameButton;
    button_quad FourthNumberButton;
};


struct editor_object
{
    game_rect AreaQuad;
};

enum editor_type
{
    LEVEL_EDITOR,
    GAME_EDITOR,
    MENU_EDITOR
};

struct level_editor
{
    /* For switching to editor mode*/
    b32 IsOn;
    
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
    
    /* Next and Previous button data*/
    game_rect PrevLevelQuad;
    game_texture *PrevLevelTexture;
    
    game_rect NextLevelQuad;
    game_texture *NextLevelTexture;
    
    /* For highlightning the button whenever pressed */
    bool ButtonSelected;
    game_rect HighlightButtonQuad;
    
    // Layer for switching between level and game confgiuration
    button_quad SwitchConfiguration;
    editor_type EditorType;
    
    /* Level properties layer quad */
    game_rect LevelPropertiesQuad;
    
    button_quad LevelConfigButton;
    
    label_button RowLabel;
    label_button ColumnLabel;
    label_button FigureLabel;
    
    /* Figure properties layer quad */
    
    game_rect FigurePropertiesQuad;
    
    button_quad FigureConfigButton;
    button_quad RotateFigureButton;
    button_quad FlipFigureButton;
    button_quad TypeFigureButton;
    button_quad FormFigureButton;
    
    /* IOproperties layer quad */
    
    game_rect IOPropertiesQuad;
    
    button_quad IOConfigButton;
    button_quad SaveLevelButton;
    button_quad LoadLevelButton;
    
    /* Resize/Positioning routine */
    bool ShiftKeyPressed;
    cursor_type CursorType;
    
    bool AreaIsMoving;
    
    /* Position panel routine*/
    game_rect PosPanelQuad;
    position_panel PosPanel;
    
    // Objects that we actually edit
    bool ObjectIsSelected;
    s32 EditorObjectIndex;
    editor_object EditorObject[3];
};



//
// level_editor end
//

//
// menu_editor start
//

struct selection_panel
{
    s32 ButtonIndex;
    bool IsSelected;
    game_rect SelectQuad;
};

struct menu_editor
{
    /* Font for the gui */ 
    game_font *Font;
    
    /* For showing gui */
    bool EditorMode;
    bool ButtonsSelected;
    
    /* For Hiding/Showing level buttons */
    label_button LevelLabel;
    
    /* Button data for loading levels from memory*/
    button_quad LoadButton;
    
    /* Button data for saving levels on the disk */
    button_quad SaveButton;
    
    /* Button data for sorting levels in memory by their level number*/
    button_quad SortButton;
    
    /* Button data for confirming deletion of selected levels */
    button_quad DeleteButton;
    
    /* Button for swaping two selected levels */
    button_quad SwapButton;
    
    /* Button for canceling selecetion of the levels */
    button_quad CancelButton;
    
    /* Button for locking/unlocking the levels */
    button_quad LockButton;
    
    /* Button for locking/unlocking the levels */
    button_quad UnlockButton;
    
    /* Buttons for switching to next/previous 20 levels */
    s32 CurrrentLevelSlide;
    game_rect PrevButtonQuad;
    game_texture *PrevButtonTexture;
    game_rect NextButtonQuad;
    game_texture *NextButtonTexture;
    
    /* For highlightning the button whenever pressed */
    bool ButtonIsPressed;
    game_rect HighlightButtonQuad;
    
    /* For highlightning selected level buttons  */
    selection_panel SelectionPanel[100];
};

//
// menu_editor end
//


//
// resolution_editor start
//

enum scale_type
{
    WIDTH,
    HEIGHT,
    BOTH
};

enum res_type 
{
    TARGET_WIDTH,
    TARGET_HEIGHT
};

enum device_orientation
{
    LANDSCAPE, PORTRAIT
};

struct resolution_editor
{
    /* Font */
    game_font *Font;
    
    /* Orientation of the screen */
    device_orientation Orientation;
    
    /* Overal area of the panel */
    game_rect ResPanelQuad;
    
    /* Resolution Scaler button routine*/
    
    button_quad ScalerHeaderButton;
    
    button_quad LeftArrowButton;
    button_quad SwitchButton;
    button_quad RightArrowButton;
    
    /* Target resolution button routine */
    
    button_quad TargetResolutionHeaderButton;
    
    button_quad TargetWidthNameButton;
    button_quad TargetWidthNumberButton;
    
    button_quad TargetHeightNameButton;
    button_quad TargetHeightNumberButton;
    
    res_type ResolutionType;
    
    s32 TargetWidth;
    s32 TargetHeight;
    
    s32 ResOldNumber;
    s32 ResNumberBufferIndex;
    char ResNumberBuffer[5];
    b32 ResNumberSelected;
    
    button_quad ApplyButton;
    
    bool ButtonSelected;
    game_rect HighlightButtonQuad;
};


//
// resolution_editor end
//


//
// game_editor start
//


struct game_editor
{
    level_editor      LevelEditor;
    menu_editor       MenuEditor;
    resolution_editor ResEditor;
};


#define EDITOR_H
#endif