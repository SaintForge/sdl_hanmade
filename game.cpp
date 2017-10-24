// game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пн окт 23 17:37:36 2017 (+0300)
//           By: Sierra
//


#include "game.h"
#include "asset_game.h"

static void
DEBUGRenderQuad(game_offscreen_buffer *Buffer, game_rect *AreaQuad, SDL_Color color)
{
     u8 r, g, b;
     SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, 0);
     
     SDL_SetRenderDrawColor(Buffer->Renderer, color.r, color.g, color.b, 255);
     SDL_RenderDrawRect(Buffer->Renderer, AreaQuad);
     SDL_SetRenderDrawColor(Buffer->Renderer, r, g, b, 255);
}

static void
GameRenderBitmapToBuffer(game_offscreen_buffer *Buffer, game_texture *Texture, game_rect *Quad)
{
     SDL_RenderCopy(Buffer->Renderer, Texture, 0, Quad);
}

#if 0
static void
GameCopyImageToBuffer(game_bitmap* GameBitmap, u32 X, u32 Y,
                      game_offscreen_buffer *Buffer)
{
     u8 BytesPerPixel = GameBitmap->BytesPerPixel;

     u8 *RowBuffer = (u8*)Buffer->Memory + (Y * GameBitmap->Pitch);
     u8 *RowTarget = (u8*)GameBitmap->Pixels;
		 
     for (u32 y = 0; y < GameBitmap->Height; ++y)
     {
          u32 *PixelBuffer = (u32*)RowBuffer + X;
          u32 *PixelTarget = (u32*)RowTarget;
					
          for (u32 x = 0; x < GameBitmap->Width; ++x)
          {
               u8 Alpha = (*PixelTarget >> 24) & 0xFF;
               u8 Blue  = (*PixelTarget >> 16) & 0xFF;
               u8 Green = (*PixelTarget >> 8)  & 0xFF;
               u8 Red   = (*PixelTarget & 0xFF);
							 
               *PixelBuffer = ((Red << 24) | (Green << 16) | (Blue << 8) | (Alpha));

               *PixelBuffer++;
               *PixelTarget++;
          }
					
          RowBuffer += Buffer->Pitch;
          RowTarget += GameBitmap->Width * BytesPerPixel;
     }
     printf("GameCopyImageToBuffer\n");
}
#endif

static figure_entity*
CreateNewFigureEntity(u32 X, u32 Y, u32 BlockSize,
                      figure_entity_form Form, figure_entity_type Type,
                      game_memory *Memory, char* AssetName, game_offscreen_buffer *Buffer)
{
     figure_entity *Figure = (figure_entity*)malloc(sizeof(figure_entity));
     Assert(Figure);

     int Index           = 0;
     int RowAmount       = 0;
     int ColumnAmount    = 0;
     float CenterOffset  = 0.5f;
		 
     std::vector<std::vector<int>> matrix(2);
     for (int i = 0; i < 2; i++)
     {
          matrix[i].resize(4);
     }

     switch(Form)
     {
          case I_figure:
          {
               matrix = { {1, 1, 1, 1}, {0, 0, 0, 0} };
               RowAmount = 4;
               ColumnAmount = 1;
          } break;
	  
          case O_figure:
          {
               matrix = { { 1, 1 }, { 1, 1 } };
               RowAmount = 2;
               ColumnAmount = 2;
          }break;
	  
          case Z_figure:
          {
               matrix = { {1, 1, 0}, {0, 1, 1} };
               RowAmount = 3;
               ColumnAmount = 2;
          }break;
	  
          case S_figure:
          {
               matrix = { {0, 1, 1}, {1, 1, 0} };
               RowAmount = 3;
               ColumnAmount = 2;
          }break;
	  
          case T_figure:
          {
               matrix = { {0, 1, 0}, {1, 1, 1} };
               CenterOffset = 0.75f;
               RowAmount = 3;
               ColumnAmount = 2;
          }break;
	  
          case L_figure:
          {
               matrix = { {0, 0, 1},{1, 1, 1} };
               CenterOffset = 0.75f;
               RowAmount = 3;
               ColumnAmount = 2;
          }break;
	  
          case J_figure:
          {
               matrix = { {1, 0, 0}, {1, 1, 1} };
               CenterOffset = 0.75f;
               RowAmount = 3;
               ColumnAmount = 2;
          }break;
     }

     Figure->AreaQuad.x = X;
     Figure->AreaQuad.y = Y;
     Figure->AreaQuad.w = RowAmount*BlockSize;
     Figure->AreaQuad.h = ColumnAmount*BlockSize;
     Figure->Center.x = Figure->AreaQuad.x + (Figure->AreaQuad.w / 2);
     Figure->Center.y = Figure->AreaQuad.y + (((float)Figure->AreaQuad.h) * CenterOffset);
     Figure->DefaultCenter = Figure->Center;
     
     for (u32 i = 0; i < 2; i++)
     {
          for (u32 j = 0; j < 4; j++)
          {
               if(matrix[i][j] == 1)
               {
                    Figure->Shell[Index].x = Figure->AreaQuad.x + (j * BlockSize) + (BlockSize / 2);
                    Figure->Shell[Index].y = Figure->AreaQuad.y + (i * BlockSize) + (BlockSize / 2);
                    Figure->DefaultShell[Index] = Figure->Shell[Index];
                    Index++;
               }
          }
     }

     Figure->IsIdle = true;
     Figure->IsIdle = false;
     Figure->Angle  = 0.0f;
     Figure->DefaultAnlge = 0.0f;
     Figure->Form = Form;
     Figure->Type = Type;
     Figure->Texture = GetTexture(Memory, AssetName, Buffer->Renderer);

     return (Figure);
}

