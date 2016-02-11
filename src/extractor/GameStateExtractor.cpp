#include "GameStateExtractor.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

GameStateExtractor::GameStateExtractor()
{
}

GameStateExtractor::~GameStateExtractor()
{
}

bool GameStateExtractor::InitFromBGRAFrameBuffer(void *data, int width, int height, int stride)
{
    // downsample to NES resolution and palette

    int xOffset = (width > GAMESCREEN_WIDTH * 2) ? 1 : 0;
    int yOffset = (height > GAMESCREEN_HEIGHT * 2) ? 1: 0;

    unsigned char *imageBytes = (unsigned char *) data;

    for (int y = 0; y < GAMESCREEN_HEIGHT; y++)
    {
        int yImage = y * height / GAMESCREEN_HEIGHT + yOffset;
        for (int x = 0; x < GAMESCREEN_WIDTH; x++)
        {
            int xImage = x * width / GAMESCREEN_WIDTH + xOffset;
            unsigned char *base;
            if (stride > 0)
            {
                base = imageBytes + yImage * stride + xImage * 4;
            } else {
                base = imageBytes + (height - yImage) * stride + xImage * 4;
            }

            int blue = *base;
            int green = *(base+1);
            int red = *(base+2);
            int alpha = *(base+3);
            _gameScreen[y][x] = IndexOfClosestPaletteColor(red, green, blue, alpha);
        }
    }
    return true;
}

bool GameStateExtractor::InitFromBGRAFrameBufferFile(const std::string &filename, int width, int height, int stride)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size))
    {
        return InitFromBGRAFrameBuffer(buffer.data(), width, height, stride); 
    }
                
    return false;
}

unsigned char GameStateExtractor::IndexOfClosestPaletteColor(
        unsigned char red, 
        unsigned char green, 
        unsigned char blue,
        unsigned char alpha)
{
    unsigned char closestIndex;
    int closestDistance = 256*256*3;

    if (alpha == 0)
    {
        return 0;
    }

    if (alpha < 200)
    {
        std::cerr << "Alpha: " << (int) alpha << "\n";
    }

    for (int i = 1; i < SMB2PVPALETTESIZE; i++) // index 0 is reserved for transparency
    {
        int red_d = red - SMB2PVPalette[i].red;
        int green_d = green - SMB2PVPalette[i].green;
        int blue_d = blue - SMB2PVPalette[i].blue;
        int d = red_d * red_d + green_d * green_d + blue_d * blue_d;
        if (d < closestDistance)
        {
            closestDistance = d;
            closestIndex = i;
        }
    }

    return closestIndex;
}

void GameStateExtractor::CalculateBitmapDiff()
{
}

void GameStateExtractor::DetectDiffRegions()
{
}

int GameStateExtractor::SpriteMatchScoreAtLocation(unsigned char **sprite, int x, int y)
{
    return 0;
}

void GameStateExtractor::WriteRGBDataToFile(const std::string &filename)
{
    std::ofstream file(filename, std::ofstream::app);
    for (int i = 0; i < GAMESCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < GAMESCREEN_WIDTH; j++)
        {
            file << (int) _gameScreen[i][j] << "\n";
            /*
            ColorRGB c = SMB2PVPalette[_gameScreen[i][j]];
            if (_gameScreen[i][j] == 0)
            {
                file << (char) 0 << (char) 0 << (char) 0 << (char) 0;
            }
            else
            {
                file << c.blue << c.green << c.red << (char) 255;
            }
            */
        }
    }
    file.close();
}
