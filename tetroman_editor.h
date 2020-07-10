// editor.h --- 
// 
// Filename: editor.h
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

#if !defined(EDITOR_H)

struct playground_editor
{
    game_font *Font;
    
    b32 IsInitialized;
};

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