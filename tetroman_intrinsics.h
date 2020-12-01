/* date = July 3rd 2020 11:50 am */

#ifndef TETROMAN_INTRINSICS_H
#define TETROMAN_INTRINSICS_H

#define Assert(Expression) if(!(Expression)) { LogErrorLine( __FILE__, __LINE__); *(int *)0 = 0; }
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

inline static game_texture*
MakeTextureFromString(game_offscreen_buffer *Buffer, game_font *Font, 
                      const char *Text, v4 Color)
{
    game_texture *Result = {};
    
    SDL_Color SDLColor = {};
    SDLColor.r = Color.r;
    SDLColor.g = Color.g;
    SDLColor.b = Color.b;
    
    game_surface *Surface = TTF_RenderUTF8_Blended(Font, Text, SDLColor);
    Assert(Surface);
    
    Result = SDL_CreateTextureFromSurface(Buffer->Renderer, Surface);
    Assert(Result);
    
    SDL_FreeSurface(Surface);
    
    return (Result);
}

inline static v2
QueryTextureDim(game_texture *Texture)
{
    v2 Result = {};
    
    s32 w, h;
    SDL_QueryTexture(Texture, 0, 0, &w, &h);
    
    Result.w = w;
    Result.h = h;
    
    return(Result);
}

inline static void
GetTimeString(char TimeString[64], r32 TimeElapsed)
{
    u32 Minutes = (u32)TimeElapsed / 60;
    u32 Seconds = (u32)TimeElapsed % 60;
    
    sprintf(TimeString, "%02d:%02d", Minutes, Seconds);
}

#endif //TETROMAN_INTRINSICS_H
