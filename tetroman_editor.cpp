/* ========================================= */
//     $File: tetroman_editor.cpp
//     $Date: October 9th 2017 07:32 pm 
//     $Creator: Maksim Sokolov
//     $Revision: $
//     $Description: $
/* ========================================= */

static figure_form
GetNextFigureShape(figure_form CurrentForm)
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

static void
PlaygroundEditorUpdateAndRender(playground        *Playground, 
                                playground_data   *PlaygroundData, 
                                playground_config *PlaygroundConfig,
                                playground_editor *PlaygroundEditor,
                                render_group      *RenderGroup,
                                game_input        *Input)

{
    if (Input->MouseButtons[0].EndedDown)
    {
        
        v2 MousePos = {};
        MousePos.x = Input->MouseX;
        MousePos.y = Input->MouseY;
        
        rectangle2 FigureArea    = Playground->FigureEntity.FigureArea;
        rectangle2 FigureButtons = PlaygroundEditor->FigureButtonsArea;
        rectangle2 GridArea      = Playground->GridEntity.GridArea;
        rectangle2 GridButtons   = PlaygroundEditor->GridButtonsArea;
        
        if (PlaygroundEditor->SelectedArea == selected_area::FIGURE_PLAYGROUND)
        {
            if (IsInRectangle(MousePos, FigureButtons))
            {
                printf("figure buttons\n");
                rectangle2 ButtonRectangle = {};
                
                u32 FigureIndex = PlaygroundEditor->FigureIndex;
                u32 FigureAmount = Playground->FigureEntity.FigureAmount;
                Assert(FigureIndex == 0 || FigureIndex < FigureAmount);
                
                // NOTE(msokolov): Change Shape Button
                ButtonRectangle.Min = FigureButtons.Min;
                SetDim(&ButtonRectangle, 320, 60);
                
                if (IsInRectangle(MousePos, ButtonRectangle))
                {
                    printf("Change Figure Shape\n");
                    
                    if (FigureAmount > 0)
                    {
                        figure_form NextForm = GetNextFigureShape(Playground->FigureEntity.FigureUnit[FigureIndex].Form);
                        FigureUnitInitFigure(&Playground->FigureEntity.FigureUnit[FigureIndex], NextForm,
                                             Playground->FigureEntity.FigureUnit[FigureIndex].Type);
                        FigureEntityAlignFigures(&Playground->FigureEntity);
                    }
                }
                
                // NOTE(msokolov): Change Rotation Button 
                ButtonRectangle.Min.x = FigureButtons.Min.x;
                ButtonRectangle.Min.y = FigureButtons.Min.y + 60.0f;
                SetDim(&ButtonRectangle, 320.0f, 60.0f);
                
                if (IsInRectangle(MousePos, ButtonRectangle))
                {
                    printf("Change Rotation Button\n");
                    if (FigureAmount > 0)
                    {
                        Playground->FigureEntity.FigureUnit[FigureIndex].Angle = Playground->FigureEntity.FigureUnit[FigureIndex].Angle + 90.0f;
                        FigureUnitInitFigure(&Playground->FigureEntity.FigureUnit[FigureIndex],
                                             Playground->FigureEntity.FigureUnit[FigureIndex].Form,
                                             Playground->FigureEntity.FigureUnit[FigureIndex].Type);
                        FigureEntityAlignFigures(&Playground->FigureEntity);
                    }
                }
                
                
                // NOTE(msokolov): Add Figure Button 
                ButtonRectangle.Min.x = FigureButtons.Min.x;
                ButtonRectangle.Min.y = FigureButtons.Min.y + 120.0f;
                SetDim(&ButtonRectangle, GetDim(FigureButtons).w, 60);
                
                if (IsInRectangle(MousePos, ButtonRectangle))
                {
                    printf("Add Figure Button\n");
                    
                    FigureUnitAddNewFigure(&Playground->FigureEntity, figure_form::O_figure, figure_type::classic);
                    FigureEntityAlignFigures(&Playground->FigureEntity);
                    
                    if (Playground->GridEntity.StickUnitsAmount < FIGURE_AMOUNT_MAXIMUM)
                        Playground->GridEntity.StickUnitsAmount += 1;
                }
                
                // NOTE(msokolov): Delete Figure Button
                ButtonRectangle.Min.x = FigureButtons.Min.x;
                ButtonRectangle.Min.y = FigureButtons.Min.y + 180.0f;
                SetDim(&ButtonRectangle, GetDim(FigureButtons).w, 60);
                
                if (IsInRectangle(MousePos, ButtonRectangle))
                {
                    printf("Delete Figure Button\n");
                    
                    if (FigureAmount > 0)
                    {
                        for (s32 Index = FigureIndex;
                             Index < FigureAmount;
                             ++Index)
                        {
                            Playground->FigureEntity.FigureUnit[Index] = Playground->FigureEntity.FigureUnit[Index + 1];
                        }
                        
                        Playground->FigureEntity.FigureAmount -= 1;
                        if (FigureIndex >= Playground->FigureEntity.FigureAmount && FigureIndex != 0)
                        {
                            PlaygroundEditor->FigureIndex -= 1;
                        }
                        
                        FigureEntityAlignFigures(&Playground->FigureEntity);
                    }
                }
            }
            else if(IsInRectangle(MousePos, FigureArea))
            {
                PlaygroundEditor->SelectedArea = selected_area::FIGURE_PLAYGROUND;
                
                u32 FigureAmount = Playground->FigureEntity.FigureAmount;
                u32 FigureIndex  = PlaygroundEditor->FigureIndex;
                
                for (u32 Index = 0;
                     Index < FigureAmount;
                     ++Index)
                {
                    rectangle2 FigureUnitArea = FigureUnitGetArea(&Playground->FigureEntity.FigureUnit[Index]);
                    if (IsInRectangle(MousePos, FigureUnitArea))
                    {
                        PlaygroundEditor->FigureIndex = Index;
                        break;
                    }
                }
            }
            else if(IsInRectangle(MousePos, GridArea))
            {
                PlaygroundEditor->SelectedArea = selected_area::GRID_PLAYGROUND;
            }
        }
        if (PlaygroundEditor->SelectedArea == selected_area::GRID_PLAYGROUND)
        {
            if(IsInRectangle(MousePos, GridButtons))
            {
                rectangle2 ButtonRectangle = {};
                
                // NOTE(msokolov): Add Row in Grid
                ButtonRectangle.Min = PlaygroundEditor->GridButtonsArea.Min;
                SetDim(&ButtonRectangle, 60, 60);
                if(IsInRectangle(MousePos, ButtonRectangle))
                {
                    if (Playground->GridEntity.RowAmount < ROW_AMOUNT_MAXIMUM)
                        Playground->GridEntity.RowAmount++;
                }
                
                // NOTE(msokolov): Delete Row in Grid
                ButtonRectangle.Min.x += 60.0f;
                SetDim(&ButtonRectangle, 60, 60);
                if(IsInRectangle(MousePos, ButtonRectangle))
                {
                    if (Playground->GridEntity.RowAmount > 0)
                        Playground->GridEntity.RowAmount--;
                }
                
                // NOTE(msokolov): Add Column in Grid
                ButtonRectangle.Min.x -= 60.0f;
                ButtonRectangle.Min.y += 60.0f;
                SetDim(&ButtonRectangle, 60, 60);
                if(IsInRectangle(MousePos, ButtonRectangle))
                {
                    if (Playground->GridEntity.ColumnAmount < COLUMN_AMOUNT_MAXIMUM)
                        Playground->GridEntity.ColumnAmount++;
                }
                
                // NOTE(msokolov): Delete Column in Grid
                ButtonRectangle.Min.x += 60.0f;
                SetDim(&ButtonRectangle, 60, 60);
                if(IsInRectangle(MousePos, ButtonRectangle))
                {
                    if (Playground->GridEntity.ColumnAmount > 0)
                        Playground->GridEntity.ColumnAmount--;
                }
                
            }
            else if(IsInRectangle(MousePos, GridArea))
            {
                PlaygroundEditor->SelectedArea = selected_area::GRID_PLAYGROUND;
            }
            else if (IsInRectangle(MousePos, FigureArea))
            {
                PlaygroundEditor->SelectedArea = selected_area::FIGURE_PLAYGROUND;
            }
        }
    }
    
    switch(PlaygroundEditor->SelectedArea)
    {
        case FIGURE_PLAYGROUND:
        {
            rectangle2 FigureArea = Playground->FigureEntity.FigureArea;
            
            v4 Color = {255, 255, 255, 50};
            PushRectangle(RenderGroup, FigureArea, Color);
            
            v2 StartingPosition = {};
            rectangle2 TempRectangle = {};
            v2 Dim = {};
            
            //
            //
            // NOTE(msokolov): change form of a figure 
            //
            //
            
            StartingPosition.x = PlaygroundEditor->FigureButtonsArea.Min.x;
            StartingPosition.y = PlaygroundEditor->FigureButtonsArea.Min.y;
            
            /* Change Shape Button */
            TempRectangle.Min.x = StartingPosition.x;
            TempRectangle.Min.y = StartingPosition.y;
            SetDim(&TempRectangle, 180.0f, 60.0f);
            
            Color = {0, 0, 255, 255};
            PushRectangle(RenderGroup, TempRectangle, Color);
            Color = {255, 255, 255, 255};
            PushRectangleOutline(RenderGroup, TempRectangle, Color);
            
            Dim = QueryTextureDim(PlaygroundEditor->FigureFormTexture);
            TempRectangle.Min.x = TempRectangle.Min.x + (GetDim(TempRectangle).w / 2.0f) - (Dim.w / 2.0f); 
            TempRectangle.Min.y = TempRectangle.Min.y + (GetDim(TempRectangle).h / 2.0f) - (Dim.h / 2.0f); 
            SetDim(&TempRectangle, Dim);
            PushBitmap(RenderGroup, PlaygroundEditor->FigureFormTexture, TempRectangle);
            
            //
            //
            // NOTE(msokolov): change rotation of a figure 
            //
            //
            
            StartingPosition.x = PlaygroundEditor->FigureButtonsArea.Min.x;
            StartingPosition.y = PlaygroundEditor->FigureButtonsArea.Min.y + 60.0f;
            
            /* Change Rotation Button */
            TempRectangle.Min.x = StartingPosition.x;
            TempRectangle.Min.y = StartingPosition.y;
            SetDim(&TempRectangle, 180.0f, 60);
            
            Color = {0, 0, 255, 255};
            PushRectangle(RenderGroup, TempRectangle, Color);
            Color = {255, 255, 255, 255};
            PushRectangleOutline(RenderGroup, TempRectangle, Color);
            
            Dim = QueryTextureDim(PlaygroundEditor->FigureRotateTexture);
            TempRectangle.Min.x = TempRectangle.Min.x + (GetDim(TempRectangle).w / 2.0f) - (Dim.w / 2.0f); 
            TempRectangle.Min.y = TempRectangle.Min.y + (GetDim(TempRectangle).h / 2.0f) - (Dim.h / 2.0f); 
            SetDim(&TempRectangle, Dim);
            PushBitmap(RenderGroup, PlaygroundEditor->FigureRotateTexture, TempRectangle);
            
            //
            //
            // NOTE(msokolov): Add a figure 
            //
            //
            
            StartingPosition.x = PlaygroundEditor->FigureButtonsArea.Min.x;
            StartingPosition.y = PlaygroundEditor->FigureButtonsArea.Min.y + 120.0f;
            
            /* left arrow button */
            TempRectangle = {};
            TempRectangle.Min = StartingPosition;
            SetDim(&TempRectangle, 180, 60);
            
            Color = {0, 0, 255, 255};
            PushRectangle(RenderGroup, TempRectangle, Color);
            Color = {255, 255, 255, 255};
            PushRectangleOutline(RenderGroup, TempRectangle, Color);
            
            Dim = QueryTextureDim(PlaygroundEditor->FigureAddTexture);
            TempRectangle.Min.x = TempRectangle.Min.x + (GetDim(TempRectangle).w / 2.0f) - (Dim.w / 2.0f); 
            TempRectangle.Min.y = TempRectangle.Min.y + (GetDim(TempRectangle).h / 2.0f) - (Dim.h / 2.0f); 
            SetDim(&TempRectangle, Dim);
            PushBitmap(RenderGroup, PlaygroundEditor->FigureAddTexture, TempRectangle);
            
            //
            //
            // NOTE(msokolov): Delete a figure 
            //
            //
            
            StartingPosition.x = PlaygroundEditor->FigureButtonsArea.Min.x;
            StartingPosition.y = PlaygroundEditor->FigureButtonsArea.Min.y + 180.0f;
            
            /* left arrow button */
            TempRectangle = {};
            TempRectangle.Min = StartingPosition;
            SetDim(&TempRectangle, 180, 60);
            
            Color = {0, 0, 255, 255};
            PushRectangle(RenderGroup, TempRectangle, Color);
            Color = {255, 255, 255, 255};
            PushRectangleOutline(RenderGroup, TempRectangle, Color);
            
            Dim = QueryTextureDim(PlaygroundEditor->FigureDeleteTexture);
            TempRectangle.Min.x = TempRectangle.Min.x + (GetDim(TempRectangle).w / 2.0f) - (Dim.w / 2.0f); 
            TempRectangle.Min.y = TempRectangle.Min.y + (GetDim(TempRectangle).h / 2.0f) - (Dim.h / 2.0f); 
            SetDim(&TempRectangle, Dim);
            PushBitmap(RenderGroup, PlaygroundEditor->FigureDeleteTexture, TempRectangle);
            
            PushRectangleOutline(RenderGroup, PlaygroundEditor->FigureButtonsArea, {255, 0, 0, 255});
            
            
            //
            //
            // NOTE(msokolov): Highlighting a figure
            //
            //
            
            u32 FigureIndex = PlaygroundEditor->FigureIndex;
            u32 FigureAmount = Playground->FigureEntity.FigureAmount;
            Assert(FigureIndex == 0 || FigureIndex < FigureAmount);
            
            if (FigureAmount > 0)
            {
                for (u32 Index = 0;
                     Index < FIGURE_BLOCKS_MAXIMUM;
                     ++Index)
                {
                    v2 ShellPosition = Playground->FigureEntity.FigureUnit[FigureIndex].Shell[Index];
                    
                    rectangle2 ShellRectangle = {};
                    ShellRectangle.Min.x = ShellPosition.x - (IDLE_BLOCK_SIZE / 2.0f);
                    ShellRectangle.Min.y = ShellPosition.y - (IDLE_BLOCK_SIZE / 2.0f);
                    SetDim(&ShellRectangle, IDLE_BLOCK_SIZE, IDLE_BLOCK_SIZE);
                    
                    v4 Color = {0, 255, 255, 100};
                    PushRectangle(RenderGroup, ShellRectangle, Color);
                }
            }
            
            
            
        } break;
        
        case GRID_PLAYGROUND:
        {
            rectangle2 GridArea = Playground->GridEntity.GridArea;
            
            v4 Color = {255, 255, 255, 50};
            PushRectangle(RenderGroup, GridArea, Color);
            
            PushRectangleOutline(RenderGroup, PlaygroundEditor->GridButtonsArea, {255, 0, 0, 255});
            
            v2 StartingPosition = {};
            rectangle2 TempRectangle = {};
            v2 Dim = {};
            
            StartingPosition.x = PlaygroundEditor->GridButtonsArea.Min.x;
            StartingPosition.y = PlaygroundEditor->GridButtonsArea.Min.y;
            
            /* + */
            TempRectangle.Min.x = StartingPosition.x;
            TempRectangle.Min.y = StartingPosition.y;
            SetDim(&TempRectangle, 60.0f, 60.0f);
            
            Color = {0, 0, 255, 255};
            PushRectangle(RenderGroup, TempRectangle, Color);
            Color = {255, 255, 255, 255};
            PushRectangleOutline(RenderGroup, TempRectangle, Color);
            
            Dim = QueryTextureDim(PlaygroundEditor->GridPlusTexture);
            TempRectangle.Min.x = TempRectangle.Min.x + (GetDim(TempRectangle).w / 2.0f) - (Dim.w / 2.0f); 
            TempRectangle.Min.y = TempRectangle.Min.y + (GetDim(TempRectangle).h / 2.0f) - (Dim.h / 2.0f); 
            SetDim(&TempRectangle, Dim);
            PushBitmap(RenderGroup, PlaygroundEditor->GridPlusTexture, TempRectangle);
            
            /* - */
            TempRectangle.Min.x = StartingPosition.x + 60.0f;
            TempRectangle.Min.y = StartingPosition.y;
            SetDim(&TempRectangle, 60.0f, 60.0f);
            
            Color = {0, 0, 255, 255};
            PushRectangle(RenderGroup, TempRectangle, Color);
            Color = {255, 255, 255, 255};
            PushRectangleOutline(RenderGroup, TempRectangle, Color);
            
            Dim = QueryTextureDim(PlaygroundEditor->GridMinusTexture);
            TempRectangle.Min.x = TempRectangle.Min.x + (GetDim(TempRectangle).w / 2.0f) - (Dim.w / 2.0f); 
            TempRectangle.Min.y = TempRectangle.Min.y + (GetDim(TempRectangle).h / 2.0f) - (Dim.h / 2.0f); 
            SetDim(&TempRectangle, Dim);
            PushBitmap(RenderGroup, PlaygroundEditor->GridMinusTexture, TempRectangle);
            
            /* Row */
            TempRectangle.Min.x = StartingPosition.x + 120.0f;
            TempRectangle.Min.y = StartingPosition.y;
            SetDim(&TempRectangle, 180.0f, 60.0f);
            
            Color = {0, 0, 255, 255};
            PushRectangle(RenderGroup, TempRectangle, Color);
            Color = {255, 255, 255, 255};
            PushRectangleOutline(RenderGroup, TempRectangle, Color);
            
            Dim = QueryTextureDim(PlaygroundEditor->GridRowTexture);
            TempRectangle.Min.x = TempRectangle.Min.x + (GetDim(TempRectangle).w / 2.0f) - (Dim.w / 2.0f); 
            TempRectangle.Min.y = TempRectangle.Min.y + (GetDim(TempRectangle).h / 2.0f) - (Dim.h / 2.0f); 
            SetDim(&TempRectangle, Dim);
            PushBitmap(RenderGroup, PlaygroundEditor->GridRowTexture, TempRectangle);
            
            StartingPosition.x = PlaygroundEditor->GridButtonsArea.Min.x;
            StartingPosition.y = PlaygroundEditor->GridButtonsArea.Min.y + 60.0f;
            
            /* + */
            TempRectangle.Min.x = StartingPosition.x;
            TempRectangle.Min.y = StartingPosition.y;
            SetDim(&TempRectangle, 60.0f, 60.0f);
            
            Color = {0, 0, 255, 255};
            PushRectangle(RenderGroup, TempRectangle, Color);
            Color = {255, 255, 255, 255};
            PushRectangleOutline(RenderGroup, TempRectangle, Color);
            
            Dim = QueryTextureDim(PlaygroundEditor->GridPlusTexture);
            TempRectangle.Min.x = TempRectangle.Min.x + (GetDim(TempRectangle).w / 2.0f) - (Dim.w / 2.0f); 
            TempRectangle.Min.y = TempRectangle.Min.y + (GetDim(TempRectangle).h / 2.0f) - (Dim.h / 2.0f); 
            SetDim(&TempRectangle, Dim);
            PushBitmap(RenderGroup, PlaygroundEditor->GridPlusTexture, TempRectangle);
            
            /* - */
            TempRectangle.Min.x = StartingPosition.x + 60.0f;
            TempRectangle.Min.y = StartingPosition.y;
            SetDim(&TempRectangle, 60.0f, 60.0f);
            
            Color = {0, 0, 255, 255};
            PushRectangle(RenderGroup, TempRectangle, Color);
            Color = {255, 255, 255, 255};
            PushRectangleOutline(RenderGroup, TempRectangle, Color);
            
            Dim = QueryTextureDim(PlaygroundEditor->GridMinusTexture);
            TempRectangle.Min.x = TempRectangle.Min.x + (GetDim(TempRectangle).w / 2.0f) - (Dim.w / 2.0f); 
            TempRectangle.Min.y = TempRectangle.Min.y + (GetDim(TempRectangle).h / 2.0f) - (Dim.h / 2.0f); 
            SetDim(&TempRectangle, Dim);
            PushBitmap(RenderGroup, PlaygroundEditor->GridMinusTexture, TempRectangle);
            
            /* Column */
            TempRectangle.Min.x = StartingPosition.x + 120.0f;
            TempRectangle.Min.y = StartingPosition.y;
            SetDim(&TempRectangle, 180.0f, 60.0f);
            
            Color = {0, 0, 255, 255};
            PushRectangle(RenderGroup, TempRectangle, Color);
            Color = {255, 255, 255, 255};
            PushRectangleOutline(RenderGroup, TempRectangle, Color);
            
            Dim = QueryTextureDim(PlaygroundEditor->GridColumnTexture);
            TempRectangle.Min.x = TempRectangle.Min.x + (GetDim(TempRectangle).w / 2.0f) - (Dim.w / 2.0f); 
            TempRectangle.Min.y = TempRectangle.Min.y + (GetDim(TempRectangle).h / 2.0f) - (Dim.h / 2.0f); 
            SetDim(&TempRectangle, Dim);
            PushBitmap(RenderGroup, PlaygroundEditor->GridColumnTexture, TempRectangle);
            
        } break;
    }
}