static void
DestroyFigureEntity(figure_entity *Entity)
{
     if(Entity)
     {
          FreeTexture(Entity->Texture);
          free(Entity);
     }
}


inline static bool
IsPointInsideRect(s32 X, s32 Y, game_rect *Quad)
{
     if(!Quad) return false;

     if(X < Quad->x)                return false;
     else if(Y < Quad->y)           return false;
     else if(X > Quad->x + Quad->w) return false;
     else if(Y > Quad->y + Quad->h) return false;
     else                           return true;
}

static void
FigureEntityMove(figure_entity *Entity, s32 XShift, s32 YShift)
{
     int XOffset = Entity->AreaQuad.x - Entity->Center.x;
     int YOffset = Entity->AreaQuad.y - Entity->Center.y;

     Entity->Center.x += XShift;
     Entity->Center.y += YShift;

     Entity->AreaQuad.x = Entity->Center.x + XOffset;
     Entity->AreaQuad.y = Entity->Center.y + YOffset;

     for (u32 i = 0; i < 4; ++i)
     {
          Entity->Shell[i].x += XShift;
          Entity->Shell[i].y += YShift;
     }
}

static void
FigureEntityMoveTo(figure_entity *Entity, s32 NewPointX, s32 NewPointY)
{
     s32 XShift = NewPointX - Entity->Center.x;
     s32 YShift = NewPointY - Entity->Center.y;
     FigureEntityMove(Entity, XShift, YShift);
}

static void
FigureEntityScaleBlock(figure_entity *Entity, u32 BlockSize, s32 ScaleRatio)
{
     if(!Entity) return;

     u32 RowBlock  = 0;
     u32 ColBlock  = 0;
     r32 OldWidth  = 0;
     r32 OldHeight = 0;
     r32 WRatio    = 0;
     r32 HRatio    = 0;

     OldWidth  = Entity->AreaQuad.w;
     OldHeight = Entity->AreaQuad.h;

     RowBlock = (Entity->AreaQuad.w) / (BlockSize);
     ColBlock = (Entity->AreaQuad.h) / (BlockSize);

     BlockSize += ScaleRatio;
     Entity->AreaQuad.w = RowBlock * BlockSize;
     Entity->AreaQuad.h = ColBlock * BlockSize;

     WRatio = (Entity->Center.x - Entity->AreaQuad.x) / OldWidth;
     HRatio = (Entity->Center.y - Entity->AreaQuad.y) / OldHeight;
		 
     Entity->Center.x = roundf((WRatio * Entity->AreaQuad.w) + Entity->AreaQuad.x);
     Entity->Center.y = roundf((HRatio * Entity->AreaQuad.h) + Entity->AreaQuad.y);
     for (u32 i = 0; i < 4; ++i)
     {
          WRatio = (Entity->Shell[i].x - Entity->AreaQuad.x) / OldWidth;
          HRatio = (Entity->Shell[i].y - Entity->AreaQuad.y) / OldHeight;
          Entity->Shell[i].x = roundf((WRatio * Entity->AreaQuad.w) + Entity->AreaQuad.x);
          Entity->Shell[i].y = roundf((HRatio * Entity->AreaQuad.h) + Entity->AreaQuad.y);
     }
		 
}

static void
FigureEntityRotateBy(figure_entity *Entity, float Angle)
{
     if((s32)Entity->Angle == 0)
     {
          Entity->Angle = 0;
     }

     for (u32 i = 0; i < 4; ++i)
     {
          float Radians = Angle * (M_PI/180.0f);
          float Cos = cos(Radians);
          float Sin = sin(Radians);

          float X = Entity->Center.x + (Entity->Shell[i].x - Entity->Center.x) * Cos
               - (Entity->Shell[i].y - Entity->Center.y) * Sin;
          float Y = Entity->Center.y + (Entity->Shell[i].x - Entity->Center.x) * Sin
               + (Entity->Shell[i].y - Entity->Center.y) * Cos;

          Entity->Shell[i].x = roundf(X);
          Entity->Shell[i].y = roundf(Y);
     }
}

