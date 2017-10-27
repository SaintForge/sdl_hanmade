// game.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
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
DEBUGRenderFigureShell(game_offscreen_buffer *Buffer, figure_unit *Entity, SDL_Color color)
{
     u8 r, g, b;
     SDL_GetRenderDrawColor(Buffer->Renderer, &r, &g, &b, 0);
     SDL_SetRenderDrawColor(Buffer->Renderer, color.r, color.g, color.b, 255);

     game_rect Rect = {};

     for (u32 i = 0; i < 4; ++i)
     {
          Rect.w = 4;
          Rect.h = 4;
          Rect.x = Entity->Shell[i].x - (Rect.w / 2);
          Rect.y = Entity->Shell[i].y - (Rect.h / 2);
          
          SDL_RenderDrawRect(Buffer->Renderer, &Rect);
     }

     SDL_SetRenderDrawColor(Buffer->Renderer, 255, 255, 255, 255);
     Rect.x = Entity->Center.x - (Rect.w / 2);
     Rect.y = Entity->Center.y - (Rect.h / 2);
     SDL_RenderDrawRect(Buffer->Renderer, &Rect);
     
     SDL_SetRenderDrawColor(Buffer->Renderer, r, g, b, 255);
}

static void
GameRenderBitmapToBuffer(game_offscreen_buffer *Buffer, game_texture *&Texture, game_rect *Quad)
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

