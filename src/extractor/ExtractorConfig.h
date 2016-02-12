#pragma once
#ifndef EXTRACTORCONFIG_H
#define EXTRACTORCONFIG_H

struct SpriteStateConfigEntry
{
    const char* bgraFilename;
    SpriteState spriteState;
};

const SpriteStateConfigEntry m_spriteStateConfig[] =
{
    {"Coin-1.bgra", Coin},
    {"Coin-2.bgra", Coin},
    {"Coin-3.bgra", Coin},

    {"Coin-score.bgra", ScoreCoin},

    {"POW-full.bgra", POWFull},
    {"POW-one.bgra", POWOneLeft},
    {"POW-two.bgra", POWTwoLeft},

    {"Crab-1.bgra", Crab},
    {"Crab-2.bgra", Crab},
    {"Crab-angry-1.bgra", CrabAngry},
    {"Crab-angry-2.bgra", CrabAngry},
    {"Crab-dead-1.bgra", CrabDead},
    {"Crab-dead-2.bgra", CrabDead},
    {"Fireball-1.bgra", Fireball},
    {"Fireball-2.bgra", Fireball},
    {"Fireball-3.bgra", Fireball},
    {"Fireball-4.bgra", Fireball},
    {"Fireball-5.bgra", Fireball},
    {"Fireball-6.bgra", Fireball},
    {"Fireball-7.bgra", Fireball},
    {"Fireball-8.bgra", Fireball},
    {"Jumper-1.bgra", Jumper},
    {"Jumper-2.bgra", Jumper},
    {"Jumper-dead.bgra", JumperDead},
    {"Luigi-bumping-left.bgra", LuigiBumpLeft},
    {"Luigi-bumping-right.bgra", LuigiBumpRight},
    {"Luigi-dazed-1.bgra", LuigiDazed},
    {"Luigi-dazed-2.bgra", LuigiDazed},
    {"Luigi-dying.bgra", LuigiDead},
    {"Luigi-jumping-left.bgra", LuigiJumpLeft},
    {"Luigi-jumping-right.bgra", LuigiJumpRight},
    {"Luigi-standing-left.bgra", LuigiStandLeft},
    {"Luigi-standing-right.bgra", LuigiStandRight},
    {"Luigi-walking-left.bgra", LuigiWalkLeft},
    {"Luigi-walking-right.bgra", LuigiWalkRight},
    {"Mario-bumping-left.bgra", MarioBumpLeft},
    {"Mario-bumping-right.bgra", MarioBumpRight},
    {"Mario-dazed-1.bgra", MarioDazed},
    {"Mario-dazed-2.bgra", MarioDazed},
    {"Mario-dying.bgra", MarioDead},
    {"Mario-jumping-left.bgra", MarioJumpLeft},
    {"Mario-jumping-right.bgra", MarioJumpRight},
    {"Mario-standing-left.bgra", MarioStandLeft},
    {"Mario-standing-right.bgra", MarioStandRight},
    {"Mario-walking-left.bgra", MarioWalkLeft},
    {"Mario-walking-right.bgra", MarioWalkRight},
    {"Spiny-dead-left.bgra", SpinyDead},
    {"Spiny-dead-right.bgra", SpinyDead},
    {"Spiny-left-1.bgra", SpinyLeft},
    {"Spiny-left-2.bgra", SpinyLeft},
    {"Spiny-right-1.bgra", SpinyRight},
    {"Spiny-right-2.bgra", SpinyRight}
};

const unsigned long m_numSprites = sizeof(m_spriteStateConfig) / sizeof(m_spriteStateConfig[0]);

#endif
