#pragma once
#ifndef GAMESTATEEXTRACTOR_H
#define GAMESTATEEXTRACTOR_H

#include <string>
#include <vector>
#include "ExtractorConstants.h"

class GameScreenRegion;

class GameObjectState
{
public:
    GameObjectState(SpriteState g, int x, int y) : gameObject(g), posX(x), posY(y)
    {}
    SpriteState gameObject;
    int posX, posY;
};

class GameStateExtractor
{
public:
    GameStateExtractor();
    virtual ~GameStateExtractor();

    bool InitFromBGRAFrameBuffer(
            void *data,
            int width,
            int height,
            int stride);
    bool InitFromBGRAFrameBufferFile(
            const std::string &filename,
            int width,
            int height,
            int stride);
    void ProcessGameState();

private:
    static bool _InitPaletteArrayFromFrameBuffer(
            unsigned char *pa,
            int paWidth,
            int paHeight,
            void *data,
            int fbWidth,
            int fbHeight,
            int fbStride);
    static bool _InitPaletteArrayFromFrameBufferFile(
            unsigned char *pa,
            int paWidth,
            int paHeight,
            const std::string &filename,
            int fbWidth,
            int fbHeight,
            int fbStride);
    static unsigned char _IndexOfClosestPaletteColor(
            unsigned char red,
            unsigned char green,
            unsigned char blue,
            unsigned char alpha);

    int _CalculateBitmapDiff();
    GameScreenRegion _FillRegion(int initialX, int initialY, int fill);
    void _DetectDiffRegions();
    int _SpriteMatchScoreAtLocation(unsigned char sprite[SPRITE_HEIGHT][SPRITE_WIDTH], int x, int y);

    void _WriteBGRADataToFile(const std::string &filename);
    void _WriteRegionsToFile(const std::string &filename);

    unsigned char _gameScreen[GAMESCREEN_HEIGHT][GAMESCREEN_WIDTH];
    int _gameScreenDiff[GAMESCREEN_HEIGHT][GAMESCREEN_WIDTH];

    std::vector<GameScreenRegion> _regions;
    std::vector<GameObjectState> _gameObjects;
};

#endif