#if 0
static void
EditorMakeTextButton(game_offscreen_buffer *Buffer, game_font *Font, const char* Text,
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
                                const char *Text, button_quad *ButtonQuad, 
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


static void
InitLabel(game_font *Font, label_button *Label, const char* Text,
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

static void
LevelEditorUpdateTextOnButton(game_offscreen_buffer *Buffer, game_font *&Font, const char *TextBuffer, game_texture *&Texture, game_rect *TextureQuad, game_rect *AreaQuad, game_color Color)
{
    GameMakeTextureFromString(Texture, TextBuffer, TextureQuad, Font, Color, Buffer);
    
    TextureQuad->x = AreaQuad->x + (AreaQuad->w / 2) - (TextureQuad->w / 2);
    TextureQuad->y = AreaQuad->y + (AreaQuad->h / 2) - (TextureQuad->h / 2);
}


static void
LevelEditorUpdateCoordinates(game_offscreen_buffer *Buffer, game_font *Font, position_panel *PosPanel,
                             game_rect GameArea, game_rect ScreenArea)
{
    game_rect Rectangle = {};
    
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
    
    LevelEditorUpdateTextOnButton(Buffer, Font, FirstNumberBuffer, PosPanel->FirstNumberButton.Texture, &PosPanel->FirstNumberButton.TextureQuad, &PosPanel->FirstNumberButton.Quad, {255, 255, 255});
    
    LevelEditorUpdateTextOnButton(Buffer, Font, SecondNumberBuffer, PosPanel->SecondNumberButton.Texture, &PosPanel->SecondNumberButton.TextureQuad, &PosPanel->SecondNumberButton.Quad, {255, 255, 255});
    
    LevelEditorUpdateTextOnButton(Buffer, Font, ThirdNumberBuffer, PosPanel->ThirdNumberButton.Texture, &PosPanel->ThirdNumberButton.TextureQuad, &PosPanel->ThirdNumberButton.Quad, {255, 255, 255});
    
    LevelEditorUpdateTextOnButton(Buffer, Font, FourthNumberBuffer, PosPanel->FourthNumberButton.Texture, &PosPanel->FourthNumberButton.TextureQuad, &PosPanel->FourthNumberButton.Quad, {255, 255, 255});
}

static void
LevelEditorUpdateCoordinateSwitch(level_editor *LevelEditor, position_panel *PosPanel, game_offscreen_buffer *Buffer)
{
    coordinate_type CoordType = PosPanel->CoordType;
    if(CoordType == PIXEL_AREA)
    {
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Pixels", LevelEditor->PosPanel.SwitchNameButton.Texture, &LevelEditor->PosPanel.SwitchNameButton.TextureQuad, &LevelEditor->PosPanel.SwitchNameButton.Quad, {255, 255, 255});
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "X", LevelEditor->PosPanel.FirstNumberNameButton.Texture, &LevelEditor->PosPanel.FirstNumberNameButton.TextureQuad, &LevelEditor->PosPanel.FirstNumberNameButton.Quad, {255, 255, 255});
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Y", LevelEditor->PosPanel.SecondNumberNameButton.Texture, &LevelEditor->PosPanel.SecondNumberNameButton.TextureQuad, &LevelEditor->PosPanel.SecondNumberNameButton.Quad, {255, 255, 255});
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Width", LevelEditor->PosPanel.ThirdNumberNameButton.Texture, &LevelEditor->PosPanel.ThirdNumberNameButton.TextureQuad, &LevelEditor->PosPanel.ThirdNumberNameButton.Quad, {255, 255, 255});
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Height", LevelEditor->PosPanel.FourthNumberNameButton.Texture, &LevelEditor->PosPanel.FourthNumberNameButton.TextureQuad, &LevelEditor->PosPanel.FourthNumberNameButton.Quad, {255, 255, 255});
    }
    else if(CoordType == GAME_AREA || CoordType == SCREEN_AREA)
    {
        if(CoordType == GAME_AREA)
        {
            LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "% game_area", LevelEditor->PosPanel.SwitchNameButton.Texture, &LevelEditor->PosPanel.SwitchNameButton.TextureQuad, &LevelEditor->PosPanel.SwitchNameButton.Quad, {255, 255, 255});
        }
        else
        {
            LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "% screen_area", LevelEditor->PosPanel.SwitchNameButton.Texture, &LevelEditor->PosPanel.SwitchNameButton.TextureQuad, &LevelEditor->PosPanel.SwitchNameButton.Quad, {255, 255, 255});
        }
        
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Left", LevelEditor->PosPanel.FirstNumberNameButton.Texture, &LevelEditor->PosPanel.FirstNumberNameButton.TextureQuad, &LevelEditor->PosPanel.FirstNumberNameButton.Quad, {255, 255, 255});
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Top", LevelEditor->PosPanel.SecondNumberNameButton.Texture, &LevelEditor->PosPanel.SecondNumberNameButton.TextureQuad, &LevelEditor->PosPanel.SecondNumberNameButton.Quad, {255, 255, 255});
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Right", LevelEditor->PosPanel.ThirdNumberNameButton.Texture, &LevelEditor->PosPanel.ThirdNumberNameButton.TextureQuad, &LevelEditor->PosPanel.ThirdNumberNameButton.Quad, {255, 255, 255});
        
        LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "Bottom", LevelEditor->PosPanel.FourthNumberNameButton.Texture, &LevelEditor->PosPanel.FourthNumberNameButton.TextureQuad, &LevelEditor->PosPanel.FourthNumberNameButton.Quad, {255, 255, 255});
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
    
    char LevelIndexString[3]  = {};
    
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
}

static void
LevelEditorDeleteMovingBlock(grid_entity *GridEntity, u32 Index)
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
LevelEditorNewGrid(game_offscreen_buffer *Buffer, playground *LevelEntity, 
                   s32 NewRowAmount, s32 NewColumnAmount, level_editor *LevelEditor)
{
    if(NewRowAmount < 0 || NewColumnAmount < 0) return;
    
    grid_entity *GridEntity = &LevelEntity->GridEntity;
    
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
    
    GridEntity->UnitField    = UnitField;
    GridEntity->RowAmount    = NewRowAmount;
    GridEntity->ColumnAmount = NewColumnAmount;
    
    u32 FigureBlockSize = LevelEntity->Configuration.InActiveBlockSize;
    
    LevelEntity->Configuration.GridBlockSize = CalculateGridBlockSize(NewRowAmount, NewColumnAmount, LevelEntity->GridEntity.GridArea.w, LevelEntity->GridEntity.GridArea.h);
    
    s32 GridBlockSize = LevelEntity->Configuration.GridBlockSize;
    
    for(u32 i = 0; i < LevelEntity->GridEntity.MovingBlocksAmount; ++i)
    {
        u32 RowNumber = LevelEntity->GridEntity.MovingBlocks[i].RowNumber;
        u32 ColNumber = LevelEntity->GridEntity.MovingBlocks[i].ColNumber;
        
        if(RowNumber >= NewRowAmount || ColNumber >= NewColumnAmount)
        {
            LevelEditorDeleteMovingBlock(GridEntity, i);
        }
    }
    
    for(u32 i = 0; i < LevelEntity->GridEntity.MovingBlocksAmount; ++i)
    {
        u32 RowNumber = LevelEntity->GridEntity.MovingBlocks[i].RowNumber;
        u32 ColNumber = LevelEntity->GridEntity.MovingBlocks[i].ColNumber;
        
        GridEntity->MovingBlocks[i].AreaQuad.w = GridBlockSize;
        GridEntity->MovingBlocks[i].AreaQuad.h = GridBlockSize;
        GridEntity->MovingBlocks[i].AreaQuad.x = GridEntity->GridArea.x + (ColNumber * GridBlockSize);
        GridEntity->MovingBlocks[i].AreaQuad.y = GridEntity->GridArea.y + (RowNumber * GridBlockSize);
    }
    
}

