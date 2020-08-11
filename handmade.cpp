

#include "handmade.h"


internal void
GameOutputSound(game_state *GameState, game_sound_output_buffer *SoundBuffer, int ToneHz)
{
    int16 ToneVolume = 300;
    int WavePeriod = SoundBuffer->SamplesPerSecond/ToneHz;

    int16 *SampleOut = SoundBuffer->Samples;
    for(int SampleIndex = 0;
        SampleIndex < SoundBuffer->SampleCount;
        ++SampleIndex)
    {
        // TODO(casey): Draw this out for people
        // real32 SineValue = sinf(GameState->tSine);
        // int16 SampleValue = (int16)(SineValue * ToneVolume);
        int16 SampleValue = 0;
        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;

        // GameState->tSine += 2.0f*Pi32*1.0f/(real32)WavePeriod;
        // if(GameState->tSine > 2.0f*Pi32)
        // {
        //     GameState->tSine -= 2.0f*Pi32;
        // }
    }
}

internal int32
RoundReal32ToInt32(real32 Real)
{
    int32 Result = (int32)(Real + 0.5f);
    return Result;
}

internal uint32
RoundReal32ToUInt32(real32 Real)
{
    uint32 Result = (uint32)(Real + 0.5f);
    return Result;
}

internal void
DrawRectangle(game_offscreen_buffer *Buffer,
            real32 RealMinX, real32 RealMinY, real32 RealMaxX, real32 RealMaxY,
            real32 R, real32 G, real32 B)
{
    int32 MinX = RoundReal32ToInt32(RealMinX);
    int32 MinY = RoundReal32ToInt32(RealMinY);
    int32 MaxX = RoundReal32ToInt32(RealMaxX);
    int32 MaxY = RoundReal32ToInt32(RealMaxY);

    if(MinX < 0)
    {
        MinX = 0;
    }

    if(MinY < 0)
    {
        MinY = 0;
    }

    if(MaxX > Buffer->Width)
    {
        MaxX = Buffer->Width;
    }

    if(MaxY > Buffer->Height)
    {
        MaxY = Buffer->Height;
    }

    uint32 Color = ((RoundReal32ToUInt32(R * 255.0f) << 16) |
                            (RoundReal32ToUInt32(G * 255.0f) << 8) |
                            RoundReal32ToUInt32(B * 255.0f));

    uint8 *Row = ((uint8 *)Buffer->Memory + MinX*Buffer->BytesPerPixel
                    + MinY*Buffer -> Pitch);

    for(int Y = MinY; Y < MaxY; ++Y)
    {
        uint32 *Pixel = (uint32 *)Row;
        for(int X= MinX; X < MaxX ; ++X)
        {
            *Pixel++ = Color;

        }
        Row += Buffer -> Pitch;
    }
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    // Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) ==
    //        (ArrayCount(Input->Controllers[0].Buttons)));
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);

    game_state *GameState = (game_state *)Memory->PermanentStorage;
    if(!Memory->IsInitialized)
    {
        Memory->IsInitialized = true;
    }

    uint32 TileMap[9][16]=
    {
        {0, 0, 0, 0, 0, 0, 0,0,1,0,0,0,0,0,0,0},
        {0, 1, 0, 0, 0, 1, 0,0,1,0,0,0,0,0,0,0},
        {0, 1, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0},
        {0, 0, 0, 0, 0, 1, 0,0,0,0,0,0,0,0,0,0},
        {0, 1, 0, 0, 0, 1, 0,0,0,0,0,0,0,0,0,0},
        {0, 1, 0, 0, 0, 1, 0,0,0,0,0,0,0,0,0,0},
        {0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0},
        {0, 1, 1, 1, 1, 0, 0,0,0,0,0,0,0,0,0,0},
        {0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0}
    };

    for(int ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers);
        ++ControllerIndex)
    {
        game_controller_input *Controller = GetController(Input, ControllerIndex);
        if(Controller->IsAnalog)
        {
        }
        else
        {
            real32 dPlayerX = 0.0f;
            real32 dPlayerY = 0.0f;
            if(Controller->MoveUp.EndedDown)
            {
                dPlayerY = -1.0f;
            }
            if(Controller->MoveDown.EndedDown)
            {
                dPlayerY = 1.0f;
            }
            if(Controller->MoveLeft.EndedDown)
            {
                dPlayerX = -1.0f;
            }
            if(Controller->MoveRight.EndedDown)
            {
                dPlayerX = 1.0f;
            }
            dPlayerX *= 32.0f;
            dPlayerY *= 32.0f;
            GameState->PlayerX += Input->dtForFrame * dPlayerX;
            GameState->PlayerY += Input->dtForFrame * dPlayerY;
        }
    }

    real32 UpperLeftX = 0;
    real32 UpperLeftY = 0;
    real32 TileWidth = 60;
    real32 TileHeight = 60;

    DrawRectangle(Buffer, 0.0f, 0.0f, (real32)Buffer->Width, (real32)Buffer->Height,
                  1.0f, 0.0f, 1.0f);

    for(int Row = 0; Row < 9; ++Row)
    {
        for(int Column = 0; Column < 16; ++Column)
        {
            uint32 TileID = TileMap[Row][Column];
            real32 Gray = 0.5f;
            if(TileID == 1)
            {
                Gray = 1.0f;
            }
            real32 MinX = UpperLeftX + ((real32)Column) * TileWidth;
            real32 MinY = UpperLeftY + ((real32)Row) * TileHeight;
            real32 MaxX = MinX + TileWidth;
            real32 MaxY = MinY + TileHeight;

            DrawRectangle(Buffer, MinX, MinY, MaxX, MaxY, Gray, Gray, Gray);
        }
    }
    real32 PlayerR = 1.0f;
    real32 PlayerG = 1.0f;
    real32 PlayerB = 0.0f;
    real32 PlayerWidth = 0.75f * TileWidth;
    real32 PlayerHeight = TileHeight;
    real32 PlayerTop = GameState->PlayerY - PlayerHeight;
    real32 PlayerLeft = GameState->PlayerX - 0.5f * PlayerWidth;
    DrawRectangle(Buffer, PlayerLeft, PlayerTop, PlayerLeft + PlayerWidth,
                  PlayerTop + PlayerHeight,
                  PlayerR, PlayerG, PlayerB);
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    GameOutputSound(GameState, SoundBuffer, 400);
}


// internal void
// RenderWeirdGradient(game_offscreen_buffer *Buffer, int BlueOffset, int GreenOffset)
// {
//     // TODO(casey): Let's see what the optimizer does
//
//     uint8 *Row = (uint8 *)Buffer->Memory;
//     for(int Y = 0;
//         Y < Buffer->Height;
//         ++Y)
//     {
//         uint32 *Pixel = (uint32 *)Row;
//         for(int X = 0;
//             X < Buffer->Width;
//             ++X)
//         {
//             uint8 Blue = (uint8)(X + BlueOffset);
//             uint8 Green = (uint8)(Y + GreenOffset);
//
//             *Pixel++ = ((Green << 8) | Blue);
//         }
//
//         Row += Buffer->Pitch;
//     }
// }
