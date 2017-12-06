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
            printf("left click is down!\n");
            Memory->MenuEntity->IsMoving  = true;
            Memory->MenuEntity->OldMouseX = Input->MouseX;
            Memory->MenuEntity->IsAnimating = false;
            Memory->MenuEntity->ScrollingTicks = SDL_GetTicks();
            Memory->MenuEntity->TargetPosition = 0;
            Memory->MenuEntity->Velocity.x = 0;
            Memory->MenuEntity->AccelerationSum = 0;
            
            printf("before Memory->MenuEntity->ButtonsArea[0].x = %d\n", Memory->MenuEntity->ButtonsArea[0].x);
        }
        else if(Input->LeftClick.WasDown)
        {
            printf("left click was down!\n");
            Memory->MenuEntity->IsMoving  = false;
            Memory->MenuEntity->IsAnimating = true;
            Memory->MenuEntity->NewMouseX = Input->MouseX;
            Memory->MenuEntity->ScrollingTicks = SDL_GetTicks() - Memory->MenuEntity->ScrollingTicks;
            
            printf("ScrollingTicks = %d\n", Memory->MenuEntity->ScrollingTicks);
            
#if 0
            if(Memory->MenuEntity->ScrollingTicks > 500)
            {
                Memory->MenuEntity->TargetPosition = Memory->MenuEntity->OldMouseX - Memory->MenuEntity->NewMouseX;
            }
            else
            {
                Memory->MenuEntity->TargetPosition = Memory->MenuEntity->NewMouseX - Memory->MenuEntity->OldMouseX;
            }
#endif
            
            s32 Center_x = Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x + (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w / 2);
            s32 Center_y = Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].y + (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].h / 2);
            
            s32 CenterOffset = Center_x - Memory->MenuEntity->TargetPosition;
            
            
            s32 LeftBoundary  = 0;
            s32 RightBoundary = 0;
            u32 ButtonsAreaAmount = (Memory->MenuEntity->ButtonsAmount / 20) + 1;
            if(ButtonsAreaAmount > 0)
            {
                LeftBoundary  = Memory->MenuEntity->ButtonsArea[0].x;
                RightBoundary = Memory->MenuEntity->ButtonsArea[ButtonsAreaAmount - 1].x + (Memory->MenuEntity->ButtonsArea[ButtonsAreaAmount - 1]. w / 2);
            }
            
            printf("TargetPosition before = %f\n", Memory->MenuEntity->TargetPosition);
            printf("Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x = %d\n", Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x);
            
            //TODO(max): Check only these first two conditions. The third condition will be checked after the movement was done
            if(LeftBoundary + Memory->MenuEntity->TargetPosition >= Buffer->Width)
            {
                printf("Left edge!\n");
                Memory->MenuEntity->TargetPosition = Center_x - Memory->MenuEntity->TargetPosition;
            }
            else if(RightBoundary + Memory->MenuEntity->TargetPosition <= 0)
            {
                printf("right edge!\n");
                Memory->MenuEntity->TargetPosition = Memory->MenuEntity->TargetPosition - RightBoundary;
            }
            else
            {
                printf("CenterOffset = %d\n", CenterOffset);
                
                if(Memory->MenuEntity->TargetPosition > 0)
                {
                    Center_x = Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x + (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w * 0.2f);
                }
                else
                {
                    Center_x = Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x + (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w * 1.5f);
                }
                
                CenterOffset = Center_x - Memory->MenuEntity->TargetPosition;
                
                for(u32 i = 0; i < ButtonsAreaAmount; ++i)
                {
                    game_rect TargetArea;
                    TargetArea.x = Memory->MenuEntity->ButtonsArea[i].x + (Memory->MenuEntity->ButtonsArea[i].w / 2) - (Buffer->Width / 2);
                    TargetArea.y = Memory->MenuEntity->ButtonsArea[i].y + (Memory->MenuEntity->ButtonsArea[i].h / 2);
                    TargetArea.w = Buffer->Width;
                    TargetArea.h = Buffer->Height;
                    
                    if(IsPointInsideRect(CenterOffset, Center_y, &TargetArea))
                    {
                        Memory->MenuEntity->TargetIndex = i;
                        printf("gotcha!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                        printf("TargetIndex = %d\n", Memory->MenuEntity->TargetIndex);
                        
                        
                        s32 ScreenCenter_x = Buffer->Width / 2;
                        Memory->MenuEntity->TargetPosition = ScreenCenter_x;
                        
                        //Memory->MenuEntity->TargetPosition = Memory->MenuEntity->ButtonsArea[i].x + (Memory->MenuEntity->ButtonsArea[i].w / 2) - ScreenCenter_x;
                        
                        
                        
                        break;
                    }
                    
                    //Memory->MenuEntity->TargetIndex = 1;
                }
            }
            
            printf("LeftBoundary = %d\n", LeftBoundary);
            printf("RightBoundary = %d\n", RightBoundary);
            printf("TargetPosition = %f\n", Memory->MenuEntity->TargetPosition);
        }
    }
    
    if(Input->MouseMotion && Memory->MenuEntity->IsMoving)
    {
        OffsetX += Input->MouseRelX;
        Memory->MenuEntity->TargetPosition += OffsetX;
    }
    
    game_rect ScreenQuad = {0, 0, Buffer->Width, Buffer->Height };
    DEBUGRenderQuadFill(Buffer, &ScreenQuad, {0, 0, 0}, 0);
    
    if(Memory->MenuEntity->IsAnimating)
    {
        s32 RelativeCenter = Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x + (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w/2);
        vector2 Vector = { Memory->MenuEntity->TargetPosition - RelativeCenter, 0 };
        
        r32 Ratio = 0;
        r32 Distance = Vector2Mag(&Vector);
        r32 ApproachRadius = 100.0f;
        Vector2Norm(&Vector);
        if(Distance < ApproachRadius)
        {
            Ratio = Distance / ApproachRadius;
            //printf("Ratio = %f\n", Ratio);
            if(Ratio > 0.02f)
            {
                Vector.x *= Distance / ApproachRadius * Memory->MenuEntity->MaxVelocity;
                Vector.y *= Distance / ApproachRadius * Memory->MenuEntity->MaxVelocity;
            }
            else
            {
                printf("TargetIndex = %d\n", Memory->MenuEntity->TargetIndex);
                s32 Width = (Buffer->Width / 2) - (Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].w / 2);
                s32 Offset = Width - Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x;
                printf("Offset = %d\n", Offset);
                
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
                
                //printf("after Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x = %d\n", Memory->MenuEntity->ButtonsArea[Memory->MenuEntity->TargetIndex].x);
            }
        }
        else
        {
            Vector.x *= Memory->MenuEntity->MaxVelocity;
            Vector.y *= Memory->MenuEntity->MaxVelocity;
        }
        
        Memory->MenuEntity->Acceleration.x = Vector.x - Memory->MenuEntity->Velocity.x;
        Vector2Add(&Memory->MenuEntity->Velocity, &Memory->MenuEntity->Acceleration);
    }
    
    
    u32 ButtonsAreaAmount = (Memory->MenuEntity->ButtonsAmount / 20) + 1;
    for(u32 i = 0; i < ButtonsAreaAmount; ++i)
    {
        if(Memory->MenuEntity->IsMoving)
        {
            Memory->MenuEntity->ButtonsArea[i].x += OffsetX;
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
        
        GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->BackTexture, &Memory->MenuEntity->Buttons[i].ButtonQuad);
        GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->Buttons[i].LevelNumberTexture,
                                 &Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad);
    }
    
}