static void
GameConfigUpdateAndRender(level_editor *LevelEditor, playground *LevelEntity,
                          game_memory *Memory, game_offscreen_buffer *Buffer,
                          game_input *Input)
{
    s32 RowAmount = LevelEntity->GridEntity.RowAmount;
    s32 ColAmount = LevelEntity->GridEntity.ColumnAmount;
    
    game_rect GridArea = LevelEntity->GridEntity.GridArea;
    
    
    if(Input->Keyboard.LeftShift.EndedDown)
    {
        if(!LevelEditor->ShiftKeyPressed)
        {
            LevelEditor->ShiftKeyPressed = true;
            printf("Shift is pressed!\n");
        } 
    }
    else if(Input->Keyboard.LeftShift.EndedUp)
    {
        if(LevelEditor->ShiftKeyPressed)
        {
            LevelEditor->ShiftKeyPressed = false;
            printf("Shift is released!\n");
        }
    }
    
    if(Input->MouseButtons[0].EndedDown)
    {
        
        if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->SwitchConfiguration.Quad))
        {
            LevelEditor->EditorType = LEVEL_EDITOR;
            LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "< Level configuration >", LevelEditor->SwitchConfiguration.Texture, &LevelEditor->SwitchConfiguration.TextureQuad, &LevelEditor->SwitchConfiguration.Quad, {255, 255, 255});
            
            LevelEditor->CursorType = ARROW;
            
            LevelEditor->ButtonSelected = true;
            LevelEditor->HighlightButtonQuad = LevelEditor->SwitchConfiguration.Quad;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->PosPanelQuad))
        {
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->PosPanel.LeftArrowButton.Quad))
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
                
                LevelEditorUpdateCoordinates(Buffer, LevelEditor->Font, &LevelEditor->PosPanel, LevelEntity->GridEntity.GridArea, 
                                             {0, 0, Buffer->Width, Buffer->Height});
                LevelEditorUpdateCoordinateSwitch(LevelEditor, &LevelEditor->PosPanel, Buffer);
                
                LevelEditor->ButtonSelected      = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->PosPanel.LeftArrowButton.Quad;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->PosPanel.RightArrowButton.Quad))
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
                
                LevelEditorUpdateCoordinates(Buffer, LevelEditor->Font, &LevelEditor->PosPanel, LevelEntity->GridEntity.GridArea, 
                                             {0, 0, Buffer->Width, Buffer->Height});
                LevelEditorUpdateCoordinateSwitch(LevelEditor, &LevelEditor->PosPanel, Buffer);
                
                LevelEditor->ButtonSelected      = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->PosPanel.RightArrowButton.Quad;
            }
            
        }
        else
        {
            
            if(LevelEditor->CursorType != ARROW)
            {
                if(LevelEditor->ObjectIsSelected)
                {
                    LevelEditor->AreaIsMoving = true;
                }
            }
            else
            {
                for(s32 i = 0; i < 3; ++i)
                {
                    if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->EditorObject[i].AreaQuad))
                    {
                        LevelEditor->ObjectIsSelected = true;
                        LevelEditor->EditorObjectIndex = i;
                        break;
                    }
                }
            }
            
            
            if(LevelEditor->ObjectIsSelected)
            {
                game_rect SelectedArea = {};
                game_rect ScreenArea = {0, 0, Buffer->Width, Buffer->Height};
                
                if(LevelEditor->EditorObjectIndex == 0)
                {
                    SelectedArea = LevelEntity->LevelNumberQuad;
                }
                else if(LevelEditor->EditorObjectIndex == 1)
                {
                    SelectedArea = LevelEntity->FigureEntity.FigureArea;
                }
                else if(LevelEditor->EditorObjectIndex == 2)
                {
                    SelectedArea = LevelEntity->GridEntity.GridArea;
                }
                
                LevelEditorUpdateCoordinates(Buffer, LevelEditor->Font, &LevelEditor->PosPanel, SelectedArea,ScreenArea);
            }
            
            
        }
        
    }
    else if(Input->MouseButtons[0].EndedUp)
    {
        LevelEditor->ButtonSelected      = true;
        LevelEditor->HighlightButtonQuad = {};
        
        if(LevelEditor->AreaIsMoving)
        {
            LevelEditor->CursorType = ARROW;
            LevelEditor->AreaIsMoving = false;
        }
    }
    
    if(LevelEditor->ObjectIsSelected && !LevelEditor->AreaIsMoving)
    {
        // check if the mouse is inside the panel first
        // if not then look if we need to change the cursor type
        
        game_rect PanelArea = {};
        
        PanelArea.x = LevelEditor->LevelPropertiesQuad.x;
        PanelArea.y = LevelEditor->LevelPropertiesQuad.y;
        PanelArea.w = LevelEditor->LevelPropertiesQuad.w;
        PanelArea.h = LevelEditor->SwitchConfiguration.Quad.h + LevelEditor->PosPanelQuad.h;
        
        if(IsPointInsideRect(Input->MouseX, Input->MouseY, &PanelArea))
        {
            if(LevelEditor->CursorType != ARROW)
            {
                LevelEditor->CursorType = ARROW;
                LevelEditorSetCursorType(ARROW);
            }
        }
        else
        {
            s32 Index = LevelEditor->EditorObjectIndex;
            game_rect AreaQuad = LevelEditor->EditorObject[Index].AreaQuad;
            
            game_rect Corner[4] = {};
            
            Corner[0] = {AreaQuad.x - 10, AreaQuad.y - 10, 20, 20};
            Corner[1] = {(AreaQuad.x + AreaQuad.w) - 10, AreaQuad.y - 10, 20, 20};
            Corner[2] = {(AreaQuad.x + AreaQuad.w) - 10, (AreaQuad.y + AreaQuad.h) - 10, 20, 20};
            Corner[3] = {AreaQuad.x - 10, (AreaQuad.y + AreaQuad.h), 20, 20};
            
            cursor_type CursorType = ARROW;
            
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
                
                Border[0] = {AreaQuad.x, AreaQuad.y - 10, AreaQuad.w, 20};
                Border[1] = {(AreaQuad.x + AreaQuad.w) - 10, AreaQuad.y, 20, AreaQuad.h};
                Border[2] = {AreaQuad.x, (AreaQuad.y + AreaQuad.h) - 10, AreaQuad.w, 20};
                Border[3] = {AreaQuad.x - 10, AreaQuad.y, 20, AreaQuad.h};
                
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
                else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &AreaQuad))
                {
                    CursorType = SIZE_ALL;
                }
            }
            
            if(CursorType != LevelEditor->CursorType)
            {
                LevelEditor->CursorType = CursorType;
                LevelEditorSetCursorType(CursorType);
            }
        }
    }
    
    if(LevelEditor->AreaIsMoving)
    {
        s32 OffsetX = Input->MouseRelX;
        s32 OffsetY = Input->MouseRelY;
        
        s32 Index = LevelEditor->EditorObjectIndex;
        
        math_rect AreaQuad = ConvertGameRectToMathRect(LevelEditor->EditorObject[Index].AreaQuad);
        
        switch(LevelEditor->CursorType)
        {
            case ARROW:
            {
                
            } break;
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
        
        if(Index == 0)
        {
            LevelEditor->EditorObject[Index].AreaQuad = ConvertMathRectToGameRect(AreaQuad);
            
            LevelEntity->LevelNumberQuad = LevelEditor->EditorObject[Index].AreaQuad;
            LevelEntity->LevelNumberShadowQuad = LevelEditor->EditorObject[Index].AreaQuad;
            
        }
        else if(Index == 1)
        {
            LevelEditor->EditorObject[Index].AreaQuad = ConvertMathRectToGameRect(AreaQuad);
            
            LevelEntity->FigureEntity.FigureArea = LevelEditor->EditorObject[Index].AreaQuad;
            
            s32 FigureBlockSize = 0;
            s32 OldFigureBlockSize = LevelEntity->Configuration.InActiveBlockSize;
            
            if(Buffer->Width > Buffer->Height)
            {
                FigureBlockSize = 
                    CalculateFigureBlockSizeByWidth(LevelEntity->FigureEntity.FigureAmount, LevelEntity->FigureEntity.FigureArea.w);
            }
            else
            {
                FigureBlockSize = 
                    CalculateFigureBlockSizeByHeight(LevelEntity->FigureEntity.FigureAmount, 
                                                     LevelEntity->FigureEntity.FigureArea.h);
            }
            
            LevelEntity->Configuration.InActiveBlockSize = FigureBlockSize;
            
            r32 BlockRatio = (r32)FigureBlockSize / (r32)OldFigureBlockSize;
            s32 FigureAmount = LevelEntity->FigureEntity.FigureAmount;
            for(s32 i = 0; i < FigureAmount; ++i)
            {
                FigureUnitResizeBy(&LevelEntity->FigureEntity.FigureUnit[i], BlockRatio);
            }
            
            if(Buffer->Width > Buffer->Height)
            {
                FigureEntityAlignFigures(&LevelEntity->FigureEntity, FigureBlockSize);
            }
            
            LevelEditorUpdateCoordinates(Buffer, LevelEditor->Font, &LevelEditor->PosPanel, LevelEntity->FigureEntity.FigureArea, 
                                         {0, 0, Buffer->Width, Buffer->Height});
            
        }
        else if(Index == 2)
        {
            game_rect GridArea = ConvertMathRectToGameRect(AreaQuad);
            
            LevelEditor->EditorObject[Index].AreaQuad = GridArea;
            LevelEntity->GridEntity.GridArea = GridArea;
            
            LevelEntity->Configuration.GridBlockSize = CalculateGridBlockSize(RowAmount, ColAmount, GridArea.w, GridArea.h);
            
            LevelEditorUpdateCoordinates(Buffer, LevelEditor->Font, &LevelEditor->PosPanel, LevelEntity->GridEntity.GridArea, 
                                         {0, 0, Buffer->Width, Buffer->Height});
        }
        
    }
    
    if(LevelEditor->ObjectIsSelected)
    {
        s32 Index = LevelEditor->EditorObjectIndex;
        DEBUGRenderQuad(Buffer, &LevelEditor->EditorObject[Index].AreaQuad, {255, 255, 0}, 255);
        DEBUGRenderQuadFill(Buffer, &LevelEditor->EditorObject[Index].AreaQuad, {255, 255, 255}, 50);
    }
    
    for(s32 i = 0; i < 3;++i)
    {
        if(i != LevelEditor->EditorObjectIndex)
        {
            DEBUGRenderQuad(Buffer, &LevelEditor->EditorObject[i].AreaQuad, {255, 255, 255}, 255);
        }
    }
    
    RenderButtonQuad(Buffer, &LevelEditor->SwitchConfiguration, 255, 192, 203, 100);
    
    /* Position panel rendering */
    
    RenderButtonQuad(Buffer, &LevelEditor->PosPanel.HeaderButton,  128, 128, 128, 100);
    
    /* Switch buttons rendering */
    RenderButtonQuad(Buffer, &LevelEditor->PosPanel.LeftArrowButton,   0, 255, 0, 100);
    RenderButtonQuad(Buffer, &LevelEditor->PosPanel.SwitchNameButton,  0, 255, 0, 100);
    RenderButtonQuad(Buffer, &LevelEditor->PosPanel.RightArrowButton,  0, 255, 0, 100);
    
    /* First coordinate buttons rendering */
    
    RenderButtonQuad(Buffer, &LevelEditor->PosPanel.FirstNumberNameButton, 0, 0, 255, 100);
    RenderButtonQuad(Buffer, &LevelEditor->PosPanel.FirstNumberButton, 0, 0, 255, 100);
    
    /* Second coordinate buttons rendering */
    
    RenderButtonQuad(Buffer, &LevelEditor->PosPanel.SecondNumberNameButton,  0, 0, 255, 100);
    RenderButtonQuad(Buffer, &LevelEditor->PosPanel.SecondNumberButton, 0, 0, 255, 100);
    
    /*Third coordinate buttons rendering */
    
    RenderButtonQuad(Buffer, &LevelEditor->PosPanel.ThirdNumberNameButton, 0, 0, 255, 100);
    RenderButtonQuad(Buffer, &LevelEditor->PosPanel.ThirdNumberButton, 0, 0, 255, 100);
    
    /*Fourth coordinate buttons rendering*/
    
    RenderButtonQuad(Buffer, &LevelEditor->PosPanel.FourthNumberNameButton,  0, 0, 255, 100);
    RenderButtonQuad(Buffer, &LevelEditor->PosPanel.FourthNumberButton,  0, 0, 255, 100);
    
    if(LevelEditor->ButtonSelected)
    {
        DEBUGRenderQuadFill(Buffer, &LevelEditor->HighlightButtonQuad, {255, 255, 0}, 150);
    }
    
}

