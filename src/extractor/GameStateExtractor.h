#pragma once
#ifndef GAMESTATEEXTRACTOR_H
#define GAMESTATEEXTRACTOR_H

#include <string>
#include "ExtractorConstants.h"

class GameStateExtractor
{
public:
    GameStateExtractor();
    virtual ~GameStateExtractor();

    bool InitFromBGRAFrameBuffer(void *data, int width, int height, int stride);
    bool InitFromBGRAFrameBufferFile(const std::string &filename, int width, int height, int stride);

    void WriteRGBDataToFile(const std::string &filename);

private:
    unsigned char IndexOfClosestPaletteColor(
            unsigned char red, 
            unsigned char green, 
            unsigned char blue,
            unsigned char alpha);
    void CalculateBitmapDiff();
    void DetectDiffRegions();
    int SpriteMatchScoreAtLocation(unsigned char **sprite, int x, int y);

    unsigned char _gameScreen[GAMESCREEN_HEIGHT][GAMESCREEN_WIDTH];
};

#endif
