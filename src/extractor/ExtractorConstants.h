#pragma once
#ifndef EXTRACTORCONSTANTS_H
#define EXTRACTORCONSTANTS_H

#define GAMESCREEN_WIDTH 256
#define GAMESCREEN_HEIGHT 240 
#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 16

typedef struct ColorRGB
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} ColorRGB;

#define NESPALETTESIZE 56
const ColorRGB NESPalette[NESPALETTESIZE] = 
{
    {0,0,0},    // index 0 represents transparent
    {124,124,124},
    {0,0,252},
    {0,0,188},
    {68,40,188},
    {148,0,132},
    {168,0,32},
    {168,16,0},
    {136,20,0},
    {80,48,0},
    {0,120,0},
    {0,104,0},
    {0,88,0},
    {0,64,88},

    {188,188,188},
    {0,120,248},
    {0,88,248},
    {104,68,252},
    {216,0,204},
    {228,0,88},
    {248,56,0},
    {228,92,16},
    {172,124,0},
    {0,184,0},
    {0,168,0},
    {0,168,68},
    {0,136,136},
    {0,0,0},

    {248,248,248},
    {60,188,252},
    {104,136,252},
    {152,120,248},
    {248,120,248},
    {248,88,152},
    {248,120,88},
    {252,160,68},
    {248,184,0},
    {184,248,24},
    {88,216,84},
    {88,248,152},
    {0,232,216},
    {120,120,120},

    {252,252,252},
    {164,228,252},
    {184,184,248},
    {216,184,248},
    {248,184,248},
    {248,164,192},
    {240,208,176},
    {252,224,168},
    {248,216,120},
    {216,248,120},
    {184,248,184},
    {184,248,216},
    {0,252,252},
    {216,216,216}
};


#define SMB2PVPALETTESIZE 56
const ColorRGB SMB2PVPalette[SMB2PVPALETTESIZE] = 
{
    {0,0,0},    // index 0 represents transparent
    {0,64,88},
    {0,88,248},
    {104,68,252},
    {228,92,16},
    {172,124,0},
    {0,168,0},
    {0,0,0},
    {60,188,252},
    {104,136,252},
    {252,160,68},
    {88,216,84},
    {252,252,252},
    {164,228,252},
    {240,208,176},
    {184,248,216},
    {168,16,0}
};

enum SpriteState
{
    ScoreCoin = 1,
    Coin,
    Crab,
    CrabAngry,
    CrabDead,
    Fireball,
    Jumper,
    JumperDead,
    LuigiBumpLeft,
    LuigiBumpRight,
    LuigiDazed,
    LuigiDead,
    LuigiJumpLeft,
    LuigiJumpRight,
    LuigiStandLeft,
    LuigiStandRight,
    LuigiWalkLeft,
    LuigiWalkRight,
    MarioBumpLeft,
    MarioBumpRight,
    MarioDazed,
    MarioDead,
    MarioJumpLeft,
    MarioJumpRight,
    MarioStandLeft,
    MarioStandRight,
    MarioWalkLeft,
    MarioWalkRight,
    POWFull,
    POWTwoLeft,
    POWOneLeft,
    SpinyLeft,
    SpinyRight,
    SpinyDead
};

#endif