static void
LevelConfigUpdateAndRender(level_editor *LevelEditor, playground *LevelEntity, 
                           game_memory *Memory, game_offscreen_buffer *Buffer, game_input *Input)
{
    game_rect FigureArea = LevelEntity->FigureEntity.FigureArea;
    
    s32 FigureAmount   = LevelEntity->FigureEntity.FigureAmount;
    s32 RowAmount      = LevelEntity->GridEntity.RowAmount;
    s32 ColAmount      = LevelEntity->GridEntity.ColumnAmount;
    
    s32 GridBlockSize  = LevelEntity->Configuration.GridBlockSize;
    
    s32 ActualGridWidth  = ColAmount * GridBlockSize;
    s32 ActualGridHeight = RowAmount * GridBlockSize;
    
    game_rect GridArea = {};
    GridArea.w = ActualGridWidth;
    GridArea.h = ActualGridHeight;
    GridArea.x = LevelEntity->GridEntity.GridArea.x + (LevelEntity->GridEntity.GridArea.w / 2) - (ActualGridWidth / 2);
    GridArea.y = LevelEntity->GridEntity.GridArea.y + (LevelEntity->GridEntity.GridArea.h / 2) - (ActualGridHeight / 2);
    
    s32 NewFigureIndex = LevelEditor->CurrentFigureIndex;
    
    s32 CurrentLevelIndex = 0;//Memory->CurrentLevelIndex;
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
        s32 DigitIndex = LevelEditor->LevelNumberBufferIndex;
        char NextDigit = GetNumberFromInput(DigitIndex, Input);
        
        if(NextDigit)
        {
            if(DigitIndex <= 2 && NextDigit != '\n')
            {
                LevelEditor->LevelNumberBuffer[DigitIndex] = NextDigit;
                LevelEditor->LevelNumberBufferIndex = ++DigitIndex;
                
                game_point QuadCenter = {};
                
                QuadCenter.x = LevelEntity->LevelNumberQuad.x + (LevelEntity->LevelNumberQuad.w / 2);
                QuadCenter.y = LevelEntity->LevelNumberQuad.y + (LevelEntity->LevelNumberQuad.h / 2);
                
                game_point QuadShadowCenter = {};
                
                QuadShadowCenter.x = LevelEntity->LevelNumberShadowQuad.x + (LevelEntity->LevelNumberShadowQuad.w / 2);
                QuadShadowCenter.y = LevelEntity->LevelNumberShadowQuad.y + (LevelEntity->LevelNumberShadowQuad.h / 2);
                
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
                
                
                
                LevelEntity->LevelNumberQuad.x = QuadCenter.x - (LevelEntity->LevelNumberQuad.w / 2);
                LevelEntity->LevelNumberQuad.y = QuadCenter.y - (LevelEntity->LevelNumberQuad.h / 2);
                
                LevelEntity->LevelNumberShadowQuad.x = QuadShadowCenter.x - (LevelEntity->LevelNumberShadowQuad.w / 2);
                
                LevelEntity->LevelNumberShadowQuad.y = QuadShadowCenter.y - (LevelEntity->LevelNumberShadowQuad.h / 2);
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
            
            //LevelEntityUpdateLevelEntityFromMemory(Memory, Memory->CurrentLevelIndex, true, Buffer);
            
            LevelEditorChangeGridCounters(LevelEditor, LevelEntity->GridEntity.RowAmount,
                                          LevelEntity->GridEntity.ColumnAmount,
                                          Buffer);
            
            LevelEditorUpdateLevelStats(LevelEditor->LevelPropertiesQuad.w + 10, 0, 
                                        LevelEditor, LevelEntity->LevelNumber, 
                                        Memory->CurrentLevelIndex, Buffer);
            
            LevelEditorChangeFigureCounter(LevelEditor, LevelEntity->FigureEntity.FigureAmount, Buffer);
        }
    }
    else if(Input->Keyboard.E_Button.EndedDown)
    {
        s32 NextLevelNumber = CurrentLevelIndex + 1;
        if(NextLevelNumber < Memory->LevelMemoryAmount)
        {
            Memory->CurrentLevelIndex = NextLevelNumber;
            
            //LevelEntityUpdateLevelEntityFromMemory(Memory, Memory->CurrentLevelIndex, true, Buffer);
            LevelEditorChangeGridCounters(LevelEditor, LevelEntity->GridEntity.RowAmount,
                                          LevelEntity->GridEntity.ColumnAmount,
                                          Buffer);
            
            LevelEditorUpdateLevelStats(LevelEditor->LevelPropertiesQuad.w + 10, 0, 
                                        LevelEditor, LevelEntity->LevelNumber, 
                                        Memory->CurrentLevelIndex, Buffer);
            
            LevelEditorChangeFigureCounter(LevelEditor, LevelEntity->FigureEntity.FigureAmount, Buffer);
        }
    }
    else if(Input->Keyboard.Enter.EndedDown)
    {
        if(LevelEditor->LevelNumberSelected)
        {
            LevelEntity->LevelNumber = strtol(LevelEditor->LevelNumberBuffer, 0, 10);
            LevelEditor->LevelNumberSelected = false;
            
            menu_entity* MenuEntity = 
                (menu_entity*) (((char*)Memory->LocalMemoryStorage) + (sizeof(playground)));
            
            MenuChangeButtonText(Memory->LevelNumberFont, LevelEditor->LevelNumberBuffer, MenuEntity, 
                                 &MenuEntity->Buttons[Memory->CurrentLevelIndex], {255, 255, 255}, Buffer);
        }
    }
    
    if(Input->MouseButtons[0].EndedDown)
    {
        if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->SwitchConfiguration.Quad))
        {
            if(LevelEditor->EditorType == LEVEL_EDITOR)
            {
                LevelEditor->EditorType = GAME_EDITOR;
                LevelEditorUpdateTextOnButton(Buffer, LevelEditor->Font, "< Game configuration >", LevelEditor->SwitchConfiguration.Texture, &LevelEditor->SwitchConfiguration.TextureQuad, &LevelEditor->SwitchConfiguration.Quad, {255, 255, 255});
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->SwitchConfiguration.Quad;
            }
            
        }
        
        
        // level number check
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
                
                
                game_point QuadCenter = {};
                
                QuadCenter.x = LevelEntity->LevelNumberQuad.x + (LevelEntity->LevelNumberQuad.w / 2);
                QuadCenter.y = LevelEntity->LevelNumberQuad.y + (LevelEntity->LevelNumberQuad.h / 2);
                
                game_point QuadShadowCenter = {};
                
                QuadShadowCenter.x = LevelEntity->LevelNumberShadowQuad.x + (LevelEntity->LevelNumberShadowQuad.w / 2);
                QuadShadowCenter.y = LevelEntity->LevelNumberShadowQuad.y + (LevelEntity->LevelNumberShadowQuad.h / 2);
                
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
                
                LevelEntity->LevelNumberQuad.x = QuadCenter.x - (LevelEntity->LevelNumberQuad.w / 2);
                LevelEntity->LevelNumberQuad.y = QuadCenter.y - (LevelEntity->LevelNumberQuad.h / 2);
                
                LevelEntity->LevelNumberShadowQuad.x = QuadShadowCenter.x - (LevelEntity->LevelNumberShadowQuad.w / 2);
                
                LevelEntity->LevelNumberShadowQuad.y = QuadShadowCenter.y - (LevelEntity->LevelNumberShadowQuad.h / 2);
            }
        }
        
        
        if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->LevelPropertiesQuad))
        {
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->RowLabel.MinusQuad))
            {
                printf("minus row!\n");
                
                LevelEditorNewGrid(Buffer, LevelEntity, RowAmount-1, ColAmount, LevelEditor);
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->RowLabel.MinusQuad;
            }
            
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->RowLabel.PlusQuad))
            {
                printf("plus row!\n");
                
                LevelEditorNewGrid(Buffer, LevelEntity, RowAmount+1, ColAmount, LevelEditor);
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->RowLabel.PlusQuad;
            }
            
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->ColumnLabel.MinusQuad))
            {
                printf("minus column!\n");
                
                LevelEditorNewGrid(Buffer, LevelEntity, RowAmount, ColAmount-1, LevelEditor);
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->ColumnLabel.MinusQuad;
            }
            
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->ColumnLabel.PlusQuad))
            {
                printf("plus column!\n");
                
                LevelEditorNewGrid(Buffer, LevelEntity, RowAmount, ColAmount+1, LevelEditor);
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->ColumnLabel.PlusQuad;
            }
            
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->FigureLabel.MinusQuad))
            {
                printf("minus figure!\n");
                
                if(FigureAmount > 0)
                {
                    //FigureUnitDeleteFigure(LevelEntity->FigureEntity, LevelEntity->FigureEntity->FigureAmount - 1);
                    
                    if(Buffer->Width > Buffer->Height)
                    {
                        FigureEntityAlignFigures(&LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                    }
                    
                    GridEntityUpdateStickUnits(&LevelEntity->GridEntity, LevelEntity->FigureEntity.FigureAmount);
                    
                    LevelEditorChangeFigureCounter(LevelEditor, FigureAmount - 1, Buffer);
                    
                    FigureAmount = LevelEntity->FigureEntity.FigureAmount;
                }
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->FigureLabel.MinusQuad;
            }
            
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->FigureLabel.PlusQuad))
            {
                printf("plus figure!\n");
                
                if(FigureAmount < FIGURE_AMOUNT_MAXIMUM)
                {
                    FigureUnitAddNewFigure(&LevelEntity->FigureEntity, O_figure, classic, 0.0f, LevelEntity->Configuration.InActiveBlockSize, Memory, Buffer);
                    
                    if(Buffer->Width > Buffer->Height)
                    {
                        FigureEntityAlignFigures(&LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                    }
                    
                    GridEntityUpdateStickUnits(&LevelEntity->GridEntity, LevelEntity->FigureEntity.FigureAmount);
                    
                    LevelEditorChangeFigureCounter(LevelEditor, FigureAmount + 1, Buffer);
                    
                    FigureAmount = LevelEntity->FigureEntity.FigureAmount;
                }
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->FigureLabel.PlusQuad;
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->FigurePropertiesQuad))
        {
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->FlipFigureButton.Quad))
            {
                printf("flip!\n");
                
                FigureUnitFlipHorizontally(&LevelEntity->FigureEntity.FigureUnit[LevelEditor->CurrentFigureIndex]);
                
                if(Buffer->Width > Buffer->Height)
                {
                    FigureEntityAlignFigures(&LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                }
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->FlipFigureButton.Quad;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->RotateFigureButton.Quad))
            {
                printf("rotate!\n");
                
                FigureUnitRotateShellBy(&LevelEntity->FigureEntity.FigureUnit[LevelEditor->CurrentFigureIndex], 90);
                LevelEntity->FigureEntity.FigureUnit[LevelEditor->CurrentFigureIndex].Angle += 90.0f;
                
                if(Buffer->Width > Buffer->Height)
                {
                    FigureEntityAlignFigures(&LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                }
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->RotateFigureButton.Quad;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->TypeFigureButton.Quad))
            {
                printf("type!\n");
                
                if(LevelEditor->CurrentFigureIndex >= 0)
                {
                    figure_type Type = LevelEditorGetNextFigureType(LevelEntity->FigureEntity.FigureUnit[LevelEditor->CurrentFigureIndex].Type);
                    
                    figure_form Form = LevelEntity->FigureEntity.FigureUnit[LevelEditor->CurrentFigureIndex].Form;
                    
                    FigureUnitInitFigure(&LevelEntity->FigureEntity.FigureUnit[LevelEditor->CurrentFigureIndex], Form,Type, 0.0f, LevelEntity->Configuration.InActiveBlockSize, Memory, Buffer);
                    
                    if(Buffer->Width > Buffer->Height)
                    {
                        FigureEntityAlignFigures(&LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                    }
                    
                }
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->TypeFigureButton.Quad;
            }
            
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->FormFigureButton.Quad))
            {
                printf("form!\n");
                
                if(LevelEditor->CurrentFigureIndex >= 0)
                {
                    figure_form Form = LevelEditorGetNextFigureForm(LevelEntity->FigureEntity.FigureUnit[LevelEditor->CurrentFigureIndex].Form);
                    
                    figure_type Type = LevelEntity->FigureEntity.FigureUnit[LevelEditor->CurrentFigureIndex].Type;
                    
                    FigureUnitInitFigure(&LevelEntity->FigureEntity.FigureUnit[LevelEditor->CurrentFigureIndex], Form,Type, 0.0f, LevelEntity->Configuration.InActiveBlockSize, Memory, Buffer);
                    
                    if(Buffer->Width > Buffer->Height)
                    {
                        FigureEntityAlignFigures(&LevelEntity->FigureEntity, LevelEntity->Configuration.InActiveBlockSize);
                    }
                    
                }
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->FormFigureButton.Quad;
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->IOPropertiesQuad))
        {
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->SaveLevelButton.Quad))
            {
                printf("save!\n");
                
                SaveLevelToMemory(Memory, LevelEntity, Memory->CurrentLevelIndex);
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->SaveLevelButton.Quad;
            }
            
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &LevelEditor->LoadLevelButton.Quad))
            {
                printf("load!\n");
                
                //LevelEntityUpdateLevelEntityFromMemory(Memory, Memory->CurrentLevelIndex,false, Buffer);
                
                LevelEditorChangeGridCounters(LevelEditor, 
                                              LevelEntity->GridEntity.RowAmount, LevelEntity->GridEntity.ColumnAmount, 
                                              Buffer);
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->LoadLevelButton.Quad;
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &FigureArea))
        {
            for(u32 i = 0; i < FigureAmount; ++i)
            {
                game_rect AreaQuad = FigureUnitGetArea(&LevelEntity->FigureEntity.FigureUnit[i]);
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
                    
                    //LevelEntityUpdateLevelEntityFromMemory(Memory, PrevLevelIndex, true, Buffer);
                    
                    LevelEditorChangeGridCounters(LevelEditor, LevelEntity->GridEntity.RowAmount,
                                                  LevelEntity->GridEntity.ColumnAmount,Buffer);
                    
                    LevelEditorUpdateLevelStats(LevelEditor->LevelPropertiesQuad.w + 10, 0, LevelEditor, LevelEntity->LevelNumber, 
                                                Memory->CurrentLevelIndex, Buffer);
                    
                    LevelEditorChangeFigureCounter(LevelEditor, LevelEntity->FigureEntity.FigureAmount, Buffer);
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
                    
                    //LevelEntityUpdateLevelEntityFromMemory(Memory, NextLevelIndex, true, Buffer);
                    
                    LevelEditorChangeGridCounters(LevelEditor, 
                                                  LevelEntity->GridEntity.RowAmount, LevelEntity->GridEntity.ColumnAmount, Buffer);
                    
                    LevelEditorUpdateLevelStats(LevelEditor->LevelPropertiesQuad.w + 10, 0, LevelEditor, LevelEntity->LevelNumber, 
                                                Memory->CurrentLevelIndex, Buffer);
                    
                    
                    LevelEditorChangeFigureCounter(LevelEditor, LevelEntity->FigureEntity.FigureAmount, Buffer);
                }
                
                LevelEditor->ButtonSelected = true;
                LevelEditor->HighlightButtonQuad = LevelEditor->NextLevelQuad;
            }
            
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &GridArea))
        {
            game_rect AreaQuad = { 0, 0, GridBlockSize, GridBlockSize };
            
            u32 StartX = 0;
            u32 StartY = 0;
            
            for(u32 Row = 0; Row < RowAmount; ++Row)
            {
                StartY = GridArea.y + (GridBlockSize * Row);
                
                for(u32 Col = 0; Col < ColAmount; ++Col)
                {
                    StartX = GridArea.x + (GridBlockSize * Col);
                    
                    AreaQuad.x = StartX;
                    AreaQuad.y = StartY;
                    
                    if(IsPointInsideRect(Input->MouseX, Input->MouseY, 
                                         &AreaQuad))
                    {
                        s32 UnitIndex = (Row * ColAmount) + Col;
                        u32 GridUnit = LevelEntity->GridEntity.UnitField[UnitIndex];
                        if(GridUnit == 0)
                        {
                            LevelEntity->GridEntity.UnitField[UnitIndex] = 1;
                        }
                        else
                        {
                            s32 Index = -1;
                            for(u32 m = 0; m < LevelEntity->GridEntity.MovingBlocksAmount; ++m)
                            {
                                u32 RowNumber = LevelEntity->GridEntity.MovingBlocks[m].RowNumber;
                                u32 ColNumber = LevelEntity->GridEntity.MovingBlocks[m].ColNumber;
                                
                                if((Row == RowNumber) && (Col == ColNumber))
                                {
                                    Index = m;
                                    break;
                                }
                            }
                            
                            if(Index >= 0)
                            {
                                
                                if(!LevelEntity->GridEntity.MovingBlocks[Index].MoveSwitch)
                                {
                                    LevelEntity->GridEntity.MovingBlocks[Index].MoveSwitch = true;
                                }
                                else if(!LevelEntity->GridEntity.MovingBlocks[Index].IsVertical)
                                {
                                    LevelEntity->GridEntity.MovingBlocks[Index].IsVertical = true;
                                    LevelEntity->GridEntity.MovingBlocks[Index].MoveSwitch = false;
                                }
                                else
                                {
                                    LevelEditorDeleteMovingBlock(&LevelEntity->GridEntity, Index);
                                    LevelEntity->GridEntity.UnitField[UnitIndex] = 0;
                                }
                            }
                            else
                            {
                                GridEntityAddMovingBlock(&LevelEntity->GridEntity, Row, Col, false, false, LevelEntity->Configuration.GridBlockSize);
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
            DEBUGRenderFigureShell(Buffer, &LevelEntity->FigureEntity.FigureUnit[i], LevelEntity->Configuration.InActiveBlockSize / 4, {0, 0, 255}, 255);
            DEBUGRenderQuad(Buffer, &LevelEntity->FigureEntity.FigureUnit[i].AreaQuad, {255, 0, 0}, 255);
        }
        
        if(LevelEntity->FigureEntity.FigureAmount > 0)
        {
            DEBUGRenderFigureShell(Buffer, &LevelEntity->FigureEntity.FigureUnit[NewFigureIndex],LevelEntity->Configuration.InActiveBlockSize, {255,
                                       255, 255}, 100);
        }
    }
    
    
    /* Level number is selected */ 
    
    if(LevelEditor->LevelNumberSelected)
    {
        DEBUGRenderQuadFill(Buffer, &LevelEntity->LevelNumberQuad, {255, 0, 0}, 150);
    }
    
    RenderButtonQuad(Buffer, &LevelEditor->SwitchConfiguration, 255, 192, 203, 100);
    
    /* Level header name rendering */ 
    
    //RenderButtonQuad(Buffer, &LevelEditor->LevelConfigButton, 128, 128, 128, 100);
    
    LevelEditorRenderLabel(&LevelEditor->RowLabel,    Buffer);
    LevelEditorRenderLabel(&LevelEditor->ColumnLabel, Buffer);
    LevelEditorRenderLabel(&LevelEditor->FigureLabel, Buffer);
    
    /* Figure header name rendering */ 
    
    RenderButtonQuad(Buffer, &LevelEditor->FigureConfigButton, 128, 128, 128, 100);
    RenderButtonQuad(Buffer, &LevelEditor->FlipFigureButton,     0,   0, 255, 100);
    RenderButtonQuad(Buffer, &LevelEditor->RotateFigureButton,   0,   0, 255, 100);
    RenderButtonQuad(Buffer, &LevelEditor->TypeFigureButton,     0,   0, 255, 100);
    RenderButtonQuad(Buffer, &LevelEditor->FormFigureButton,     0,   0, 255, 100);
    
    /* IO header name rendering */ 
    
    RenderButtonQuad(Buffer, &LevelEditor->IOConfigButton,  128, 128, 128, 100);
    RenderButtonQuad(Buffer, &LevelEditor->SaveLevelButton,   0, 255, 255, 100);
    RenderButtonQuad(Buffer, &LevelEditor->LoadLevelButton,   0, 255, 255, 100);
    
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
    
    if(LevelEditor->ButtonSelected)
    {
        DEBUGRenderQuadFill(Buffer, &LevelEditor->HighlightButtonQuad, {255, 255, 0}, 150);
    }
}

static void
LevelEditorUpdateAndRender(level_editor *LevelEditor, playground *LevelEntity, 
                           game_memory *Memory, game_offscreen_buffer *Buffer, game_input *Input)
{
    if(LevelEditor->EditorType == LEVEL_EDITOR)
    {
        LevelConfigUpdateAndRender(LevelEditor, LevelEntity, 
                                   Memory, Buffer, Input);
    }
    else if(LevelEditor->EditorType == GAME_EDITOR)
    {
        GameConfigUpdateAndRender(LevelEditor, LevelEntity,
                                  Memory, Buffer, Input);
    }
    else if(LevelEditor->EditorType == MENU_EDITOR)
    {
        
    }
    
}

static void
LevelEditorUpdatePositionsPortrait(game_offscreen_buffer *Buffer, level_editor *LevelEditor, playground *LevelEntity, game_memory *Memory)
{
    s32 ActualWidth = Buffer->Width;
    s32 ActualHeight = Buffer->Height;
    
    s32 ScreenWidth     = Buffer->Width;
    s32 ScreenHeight    = Buffer->Height;
    
    s32 ReferenceWidth  = Buffer->ReferenceWidth;
    s32 ReferenceHeight = Buffer->ReferenceHeight;
    
    r32 ScaleByWidth = GetScale(ScreenWidth, ScreenHeight, 600, 800, 0.0f);
    
    r32 ScaleByHeight = GetScale(ScreenWidth, ScreenHeight, 600, 800, 1.0f);
    
    // New Font size
    {
        s32 FontSize = 12;
        
        if(LevelEditor->Font)
        {
            TTF_CloseFont(LevelEditor->Font);
        }
        
        FontSize = (r32)FontSize * ScaleByHeight;
        
        LevelEditor->Font = TTF_OpenFont(FontPath, FontSize);
        Assert(LevelEditor->Font);
    }
    
    
    // Level Properties location
    
    {
        s32 ButtonWidth  = 150;
        s32 ButtonHeight = 30;
        s32 ButtonAmount = 4;
        
        ButtonWidth  = roundf((r32)ButtonWidth  * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        
        LevelEditor->LevelPropertiesQuad.x = 0;
        LevelEditor->LevelPropertiesQuad.y = 0;
        LevelEditor->LevelPropertiesQuad.w = ButtonWidth;
        LevelEditor->LevelPropertiesQuad.h = ButtonHeight * ButtonAmount;
        
        /* Level header name initialization */
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Level Configuration", 
                             LevelEditor->LevelPropertiesQuad.x,
                             LevelEditor->LevelPropertiesQuad.y,
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->SwitchConfiguration, 255, 255, 255);
        
        s32 InfoWidth = 75;
        s32 BoxWidth  = 25;
        
        InfoWidth = roundf((r32)InfoWidth * ScaleByHeight);
        BoxWidth  = roundf((r32)BoxWidth  * ScaleByHeight);
        
        /* Row label initialization */
        
        s32 RowAmount = LevelEntity->GridEntity.RowAmount;
        InitLabel(LevelEditor->Font, &LevelEditor->RowLabel, "Row amount",
                  RowAmount, 0, ButtonHeight, ButtonWidth, ButtonHeight,
                  InfoWidth, BoxWidth, Buffer);
        
        /* Column label initialization */
        
        s32 ColumnAmount = LevelEntity->GridEntity.ColumnAmount;
        InitLabel(LevelEditor->Font, &LevelEditor->ColumnLabel, "Column amount",
                  ColumnAmount, 0, ButtonHeight*2, ButtonWidth, ButtonHeight, InfoWidth, BoxWidth, Buffer);
        
        /* Figure label initialization */
        
        s32 FigureAmount = LevelEntity->FigureEntity.FigureAmount;
        InitLabel(LevelEditor->Font, &LevelEditor->FigureLabel, "Figure amount",
                  FigureAmount, 0, ButtonHeight*3, ButtonWidth, ButtonHeight, InfoWidth, BoxWidth, Buffer);
    }
    
    // Figure Properties location
    
    {
        s32 ButtonWidth  = 150;
        s32 ButtonHeight = 30;
        s32 ButtonAmount = 5;
        
        ButtonWidth  = roundf((r32)ButtonWidth  * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight); 
        
        LevelEditor->FigurePropertiesQuad.w = ButtonWidth;
        LevelEditor->FigurePropertiesQuad.h = ButtonHeight * ButtonAmount;
        LevelEditor->FigurePropertiesQuad.x = LevelEditor->LevelPropertiesQuad.x;
        LevelEditor->FigurePropertiesQuad.y = LevelEditor->LevelPropertiesQuad.y
            + LevelEditor->LevelPropertiesQuad.h;
        
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Figure Configuration", 
                             LevelEditor->FigurePropertiesQuad.x,
                             LevelEditor->FigurePropertiesQuad.y,
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->FigureConfigButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Flip figure", 
                             LevelEditor->FigurePropertiesQuad.x,
                             LevelEditor->FigurePropertiesQuad.y + ButtonHeight,
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->FlipFigureButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Rotate figure", 
                             LevelEditor->FigurePropertiesQuad.x,
                             LevelEditor->FigurePropertiesQuad.y + (ButtonHeight * 2),
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->RotateFigureButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Change type", 
                             LevelEditor->FigurePropertiesQuad.x,
                             LevelEditor->FigurePropertiesQuad.y + (ButtonHeight * 3),
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->TypeFigureButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Change form", 
                             LevelEditor->FigurePropertiesQuad.x,
                             LevelEditor->FigurePropertiesQuad.y + (ButtonHeight * 4),
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->FormFigureButton, 255, 255, 255);
        
    }
    
    // IO properties location
    
    {
        s32 ButtonWidth  = 150;
        s32 ButtonHeight = 30;
        s32 ButtonAmount = 3;
        
        ButtonWidth  = roundf((r32)ButtonWidth  * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight); 
        
        LevelEditor->IOPropertiesQuad.w = ButtonWidth;
        LevelEditor->IOPropertiesQuad.h = ButtonHeight * ButtonAmount;
        LevelEditor->IOPropertiesQuad.x = LevelEditor->FigurePropertiesQuad.x;
        LevelEditor->IOPropertiesQuad.y = LevelEditor->FigurePropertiesQuad.y + LevelEditor->FigurePropertiesQuad.h;
        
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Save/Load level", 
                             LevelEditor->IOPropertiesQuad.x,
                             LevelEditor->IOPropertiesQuad.y,
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->IOConfigButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Save to disk", 
                             LevelEditor->IOPropertiesQuad.x,
                             LevelEditor->IOPropertiesQuad.y + (ButtonHeight),
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->SaveLevelButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Load from disk", 
                             LevelEditor->IOPropertiesQuad.x,
                             LevelEditor->IOPropertiesQuad.y + (ButtonHeight * 2),
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->LoadLevelButton, 255, 255, 255);
        
    }
    
    // Level stats location
    {
        s32 PosX = LevelEditor->LevelPropertiesQuad.x
            + LevelEditor->LevelPropertiesQuad.w + 10;
        s32 PosY = 0;
        
        LevelEditorUpdateLevelStats(PosX, PosY, LevelEditor, 0, Memory->CurrentLevelIndex, Buffer);
    }
    
    
    // Position Panel location
    
    {
        s32 ButtonWidth  = 150;
        s32 ButtonHeight = 20;
        s32 ButtonAmount = 6;
        
        ButtonWidth  = roundf((r32) ButtonWidth * ScaleByHeight);
        ButtonHeight = roundf((r32) ButtonHeight * ScaleByHeight);
        
        LevelEditor->PosPanelQuad.x = 0;
        LevelEditor->PosPanelQuad.y = LevelEditor->LevelPropertiesQuad.y + LevelEditor->SwitchConfiguration.Quad.h;
        LevelEditor->PosPanelQuad.w = ButtonWidth;
        LevelEditor->PosPanelQuad.h = ButtonHeight * ButtonAmount;
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Position properties", 
                             LevelEditor->PosPanelQuad.x,
                             LevelEditor->PosPanelQuad.y,
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->PosPanel.HeaderButton, 255, 255, 255);
        
        s32 ArrowWidth  = 32;
        s32 SwitchWidth = 86;
        
        ArrowWidth  = roundf((r32)ArrowWidth * ScaleByHeight);
        SwitchWidth = roundf((r32)SwitchWidth * ScaleByHeight);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "<", 
                             LevelEditor->PosPanelQuad.x,
                             LevelEditor->PosPanelQuad.y + ButtonHeight,
                             ArrowWidth, ButtonHeight,
                             &LevelEditor->PosPanel.LeftArrowButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Pixels", 
                             LevelEditor->PosPanelQuad.x + ArrowWidth,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight),
                             SwitchWidth, ButtonHeight,
                             &LevelEditor->PosPanel.SwitchNameButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, ">", 
                             LevelEditor->PosPanelQuad.x + ArrowWidth + SwitchWidth,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight),
                             ArrowWidth, ButtonHeight,
                             &LevelEditor->PosPanel.RightArrowButton, 255, 255, 255);
        
        s32 NameNumberWidth = 60;
        s32 NumberWidth     = 90;
        
        NameNumberWidth = roundf((r32)NameNumberWidth * ScaleByHeight);
        NumberWidth     = roundf((r32)NumberWidth * ScaleByHeight);
        
        char NumberString[128] = {};
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "X", 
                             LevelEditor->PosPanelQuad.x,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 2),
                             NameNumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.FirstNumberNameButton, 255, 255, 255);
        
        sprintf(NumberString, "%d", LevelEntity->GridEntity.GridArea.x);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, NumberString, 
                             LevelEditor->PosPanelQuad.x + NameNumberWidth,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 2),
                             NumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.FirstNumberButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Y", 
                             LevelEditor->PosPanelQuad.x,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 3),
                             NameNumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.SecondNumberNameButton, 255, 255, 255);
        
        sprintf(NumberString, "%d", LevelEntity->GridEntity.GridArea.y);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, NumberString, 
                             LevelEditor->PosPanelQuad.x + NameNumberWidth,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 3),
                             NumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.SecondNumberButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Width", 
                             LevelEditor->PosPanelQuad.x,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 4),
                             NameNumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.ThirdNumberNameButton, 255, 255, 255);
        sprintf(NumberString, "%d", LevelEntity->GridEntity.GridArea.w);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, NumberString, 
                             LevelEditor->PosPanelQuad.x + NameNumberWidth,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 4),
                             NumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.ThirdNumberButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Height", 
                             LevelEditor->PosPanelQuad.x,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 5),
                             NameNumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.FourthNumberNameButton, 255, 255, 255);
        
        sprintf(NumberString, "%d", LevelEntity->GridEntity.GridArea.h);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, NumberString, 
                             LevelEditor->PosPanelQuad.x + NameNumberWidth,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 5),
                             NumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.FourthNumberButton, 255, 255, 255);
        
    }
    
    // Previous level button
    { 
        s32 X = 0;
        s32 Y = 50;
        s32 ButtonWidth  = 30;
        s32 ButtonHeight = 30;
        
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        X = (ActualWidth / 2) - (ButtonWidth);
        Y = roundf((r32)Y * ScaleByHeight);
        
        LevelEditor->PrevLevelQuad.w = ButtonWidth;
        LevelEditor->PrevLevelQuad.h = ButtonHeight;
        LevelEditor->PrevLevelQuad.x = X;
        LevelEditor->PrevLevelQuad.y = Y;
    }
    
    // Next level editor
    {
        s32 X = 0;
        s32 Y = 50;
        s32 ButtonWidth = 30;
        s32 ButtonHeight = 30;
        
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        X = (ActualWidth / 2) + ButtonWidth;
        Y = roundf((r32)Y * ScaleByHeight);
        
        LevelEditor->NextLevelQuad.w = ButtonWidth;
        LevelEditor->NextLevelQuad.h = ButtonHeight;
        LevelEditor->NextLevelQuad.x = X;
        LevelEditor->NextLevelQuad.y = Y;
    }
    
    LevelEditor->EditorObject[0].AreaQuad = LevelEntity->LevelNumberQuad;
    LevelEditor->EditorObject[1].AreaQuad = LevelEntity->FigureEntity.FigureArea;
    LevelEditor->EditorObject[2].AreaQuad = LevelEntity->GridEntity.GridArea;
}

