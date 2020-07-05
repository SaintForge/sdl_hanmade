/* date = July 3rd 2020 11:50 am */

#ifndef TETROMAN_INTRINSICS_H
#define TETROMAN_INTRINSICS_H

#define Assert(Expression) if(!(Expression)) { LogErrorLine( __FILE__, __LINE__); abort();  }
void LogErrorLine(const char* Message, int Line)
{
    fprintf(stderr, "Assert fail in %s: %d\n",Message, Line);
}

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

inline static b32
IsInRectangle(v2 Position, rectangle2 Rectangle)
{
    b32 Result = false;
    
    Result = ((Position.x >= Rectangle.Min.x) &&
              (Position.y >= Rectangle.Min.y) &&
              (Position.x < Rectangle.Max.x) &&
              (Position.y < Rectangle.Max.y));
    
    return(Result);
}

inline static b32
IsInRectangle(v2 *Position, u32 Amount, rectangle2 Rectangle)
{
    b32 Result = false;
    
    for (u32 Index = 0;
         Index < Amount;
         Index++)
    {
        Result = IsInRectangle(Position[Index], Rectangle);
        if (Result) break;
    }
    
    return (Result);
}

inline static b32
IsInRectangle(v2 Position, rectangle2 *Target, u32 TargetAmount)
{
    b32 Result = false;
    
    return (Result);
}



#endif //TETROMAN_INTRINSICS_H
