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
            Vector2Mult(&Memory->MenuEntity->Velocity, 0);
            Memory->MenuEntity->AccelerationSum = 0;
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
            
            s32 LeftBoundary  = 0;
            s32 RightBoundary = 0;
            u32 ButtonsAreaAmount = (Memory->MenuEntity->ButtonsAmount / 20) + 1;
            if(ButtonsAreaAmount > 0)
            {
                LeftBoundary  = Memory->MenuEntity->ButtonsArea[0].x;
                RightBoundary = Memory->MenuEntity->ButtonsArea[ButtonsAreaAmount - 1].x;
            }
            
            printf("TargetPosition before = %d\n", Memory->MenuEntity->TargetPosition);
            if(LeftBoundary + Memory->MenuEntity->TargetPosition >= Buffer->Width)
            {
                printf("Left edge!\n");
                //Memory->MenuEntity->TargetPosition = Memory->MenuEntity->OldMouseX - Memory->MenuEntity->NewMouseX;
                //Memory->MenuEntity->TargetPosition = -(Memory->MenuEntity->TargetPosition + (LeftBoundary - Buffer->Width));
                Memory->MenuEntity->TargetPosition = -Memory->MenuEntity->TargetPosition;
            }
            else if(RightBoundary + Memory->MenuEntity->TargetPosition <= 0)
            {
                printf("right edge!\n");
                Memory->MenuEntity->TargetPosition = Memory->MenuEntity->TargetPosition - RightBoundary;
            }
            else
            {
                Memory->MenuEntity->TargetPosition = Memory->MenuEntity->NewMouseX - Memory->MenuEntity->OldMouseX;
            }
            
            printf("LeftBoundary = %d\n", LeftBoundary);
            printf("RightBoundary = %d\n", RightBoundary);
            printf("NewMouseX = %d\n", Memory->MenuEntity->NewMouseX);
            printf("OldMouseX = %d\n", Memory->MenuEntity->OldMouseX);
            printf("TargetPosition = %d\n", Memory->MenuEntity->TargetPosition);
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
        vector2 Vector = { Memory->MenuEntity->TargetPosition, 0 };
        
        r32 Ratio = 0;
        r32 Distance = Vector2Mag(&Vector);
        r32 ApproachRadius = 100.0f;
        Vector2Norm(&Vector);
        if(Distance < ApproachRadius)
        {
            Ratio = Distance / ApproachRadius;
            if(Ratio > 0.01)
            {
                Vector.x *= Distance / ApproachRadius * Memory->MenuEntity->MaxVelocity;
                Vector.y *= Distance / ApproachRadius * Memory->MenuEntity->MaxVelocity;
            }
            else
            {
                printf("TargetPosition after = %d\n", Memory->MenuEntity->TargetPosition);
                printf("AccelerationSum = %d\n", Memory->MenuEntity->AccelerationSum);
                
                if(Memory->MenuEntity->TargetPosition == 0)
                {
                    Memory->MenuEntity->IsAnimating = false;
                    Vector.x = 0;
                }
                else
                {
                    Memory->MenuEntity->Velocity.x = Memory->MenuEntity->TargetPosition;
                }
            }
        }
        else
        {
            Vector.x *= Memory->MenuEntity->MaxVelocity;
            Vector.y *= Memory->MenuEntity->MaxVelocity;
        }
        
        Memory->MenuEntity->Acceleration.x = Vector.x - Memory->MenuEntity->Velocity.x;
        
        Memory->MenuEntity->TargetPosition -= Memory->MenuEntity->Velocity.x;
        Memory->MenuEntity->AccelerationSum += Memory->MenuEntity->Velocity.x;
        
        Vector2Add(&Memory->MenuEntity->Velocity, &Memory->MenuEntity->Acceleration);
        printf("Velocity.x = %f\n", Memory->MenuEntity->Velocity.x);
        printf("TargetPosition = %d\n", Memory->MenuEntity->TargetPosition);
        
        
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


