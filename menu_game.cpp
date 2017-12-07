// menu_game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

#include "menu_game.h"

static void
MenuUpdateAndRender(game_offscreen_buffer *Buffer, game_memory *Memory, game_input *Input)
{
    s32 OffsetX = 0;
    
    if(Input->WasPressed)
    {
        if(Input->LeftClick.IsDown)
        {
            Memory->MenuEntity->IsMoving    = true;
            Memory->MenuEntity->IsAnimating = false;
            
            Memory->MenuEntity->OldMouseX      = Input->MouseX;
            Memory->MenuEntity->ScrollingTicks = SDL_GetTicks();
            
            Memory->MenuEntity->TargetPosition  = 0;
            Memory->MenuEntity->Velocity.x      = 0;
            
            
            u32 BeginIndex = Memory->MenuEntity->TargetIndex * 20;
            u32 EndIndex   = BeginIndex + 20;
            for(u32 i = BeginIndex; i < EndIndex; ++i)
            {
                if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Memory->MenuEntity->Buttons[i].ButtonQuad))
                {
                    printf("Button index = %d\n", i);
                    Memory->MenuEntity->ButtonIndex = i;
                    break;
                }
            }
        }
        else if(Input->LeftClick.WasDown)
        {
            Memory->MenuEntity->IsMoving    = false;
            Memory->MenuEntity->ScrollingTicks = SDL_GetTicks() - Memory->MenuEntity->ScrollingTicks;
            Memory->MenuEntity->MaxVelocity = 20.0f;
            Memory->MenuEntity->ButtonIndex = -1;
            
            u32 ButtonsAreaAmount = (Memory->MenuEntity->ButtonsAmount / 20) + 1;
            
            if(abs(Memory->MenuEntity->TargetPosition) > Memory->MenuEntity->ButtonSizeWidth / 4)
            {
                Memory->MenuEntity->IsAnimating = true;
                
                s32 Center_x = 0;
                s32 Center_y = 0;
                s32 CenterOffset = 0;
                
                if(Memory->MenuEntity->ScrollingTicks < 500)
                {
                    bool ShouldJump = false;
                    
                    s32 LeftBorder  = Memory->MenuEntity->ButtonsArea[0].x;
                    s32 RightBorder = Memory->MenuEntity->ButtonsArea[ButtonsAreaAmount-1].x + Memory->MenuEntity->ButtonsArea[ButtonsAreaAmount-1].w;
                    
                    ShouldJump = (LeftBorder >= Buffer->Width / 2) || (RightBorder <= Buffer->Width / 2);
                    
                    if(!ShouldJump)
                    {
                        s32 TargetOffset = Memory->MenuEntity->TargetPosition;
                        if(abs(TargetOffset) < Memory->MenuEntity->ButtonsArea[0].w)
                        {
                            TargetOffset = TargetOffset > 0
                                ? Buffer->Width
                                : -Buffer->Width;
                            
                            Memory->MenuEntity->MaxVelocity   *= 2;
                            Memory->MenuEntity->TargetPosition = TargetOffset;
                            Memory->MenuEntity->ButtonIndex = -1;
                        }
                    }
                }
                
                if(Memory->MenuEntity->TargetPosition > 0)
                {
                    Center_x = Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x + (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w * 0.2f);
                }
                else
                {
                    Center_x = Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x + (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w * 0.8f);
                }
                
                CenterOffset = Center_x - Memory->MenuEntity->TargetPosition;
                
                
                for(u32 i = 0; i < ButtonsAreaAmount; ++i)
                {
                    game_rect TargetArea;
                    TargetArea.x = Memory->MenuEntity->ButtonsArea[i].x + (Memory->MenuEntity->ButtonsArea[i].w / 2) - (Buffer->Width / 2);
                    TargetArea.y = 0;
                    TargetArea.w = Buffer->Width;
                    TargetArea.h = Buffer->Height;
                    
                    if(IsPointInsideRect(CenterOffset, Center_y, &TargetArea))
                    {
                        Memory->MenuEntity->TargetIndex = i;
                        break;
                    }
                }
                
                Memory->MenuEntity->TargetPosition = Buffer->Width / 2;
            }
            else
            {
                printf("gotta be button click!\n");
                
                Memory->MenuEntity->IsAnimating = true;
                Memory->MenuEntity->TargetPosition = Buffer->Width / 2;
            }
            
        }
    }
    
    if(Input->MouseMotion && Memory->MenuEntity->IsMoving)
    {
        OffsetX += Input->MouseRelX;
        Memory->MenuEntity->TargetPosition += OffsetX;
        
        if(abs(Memory->MenuEntity->TargetPosition) >= Memory->MenuEntity->ButtonSizeWidth / 4)
        {
            Memory->MenuEntity->ButtonIndex = -1;
        }
    }
    
    
    if(Memory->MenuEntity->IsAnimating)
    {
        s32 RelativeCenter = Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x + (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w/2);
        
        vector2 Vector = { Memory->MenuEntity->TargetPosition - RelativeCenter, 0 };
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
                Vector.x *= Distance / ApproachRadius * Memory->MenuEntity->MaxVelocity;
                Vector.y *= Distance / ApproachRadius * Memory->MenuEntity->MaxVelocity;
            }
            else
            {
                s32 Width = (Buffer->Width / 2) - (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w / 2);
                s32 Offset = Width - Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x;
                
                Vector.x = 0;
                Vector.y = 0;
                
                Memory->MenuEntity->Velocity.x     = 0;
                Memory->MenuEntity->TargetPosition = 0;
                Memory->MenuEntity->IsAnimating    = false;
                
                u32 ButtonsAreaAmount = (Memory->MenuEntity->ButtonsAmount / 20) + 1;
                for(u32 i = 0; i < ButtonsAreaAmount; ++i)
                {
                    Memory->MenuEntity->ButtonsArea[i].x += Offset;
                }
                
                for(u32 i = 0; i < Memory->MenuEntity->ButtonsAmount; ++i)
                {
                    Memory->MenuEntity->Buttons[i].ButtonQuad.x += Offset;
                    Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad.x += Offset;
                }
            }
        }
        else
        {
            Vector.x *= Memory->MenuEntity->MaxVelocity;
            Vector.y *= Memory->MenuEntity->MaxVelocity;
        }
        
        
        Acceleration.x = Vector.x - Memory->MenuEntity->Velocity.x;
        Vector2Add(&Memory->MenuEntity->Velocity, &Acceleration);
    }
    
    //
    // Menu Rendering
    //
    
    game_rect ScreenQuad = {0, 0, Buffer->Width, Buffer->Height };
    DEBUGRenderQuadFill(Buffer, &ScreenQuad, {0, 0, 0}, 0);
    
    u32 ButtonsAreaAmount = (Memory->MenuEntity->ButtonsAmount / 20) + 1;
    for(u32 i = 0; i < ButtonsAreaAmount; ++i)
    {
        if(Memory->MenuEntity->IsMoving)
        {
            Memory->MenuEntity->ButtonsArea[i].x += OffsetX;
            {
                game_rect TargetArea;
                TargetArea.x = Memory->MenuEntity->ButtonsArea[i].x + (Memory->MenuEntity->ButtonsArea[i].w / 2) - (Buffer->Width / 2);
                TargetArea.y = 0;
                TargetArea.w = Buffer->Width;
                TargetArea.h = Buffer->Height;
                
                DEBUGRenderQuad(Buffer, &TargetArea, {255, 0, 0}, 255);
            }
        }
        
        if(Memory->MenuEntity->IsAnimating)
        {
            Memory->MenuEntity->ButtonsArea[i].x += roundf(Memory->MenuEntity->Velocity.x);
        }
        
        DEBUGRenderQuad(Buffer, &Memory->MenuEntity->ButtonsArea[i], {255, 255, 255}, 255);
    }
    
    for(u32 i = 0; i < Memory->MenuEntity->ButtonsAmount; ++i)
    {
        if(Memory->MenuEntity->IsMoving)
        {
            Memory->MenuEntity->Buttons[i].ButtonQuad.x += OffsetX;
            Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad.x += OffsetX;
        }
        
        if(Memory->MenuEntity->IsAnimating)
        {
            Memory->MenuEntity->Buttons[i].ButtonQuad.x += roundf(Memory->MenuEntity->Velocity.x);
            Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad.x += roundf(Memory->MenuEntity->Velocity.x);
        }
        
        if(Memory->MenuEntity->ButtonIndex == i)
        {
            game_point AreaCenter = {0};
            AreaCenter.x = Memory->MenuEntity->Buttons[i].ButtonQuad.x + (Memory->MenuEntity->Buttons[i].ButtonQuad.w / 2);
            AreaCenter.y = Memory->MenuEntity->Buttons[i].ButtonQuad.y + (Memory->MenuEntity->Buttons[i].ButtonQuad.h / 2);
            
            game_rect AreaQuad = {0};
            AreaQuad.w = Memory->MenuEntity->Buttons[i].ButtonQuad.w * 1.5f;
            AreaQuad.h = Memory->MenuEntity->Buttons[i].ButtonQuad.h * 1.5f;
            AreaQuad.x = AreaCenter.x - (AreaQuad.w / 2);
            AreaQuad.y = AreaCenter.y - (AreaQuad.h / 2);
            
            GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->BackTexture, &AreaQuad);
            GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->Buttons[i].LevelNumberTexture,
                                     &Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad);
        }
        else
        {
            GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->BackTexture, &Memory->MenuEntity->Buttons[i].ButtonQuad);
            GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->Buttons[i].LevelNumberTexture,
                                     &Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad);
        }
        
        
    }
    
    
    if(Memory->MenuEntity->IsMoving)
    {
        s32 ButtonCenter = Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x + (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w / 2);
        s32 Target = ButtonCenter + Memory->MenuEntity->TargetPosition;
        DEBUGRenderLine(Buffer, 
                        ButtonCenter, Buffer->Height / 2,
                        Target, Buffer->Height / 2,
                        {0, 255, 0}, 255);
    }
}