static void
OrderLowPriority(vector<u32> &Order, u32 Index)
{
     u32 Size = Order.size();

     if(Index < 0 || Index > Size - 1)
     {
          return;
     }

     if(Order[0] == Index)
     {
          return;
     }

     u32 StartIndex = 0;
     for (u32 i = 0; i < Size; ++i)
     {
          if(Order[i] == Index)
          {
               StartIndex = i;
               break;
          }
     }

     for (u32 i = StartIndex; i > 0; i--)
     {
          Order[i] = Order[i - 1];
     }

     Order[0] = Index;

}

static void
OrderHighPriority(vector<u32> &Order, u32 Index)
{
     u32 Value = 0;
     u32 UpIndex = 0;

     u32 Size = Order.size();
     for (u32 i = 0; i < Size; ++i)
     {
          if(Order[i] == Index)
          {
               UpIndex = i;
               Value = Order[i];
          }
     }

     for (u32 i = UpIndex; i < Size - 1; ++i)
     {
          Order[i] = Order[i+1];
     }

     Order[Size-1] = Value;
}

static void
FigureEntityGroupUpdate(game_input *Input, figure_group *Group)
{
     u32 Size = Group->Figure.size();
               
     s32 MouseX = Input->MouseX;
     s32 MouseY = Input->MouseY;
     s32 Offset = Group->BlockSize >> 1;
     
     if(Input->LeftClick.IsDown)
     {
          if(!Group->IsGrabbed)
          {
               for (u32 i = 0;
                    i < Size; ++i)
               {
                    if(IsPointInsideRect(MouseX, MouseY, &Group->Figure[i]->AreaQuad))
                    {
                         Group->GrabIndex = i;
                         Group->IsGrabbed = true;

                         FigureEntityScaleBlock(Group->Figure[i], Group->BlockSize, Offset);
                         FigureEntityMove(Group->Figure[i], -Offset, -Offset);
                         OrderHighPriority(Group->Order, i);
                         
                         Group->OffsetX = MouseX - Group->Figure[i]->Center.x;
                         Group->OffsetY = MouseY - Group->Figure[i]->Center.y;
                         FigureEntityMove(Group->Figure[i], Group->OffsetX, Group->OffsetY);
                         SDL_ShowCursor(SDL_DISABLE);
                    }
               }
          }
          else
          {
               if(Group->GrabIndex != -1)
               {
                    FigureEntityScaleBlock(Group->Figure[Group->GrabIndex], Group->BlockSize+Offset, -Offset);
                    FigureEntityMove(Group->Figure[Group->GrabIndex], Offset, Offset);
                    Group->IsGrabbed = false;
                    SDL_ShowCursor(SDL_ENABLE);
               }
          }
     }
}

static void
FigureEntityRenderBitmap(game_offscreen_buffer *Buffer, figure_entity *Entity)
{
     game_point Center;
     Center.x = Entity->Center.x - Entity->AreaQuad.x;
     Center.y = Entity->Center.y - Entity->AreaQuad.y;

     SDL_RenderCopyEx(Buffer->Renderer, Entity->Texture,
                      0, &Entity->AreaQuad, Entity->Angle, &Center, SDL_FLIP_NONE);
}

// TODO: Should be std::vector


static void
PrintArray(vector<u32> &Array)
{
     for (u32 i = 0; i < Array.size(); ++i)
     {
          printf("%d ", Array[i]);
     }

     printf("\n");
}

