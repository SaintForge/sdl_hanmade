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
                        Playground->GridEntity.StickUnitsAmount -= 1;
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
                    
                    printf("RowAmount: %d\n", Playground->GridEntity.RowAmount);
                }
                
                // NOTE(msokolov): Add Column in Grid
                ButtonRectangle.Min.x -= 60.0f;
                ButtonRectangle.Min.y += 60.0f;
                SetDim(&ButtonRectangle, 60, 60);
                if(IsInRectangle(MousePos, ButtonRectangle))
                {
                    if (Playground->GridEntity.ColumnAmount < COLUMN_AMOUNT_MAXIMUM)
                        Playground->GridEntity.ColumnAmount++;
                    
                    printf("ColumnAmount: %d\n", Playground->GridEntity.ColumnAmount);
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
    
    /* Level Number Rendering */
    v2 Dim = QueryTextureDim(Playground->LevelNumberTexture);
    PushBitmap(RenderGroup, Playground->LevelNumberTexture, {0.0f, 0.0f, Dim.w, Dim.h});
}
