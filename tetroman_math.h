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


inline v2
V2(r32 x, r32 y)
{
    v2 Result;
    
    Result.x = x;
    Result.y = y;
    
    return(Result);
}


inline v4
V4(r32 x, r32 y, r32 z, r32 w)
{
    v4 Result;
    
    Result.x = x;
    Result.y = y;
    Result.z = z;
    Result.w = w;
    
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
operator/(v2 A, r32 B)
{
    v2 Result;
    
    Result.x = A.x / B;
    Result.y = A.y / B;
    
    return(Result);
}

inline v2 &
operator/=(v2 &A, r32 B)
{
    A = A / B;
    
    return(A);
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
operator-(v2 A, r32 B)
{
    v2 Result;
    
    Result.x = A.x - B;
    Result.y = A.y - B;
    
    return(Result);
}

inline v2
operator-(v2 A, v2 B)
{
    v2 Result;
    
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    
    return(Result);
}

inline v2 &
operator-=(v2 &A, v2 B)
{
    A = A - B;
    
    return(A);
}


inline v2 &
operator-=(v2 &A, r32 B)
{
    A = A - B;
    
    return(A);
}

inline v2
operator+(v2 A, v2 B)
{
    v2 Result;
    
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    
    return(Result);
}

inline v2
operator+(v2 A, r32 B)
{
    v2 Result;
    
    Result.x = A.x + B;
    Result.y = A.y + B;
    
    return(Result);
}

inline v2 &
operator+=(v2 &A, v2 B)
{
    A = A + B;
    
    return(A);
}



inline v2 &
operator+=(v2 &A, r32 B)
{
    A.x = A.x + B;
    A.y = A.y + B;
    
    return(A);
}

inline b32
operator==(v2 &A, v2 &B)
{
    b32 Result;
    
    Result = (A.x == B.x && A.y == B.y);
    
    return (Result);
}

inline b32
operator!=(v2 &A, v2 &B)
{
    b32 Result;
    
    Result = !(A == B);
    
    return (Result);
}

inline r32
Square(r32 A)
{
    r32 Result = A * A;
    
    return (Result);
}

inline r32
SquareRoot(v2 v)
{
    r32 Result = 0.0f;
    Result = sqrt(v.x * v.x + v.y * v.y);
    
    return(Result);
}

inline v2
Normalize(v2 A)
{
    v2 Result = {};
    
    r32 Magnitude = SquareRoot(A);
    if (Magnitude)
    {
        Result = A / Magnitude;
    }
    
    return(Result);
}

struct rectangle2
{
    v2 Min;
    v2 Max;
};

inline static v2
GetDim(rectangle2 Rectangle)
{
    v2 Result = {};
    
    Result.w = Rectangle.Max.x - Rectangle.Min.x;
    Result.h = Rectangle.Max.y - Rectangle.Min.y;
    
    return (Result);
}

inline static void
SetDim(rectangle2 *Rectangle, v2 Dimension)
{
    Rectangle->Max.x = Rectangle->Min.x + Dimension.w;
    Rectangle->Max.y = Rectangle->Min.y + Dimension.h;
}

inline static void
SetDim(rectangle2 *Rectangle, r32 DimensionWidth, r32 DimensionHeight)
{
    Rectangle->Max.x = Rectangle->Min.x + DimensionWidth;
    Rectangle->Max.y = Rectangle->Min.y + DimensionHeight;
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

static r32
Lerp1(r32 a, r32 b, r32 t) {
    r32 Result = a + (t * (b - a));
    return (Result);
}

static v2
Lerp2(v2 a, v2 b, r32 t) {
    
    v2 Result = {};
    
    Result.x = Lerp1(a.x, b.x, t);
    Result.y = Lerp1(a.y, b.y, t);
    
    return (Result);
}

inline static void
TransposeMatrix(u32 Matrix[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = i; j < 4; j++)
        {
            u32 Value = Matrix[i][j];
            Matrix[i][j] = Matrix[j][i];
            Matrix[j][i] = Value;
        }
    }
}

inline static void
ReverseMatrix(u32 Matrix[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        u32 k = 3;
        for (int j = 0; j < k; j++)
        {
            u32 Value = Matrix[i][j];
            Matrix[i][j] = Matrix[i][k];
            Matrix[i][k] = Value;
            k--;
        }
    }
}

inline static void
RotateMatrixClockwise(u32 Matrix[4][4])
{
    TransposeMatrix(Matrix);
    ReverseMatrix(Matrix);
}

#define GAME_MATH_H
#endif 