static void
LevelEditorUpdatePositionsLandscape(game_offscreen_buffer *Buffer, level_editor *LevelEditor, playground *LevelEntity, game_memory *Memory)
{
    //
    // Reference dimension assumed to be 800x600
    //
    s32 ActualWidth  = Buffer->Width;
    s32 ActualHeight = Buffer->Height;
    
    s32 ScreenWidth     = Buffer->Width;
    s32 ScreenHeight    = Buffer->Height;
    s32 ReferenceWidth  = Buffer->ReferenceWidth;
    s32 ReferenceHeight = Buffer->ReferenceHeight;
    
    r32 ScaleByWidth = GetScale(ActualWidth, ActualHeight, ReferenceWidth, ReferenceHeight, 0.0f);
    
    r32 ScaleByHeight = GetScale(ActualWidth, ActualHeight, ReferenceWidth, ReferenceHeight, 1.0f);
    
    r32 ScaleByAll = GetScale(ActualWidth, ActualHeight, ReferenceWidth, ReferenceHeight, 0.5f);
    
    // New Font size
    {
        s32 FontSize = 12;
        
        if(LevelEditor->Font)
        {
            TTF_CloseFont(LevelEditor->Font);
        }
        
        FontSize = (r32)FontSize * ScaleByWidth;
        printf("FontSize = %d\n",FontSize);
        
        LevelEditor->Font = TTF_OpenFont(FontPath, FontSize);
        Assert(LevelEditor->Font);
    }
    
    
    
    // Level Properties location
    
    {
        s32 ButtonWidth  = 150;
        s32 ButtonHeight = 30;
        s32 ButtonAmount = 4;
        
        ButtonWidth  = roundf((r32)ButtonWidth  * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        
        LevelEditor->LevelPropertiesQuad.x = 0;
        LevelEditor->LevelPropertiesQuad.y = 0;
        LevelEditor->LevelPropertiesQuad.w = ButtonWidth;
        LevelEditor->LevelPropertiesQuad.h = ButtonHeight * ButtonAmount;
        
        /* Level header name initialization */
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Level Configuration", 
                             LevelEditor->LevelPropertiesQuad.x,
                             LevelEditor->LevelPropertiesQuad.y,
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->SwitchConfiguration, 255, 255, 255);
        
        s32 InfoWidth = 75;
        s32 BoxWidth  = 25;
        
        InfoWidth = roundf((r32)InfoWidth * ScaleByWidth);
        BoxWidth  = roundf((r32)BoxWidth  * ScaleByWidth);
        
        /* Row label initialization */
        
        s32 RowAmount = LevelEntity->GridEntity.RowAmount;
        InitLabel(LevelEditor->Font, &LevelEditor->RowLabel, "Row amount",
                  RowAmount, 0, ButtonHeight, ButtonWidth, ButtonHeight,
                  InfoWidth, BoxWidth, Buffer);
        
        /* Column label initialization */
        
        s32 ColumnAmount = LevelEntity->GridEntity.ColumnAmount;
        InitLabel(LevelEditor->Font, &LevelEditor->ColumnLabel, "Column amount",
                  ColumnAmount, 0, ButtonHeight*2, ButtonWidth, ButtonHeight, InfoWidth, BoxWidth, Buffer);
        
        /* Figure label initialization */
        
        s32 FigureAmount = LevelEntity->FigureEntity.FigureAmount;
        InitLabel(LevelEditor->Font, &LevelEditor->FigureLabel, "Figure amount",
                  FigureAmount, 0, ButtonHeight*3, ButtonWidth, ButtonHeight, InfoWidth, BoxWidth, Buffer);
    }
    
    // Figure Properties location
    
    {
        s32 ButtonWidth  = 150;
        s32 ButtonHeight = 30;
        s32 ButtonAmount = 5;
        
        ButtonWidth  = roundf((r32)ButtonWidth  * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight); 
        
        LevelEditor->FigurePropertiesQuad.w = ButtonWidth;
        LevelEditor->FigurePropertiesQuad.h = ButtonHeight * ButtonAmount;
        LevelEditor->FigurePropertiesQuad.x = LevelEditor->LevelPropertiesQuad.x;
        LevelEditor->FigurePropertiesQuad.y = LevelEditor->LevelPropertiesQuad.y
            + LevelEditor->LevelPropertiesQuad.h;
        
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Figure Configuration", 
                             LevelEditor->FigurePropertiesQuad.x,
                             LevelEditor->FigurePropertiesQuad.y,
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->FigureConfigButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Flip figure", 
                             LevelEditor->FigurePropertiesQuad.x,
                             LevelEditor->FigurePropertiesQuad.y + ButtonHeight,
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->FlipFigureButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Rotate figure", 
                             LevelEditor->FigurePropertiesQuad.x,
                             LevelEditor->FigurePropertiesQuad.y + (ButtonHeight * 2),
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->RotateFigureButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Change type", 
                             LevelEditor->FigurePropertiesQuad.x,
                             LevelEditor->FigurePropertiesQuad.y + (ButtonHeight * 3),
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->TypeFigureButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Change form", 
                             LevelEditor->FigurePropertiesQuad.x,
                             LevelEditor->FigurePropertiesQuad.y + (ButtonHeight * 4),
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->FormFigureButton, 255, 255, 255);
        
    }
    
    // IO properties location
    
    {
        s32 ButtonWidth  = 150;
        s32 ButtonHeight = 30;
        s32 ButtonAmount = 3;
        
        ButtonWidth  = roundf((r32)ButtonWidth  * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight); 
        
        LevelEditor->IOPropertiesQuad.w = ButtonWidth;
        LevelEditor->IOPropertiesQuad.h = ButtonHeight * ButtonAmount;
        LevelEditor->IOPropertiesQuad.x = LevelEditor->FigurePropertiesQuad.x;
        LevelEditor->IOPropertiesQuad.y = LevelEditor->FigurePropertiesQuad.y + LevelEditor->FigurePropertiesQuad.h;
        
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Save/Load level", 
                             LevelEditor->IOPropertiesQuad.x,
                             LevelEditor->IOPropertiesQuad.y,
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->IOConfigButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Save to disk", 
                             LevelEditor->IOPropertiesQuad.x,
                             LevelEditor->IOPropertiesQuad.y + (ButtonHeight),
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->SaveLevelButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Load from disk", 
                             LevelEditor->IOPropertiesQuad.x,
                             LevelEditor->IOPropertiesQuad.y + (ButtonHeight * 2),
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->LoadLevelButton, 255, 255, 255);
        
    }
    
    // Level stats location
    {
        s32 PosX = LevelEditor->LevelPropertiesQuad.x
            + LevelEditor->LevelPropertiesQuad.w + 10;
        s32 PosY = 0;
        
        LevelEditorUpdateLevelStats(PosX, PosY, LevelEditor, 0, Memory->CurrentLevelIndex, Buffer);
    }
    
    
    // Position Panel location
    
    {
        s32 ButtonWidth  = 150;
        s32 ButtonHeight = 20;
        s32 ButtonAmount = 6;
        
        ButtonWidth  = roundf((r32) ButtonWidth * ScaleByWidth);
        ButtonHeight = roundf((r32) ButtonHeight * ScaleByHeight);
        
        LevelEditor->PosPanelQuad.x = 0;
        LevelEditor->PosPanelQuad.y = LevelEditor->LevelPropertiesQuad.y + LevelEditor->SwitchConfiguration.Quad.h;
        LevelEditor->PosPanelQuad.w = ButtonWidth;
        LevelEditor->PosPanelQuad.h = ButtonHeight * ButtonAmount;
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Position properties", 
                             LevelEditor->PosPanelQuad.x,
                             LevelEditor->PosPanelQuad.y,
                             ButtonWidth, ButtonHeight,
                             &LevelEditor->PosPanel.HeaderButton, 255, 255, 255);
        
        s32 ArrowWidth  = 32;
        s32 SwitchWidth = 86;
        
        ArrowWidth  = roundf((r32)ArrowWidth * ScaleByWidth);
        SwitchWidth = roundf((r32)SwitchWidth * ScaleByHeight);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "<", 
                             LevelEditor->PosPanelQuad.x,
                             LevelEditor->PosPanelQuad.y + ButtonHeight,
                             ArrowWidth, ButtonHeight,
                             &LevelEditor->PosPanel.LeftArrowButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Pixels", 
                             LevelEditor->PosPanelQuad.x + ArrowWidth,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight),
                             SwitchWidth, ButtonHeight,
                             &LevelEditor->PosPanel.SwitchNameButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, ">", 
                             LevelEditor->PosPanelQuad.x + ArrowWidth + SwitchWidth,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight),
                             ArrowWidth, ButtonHeight,
                             &LevelEditor->PosPanel.RightArrowButton, 255, 255, 255);
        
        s32 NameNumberWidth = 60;
        s32 NumberWidth     = 90;
        
        NameNumberWidth = roundf((r32)NameNumberWidth * ScaleByWidth);
        NumberWidth     = roundf((r32)NumberWidth * ScaleByWidth);
        
        char NumberString[128] = {};
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "X", 
                             LevelEditor->PosPanelQuad.x,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 2),
                             NameNumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.FirstNumberNameButton, 255, 255, 255);
        
        sprintf(NumberString, "%d", LevelEntity->GridEntity.GridArea.x);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, NumberString, 
                             LevelEditor->PosPanelQuad.x + NameNumberWidth,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 2),
                             NumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.FirstNumberButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Y", 
                             LevelEditor->PosPanelQuad.x,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 3),
                             NameNumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.SecondNumberNameButton, 255, 255, 255);
        
        sprintf(NumberString, "%d", LevelEntity->GridEntity.GridArea.y);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, NumberString, 
                             LevelEditor->PosPanelQuad.x + NameNumberWidth,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 3),
                             NumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.SecondNumberButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Width", 
                             LevelEditor->PosPanelQuad.x,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 4),
                             NameNumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.ThirdNumberNameButton, 255, 255, 255);
        sprintf(NumberString, "%d", LevelEntity->GridEntity.GridArea.w);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, NumberString, 
                             LevelEditor->PosPanelQuad.x + NameNumberWidth,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 4),
                             NumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.ThirdNumberButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, "Height", 
                             LevelEditor->PosPanelQuad.x,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 5),
                             NameNumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.FourthNumberNameButton, 255, 255, 255);
        
        sprintf(NumberString, "%d", LevelEntity->GridEntity.GridArea.h);
        
        EditorMakeTextButton(Buffer, LevelEditor->Font, NumberString, 
                             LevelEditor->PosPanelQuad.x + NameNumberWidth,
                             LevelEditor->PosPanelQuad.y + (ButtonHeight * 5),
                             NumberWidth, ButtonHeight,
                             &LevelEditor->PosPanel.FourthNumberButton, 255, 255, 255);
        
    }
    
    /* Previous level button */
    
    { 
        s32 X = 0;
        s32 Y = 550;
        s32 ButtonWidth  = 30;
        s32 ButtonHeight = 30;
        
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByWidth);
        X = (ActualWidth / 2) - (ButtonWidth);
        Y = roundf((r32)Y * ScaleByHeight);
        
        LevelEditor->PrevLevelQuad.w = ButtonWidth;
        LevelEditor->PrevLevelQuad.h = ButtonHeight;
        LevelEditor->PrevLevelQuad.x = X;
        LevelEditor->PrevLevelQuad.y = Y;
    }
    
    // Next level editor
    {
        s32 X = 0;
        s32 Y = 550;
        s32 ButtonWidth = 30;
        s32 ButtonHeight = 30;
        
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByWidth);
        X = (ActualWidth / 2) + ButtonWidth;
        Y = roundf((r32)Y * ScaleByHeight);
        
        LevelEditor->NextLevelQuad.w = ButtonWidth;
        LevelEditor->NextLevelQuad.h = ButtonHeight;
        LevelEditor->NextLevelQuad.x = X;
        LevelEditor->NextLevelQuad.y = Y;
    }
    
    LevelEditor->EditorObject[0].AreaQuad = LevelEntity->LevelNumberQuad;
    LevelEditor->EditorObject[1].AreaQuad = LevelEntity->FigureEntity.FigureArea;
    LevelEditor->EditorObject[2].AreaQuad = LevelEntity->GridEntity.GridArea;
}

