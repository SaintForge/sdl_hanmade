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
        }
        else if(Input->LeftClick.WasDown)
        {
            printf("left click was down!\n");
            Memory->MenuEntity->IsMoving  = false;
            Memory->MenuEntity->IsAnimating = true;
            Memory->MenuEntity->NewMouseX = Input->MouseX;
            
            printf("AccelerationSum = %d\n", Memory->MenuEntity->AccelerationSum);
        }
    }
    
    if(Input->MouseMotion && Memory->MenuEntity->IsMoving)
    {
        OffsetX += Input->MouseRelX;
    }
    
    game_rect ScreenQuad = {0, 0, Buffer->Width, Buffer->Height };
    DEBUGRenderQuadFill(Buffer, &ScreenQuad, {0, 0, 0}, 0);
    
    if(Memory->MenuEntity->IsAnimating)
    {
        vector2 Vector =
        {
            Memory->MenuEntity->NewMouseX - Memory->MenuEntity->OldMouseX, 0
        };
        
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
                Vector.x = 0;
                Vector.y = 0;
            }
        }
        else
        {
            Vector.x *= Memory->MenuEntity->MaxVelocity;
            Vector.y *= Memory->MenuEntity->MaxVelocity;
        }
        
        
        Memory->MenuEntity->Acceleration.x = Vector.x - Memory->MenuEntity->Velocity.x;
        Memory->MenuEntity->Acceleration.y = Vector.y - Memory->MenuEntity->Velocity.y;
        
        Vector2Add(&Memory->MenuEntity->Velocity, &Memory->MenuEntity->Acceleration);
        
        printf("Acceleration = %f\n", Memory->MenuEntity->Acceleration.x);
        printf("Velocity = %f\n", Memory->MenuEntity->Velocity.x);
        
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
            Memory->MenuEntity->ButtonsArea[i].x += Memory->MenuEntity->Velocity.x;
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
            Memory->MenuEntity->Buttons[i].ButtonQuad.x += Memory->MenuEntity->Velocity.x;
            Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad.x += Memory->MenuEntity->Velocity.x;
        }
        
        GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->BackTexture, &Memory->MenuEntity->Buttons[i].ButtonQuad);
        GameRenderBitmapToBuffer(Buffer, Memory->MenuEntity->Buttons[i].LevelNumberTexture,
                                 &Memory->MenuEntity->Buttons[i].LevelNumberTextureQuad);
    }
    
}


