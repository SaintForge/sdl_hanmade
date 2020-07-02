// math.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

#if !defined(GAME_MATH_H)

union v2
{
    struct 
    {
        r32 x, y;
    };
    struct
    {
        r32 w, h;
    };
    r32 E[2];
    
};

struct v3
{
    r32 x;
    r32 y;
    r32 z;
};

union v4
{
    struct
    {
        union
        {
            v3 xyz;
            struct
            {
                r32 x, y, z;
            };
        };
        
        r32 w;        
    };
    struct
    {
        union
        {
            v3 rgb;
            struct
            {
                r32 r, g, b;
            };
        };
        
        r32 a;        
    };
    r32 E[4];
};


struct rectange2
{
    v2 Min;
    v2 Max;
};


inline v2
V2(r32 x, r32 y)
{
    v2 Result;
    
    Result.x = x;
    Result.y = y;
    
    return(Result);
}

inline v2
operator*(r32 A, v2 B)
{
    v2 Result;
    
    Result.x = A*B.x;
    Result.y = A*B.y;
    
    return(Result);
}

inline v2
operator*(v2 B, r32 A)
{
    v2 Result = A*B;
    
    return(Result);
}

inline v2 &
operator*=(v2 &B, r32 A)
{
    B = A * B;
    
    return(B);
}

inline v2
operator-(v2 A)
{
    v2 Result;
    
    Result.x = -A.x;
    Result.y = -A.y;
    
    return(Result);
}

inline v2
operator+(v2 A, v2 B)
{
    v2 Result;
    
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    
    return(Result);
}

inline v2 &
operator+=(v2 &A, v2 B)
{
    A = A + B;
    
    return(A);
}

inline v2
operator-(v2 A, v2 B)
{
    v2 Result;
    
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    
    return(Result);
}


struct math_rect
{
    r32 Left;
    r32 Top;
    r32 Right;
    r32 Bottom;
};

struct math_point
{
    r32 x;
    r32 y;
};

struct vector2
{
    r32 x;
    r32 y;
};

struct rectangle2
{
    v2 Min;
    v2 Max;
};

enum anchor_presets
{
    TopLeft = 0,
    TopCenter,
    TopRight,
    
    MiddleLeft,
    MiddleCenter,
    MiddleRight,
    
    BottomLeft,
    BottonCenter,
    BottomRight,
    
    VertStretchLeft,
    VertStretchCenter,
    VertStretchRight,
    VertStretchObject,
    
    HorStretchTop,
    HorStretchMiddle,
    HorStretchBottom,
    HorStretchObject,
    
    StretchObject,
    StretchAll
};

static void 
Vector2Sub(vector2 *Vector, r32 x, r32 y)
{
    Vector->x -= x;
    Vector->y -= y;
}

static void 
Vector2Sub(vector2 *Vector1, vector2 *Vector2)
{
    Vector1->x += Vector2->x;
    Vector1->y += Vector2->y;
}

static void 
Vector2Mult(vector2 *Vector, r32 Value)
{
    Vector->x *= Value;
    Vector->y *= Value;
}

static void 
Vector2Div(vector2 *Vector, r32 Value)
{
    Vector->x /= Value;
    Vector->y /= Value;
}

static r32 
Vector2Mag(vector2 *Vector)
{
    return sqrt((Vector->x * Vector->x) + (Vector->y * Vector->y));
}

static void
Vector2Norm(vector2 *Vector)
{
    r32 Magnitude = Vector2Mag(Vector);
    if(Magnitude != 0) Vector2Div(Vector, Magnitude);
}

static void
Vector2Limit(vector2 *Vector, r32 MaxValue)
{
    r32 Magnitude = Vector2Mag(Vector);
    if(Magnitude > MaxValue)
    {
        Vector2Norm(Vector);
        Vector2Mult(Vector, MaxValue);
    }
}

inline void
Swap(r32 *A, r32 *B)
{
    r32 C = *A;
    *A = *B;
    *B = C;
}

inline static s32
Max2(s32 a, s32 b)
{
    return (a > b) ? a : b;
}

inline static s32
Min2(s32 a, s32 b)
{
    return (a < b) ? a : b;
}

inline static s32 
Min3(s32 a, s32 b, s32 c)
{
    return fmin(fmin(a,b),c);
}

inline static bool
IsPointInsideRect(s32 X, s32 Y, game_rect *Quad)
{
    if(!Quad) return false;
    
    if(X < Quad->x)                   return false;
    else if(Y < Quad->y)              return false;
    else if(X >= (Quad->x + Quad->w)) return false;
    else if(Y >= (Quad->y + Quad->h)) return false;
    else                              return true;
}

inline static bool
IsPointInsideRect(s32 X, s32 Y, math_rect Rectangle)
{
    if(X < Rectangle.Left)           return false;
    else if(Y < Rectangle.Top)       return false;
    else if(X >= (Rectangle.Right))  return false;
    else if(Y >= (Rectangle.Bottom)) return false;
    else                             return true;
}