static void
LevelEditorInit(level_editor *LevelEditor, playground *LevelEntity, game_memory *Memory, game_offscreen_buffer *Buffer)
{
    //*LevelEditor = {};
    
    LevelEditor->EditorType = LEVEL_EDITOR;
    
    LevelEditor->ObjectIsSelected  = false;
    LevelEditor->EditorObjectIndex = -1;
    
    // Level Number Object
    LevelEditor->EditorObject[0].AreaQuad  = LevelEntity->LevelNumberQuad;
    
    // Figure Area Object
    LevelEditor->EditorObject[1].AreaQuad  = LevelEntity->FigureEntity.FigureArea; 
    
    // Grid Area Object
    LevelEditor->EditorObject[2].AreaQuad  = LevelEntity->GridEntity.GridArea;
    
    /* Next/Prev level buttons */
    
    LevelEditor->PrevLevelTexture = GetTexture(Memory, "left_arrow.png", Buffer->Renderer);
    Assert(LevelEditor->PrevLevelTexture);
    
    LevelEditor->NextLevelTexture = GetTexture(Memory, "right_arrow.png", Buffer->Renderer);
    Assert(LevelEditor->NextLevelTexture);
    
    LevelEditorUpdatePositionsLandscape(Buffer, LevelEditor, LevelEntity, Memory);
    
}

//
// level_editor end
//

//
// menu_editor start
//


static void
MenuEditorUpdatePositionsLandscape(game_offscreen_buffer *Buffer, menu_editor *MenuEditor, menu_entity *MenuEntity, game_memory *Memory)
{
    //
    // Reference dimension assumed to be 800x600
    //
    
    s32 ActualWidth     = Buffer->Width;
    s32 ActualHeight    = Buffer->Height;
    s32 ReferenceWidth  = Buffer->ReferenceWidth;
    s32 ReferenceHeight = Buffer->ReferenceHeight;
    
    r32 ScaleByWidth = GetScale(ActualWidth, ActualHeight, ReferenceWidth, ReferenceHeight, 0.0f);
    
    r32 ScaleByHeight = GetScale(ActualWidth, ActualHeight, ReferenceWidth, ReferenceHeight, 1.0f);
    
    // Font for editor gui
    {
        s32 FontSize = 12;
        FontSize = roundf((r32)FontSize * ScaleByWidth);
        
        if(MenuEditor->Font)
        {
            TTF_CloseFont(MenuEditor->Font);
        }
        
        MenuEditor->Font = TTF_OpenFont(FontPath, FontSize);
        Assert(MenuEditor->Font);
    }
    
    // figure label button
    {
        s32 X = 0;
        s32 Y = 0;
        s32 ButtonWidth  = 150;
        s32 ButtonHeight = 30;
        //s32 ButtonAmount = 4;
        
        ButtonWidth  = roundf((r32)ButtonWidth  * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByWidth);
        
        s32 InfoWidth = 75;
        s32 BoxWidth  = 25;
        
        InfoWidth = roundf((r32)InfoWidth * ScaleByWidth);
        BoxWidth  = roundf((r32)BoxWidth  * ScaleByWidth);
        
        // LevelAmount label initialization
        s32 LevelAmount = MenuEntity->ButtonsAmount;
        InitLabel(MenuEditor->Font, &MenuEditor->LevelLabel, "Hide/Show", LevelAmount, X, Y, ButtonWidth, ButtonHeight, InfoWidth, BoxWidth, Buffer);
    }
    
    // Load button
    {
        s32 X = 0;
        s32 Y = 30;
        s32 ButtonWidth  = 75;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByWidth);
        Y = roundf((r32)Y * ScaleByWidth);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByWidth);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Load", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->LoadButton, 255, 255, 255);
    }
    
    // Save button
    {
        s32 X = 75;
        s32 Y = 30;
        s32 ButtonWidth  = 75;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByWidth);
        Y = roundf((r32)Y * ScaleByWidth);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByWidth);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Save", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->SaveButton, 255, 255, 255);
    }
    
    // Sort button
    {
        s32 X = 0;
        s32 Y = 60;
        s32 ButtonWidth  = 150;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByWidth);
        Y = roundf((r32)Y * ScaleByWidth);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByWidth);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Sort", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->SortButton, 255, 255, 255);
    }
    
    // Delete button
    {
        s32 X = 0;
        s32 Y = 90;
        s32 ButtonWidth  = 75;
        s32 ButtonHeight = 30;
        X = roundf((r32)X * ScaleByWidth);
        Y = roundf((r32)Y * ScaleByWidth);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByWidth);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Delete", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->DeleteButton, 255, 255, 255);
    }
    
    // Swap button
    {
        s32 X = 75;
        s32 Y = 90;
        s32 ButtonWidth  = 75;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByWidth);
        Y = roundf((r32)Y * ScaleByWidth);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByWidth);
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Swap", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->SwapButton, 255, 255, 255);
    }
    
    // Lock button
    {
        s32 X = 0;
        s32 Y = 120;
        s32 ButtonWidth  = 75;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByWidth);
        Y = roundf((r32)Y * ScaleByWidth);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByWidth);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Lock", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->LockButton, 255, 255, 255);
    }
    
    // Unlock button
    {
        s32 X = 75;
        s32 Y = 120;
        s32 ButtonWidth  = 75;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByWidth);
        Y = roundf((r32)Y * ScaleByWidth);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByWidth);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Unlock", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->UnlockButton, 255, 255, 255);
    }
    
    // Cancel  button
    {
        s32 X = 0;
        s32 Y = 150;
        s32 ButtonWidth  = 150;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByWidth);
        Y = roundf((r32)Y * ScaleByWidth);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByWidth);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Cancel", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->CancelButton, 255, 255, 255);
    }
    
    // Previous levels button
    {
        s32 X = 0;
        s32 Y = 0;
        s32 ButtonWidth  = 30;
        s32 ButtonHeight = 30;
        
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByWidth);
        Y = (ActualHeight / 2) - (ButtonHeight / 2);
        
        MenuEditor->PrevButtonQuad.w = ButtonWidth;
        MenuEditor->PrevButtonQuad.h = ButtonHeight;
        MenuEditor->PrevButtonQuad.x = X;
        MenuEditor->PrevButtonQuad.y = Y;
    }
    
    // Next levels button
    {
        s32 X = 0;
        s32 Y = 285;
        s32 ButtonWidth  = 30;
        s32 ButtonHeight = 30;
        
        ButtonWidth  = roundf((r32)ButtonWidth * ScaleByWidth);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByWidth);
        X = ActualWidth - ButtonWidth;
        Y = (ActualHeight / 2) - (ButtonHeight / 2);
        
        MenuEditor->NextButtonQuad.w = ButtonWidth;
        MenuEditor->NextButtonQuad.h = ButtonHeight;
        MenuEditor->NextButtonQuad.x = X;
        MenuEditor->NextButtonQuad.y = Y;
    }
}

static void
MenuEditorUpdatePositionsPortrait(game_offscreen_buffer *Buffer,
                                  menu_editor *MenuEditor, menu_entity *MenuEntity, 
                                  game_memory *Memory)
{
    //
    // Reference dimension assumed to be 800x600
    //
    
    s32 ActualWidth     = Buffer->Width;
    s32 ActualHeight    = Buffer->Height;
    s32 ReferenceWidth  = Buffer->ReferenceWidth;
    s32 ReferenceHeight = Buffer->ReferenceHeight;
    
    r32 ScaleByWidth = GetScale(ActualWidth, ActualHeight, 600, 800, 0.0f);
    r32 ScaleByHeight = GetScale(ActualWidth, ActualHeight, 600, 800, 1.0f);
    
    // Font for editor gui
    {
        s32 FontSize = 12;
        FontSize = (r32)FontSize * ScaleByHeight;
        
        if(MenuEditor->Font)
        {
            TTF_CloseFont(MenuEditor->Font);
        }
        
        MenuEditor->Font = TTF_OpenFont(FontPath, FontSize);
        Assert(MenuEditor->Font);
    }
    
    // figure label button
    {
        s32 X = 0;
        s32 Y = 0;
        s32 ButtonWidth  = 150;
        s32 ButtonHeight = 30;
        //s32 ButtonAmount = 4;
        
        ButtonWidth  = roundf((r32)ButtonWidth  * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        
        s32 InfoWidth = 75;
        s32 BoxWidth  = 25;
        
        InfoWidth = roundf((r32)InfoWidth * ScaleByHeight);
        BoxWidth  = roundf((r32)BoxWidth  * ScaleByHeight);
        
        // LevelAmount label initialization
        s32 LevelAmount = MenuEntity->ButtonsAmount;
        InitLabel(MenuEditor->Font, &MenuEditor->LevelLabel, "Hide/Show", LevelAmount, X, Y, ButtonWidth, ButtonHeight, InfoWidth, BoxWidth, Buffer);
    }
    
    // Load button
    {
        s32 X = 0;
        s32 Y = 30;
        s32 ButtonWidth  = 75;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByHeight);
        Y = roundf((r32)Y * ScaleByHeight);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Load", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->LoadButton, 255, 255, 255);
    }
    
    // Save button
    {
        s32 X = 75;
        s32 Y = 30;
        s32 ButtonWidth  = 75;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByHeight);
        Y = roundf((r32)Y * ScaleByHeight);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Save", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->SaveButton, 255, 255, 255);
    }
    
    // Sort button
    {
        s32 X = 0;
        s32 Y = 60;
        s32 ButtonWidth  = 150;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByHeight);
        Y = roundf((r32)Y * ScaleByHeight);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Sort", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->SortButton, 255, 255, 255);
    }
    
    // Delete button
    {
        s32 X = 0;
        s32 Y = 90;
        s32 ButtonWidth  = 75;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByHeight);
        Y = roundf((r32)Y * ScaleByHeight);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Delete", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->DeleteButton, 255, 255, 255);
    }
    
    // Swap button
    {
        s32 X = 75;
        s32 Y = 90;
        s32 ButtonWidth  = 75;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByHeight);
        Y = roundf((r32)Y * ScaleByHeight);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Swap", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->SwapButton, 255, 255, 255);
    }
    
    // Lock button
    {
        s32 X = 0;
        s32 Y = 120;
        s32 ButtonWidth  = 75;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByHeight);
        Y = roundf((r32)Y * ScaleByHeight);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Lock", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->LockButton, 255, 255, 255);
    }
    
    // Unlock button
    {
        s32 X = 75;
        s32 Y = 120;
        s32 ButtonWidth  = 75;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByHeight);
        Y = roundf((r32)Y * ScaleByHeight);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Unlock", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->UnlockButton, 255, 255, 255);
    }
    
    // Cancel  button
    {
        s32 X = 0;
        s32 Y = 150;
        s32 ButtonWidth  = 150;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByHeight);
        Y = roundf((r32)Y * ScaleByHeight);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        
        EditorMakeTextButton(Buffer, MenuEditor->Font, "Cancel", 
                             X, Y, ButtonWidth, ButtonHeight,
                             &MenuEditor->CancelButton, 255, 255, 255);
    }
    
    // Previous levels button
    {
        s32 X = 0;
        s32 Y = 385;
        s32 ButtonWidth  = 30;
        s32 ButtonHeight = 30;
        
        X = roundf((r32)X * ScaleByHeight);
        Y = roundf((r32)Y * ScaleByHeight);
        ButtonWidth = roundf((r32)ButtonWidth * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        
        MenuEditor->PrevButtonQuad.w = ButtonWidth;
        MenuEditor->PrevButtonQuad.h = ButtonHeight;
        MenuEditor->PrevButtonQuad.x = X;
        MenuEditor->PrevButtonQuad.y = Y;
    }
    
    // Next levels button
    {
        s32 X = 0;
        s32 Y = 385;
        s32 ButtonWidth  = 30;
        s32 ButtonHeight = 30;
        
        ButtonWidth  = roundf((r32)ButtonWidth * ScaleByHeight);
        ButtonHeight = roundf((r32)ButtonHeight * ScaleByHeight);
        
        X = ActualWidth - ButtonWidth;
        Y = (ActualHeight / 2) - (ButtonHeight / 2);
        
        MenuEditor->NextButtonQuad.w = ButtonWidth;
        MenuEditor->NextButtonQuad.h = ButtonHeight;
        MenuEditor->NextButtonQuad.x = X;
        MenuEditor->NextButtonQuad.y = Y;
    }
}


