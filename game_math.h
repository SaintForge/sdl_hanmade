// math.cpp --- 
// 
// Filename: game.cpp
// Author: Sierra
// Created: Вт окт 10 10:32:14 2017 (+0300)
// Last-Updated: Пт окт 27 14:17:15 2017 (+0300)
//           By: Sierra
//

#if !defined(GAME_MATH_H)

struct vector2
{
    r32 x;
    r32 y;
};


static void 
Vector2Add(vector2 *Vector, r32 x, r32 y)
{
    Vector->x += x;
    Vector->y += y;
}

static void 
Vector2Add(vector2 *Vector1, vector2 *Vector2)
{
    Vector1->x += Vector2->x;
    Vector1->y += Vector2->y;
}

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
    
    if(X <= Quad->x)                 return false;
    else if(Y <= Quad->y)            return false;
    else if(X > (Quad->x + Quad->w)) return false;
    else if(Y > (Quad->y + Quad->h)) return false;
    else                             return true;
}

#define GAME_MATH_H
#endif 