static math_rect
ConvertGameRectToMathRect(game_rect Rectangle)
{
    math_rect Result = {};
    
    Result.Left   = Rectangle.x;
    Result.Top    = Rectangle.y;
    Result.Right  = Rectangle.x + Rectangle.w;
    Result.Bottom = Rectangle.y + Rectangle.h;
    
    return(Result);
}

static game_rect
ConvertMathRectToGameRect(math_rect Rectangle)
{
    game_rect Result = {};
    
    Result.x = roundf(Rectangle.Left);
    Result.y = roundf(Rectangle.Top);
    Result.w = roundf(Rectangle.Right  - Rectangle.Left);
    Result.h = roundf(Rectangle.Bottom - Rectangle.Top);
    
    return(Result);
}

static game_point
ConvertNormPointToPixel(game_point Point, math_rect RelQuad)
{
    game_point Result;
    
    r32 RelWidth  = RelQuad.Right - RelQuad.Left;
    r32 RelHeight = RelQuad.Bottom - RelQuad.Top;
    
    Result.x = RelQuad.Left + (Point.x * RelWidth);
    Result.y = RelQuad.Top +  RelHeight - (Point.y * RelHeight);
    
    return(Result);
}

static game_point
ConvertPixelPointToNormal(game_point Point, math_rect RelQuad)
{
    game_point Result;
    
    r32 Width  = RelQuad.Right - RelQuad.Left;
    r32 Heigth = RelQuad.Bottom - RelQuad.Top;
    
    Result.x = (Point.x - RelQuad.Left) / Width;
    Result.y = 1.0f - ((Point.y - RelQuad.Top) / Heigth);
    
    return(Result);
}

static math_rect
CreateMathRect(r32 Left, r32 Top, r32 Right, r32 Bottom,
               math_rect RelQuad)
{
    math_rect Result;
    
    if(Left > Right)
    {
        Swap(&Left, &Right);
    }
    
    if(Top < Bottom)
    {
        Swap(&Top, &Bottom);
    }
    
    r32 RelWidth  = RelQuad.Right  - RelQuad.Left;
    r32 RelHeight = RelQuad.Bottom - RelQuad.Top;
    
    Result.Left = RelQuad.Left + roundf(Left * RelWidth);
    Result.Top  = RelQuad.Top +  roundf(RelHeight - (Top * RelHeight));
    Result.Right = RelQuad.Left + roundf(Right * RelWidth);
    Result.Bottom = RelQuad.Top + roundf(RelHeight - (Bottom * RelHeight));
    
    return(Result);
}

static math_rect
NormalizeRectangle(math_rect Rectangle,
                   math_rect RelativeRectangle)
{
    math_rect Result = {};
    
    s32 RelWidth  = RelativeRectangle.Right - RelativeRectangle.Left;
    s32 RelHeight = RelativeRectangle.Bottom - RelativeRectangle.Top;
    
    Result.Left   = (Rectangle.Left - RelativeRectangle.Left) / RelWidth;
    Result.Top    = 1.0f - ((Rectangle.Top - RelativeRectangle.Top) / RelHeight);
    Result.Right  = (Rectangle.Right - RelativeRectangle.Left) / RelWidth;
    Result.Bottom = 1.0f - ((Rectangle.Bottom - RelativeRectangle.Top) / RelHeight);
    
    return(Result);
}