static void
MenuEditorInit(menu_editor *MenuEditor, menu_entity *MenuEntity, 
               game_memory *Memory, game_offscreen_buffer *Buffer)
{
    MenuEditor->PrevButtonTexture = GetTexture(Memory, "left_arrow.png", Buffer->Renderer);
    Assert(MenuEditor->PrevButtonTexture);
    
    MenuEditor->NextButtonTexture = GetTexture(Memory, "right_arrow.png", Buffer->Renderer);
    Assert(MenuEditor->NextButtonTexture);
    
    MenuEditorUpdatePositionsLandscape(Buffer, MenuEditor, MenuEntity, Memory);
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
    for(s32 i = 0; i < 100; i++)
    {
        if(MenuEditor->SelectionPanel[i].IsSelected)
        {
            s32 Index = MenuEditor->SelectionPanel[i].ButtonIndex;
            MenuEditor->SelectionPanel[i].SelectQuad = MenuEntity->Buttons[Index].ButtonQuad;
        }
    }
    
    if(Input->MouseButtons[0].EndedDown)
    {
        if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->LevelLabel.MinusQuad))
        {
            if(MenuEntity->ButtonsAmount > 0)
            {
                MenuEntity->ButtonsAmount -= 1;
                Memory->LevelMemoryAmount -= 1;
            }
            
            UpdateLabelNumber(Buffer, MenuEditor->Font, &MenuEditor->LevelLabel, MenuEntity->ButtonsAmount);
            
            MenuEditor->ButtonIsPressed = true;
            MenuEditor->HighlightButtonQuad = MenuEditor->LevelLabel.MinusQuad;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->LevelLabel.PlusQuad))
        {
            // New Level button
            
            if(MenuEntity->ButtonsAmount < MenuEntity->ButtonsAmountReserved)
            {
                MenuEntity->ButtonsAmount += 1;
                Memory->LevelMemoryAmount += 1;
            }
            
            UpdateLabelNumber(Buffer, MenuEditor->Font, &MenuEditor->LevelLabel, MenuEntity->ButtonsAmount);
            
            MenuEditor->ButtonIsPressed = true;
            MenuEditor->HighlightButtonQuad = MenuEditor->LevelLabel.PlusQuad;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->LoadButton.Quad))
        {
            // Load levels from memory
            
            LoadLevelMemoryFromFile("package2.bin", Memory);
            MenuLoadButtonsFromMemory(MenuEntity, Memory, Buffer);
            MenuEntityAlignButtons(MenuEntity, Buffer->Width, Buffer->Height);
            
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
            MenuEditor->HighlightButtonQuad = MenuEditor->LoadButton.Quad;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->SaveButton.Quad))
        {
            // Save levels to the memory
            
            SaveLevelMemoryToFile(Memory);
            
            MenuEditor->ButtonIsPressed = true;
            MenuEditor->HighlightButtonQuad = MenuEditor->SaveButton.Quad;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->SortButton.Quad))
        {
            // Sort buttons in ascending order
            
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
            MenuEntityAlignButtons(MenuEntity, Buffer->Width, Buffer->Height);
            
            MenuEditor->ButtonIsPressed = true;
            MenuEditor->HighlightButtonQuad = MenuEditor->SortButton.Quad;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->DeleteButton.Quad))
        {
            // delete level/levels button
            
            if(MenuEditor->ButtonsSelected)
            {
                s32 LevelCounter = 0;
                for(s32 Index = 100; Index >= 0; Index--)
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
            
            MenuEditor->ButtonIsPressed = true;
            MenuEditor->HighlightButtonQuad = MenuEditor->DeleteButton.Quad;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->SwapButton.Quad))
        {
            // Swap selected buttons
            
            s32 IndexOne = 0;
            s32 IndexTwo = 0;
            s32 Counter = 0;
            for(s32 i = 0; i < 100; ++i)
            {
                if(MenuEditor->SelectionPanel[i].IsSelected)
                {
                    if(Counter == 0)
                    {
                        IndexOne = i;
                    }
                    
                    if(Counter == 1)
                    {
                        IndexTwo = i;
                    }
                    
                    Counter ++;
                }
                
                if(Counter > 2) break;
                
            }
            
            if(Counter == 2)
            {
                level_memory *LevelMemory = (level_memory *)Memory->GlobalMemoryStorage;
                
                SwapLevelMemory(LevelMemory, IndexOne, IndexTwo);
                MenuLoadButtonsFromMemory(MenuEntity, Memory, Buffer);
                MenuEntityAlignButtons(MenuEntity, Buffer->Width, Buffer->Height);
            }
            
            MenuEditor->ButtonIsPressed = true;
            MenuEditor->HighlightButtonQuad = MenuEditor->SwapButton.Quad;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->CancelButton.Quad))
        {
            // Cancel selection
            
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
                MenuEditor->HighlightButtonQuad = MenuEditor->CancelButton.Quad;
            }
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->LockButton.Quad))
        {
            // Lock levels 
            
            level_memory *LevelMemory = (level_memory *)Memory->GlobalMemoryStorage;
            
            if(MenuEditor->ButtonsSelected)
            {
                for(s32 Index = 0; Index < 100; ++Index)
                {
                    if(MenuEditor->SelectionPanel[Index].IsSelected)
                    {
                        s32 LevelIndex = MenuEditor->SelectionPanel[Index].ButtonIndex;
                        LevelMemory[LevelIndex].IsLocked = 1;
                        MenuEntity->Buttons[LevelIndex].IsLocked = true;
                        
                        MenuEditor->SelectionPanel[Index].IsSelected = false;
                    }
                }
                
                MenuEditor->ButtonsSelected = false;
            }
            else
            {
                for(s32 Index = 0; Memory->LevelMemoryAmount; ++Index)
                {
                    LevelMemory[Index].IsLocked = 1;
                    MenuEntity->Buttons[Index].IsLocked = true;
                }
            }
            
            MenuEditor->ButtonIsPressed = true;
            MenuEditor->HighlightButtonQuad = MenuEditor->LockButton.Quad;
        }
        else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->UnlockButton.Quad))
        {
            // Unlock levels
            
            level_memory *LevelMemory = (level_memory *)Memory->GlobalMemoryStorage;
            
            if(MenuEditor->ButtonsSelected)
            {
                for(s32 Index = 0; Index < 100; ++Index)
                {
                    if(MenuEditor->SelectionPanel[Index].IsSelected)
                    {
                        s32 LevelIndex = MenuEditor->SelectionPanel[Index].ButtonIndex;
                        LevelMemory[LevelIndex].IsLocked = 0;
                        MenuEntity->Buttons[LevelIndex].IsLocked = false;
                        
                        MenuEditor->SelectionPanel[Index].IsSelected = false;
                    }
                }
                
                MenuEditor->ButtonsSelected = false;
            }
            else
            {
                for(s32 Index = 0; Memory->LevelMemoryAmount; ++Index)
                {
                    LevelMemory[Index].IsLocked = 0;
                    MenuEntity->Buttons[Index].IsLocked = false;
                }
            }
            
            MenuEditor->ButtonIsPressed = true;
            MenuEditor->HighlightButtonQuad = MenuEditor->UnlockButton.Quad;
        }
        else if (IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->PrevButtonQuad))
        {
            MenuEditor->ButtonIsPressed = true;
            MenuEditor->HighlightButtonQuad = MenuEditor->PrevButtonQuad;
        }
        else if (IsPointInsideRect(Input->MouseX, Input->MouseY, &MenuEditor->NextButtonQuad))
        {
            MenuEditor->ButtonIsPressed = true;
            MenuEditor->HighlightButtonQuad = MenuEditor->NextButtonQuad;
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
        
        for(s32 i = 0; i < MenuEntity->ButtonsAmount; ++i)
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
                MenuEditor->SelectionPanel[Index].ButtonIndex = Index;
                MenuEditor->SelectionPanel[Index].IsSelected = true;
                MenuEditor->SelectionPanel[Index].SelectQuad = MenuEntity->Buttons[Index].ButtonQuad;
            }
            else
            {
                MenuEditor->SelectionPanel[Index].IsSelected = false;
            }
        }
    }
    
    if(Input->Keyboard.Q_Button.EndedDown)
    {
        MenuEditor->ButtonIsPressed = true;
        MenuEditor->HighlightButtonQuad = MenuEditor->PrevButtonQuad;
    }
    
    if(Input->Keyboard.E_Button.EndedDown)
    {
        MenuEditor->ButtonIsPressed = true;
        MenuEditor->HighlightButtonQuad = MenuEditor->NextButtonQuad;
    }
    
    LevelEditorRenderLabel(&MenuEditor->LevelLabel, Buffer, 0, 255, 0, 0, 255, 0, 150);
    
    RenderButtonQuad(Buffer, &MenuEditor->LoadButton, 0, 255, 0, 150);
    RenderButtonQuad(Buffer, &MenuEditor->SaveButton, 0, 255, 0, 150);
    
    RenderButtonQuad(Buffer, &MenuEditor->SortButton, 255, 0, 0, 150);
    
    RenderButtonQuad(Buffer, &MenuEditor->DeleteButton, 255, 0, 0, 150);
    RenderButtonQuad(Buffer, &MenuEditor->SwapButton, 255, 0, 0, 150);
    
    RenderButtonQuad(Buffer, &MenuEditor->LockButton, 255, 0, 0, 150);
    RenderButtonQuad(Buffer, &MenuEditor->UnlockButton, 255, 0, 0, 150);
    
    RenderButtonQuad(Buffer, &MenuEditor->CancelButton, 255, 0, 0, 150);
    
    GameRenderBitmapToBuffer(Buffer, MenuEditor->PrevButtonTexture, &MenuEditor->PrevButtonQuad);
    GameRenderBitmapToBuffer(Buffer, MenuEditor->NextButtonTexture, &MenuEditor->NextButtonQuad);
    
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

//
// menu_editor end
//

//
//  resolution_editor start
//

static scale_type
UpdateScaleType(scale_type ScaleType)
{
    scale_type Result;
    
    switch(ScaleType) 
    {
        case WIDTH:
        {
            Result = HEIGHT;
        } break;
        case HEIGHT:
        {
            Result = BOTH;
        } break;
        case BOTH:
        {
            Result = WIDTH;
        } break;
    }
    
    return(Result);
}

static const char*
GetOrientationString(device_orientation Orientation)
{
    const char *Result = {};
    
    if(Orientation == LANDSCAPE)
    {
        Result = "Landscape";
    }
    else if(Orientation == PORTRAIT)
    {
        Result = "Portrait";
    }
    
    return(Result);
}

inline static void
CleanResolutionNumber(resolution_editor *ResPanel, s32 OldNumber)
{
    printf("ResNumberSelected = true\n");
    
    ResPanel->ResNumberSelected = true;
    ResPanel->ResNumberBufferIndex = 0;
    ResPanel->ResOldNumber = OldNumber;
    
    ResPanel->ResNumberBuffer[0] = '\0';
    ResPanel->ResNumberBuffer[1] = '\0';
    ResPanel->ResNumberBuffer[2] = '\0';
    ResPanel->ResNumberBuffer[3] = '\0';
    ResPanel->ResNumberBuffer[4] = '\0';
}


static void
ResolutionEditorScaleLandscape(game_offscreen_buffer *Buffer,resolution_editor *ResPanel, game_memory *Memory)
{
    s32 ActualWidth = Buffer->Width;
    s32 ActualHeight = Buffer->Height;
    s32 ReferenceWidth = Buffer->ReferenceWidth;
    s32 ReferenceHeight = Buffer->ReferenceHeight;
    
    r32 ScaleByWidth = GetScale(ActualWidth, ActualHeight, ReferenceWidth, ReferenceHeight, 0.0f);
    
    r32 ScaleByHeight = GetScale(ActualWidth, ActualHeight, ReferenceWidth, ReferenceHeight, 1.0f);
    
    // Resolution font
    {
        s32 FontSize = 12;
        
        FontSize = (r32)FontSize * ScaleByWidth;
        
        if(ResPanel->Font)
        {
            TTF_CloseFont(ResPanel->Font);
        }
        
        ResPanel->Font = TTF_OpenFont(FontPath, FontSize);
        Assert(ResPanel->Font);
    }
    
    // Buttons
    {
        s32 ButtonWidth = 150;
        s32 ButtonHeight = 20;
        s32 ButtonAmount = 5;
        
        ButtonWidth  = roundf((r32) ButtonWidth  * ScaleByWidth);
        ButtonHeight = roundf((r32) ButtonHeight * ScaleByWidth);
        
        ResPanel->ResPanelQuad.w = ButtonWidth;
        ResPanel->ResPanelQuad.h = ButtonHeight * ButtonAmount;
        ResPanel->ResPanelQuad.x = 0;
        ResPanel->ResPanelQuad.y = Buffer->Height - ResPanel->ResPanelQuad.h;
        
        EditorMakeTextButton(Buffer, ResPanel->Font, "Orientation", 
                             ResPanel->ResPanelQuad.x,
                             ResPanel->ResPanelQuad.y,
                             ButtonWidth, ButtonHeight,
                             &ResPanel->ScalerHeaderButton, 255, 255, 255);
        
        s32 ArrowWidth  = 30;
        s32 SwitchWidth = 90;
        ArrowWidth  = roundf((r32)ArrowWidth * ScaleByWidth);
        SwitchWidth = roundf((r32)SwitchWidth * ScaleByWidth);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, "<", 
                             ResPanel->ResPanelQuad.x,
                             ResPanel->ResPanelQuad.y + ButtonHeight,
                             ArrowWidth, ButtonHeight,
                             &ResPanel->LeftArrowButton, 255, 255, 255);
        
        ResPanel->Orientation = LANDSCAPE;
        const char *OrientationString = GetOrientationString(LANDSCAPE);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, OrientationString, 
                             ResPanel->ResPanelQuad.x + ArrowWidth,
                             ResPanel->ResPanelQuad.y + ButtonHeight,
                             SwitchWidth, ButtonHeight,
                             &ResPanel->SwitchButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, ">", 
                             ResPanel->ResPanelQuad.x + ArrowWidth + SwitchWidth,
                             ResPanel->ResPanelQuad.y + ButtonHeight,
                             ArrowWidth, ButtonHeight,
                             &ResPanel->RightArrowButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, "Target Resolution", 
                             ResPanel->ResPanelQuad.x,
                             ResPanel->ResPanelQuad.y + (ButtonHeight * 2),
                             ButtonWidth, ButtonHeight,
                             &ResPanel->TargetResolutionHeaderButton, 255, 255, 255);
        
        s32 ButtonName  = 38;
        ButtonName = roundf((r32)ButtonName * ScaleByWidth);
        
        s32 ButtonValue = 37;
        ButtonValue = roundf((r32) ButtonValue * ScaleByWidth);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, "Width", 
                             ResPanel->ResPanelQuad.x,
                             ResPanel->ResPanelQuad.y + (ButtonHeight * 3),
                             ButtonName, ButtonHeight,
                             &ResPanel->TargetWidthNameButton, 255, 255, 255);
        
        char TargetWidthBuffer[8] = {};
        ResPanel->TargetWidth = ActualWidth;
        sprintf(TargetWidthBuffer, "%d", ResPanel->TargetWidth);
        
        char TargetHeightBuffer[8] = {};
        ResPanel->TargetHeight = ActualHeight;
        sprintf(TargetHeightBuffer, "%d", ResPanel->TargetHeight);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, TargetWidthBuffer, 
                             ResPanel->ResPanelQuad.x + ButtonName,
                             ResPanel->ResPanelQuad.y + (ButtonHeight * 3),
                             ButtonValue, ButtonHeight,
                             &ResPanel->TargetWidthNumberButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, "Height", 
                             ResPanel->ResPanelQuad.x + (ButtonName + ButtonValue),
                             ResPanel->ResPanelQuad.y + (ButtonHeight * 3),
                             ButtonName, ButtonHeight,
                             &ResPanel->TargetHeightNameButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, TargetHeightBuffer, 
                             ResPanel->ResPanelQuad.x + ((ButtonName * 2) + ButtonValue),
                             ResPanel->ResPanelQuad.y + (ButtonHeight * 3),
                             ButtonValue, ButtonHeight,
                             &ResPanel->TargetHeightNumberButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, "Apply", 
                             ResPanel->ResPanelQuad.x,
                             ResPanel->ResPanelQuad.y + (ButtonHeight * 4),
                             ButtonWidth, ButtonHeight,
                             &ResPanel->ApplyButton, 255, 255, 255);
        
    }
    
}

static void
ResolutionEditorScalePortrait(game_offscreen_buffer *Buffer,resolution_editor *ResPanel, game_memory *Memory)
{
    s32 ActualWidth = Buffer->Width;
    s32 ActualHeight = Buffer->Height;
    s32 ReferenceWidth = Buffer->ReferenceWidth;
    s32 ReferenceHeight = Buffer->ReferenceHeight;
    
    r32 ScaleByWidth = GetScale(ActualWidth, ActualHeight, 600, 800, 0.0f);
    r32 ScaleByHeight = GetScale(ActualWidth, ActualHeight, 600, 800, 1.0f);
    
    // Resolution font
    {
        s32 FontSize = 12;
        
        FontSize = (r32)FontSize * ScaleByHeight;
        
        if(ResPanel->Font)
        {
            TTF_CloseFont(ResPanel->Font);
        }
        
        ResPanel->Font = TTF_OpenFont(FontPath, FontSize);
        Assert(ResPanel->Font);
    }
    
    // Buttons
    {
        s32 ButtonWidth = 150;
        s32 ButtonHeight = 20;
        s32 ButtonAmount = 5;
        
        ButtonWidth  = roundf((r32) ButtonWidth  * ScaleByHeight);
        ButtonHeight = roundf((r32) ButtonHeight * ScaleByHeight);
        
        ResPanel->ResPanelQuad.w = ButtonWidth;
        ResPanel->ResPanelQuad.h = ButtonHeight * ButtonAmount;
        ResPanel->ResPanelQuad.x = 0;
        ResPanel->ResPanelQuad.y = Buffer->Height - ResPanel->ResPanelQuad.h;
        
        EditorMakeTextButton(Buffer, ResPanel->Font, "Orientation", 
                             ResPanel->ResPanelQuad.x,
                             ResPanel->ResPanelQuad.y,
                             ButtonWidth, ButtonHeight,
                             &ResPanel->ScalerHeaderButton, 255, 255, 255);
        
        s32 ArrowWidth  = 30;
        s32 SwitchWidth = 90;
        ArrowWidth  = roundf((r32)ArrowWidth * ScaleByHeight);
        SwitchWidth = roundf((r32)SwitchWidth * ScaleByHeight);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, "<", 
                             ResPanel->ResPanelQuad.x,
                             ResPanel->ResPanelQuad.y + ButtonHeight,
                             ArrowWidth, ButtonHeight,
                             &ResPanel->LeftArrowButton, 255, 255, 255);
        
        ResPanel->Orientation = LANDSCAPE;
        const char *OrientationString = GetOrientationString(LANDSCAPE);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, OrientationString, 
                             ResPanel->ResPanelQuad.x + ArrowWidth,
                             ResPanel->ResPanelQuad.y + ButtonHeight,
                             SwitchWidth, ButtonHeight,
                             &ResPanel->SwitchButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, ">", 
                             ResPanel->ResPanelQuad.x + ArrowWidth + SwitchWidth,
                             ResPanel->ResPanelQuad.y + ButtonHeight,
                             ArrowWidth, ButtonHeight,
                             &ResPanel->RightArrowButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, "Target Resolution", 
                             ResPanel->ResPanelQuad.x,
                             ResPanel->ResPanelQuad.y + (ButtonHeight * 2),
                             ButtonWidth, ButtonHeight,
                             &ResPanel->TargetResolutionHeaderButton, 255, 255, 255);
        
        s32 ButtonName  = 38;
        ButtonName = roundf((r32)ButtonName * ScaleByHeight);
        
        s32 ButtonValue = 37;
        ButtonValue = roundf((r32) ButtonValue * ScaleByHeight);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, "Width", 
                             ResPanel->ResPanelQuad.x,
                             ResPanel->ResPanelQuad.y + (ButtonHeight * 3),
                             ButtonName, ButtonHeight,
                             &ResPanel->TargetWidthNameButton, 255, 255, 255);
        
        char TargetWidthBuffer[8] = {};
        ResPanel->TargetWidth = ActualWidth;
        sprintf(TargetWidthBuffer, "%d", ResPanel->TargetWidth);
        
        char TargetHeightBuffer[8] = {};
        ResPanel->TargetHeight = ActualHeight;
        sprintf(TargetHeightBuffer, "%d", ResPanel->TargetHeight);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, TargetWidthBuffer, 
                             ResPanel->ResPanelQuad.x + ButtonName,
                             ResPanel->ResPanelQuad.y + (ButtonHeight * 3),
                             ButtonValue, ButtonHeight,
                             &ResPanel->TargetWidthNumberButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, "Height", 
                             ResPanel->ResPanelQuad.x + (ButtonName + ButtonValue),
                             ResPanel->ResPanelQuad.y + (ButtonHeight * 3),
                             ButtonName, ButtonHeight,
                             &ResPanel->TargetHeightNameButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, TargetHeightBuffer, 
                             ResPanel->ResPanelQuad.x + ((ButtonName * 2) + ButtonValue),
                             ResPanel->ResPanelQuad.y + (ButtonHeight * 3),
                             ButtonValue, ButtonHeight,
                             &ResPanel->TargetHeightNumberButton, 255, 255, 255);
        
        EditorMakeTextButton(Buffer, ResPanel->Font, "Apply", 
                             ResPanel->ResPanelQuad.x,
                             ResPanel->ResPanelQuad.y + (ButtonHeight * 4),
                             ButtonWidth, ButtonHeight,
                             &ResPanel->ApplyButton, 255, 255, 255);
        
    }
}