static figure_unit*
GetFigureUnitAt(figure_entity *Group, u32 Index)
{
     u32 Size = Group->FigureAmount;
     
     figure_unit *Figure = Group->HeadFigure;
     for (u32 i = 0; i < Group->FigureAmount; ++i)
     {
          if(Figure->Index == Index)
          {
               return Figure;
          }

          Figure = Figure->Next;
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
FigureUnitSwapAtEnd(figure_unit *&Head, u32 FigureIndex)
{
     figure_unit *TargetNode  = NULL;
     figure_unit *PrevNode    = NULL;
     figure_unit *CurrentNode = Head;

     u32 Index = 0;
     
     while(CurrentNode)
     {
          Index = CurrentNode->Index;

          if(Index == FigureIndex)
          {
               TargetNode = CurrentNode;

               if(PrevNode)
               {
                    if(CurrentNode->Next)
                         PrevNode->Next = CurrentNode->Next;
                    else
                         PrevNode->Next = CurrentNode;
               }
               else
               {
                    Head = CurrentNode->Next;
               }
          }

          PrevNode = CurrentNode;
          CurrentNode = CurrentNode->Next;
     }
     
     PrevNode->Next = TargetNode;
     TargetNode->Next = NULL;
}

static void
FigureUnitResizeBy(figure_unit *Entity, r32 ScaleFactor)
{
     game_rect *Rectangle = &Entity->AreaQuad;
     game_point OldCenter = {};
     game_point NewCenter = {};
     
     s32 OffsetX = 0;
     s32 OffsetY = 0;
     s32 OldX = Entity->AreaQuad.x;
     s32 OldY = Entity->AreaQuad.y;

     OldCenter.x = Entity->AreaQuad.x + (Entity->AreaQuad.w / 2);
     OldCenter.y = Entity->AreaQuad.y + (Entity->AreaQuad.h / 2);
  
     Rectangle->w = roundf(Rectangle->w * ScaleFactor);
     Rectangle->h = roundf(Rectangle->h * ScaleFactor);

     NewCenter.x = Entity->AreaQuad.x + (Entity->AreaQuad.w / 2);
     NewCenter.y = Entity->AreaQuad.y + (Entity->AreaQuad.h / 2);
     
     Rectangle->x += (OldCenter.x - NewCenter.x);
     Rectangle->y += (OldCenter.y - NewCenter.y);

     OffsetX = roundf((Entity->Center.x - OldX) * ScaleFactor);
     OffsetY = roundf((Entity->Center.y - OldY) * ScaleFactor);
     Entity->Center.x = OldX + OffsetX;
     Entity->Center.y = OldY + OffsetY;
     Entity->Center.x += (OldCenter.x - NewCenter.x);
     Entity->Center.y += (OldCenter.y - NewCenter.y);
     
     for (u32 i = 0; i < 4; ++i)
     {
          OffsetX = roundf((Entity->Shell[i].x - OldX) * ScaleFactor);
          OffsetY = roundf((Entity->Shell[i].y - OldY) * ScaleFactor);
          Entity->Shell[i].x = OldX + OffsetX;
          Entity->Shell[i].y = OldY + OffsetY;
          Entity->Shell[i].x += (OldCenter.x - NewCenter.x);
          Entity->Shell[i].y += (OldCenter.y - NewCenter.y);
     }

}

static void
FigureUnitRotateShellBy(figure_unit *Entity, float Angle)
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
CreateNewFigureUnit(char* AssetName, game_offscreen_buffer *Buffer,
                    figure_unit *&Entity, u32 EntityIndex, u32 X, u32 Y, u32 BlockSize,
                    figure_form Form, figure_type Type, game_memory *Memory)
{
     figure_unit *Figure = NULL;
     Figure = (figure_unit*)malloc(sizeof(figure_unit));
     Figure->Next = NULL;

     int Index           = 0;
     int RowAmount       = 0;
     int ColumnAmount    = 0;
     float CenterOffset  = 0.5f;
     vector<vector<int>> matrix(2);
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

     Figure->Index  = EntityIndex;
     Figure->IsStick = false;
     Figure->IsEnlarged = false;
     Figure->Angle  = 0.0f;
     Figure->DefaultAnlge = 0.0f;
     Figure->Form = Form;
     Figure->Type = Type;
     Figure->Texture = GetTexture(Memory, AssetName, Buffer->Renderer);

     // NOTE: insert at the beginning
     Figure->Next = Entity;
     Entity = Figure;

     // NOTE: insert at the end
     // figure_unit *TmpFigure = Entity;
     // if(TmpFigure != NULL)
     // {
     //      while(TmpFigure->Next != NULL)
     //      {
     //           TmpFigure = TmpFigure->Next;
     //      }

     //      TmpFigure->Next = Figure;
     // }
     // else
     // {
     //      Entity = Figure;
     // }
}
static void
FigureUnitMove(figure_unit *Entity, s32 XShift, s32 YShift)
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
FigureUnitMoveTo(figure_unit *Entity, s32 NewPointX, s32 NewPointY)
{
     s32 XShift = NewPointX - Entity->Center.x;
     s32 YShift = NewPointY - Entity->Center.y;
     FigureUnitMove(Entity, XShift, YShift);
}



static void
DestroyFigureEntity(figure_unit *Entity)
{
     if(Entity)
     {
          FreeTexture(Entity->Texture);
          free(Entity);
     }
}

static void
FigureUnitRenderBitmap(game_offscreen_buffer *Buffer, figure_unit *Entity)
{
     game_point Center;
     Center.x = Entity->Center.x - Entity->AreaQuad.x;
     Center.y = Entity->Center.y - Entity->AreaQuad.y;

     SDL_RenderCopyEx(Buffer->Renderer, Entity->Texture,
                      0, &Entity->AreaQuad, Entity->Angle, &Center, SDL_FLIP_NONE);
}

static void
FigureEntityUpdateAndRender(game_offscreen_buffer *Buffer, figure_entity *Group,
                            r32 TimeElapsed)
{
     Assert(Group);
     u32 Size = Group->FigureAmount;

     if(Group->IsRotating)
     {
          r32 RotationVel = 630.0f;
          r32 AngleDt = TimeElapsed * RotationVel;

          if(Group->RotationSum < 90.0f && !(Group->RotationSum + AngleDt >= 90.0f))
          {
               Group->GrabbedFigure->Angle += AngleDt;
               Group->RotationSum += AngleDt;
          }
          else
          {
               Group->GrabbedFigure->Angle += 90.0f - Group->RotationSum;
               Group->RotationSum = 0;
               Group->IsRotating = false;
          }
     }

     figure_unit *Figure = Group->HeadFigure;
     while(Figure != NULL)
     {
          FigureUnitRenderBitmap(Buffer, Figure);
          
          DEBUGRenderQuad(Buffer, &Figure->AreaQuad, {255, 0, 0});
          DEBUGRenderFigureShell(Buffer, Figure, {255, 0, 0});
          
          Figure = Figure->Next;
     }
}

static void
FigureEntityUpdateEvent(game_input *Input, figure_entity *Group)
{
     u32 Size = Group->FigureAmount;
     s32 MouseX = Input->MouseX;
     s32 MouseY = Input->MouseY;
     s32 Offset = Group->BlockSize >> 1;

     if(Input->WasPressed)
     {
          if(Input->LeftClick.IsDown)
          {
               if(!Group->IsGrabbed)
               {
                    figure_unit *Figure = Group->HeadFigure;
                    for (u32 i = 0; i < Size; ++i)
                    {
                         if(IsPointInsideRect(MouseX, MouseY, &Figure->AreaQuad))
                         {
                              // printf("Before resize: \n");
                              // printf("Figure->AreaQuad.w = %d\n", Figure->AreaQuad.w);
                              // printf("Figure->AreaQuad.h = %d\n", Figure->AreaQuad.h);
                              Group->IsGrabbed     = true;
                              Group->GrabbedFigure = Figure;
                              Figure->IsEnlarged   = true;
                              FigureUnitResizeBy(Group->GrabbedFigure, 1.5f);
                              FigureUnitSwapAtEnd(Group->HeadFigure, Figure->Index);
                              // printf("After resize: \n");
                              // printf("Figure->AreaQuad.w = %d\n", Figure->AreaQuad.w);
                              // printf("Figure->AreaQuad.h = %d\n", Figure->AreaQuad.h);
                              SDL_ShowCursor(SDL_DISABLE);
                              break;   
                         }
                         
                         Figure = Figure->Next;
                    }
               }
               else
               {
                    if(!Group->IsRotating)
                    {
                         Group->IsGrabbed = false;
                         SDL_ShowCursor(SDL_ENABLE);
                         FigureUnitResizeBy(Group->GrabbedFigure, 0.667f);
                    }
               }
          }
          if(Input->RightClick.IsDown)
          {
               if(Group->IsGrabbed)
               {
                    Group->IsRotating = true;
                    FigureUnitRotateShellBy(Group->GrabbedFigure, 90);
               }
          } 
     }
     if(Input->MouseMotion)
     {
          if(Group->IsGrabbed)
          {
               s32 x = Input->MouseRelX;
               s32 y = Input->MouseRelY;
               FigureUnitMove(Group->GrabbedFigure, x, y);
          }           
     }
}

static void
PrintArray1D(vector<u32> &Array)
{
     for (u32 i = 0; i < Array.size(); ++i)
     {
          printf("%d ", Array[i]);
     }

     printf("\n");
}

static void
FigureEntityAlignHorizontally(figure_entity* Entity)
{
     u32 RowSize1 = 0;
     u32 RowSize2 = 0;
     u32 FigureIntervalX = Entity->BlockSize / 4;
     u32 FigureIntervalY = Entity->BlockSize / 6;
}

static game_rect
FigureUnitGetArea(figure_unit *Unit)
{
     game_rect Area = {Unit->Shell[0].x, Unit->Shell[0].y, -500, -500};
     u32 OffsetX = 0;
     u32 OffsetY = 0;
     bool ZeroArea = false;

     for (u32 i = 0; i < 4; ++i)
     {
          if(Area.x >= Unit->Shell[i].x)
          {
               Area.x = Unit->Shell[i].x;
          }
          if(Area.y >= Unit->Shell[i].y)
          {
               Area.y = Unit->Shell[i].y;
          }
          if(Area.w <= Unit->Shell[i].x)
          {
               Area.w = Unit->Shell[i].x;
          }
          if(Area.h <= Unit->Shell[i].y)
          {
               Area.h = Unit->Shell[i].y;
          }
     }

     Area.w -= Area.x;
     Area.h -= Area.y;
     
     if(Area.w == 0 || Area.h == 0)
     {
          ZeroArea = true;
     }

     if(Area.w >= Unit->AreaQuad.h)
     {
          Area.h = Unit->AreaQuad.h;
          Area.w = Unit->AreaQuad.w;
          OffsetX = (Area.y + Area.h) - (Area.y + Area.h / 2);
     }
     else
     {
          Area.h = Unit->AreaQuad.w;
          Area.w = Unit->AreaQuad.h;
          OffsetX = (Area.x + Area.w) - (Area.x + Area.w / 2);
     }

     if(!ZeroArea)
     {
          OffsetX /= 2;
     }

     Area.x -= OffsetX;
     Area.y -= OffsetX;

     return(Area);
}

static void
GridEntityUpdateAndRender(game_offscreen_buffer *Buffer, grid_entity *Entity)
{
     game_rect Area;
     Area.w = Entity->BlockSize;
     Area.h = Entity->BlockSize;

     for (u32 i = 0; i < Entity->RowAmount; ++i)
     {
          Area.y = Entity->GridArea.y + (i * Entity->BlockSize);
          for (u32 j = 0; j < Entity->ColumnAmount; ++j)
          {
               Area.x = Entity->GridArea.x + (j * Entity->BlockSize);
               if(Entity->UnitField[i][j] == 0)
               {
                    GameRenderBitmapToBuffer(Buffer, Entity->NormalSquareTexture, &Area);
               }
          }
     }
}

static bool
GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
     static u32 BlockSize   = 40;
     static r32 TimeElapsed = 0.0f;
     TimeElapsed = (SDL_GetTicks() - TimeElapsed) / 1000.0f;
     
     bool ShouldQuit = false;

     grid_entity   *&GridEntity   = Memory->State.GridEntity;
     figure_entity *&FigureEntity = Memory->State.FigureEntity;

     if(!Memory->IsInitialized)
     {
          FigureEntity = (figure_entity *)malloc(sizeof(figure_entity));
          Assert(FigureEntity);

          FigureEntity->FigureAmount  = 7;
          FigureEntity->BlockSize     = BlockSize;
          FigureEntity->IsGrabbed     = false;
          FigureEntity->IsRotating    = false;
          FigureEntity->RotationSum   = 0;
          FigureEntity->HeadFigure    = 0;
          FigureEntity->GrabbedFigure = 0;
          FigureEntity->FigureArea.w  = Buffer->Width;
          FigureEntity->FigureArea.h  = BlockSize * 8;
          FigureEntity->FigureArea.y  = Buffer->Height - (FigureEntity->FigureArea.h);
          FigureEntity->FigureArea.x  = 0;

          CreateNewFigureUnit("i_d.png", Buffer, FigureEntity->HeadFigure, 0, 0,   0,   BlockSize, I_figure, classic, Memory);
          CreateNewFigureUnit("o_d.png", Buffer, FigureEntity->HeadFigure, 1, 100, 100, BlockSize, O_figure, classic, Memory);
          CreateNewFigureUnit("l_d.png", Buffer, FigureEntity->HeadFigure, 2, 200, 200, BlockSize, L_figure, classic, Memory);
          CreateNewFigureUnit("j_d.png", Buffer, FigureEntity->HeadFigure, 3, 0,   0,   BlockSize, J_figure, classic, Memory);
          CreateNewFigureUnit("s_d.png", Buffer, FigureEntity->HeadFigure, 4, 100, 100, BlockSize, S_figure, classic, Memory);
          CreateNewFigureUnit("z_d.png", Buffer, FigureEntity->HeadFigure, 5, 200, 200, BlockSize, Z_figure, classic, Memory);
          CreateNewFigureUnit("t_d.png", Buffer, FigureEntity->HeadFigure, 6, 0,   0,   BlockSize, T_figure, classic, Memory);

          GridEntity  = (grid_entity *) malloc(sizeof(grid_entity));
          Assert(GridEntity);

          GridEntity->RowAmount    = 5;
          GridEntity->ColumnAmount = 5;
          GridEntity->BlockSize    = BlockSize;
          GridEntity->BlockIsGrabbed = false;
          GridEntity->BeginAnimationStart = true;
          GridEntity->GridArea.w = GridEntity->RowAmount * BlockSize;
          GridEntity->GridArea.h = GridEntity->ColumnAmount * BlockSize;
          GridEntity->GridArea.x = (Buffer->Width / 2) - (GridEntity->GridArea.w / 2);
          GridEntity->GridArea.y = (Buffer->Height / 2) - (GridEntity->GridArea.h / 2);

          GridEntity->UnitField = (u8**)malloc(GridEntity->RowAmount * sizeof(u8*));
          Assert(GridEntity->UnitField);
          for (u32 i = 0; i < GridEntity->RowAmount; ++i)
          {
               GridEntity->UnitField[i] = (u8*)malloc(sizeof(u8) * GridEntity->ColumnAmount);
               Assert(GridEntity->UnitField[i]);
               for (u32 j = 0; j < GridEntity->ColumnAmount; ++j)
               {
                    GridEntity->UnitField[i][j] = 0;
               }
          }

          GridEntity->NormalSquareTexture     = GetTexture(Memory, "grid_cell.png", Buffer->Renderer);
          GridEntity->VerticalSquareTexture   = GetTexture(Memory, "grid_cell1.png", Buffer->Renderer);
          GridEntity->HorizontlaSquareTexture = GetTexture(Memory, "grid_cell2.png", Buffer->Renderer);

          Memory->IsInitialized = true;
          printf("memory init!\n");
     }

     FigureEntityUpdateEvent(Input, FigureEntity);
     
     if(Input->WasPressed)
     {
          if(Input->Escape.IsDown)
          {
               ShouldQuit = true;
          }
     }

     GridEntityUpdateAndRender(Buffer, GridEntity);
     FigureEntityUpdateAndRender(Buffer, FigureEntity, TimeElapsed);

     if(FigureEntity->GrabbedFigure)
     {
          game_rect AreaQuad = FigureUnitGetArea(FigureEntity->GrabbedFigure);
          DEBUGRenderQuad(Buffer, &AreaQuad, {255, 255, 255});
     }

     TimeElapsed = SDL_GetTicks();
     return(ShouldQuit);
}