static math_rect
GetAnchorPreset(anchor_presets Preset, math_rect RectQuad,
                math_rect RelativeQuad)
{
    math_rect Result = {};
    
    math_point MinAnchor = {}, MaxAnchor = {};
    math_rect RectNorm = NormalizeRectangle(RectQuad, RelativeQuad);
    
    switch(Preset)
    {
        case TopLeft: 
        {
            MinAnchor = {0.0f, 1.0f};
            MaxAnchor = {0.0f, 1.0f};
        } break;
        case TopCenter:
        {
            MinAnchor = {0.5f, 1.0f};
            MaxAnchor = {0.5f, 1.0f};
        } break;
        case TopRight:
        {
            MinAnchor = {1.0f, 1.0f};
            MaxAnchor = {1.0f, 1.0f};
        } break;
        
        case MiddleLeft:
        { 
            MinAnchor = {0.0f, 0.5f};
            MaxAnchor = {0.0f, 0.5f};
        } break;
        case MiddleCenter:
        {
            MinAnchor = {0.5f, 0.5f};
            MaxAnchor = {0.5f, 0.5f};
        } break;
        case MiddleRight:
        { 
            MinAnchor = {1.0f, 0.5f};
            MaxAnchor = {1.0f, 0.5f};
        } break;
        
        case BottomLeft:
        {
            MinAnchor = {0.0f, 0.0f};
            MaxAnchor = {0.0f, 0.0f};
        } break;
        case BottonCenter:
        {
            MinAnchor = {0.5f, 0.0f};
            MaxAnchor = {0.5f, 0.0f};
        } break;
        case BottomRight:
        {
            MinAnchor = {1.0f, 0.0f};
            MaxAnchor = {1.0f, 0.0f};
        } break;
        
        case VertStretchLeft:
        {
            MinAnchor = {0.0f, 0.0f};
            MaxAnchor = {0.0f, 1.0f};
        } break;
        case VertStretchRight:
        { 
            MinAnchor = {1.0f, 0.0f};
            MaxAnchor = {1.0f, 1.0f};
        } break;
        case VertStretchCenter:
        { 
            MinAnchor = {0.5f, 0.0f};
            MaxAnchor = {0.5f, 1.0f};
        } break;
        
        case HorStretchTop:
        {
            MinAnchor = {0.0f, 1.0f};
            MaxAnchor = {1.0f, 1.0f};
        } break;
        case HorStretchMiddle:
        { 
            MinAnchor = {0.0f, 0.5f};
            MaxAnchor = {1.0f, 0.5f};
        } break;
        case HorStretchBottom:
        {
            MinAnchor = {0.0f, 0.0f};
            MaxAnchor = {1.0f, 0.0f};
        } break;
        case StretchAll:
        {
            MinAnchor = {0.0f, 0.0f};
            MaxAnchor = {1.0f, 1.0f};
        } break;
        
        case HorStretchObject:
        {
            MinAnchor = {0.0f, RectNorm.Bottom};
            MaxAnchor = {1.0f, RectNorm.Top};
        } break;
        
        case VertStretchObject:
        {
            MinAnchor = {RectNorm.Left, 0.0f};
            MaxAnchor = {RectNorm.Right, 1.0f};
        } break;
        
        case StretchObject:
        {
            MinAnchor = {RectNorm.Left, RectNorm.Bottom};
            MaxAnchor = {RectNorm.Right, RectNorm.Top};
        } break;
    }
    
    Result = CreateMathRect(MinAnchor.x, MaxAnchor.y, MaxAnchor.x, MinAnchor.y, RelativeQuad);
    
    return(Result);
}


static r32
CalculateRatio(r32 ObjectWidth, r32 ObjectHeight)
{
    r32 Result = 0.0f;
    
    Result = ObjectHeight / ObjectWidth;
    
    return(Result);
}

static math_point 
CalculateNewPosition(math_point RefPosition, 
                     math_point RefScreenSize,
                     math_point CurrentScreenSize)
{
    math_point Result = {};
    
    Result.x = (RefPosition.x / RefScreenSize.x) * CurrentScreenSize.x;
    Result.y = (RefPosition.y / RefScreenSize.y) * CurrentScreenSize.y;
    
    return(Result);
}

static math_point
CalculateNewSize(math_point RefSize, math_point RefScreenSize,
                 math_point CurrentScreenSize)
{
    math_point Result = {};
    
    r32 OriginalRatio = CalculateRatio(RefSize.x, RefSize.y);
    r32 WidthRatio    = RefSize.x * (100.0f / RefScreenSize.x);
    
    Result.x = (WidthRatio / 100.0f) * CurrentScreenSize.x;
    Result.y = Result.x * OriginalRatio;
    
    return(Result);
}

static math_point
CalculateNewSizeByWidth(math_point RefSize, math_point RefScreenSize,
                        math_point CurrentScreenSize)
{
    math_point Result = {};
    
    r32 WidthRatio  = RefSize.x / RefScreenSize.x;
    r32 HeightRatio = RefSize.y / RefScreenSize.y;
    
    Result.x = WidthRatio  * CurrentScreenSize.x;
    Result.y = HeightRatio * CurrentScreenSize.x;
    
    return(Result);
}

static math_point
CalculateNewSizeByHeight(math_point RefSize, math_point RefScreenSize,
                         math_point CurrentScreenSize)
{
    math_point Result = {};
    
    r32 WidthRatio  = RefSize.x / RefScreenSize.x;
    r32 HeightRatio = RefSize.y / RefScreenSize.y;
    
    Result.x = WidthRatio  * CurrentScreenSize.y;
    Result.y = HeightRatio * CurrentScreenSize.y;
    
    return(Result);
}

static r32
GetScale(s32 ScreenWidth, s32 ScreenHeight, 
         s32 RefWidth, s32 RefHeight,
         float ScaleFactor)
{
    r32 Result;
    
    Result = pow((r32)ScreenWidth / (r32)RefWidth, 1.0f - ScaleFactor) * 
        pow((r32)ScreenHeight / (r32)RefHeight, ScaleFactor);
    
    return(Result);
}


#define GAME_MATH_H
#endif 