static void
ResolutionEditorInit(game_offscreen_buffer *Buffer, game_memory *Memory, resolution_editor *ResEditor)
{
    ResolutionEditorScaleLandscape(Buffer, ResEditor, Memory);
}


static void
ResolutionEditorUpdateAndRender(game_offscreen_buffer *Buffer, game_input *Input, game_memory *Memory, resolution_editor *ResPanel)
{
    playground *LevelEntity  = (playground *)Memory->LocalMemoryStorage;
    menu_entity  *MenuEntity   = (menu_entity*) (((char*)Memory->LocalMemoryStorage) + (sizeof(playground))); 
    
    game_editor *GameEditor = (game_editor *)Memory->EditorMemoryStorage;
    
    level_editor *LevelEditor = &GameEditor->LevelEditor;
    menu_editor *MenuEditor   = &GameEditor->MenuEditor;
    
    
    if(ResPanel->ResNumberSelected)
    {
        s32 DigitIndex = ResPanel->ResNumberBufferIndex;
        char NextDigit = GetNumberFromInput(DigitIndex, Input);
        
        if(NextDigit)
        {
            if(DigitIndex <= 3 && NextDigit != '\n')
            {
                ResPanel->ResNumberBuffer[DigitIndex] = NextDigit;
                ResPanel->ResNumberBufferIndex = ++DigitIndex;
                
                button_quad *CurrentButtonQuad;
                switch(ResPanel->ResolutionType)
                {
                    case TARGET_WIDTH:
                    {
                        CurrentButtonQuad = &ResPanel->TargetWidthNumberButton;
                    } break;
                    
                    case TARGET_HEIGHT:
                    {
                        CurrentButtonQuad = &ResPanel->TargetHeightNumberButton;
                    } break;
                }
                
                ButtonQuadUpdateTextureOnButton(Buffer, ResPanel->Font,
                                                ResPanel->ResNumberBuffer, CurrentButtonQuad, 255, 255, 255);
                
            }
        }
    }
    
    if(Input->Keyboard.Enter.EndedDown)
    {
        if(ResPanel->ResNumberSelected)
        {
            ResPanel->ResNumberSelected = false;
            s32 ResultNumber = strtol(ResPanel->ResNumberBuffer, 0, 10);
            
            switch(ResPanel->ResolutionType)
            {
                case TARGET_WIDTH:
                {
                    ResPanel->TargetWidth = ResultNumber;
                } break;
                
                case TARGET_HEIGHT:
                {
                    ResPanel->TargetHeight = ResultNumber;
                } break;
            }
        }
    }
    
    if(Input->MouseButtons[0].EndedDown)
    {
        if(ResPanel->ResNumberSelected)
        {
            ResPanel->ResNumberSelected = false;
            
            char StringBuffer[4] = {};
            sprintf(StringBuffer, "%d", ResPanel->ResOldNumber);
            
            button_quad *CurrentButtonQuad;
            res_type ResType = ResPanel->ResolutionType;
            
            switch (ResType)
            {
                case TARGET_WIDTH:
                {
                    CurrentButtonQuad = &ResPanel->TargetWidthNumberButton;
                } break;
                
                case TARGET_HEIGHT:
                {
                    CurrentButtonQuad = &ResPanel->TargetHeightNumberButton;
                } break;
            }
            
            ButtonQuadUpdateTextureOnButton(Buffer, ResPanel->Font,
                                            StringBuffer, CurrentButtonQuad, 
                                            255, 255, 255);
        }
        
        if(IsPointInsideRect(Input->MouseX, Input->MouseY, &ResPanel->ResPanelQuad))
        {
            if(IsPointInsideRect(Input->MouseX, Input->MouseY, &ResPanel->LeftArrowButton.Quad))
            {
                printf("Left arrow\n");
                
                s32 TempVariable = Buffer->Width;
                Buffer->Width = Buffer->Height;
                Buffer->Height = TempVariable;
                
                SDL_RenderSetLogicalSize(Buffer->Renderer, Buffer->Width, Buffer->Height);
                
                if(Buffer->Width > Buffer->Height)
                {
                    //LevelEntityUpdatePositionsLandscape(Buffer, Memory, LevelEntity);
                    //MenuEntityUpdatePositionsLandscape(Buffer, MenuEntity, Memory);
                    
                    //LevelEditorUpdatePositionsLandscape(Buffer, LevelEditor, LevelEntity, Memory);
                    //MenuEditorUpdatePositionsLandscape(Buffer, MenuEditor, MenuEntity, Memory);
                    //ResolutionEditorScaleLandscape(Buffer,ResPanel, Memory);
                    
                    ButtonQuadUpdateTextureOnButton(Buffer, ResPanel->Font, "Landscape", &ResPanel->SwitchButton, 255, 255, 255);
                }
                else
                {
                    //LevelEntityUpdatePositionsPortrait(Buffer, Memory);
                    //MenuEntityUpdatePositionsPortrait(Buffer, MenuEntity, Memory);
                    
                    //LevelEditorUpdatePositionsPortrait(Buffer, LevelEditor, LevelEntity, Memory);
                    //MenuEditorUpdatePositionsPortrait(Buffer, MenuEditor, MenuEntity, Memory);
                    
                    //ResolutionEditorScalePortrait(Buffer,ResPanel, Memory);
                    
                    ButtonQuadUpdateTextureOnButton(Buffer, ResPanel->Font, "Portrait", &ResPanel->SwitchButton, 255, 255, 255);
                    
                }
                
                
                
                ResPanel->ButtonSelected = true;
                ResPanel->HighlightButtonQuad = ResPanel->LeftArrowButton.Quad;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &ResPanel->RightArrowButton.Quad))
            {
                printf("Right arrow\n");
                
                s32 TempVariable = Buffer->Width;
                Buffer->Width = Buffer->Height;
                Buffer->Height = TempVariable;
                
                SDL_RenderSetLogicalSize(Buffer->Renderer, Buffer->Width, Buffer->Height);
                
                if(Buffer->Width > Buffer->Height)
                {
                    //LevelEntityUpdatePositionsLandscape(Buffer, Memory, LevelEntity);
                    //MenuEntityUpdatePositionsLandscape(Buffer, MenuEntity, Memory);
                    //LevelEditorUpdatePositionsLandscape(Buffer, LevelEditor, LevelEntity, Memory);
                    //MenuEditorUpdatePositionsLandscape(Buffer, MenuEditor, MenuEntity, Memory);
                    
                    //ResolutionEditorScaleLandscape(Buffer,ResPanel, Memory);
                    ButtonQuadUpdateTextureOnButton(Buffer, ResPanel->Font, "Landscape", &ResPanel->SwitchButton, 255, 255, 255);
                }
                else
                {
                    //LevelEntityUpdatePositionsPortrait(Buffer, Memory);
                    //MenuEntityUpdatePositionsLandscape(Buffer, MenuEntity, Memory);
                    //LevelEditorUpdatePositionsPortrait(Buffer, LevelEditor, LevelEntity, Memory);
                    //MenuEditorUpdatePositionsPortrait(Buffer, MenuEditor, MenuEntity, Memory);
                    //ResolutionEditorScalePortrait(Buffer,ResPanel, Memory);
                    ButtonQuadUpdateTextureOnButton(Buffer, ResPanel->Font, "Portrait", &ResPanel->SwitchButton, 255, 255, 255);
                }
                
                
                ResPanel->ButtonSelected = true;
                ResPanel->HighlightButtonQuad = ResPanel->RightArrowButton.Quad;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &ResPanel->TargetWidthNumberButton.Quad))
            {
                printf("target width\n");
                
                CleanResolutionNumber(ResPanel, ResPanel->TargetWidth);
                ResPanel->ResolutionType = TARGET_WIDTH;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &ResPanel->TargetHeightNumberButton.Quad))
            {
                printf("target heigth\n");
                
                CleanResolutionNumber(ResPanel, ResPanel->TargetHeight);
                
                ResPanel->ResolutionType = TARGET_HEIGHT;
            }
            else if(IsPointInsideRect(Input->MouseX, Input->MouseY, &ResPanel->ApplyButton.Quad))
            {
                printf("apply button\n");
                
                Buffer->Width  = ResPanel->TargetWidth;
                Buffer->Height = ResPanel->TargetHeight;
                
                SDL_RenderSetLogicalSize(Buffer->Renderer, Buffer->Width, Buffer->Height);
                
                if(Buffer->Width > Buffer->Height)
                {
                    //LevelEntityUpdatePositionsLandscape(Buffer, Memory, LevelEntity);
                    //MenuEntityUpdatePositionsLandscape(Buffer, MenuEntity, Memory);
                    //LevelEditorUpdatePositionsLandscape(Buffer, LevelEditor, LevelEntity, Memory);
                    //MenuEditorUpdatePositionsLandscape(Buffer, MenuEditor, MenuEntity, Memory);
                    //ResolutionEditorScaleLandscape(Buffer,ResPanel, Memory);
                    
                    ButtonQuadUpdateTextureOnButton(Buffer, ResPanel->Font, "Landscape", &ResPanel->SwitchButton, 255, 255, 255);
                }
                else
                {
                    //LevelEntityUpdatePositionsPortrait(Buffer, Memory);
                    //MenuEntityUpdatePositionsPortrait(Buffer, MenuEntity, Memory);
                    //LevelEditorUpdatePositionsPortrait(Buffer, LevelEditor, LevelEntity, Memory);
                    //MenuEditorUpdatePositionsPortrait(Buffer, MenuEditor, MenuEntity, Memory);
                    //ResolutionEditorScalePortrait(Buffer,ResPanel, Memory);
                    
                    ButtonQuadUpdateTextureOnButton(Buffer, ResPanel->Font, "Landscape", &ResPanel->SwitchButton, 255, 255, 255);
                }
                
            }
        }
    }
    
    if(ResPanel->ResNumberSelected)
    {
        game_rect CurrentNumberTextureQuad = {};
        
        switch(ResPanel->ResolutionType)
        {
            case TARGET_WIDTH:
            {
                CurrentNumberTextureQuad = ResPanel->TargetWidthNumberButton.TextureQuad;
            } break;
            
            case TARGET_HEIGHT:
            {
                CurrentNumberTextureQuad = ResPanel->TargetHeightNumberButton.TextureQuad;
            } break;
        }
        
        DEBUGRenderQuadFill(Buffer, &CurrentNumberTextureQuad, {255, 0, 255}, 255);
    }
    
    
    /* Resolution panel rendering */
    RenderButtonQuad(Buffer, &ResPanel->ScalerHeaderButton,  128, 128, 128, 100);
    
    RenderButtonQuad(Buffer, &ResPanel->LeftArrowButton,  0, 255, 0, 100);
    RenderButtonQuad(Buffer, &ResPanel->SwitchButton,     0, 255, 0, 100);
    RenderButtonQuad(Buffer, &ResPanel->RightArrowButton, 0, 255, 0, 100);
    
    
    /* Target resolution buttons*/
    RenderButtonQuad(Buffer, &ResPanel->TargetResolutionHeaderButton,  128, 128, 128, 100);
    
    RenderButtonQuad(Buffer, &ResPanel->TargetWidthNameButton, 0, 0, 255, 100);
    RenderButtonQuad(Buffer, &ResPanel->TargetWidthNumberButton, 0, 0, 255, 100);
    RenderButtonQuad(Buffer, &ResPanel->TargetHeightNameButton, 0, 0, 255, 100);
    RenderButtonQuad(Buffer, &ResPanel->TargetHeightNumberButton, 0, 0, 255, 100);
    
    /* Apply button */
    RenderButtonQuad(Buffer, &ResPanel->ApplyButton, 0, 255, 255, 100);
}

//
// resolution_editor end
//


//
// game_editor start
//

static void
GameEditorInit(game_offscreen_buffer *Buffer, playground *LevelEntity, menu_entity *MenuEntity, game_memory *Memory, game_editor *GameEditor)
{
    /* level_editor initialization */ 
    LevelEditorInit(&GameEditor->LevelEditor, LevelEntity, Memory, Buffer);
    
    /* menu_editor initialization */ 
    MenuEditorInit(&GameEditor->MenuEditor, MenuEntity, Memory, Buffer);
    
    /* resolution_editor initialization */ 
    ResolutionEditorInit(Buffer, Memory, &GameEditor->ResEditor);
}

static void
GameEditorUpdateAndRender(game_offscreen_buffer *Buffer, game_state *GameState, game_memory *Memory, game_input *Input, game_editor *GameEditor, playground *LevelEntity, menu_entity *MenuEntity)
{
    b32 EditorMode         = GameState->EditorMode;
    game_mode CurrentMode  = GameState->CurrentMode;
    
    if(Input->Keyboard.BackQuote.EndedDown)
    {
        if(!GameState->EditorMode)
        {
            GameState->EditorMode = true;
        }
        else
        {
            GameState->EditorMode = false;
        }
    }
    
    /* if editor mode was not on and it still is not on then quit*/
    if(!EditorMode && !GameState->EditorMode)
    {
        return;
    }
    
    
    if(!EditorMode && GameState->EditorMode)
    {
        /* Doing things necessary before entering in the editor mode*/
        
        LevelEntity->LevelPaused = true;
        MenuEntity->IsPaused     = true;
        
        switch(CurrentMode)
        {
            case LEVEL:
            {
                RestartLevelEntity(LevelEntity);
                
            }break;
            
            case LEVEL_MENU:
            {
                
            } break;
            
            case MAIN_MENU:
            {
                
            } break;
        }
    }
    
    if(EditorMode && !GameState->EditorMode)
    {
        /* Doing things necessary before closing the editor mode */
        
        LevelEntity->LevelPaused = false;
        MenuEntity->IsPaused     = false;
        
        switch(CurrentMode)
        {
            case LEVEL:
            {
                if(GameEditor->LevelEditor.CursorType != ARROW)
                {
                    LevelEditorSetCursorType(ARROW);
                }
                
            }break;
            
            case LEVEL_MENU:
            {
                
            } break;
            case MAIN_MENU:
            {
                
            } break;
        }
        
        return;
    }
    
    
    /* game_editor update routine */
    
    switch(CurrentMode)
    {
        case LEVEL:
        {
            LevelEditorUpdateAndRender(&GameEditor->LevelEditor, LevelEntity, Memory, Buffer, Input);
        } break;
        
        case LEVEL_MENU:
        {
            MenuEditorUpdateAndRender(&GameEditor->MenuEditor, MenuEntity, Memory, Input, Buffer);
        } break;
        
        case MAIN_MENU:
        {
            
        } break;
    }
    
    ResolutionEditorUpdateAndRender(Buffer, Input, Memory, &GameEditor->ResEditor);
}


//
// game_editor end
//

#endif