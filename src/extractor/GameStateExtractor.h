#pragma once
#ifndef GAMESTATEEXTRACTOR_H
#define GAMESTATEEXTRACTOR_H

#include <string>
#include <vector>
#include "ExtractorConstants.h"

class GameScreenRegion;

class GameStateExtractor
{
public:
    GameStateExtractor();
    virtual ~GameStateExtractor();

    bool InitFromBGRAFrameBuffer(void *data, int width, int height, int stride);
    bool InitFromBGRAFrameBufferFile(const std::string &filename, int width, int height, int stride);
    void ProcessGameState();

    void WriteRGBDataToFile(const std::string &filename);

private:
    bool _InitGameScreenFromFrameBuffer(unsigned char gameScreen[][GAMESCREEN_WIDTH], void *data, int width, int height, int stride);
    bool _InitGameScreenFromFrameBufferFile(const std::string &filename, unsigned char gameScreen[][GAMESCREEN_WIDTH], int width, int height, int stride);
    unsigned char _IndexOfClosestPaletteColor(
            unsigned char red, 
            unsigned char green, 
            unsigned char blue,
            unsigned char alpha);
    int _CalculateBitmapDiff();
    GameScreenRegion _FillRegion(int initialX, int initialY, int fill);
    void _DetectDiffRegions();
    int _SpriteMatchScoreAtLocation(unsigned char **sprite, int x, int y);

    unsigned char _gameScreen[GAMESCREEN_HEIGHT][GAMESCREEN_WIDTH];
    int _gameScreenDiff[GAMESCREEN_HEIGHT][GAMESCREEN_WIDTH];

    std::vector<GameScreenRegion> _regions;
};

#endif