static bool
GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
     bool ShouldQuit = false;
     u32 BlockSize = 40;
     static u32 Ticks = 0;
     
     figure_group *Group = NULL;
		 
     if(!Memory->IsInitialized)
     {
          Group = &Memory->State.Group;

          Group->IsGrabbed  = false;
          Group->IsRotating = false;

          Group->OffsetX = 0;
          Group->OffsetY = 0;
          Group->GrabIndex = 0;
          Group->TimeTicks = 0;
          Group->RotationSum = 0;
          Group->BlockSize = BlockSize;
          figure_entity *Figure = NULL;
          
          Figure = CreateNewFigureEntity(0, 0, BlockSize, I_figure, classic, Memory, "i_d.png", Buffer);
          Group->Figure.push_back(Figure);
          Figure = CreateNewFigureEntity(0, 0, BlockSize, O_figure, classic, Memory, "o_d.png", Buffer);
          Group->Figure.push_back(Figure);
          Figure = CreateNewFigureEntity(0, 0, BlockSize, L_figure, classic, Memory, "l_d.png", Buffer);
          Group->Figure.push_back(Figure);

          Group->Order.push_back(0);
          Group->Order.push_back(1);
          Group->Order.push_back(2);
          
          Memory->Music = GetMusic(Memory, "amb_ending_water.ogg");
          Mix_PlayMusic(Memory->Music, -1);

          Memory->IsInitialized = true;
          printf("memory init!\n");
     }

     Group = &Memory->State.Group;
     
     if(Input->WasPressed)
     {
          if(Input->Up.IsDown)
          {
               printf("Up is down!\n");
               DestroyFigureEntity(Group->Figure[0]);
               Group->Figure.erase(Group->Figure.begin());
          }
					
          if(Input->Escape.IsDown)
          {
               printf("Quiting!\n");
               ShouldQuit = true;
          }

          if(Input->LeftClick.IsDown)
          {
               if(!Group->IsGrabbed)
               {
                    for (u32 i = 0; i < 3; ++i)
                    {
                         if(IsPointInsideRect(Input->MouseX, Input->MouseY, &Group->Figure[i]->AreaQuad))
                         {
                              Group->GrabIndex = i;
                              Group->IsGrabbed = true;
                              
                              FigureEntityScaleBlock(Group->Figure[i], BlockSize, 20);
                              FigureEntityMove(Group->Figure[i], -20, -20);
                              OrderHighPriority(Group->Order, i);
                              
                              Group->OffsetX = Input->MouseX - Group->Figure[i]->Center.x;
                              Group->OffsetY = Input->MouseY - Group->Figure[i]->Center.y;
                              FigureEntityMove(Group->Figure[i], Group->OffsetX, Group->OffsetY);
                              SDL_ShowCursor(SDL_DISABLE);
                              break;
                         }		 
                    }
               }
               else
               {
                    if(Group->GrabIndex != -1)
                    {
                         FigureEntityScaleBlock(Group->Figure[Group->GrabIndex], BlockSize+20, -20);
                         FigureEntityMove(Group->Figure[Group->GrabIndex], 20, 20);
                         Group->IsGrabbed = false;
                         SDL_ShowCursor(SDL_ENABLE);
                    }
               }
          }
          if(Input->RightClick.IsDown)
          {
               printf("Right click \n");
               if(Group->IsGrabbed)
               {
                    FigureEntityRotateBy(Group->Figure[Group->GrabIndex], 90);
                    Group->IsRotating = true;
                    Ticks = SDL_GetTicks();
               }
               
          }

          Input->WasPressed = false;
     }

     r32 TimeDX = (SDL_GetTicks() - Ticks) / 1000.0f;

     if(Group->IsRotating)
     {
          r32 RotationVel = 630.0f;
          r32 AngleDt = TimeDX * RotationVel;

          if(Group->RotationSum < 90.0f && !(Group->RotationSum + AngleDt >= 90.0f))
          {
               Group->Figure[Group->GrabIndex]->Angle += AngleDt;
               Group->RotationSum += AngleDt;
               Ticks = SDL_GetTicks();
          }
          else
          {
               Group->Figure[Group->GrabIndex]->Angle += 90.0f - Group->RotationSum;
               Group->RotationSum = 0;
               Group->IsRotating = false;
          }

     }

     if(Group->IsGrabbed)
     {
          FigureEntityMoveTo(Group->Figure[Group->GrabIndex], Input->MouseX - Group->OffsetX, Input->MouseY - Group->OffsetY);
     }

     DEBUGRenderQuad(Buffer, &Group->Figure[0]->AreaQuad, {255, 0, 0});

     game_rect Rect = {};
     Rect.x = Group->Figure[1]->Center.x - 2;
     Rect.y = Group->Figure[1]->Center.y - 2;
     Rect.w = 4;
     Rect.h = 4;
     
     for (u32 i = 0; i < 3; ++i)
     {
          FigureEntityRenderBitmap(Buffer, Group->Figure[Group->Order[i]]);
     }

     
     DEBUGRenderQuad(Buffer, &Rect, {255, 0, 0});

     for (u32 i = 0; i < 4; ++i)
     {
          game_rect ShellRect = {};
          ShellRect.x = Group->Figure[1]->Shell[i].x - 2;
          ShellRect.y = Group->Figure[1]->Shell[i].y - 2;
          ShellRect.w = 4;
          ShellRect.h = 4;
          
          DEBUGRenderQuad(Buffer, &ShellRect, {255, 0, 0});
     }


     return(ShouldQuit);
}

