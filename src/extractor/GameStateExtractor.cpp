#include "GameStateExtractor.h"
#include "ExtractorConfig.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

static bool m_globalsInitialized = false;
static unsigned char m_emptyGameScreen[GAMESCREEN_HEIGHT][GAMESCREEN_WIDTH];
static unsigned char m_spriteData[m_numSprites][SPRITE_HEIGHT][SPRITE_WIDTH];

class GameScreenRegion
{
public:
    GameScreenRegion(int i, int j) : lowX(i), highX(i), lowY(j), highY(j), numPixels(0)
    {};

    int lowX, highX;
    int lowY, highY;
    int numPixels;
};

GameStateExtractor::GameStateExtractor()
{
    if (!m_globalsInitialized)
    {
        m_globalsInitialized = true;
        _InitPaletteArrayFromFrameBufferFile(
                (unsigned char *)m_emptyGameScreen,
                GAMESCREEN_WIDTH,
                GAMESCREEN_HEIGHT,
                "../../images/bgra/Board-empty.bgra",
                GAMESCREEN_WIDTH,
                GAMESCREEN_HEIGHT,
                GAMESCREEN_WIDTH*4);

        for (int i = 0; i < (int) m_numSprites; i++)
        {
            std::string filename = std::string("../../images/sprites/bgra/") +
                m_spriteStateConfig[i].bgraFilename;
            _InitPaletteArrayFromFrameBufferFile(
                    (unsigned char *)m_spriteData[i],
                    SPRITE_WIDTH,
                    SPRITE_HEIGHT,
                    filename,
                    SPRITE_WIDTH,
                    SPRITE_HEIGHT,
                    SPRITE_HEIGHT*4);
        }
    }
}

GameStateExtractor::~GameStateExtractor()
{
}

bool GameStateExtractor::InitFromBGRAFrameBuffer(void *data, int width, int height, int stride)
{
    _InitPaletteArrayFromFrameBuffer(
            (unsigned char *)_gameScreen,
            GAMESCREEN_WIDTH,
            GAMESCREEN_HEIGHT,
            data,
            width,
            height,
            stride);
    return true;
}

bool GameStateExtractor::_InitPaletteArrayFromFrameBuffer(
        unsigned char *pa,
        int paWidth,
        int paHeight,
        void *data,
        int fbWidth,
        int fbHeight,
        int fbStride)
{
    int xOffset = (fbWidth > paWidth * 2) ? 1 : 0;
    int yOffset = (fbHeight > paHeight * 2) ? 1: 0;

    unsigned char *imageBytes = (unsigned char *) data;

    for (int y = 0; y < paHeight; y++)
    {
        int yImage = y * fbHeight / paHeight + yOffset;
        for (int x = 0; x < paWidth; x++)
        {
            int xImage = x * fbWidth / paWidth + xOffset;
            unsigned char *base;
            if (fbStride > 0)
            {
                base = imageBytes + yImage * fbStride + xImage * 4;
            } else {
                base = imageBytes + (fbHeight-1 - yImage) * (-fbStride) + xImage * 4;
            }

            int blue = *base;
            int green = *(base+1);
            int red = *(base+2);
            int alpha = *(base+3);
            *(pa+y*paWidth+x) = _IndexOfClosestPaletteColor(red, green, blue, alpha);
        }
    }
    return true;
}

bool GameStateExtractor::InitFromBGRAFrameBufferFile(const std::string &filename, int width, int height, int stride)
{
    return _InitPaletteArrayFromFrameBufferFile(
            (unsigned char *)_gameScreen,
            GAMESCREEN_WIDTH,
            GAMESCREEN_HEIGHT,
            filename,
            width,
            height,
            stride);
}

bool GameStateExtractor::_InitPaletteArrayFromFrameBufferFile(
        unsigned char *pa,
        int paWidth,
        int paHeight,
        const std::string &filename,
        int fbWidth,
        int fbHeight,
        int fbStride)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size))
    {
        return _InitPaletteArrayFromFrameBuffer(
                (unsigned char *)pa,
                paWidth,
                paHeight,
                buffer.data(),
                fbWidth,
                fbHeight,
                fbStride);
    }

    return false;
}

unsigned char GameStateExtractor::_IndexOfClosestPaletteColor(
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
//        std::cerr << "Alpha: " << (int) alpha << "\n";
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

int GameStateExtractor::_CalculateBitmapDiff()
{
    int numPixelsDifferent = 0;
    for (int i = 0; i < GAMESCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < GAMESCREEN_WIDTH; j++)
        {
            if (m_emptyGameScreen[i][j] == _gameScreen[i][j])
            {
                _gameScreenDiff[i][j] = 0;
            }
            else
            {
                _gameScreenDiff[i][j] = -1;
                numPixelsDifferent++;
            }
        }
    }
    return numPixelsDifferent;
}

typedef std::pair<int, int> intPair;
GameScreenRegion GameStateExtractor::_FillRegion(int initialX, int initialY, int fill)
{
    GameScreenRegion region(initialX, initialY);

    std::vector<intPair> pixelsToVisit;
    pixelsToVisit.push_back(intPair(initialY, initialX));

    while (!pixelsToVisit.empty())
    {
        intPair p = pixelsToVisit.back();
        pixelsToVisit.pop_back();

        // make sure we don't re-visit a node that's already been processed
        // we could check if this node is already in the stack before adding but this seems faster
        if (_gameScreenDiff[p.first][p.second] != -1)
        {
            continue;
        }

        _gameScreenDiff[p.first][p.second] = fill;

        region.numPixels++;
        if (p.first < region.lowY) region.lowY = p.first;
        if (p.first > region.highY) region.highY = p.first;
        if (p.second < region.lowX) region.lowX = p.second;
        if (p.second > region.highX) region.highX = p.second;

        for (int i = p.first-1; i <= p.first+1; i++)
        {
            for (int j = p.second-1; j <= p.second+1; j++)
            {
                if (i > 0 && i < GAMESCREEN_HEIGHT && j > 0 && j < GAMESCREEN_WIDTH &&
                        !(i == p.first && j == p.second))
                {
                    if (_gameScreenDiff[i][j] == -1)
                    {
                        pixelsToVisit.push_back(intPair(i, j));
                    }
                }
            }
        }
    }
    return region;
}

void GameStateExtractor::_DetectDiffRegions()
{
    int numRegions = 0;
    for (int i = 0; i < GAMESCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < GAMESCREEN_HEIGHT; j++)
        {
            if (_gameScreenDiff[i][j] == -1)
            {
                numRegions += 1;
                GameScreenRegion region = _FillRegion(j, i, numRegions);
                if (region.numPixels > 10)
                {
                    _regions.push_back(region);
                }
                else
                {
                    //std::cout << "Detected a region of size " << region.numPixels << "\n";
                }
            }
        }
    }
}

int GameStateExtractor::_SpriteMatchScoreAtLocation(
        unsigned char sprite[SPRITE_HEIGHT][SPRITE_WIDTH],
        int x,
        int y)
{
    int score = 0;
    int activePixels = 0;

    for (int i = 0; i < SPRITE_HEIGHT && y+i < GAMESCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SPRITE_WIDTH; j++)
        {
            if (sprite[i][j] == 0) // transparent
            {
                if (_gameScreenDiff[y+i][(x+j)%GAMESCREEN_WIDTH] == 0)
                {
                }
            }
            else
            {
                activePixels++;
                if (_gameScreen[y+i][(x+j)%GAMESCREEN_WIDTH] == sprite[i][j])
                {
                    score += 1; // add a point if the pixels match in color
                }
            }
        }
    }
    return score * 100 / activePixels;
}

void GameStateExtractor::ProcessGameState()
{
    _regions.clear();
    _gameObjects.clear();

    int numPixels = _CalculateBitmapDiff();
    //std::cout << "Number of differing pixels: " << numPixels << "\n";
    if (numPixels > 4000)
    {
        return;
    }

    _DetectDiffRegions();
    for (std::vector<GameScreenRegion>::iterator it = _regions.begin(); it != _regions.end(); ++it) {
        //std::cout << "Region with " << it->numPixels << " pixels detected from (" << it->lowX << ", " << it->lowY <<
            //") to (" << it->highX << ", " << it->highY << ")\n";

        int maxScore = 0;
        int bestSprite = -1;
        int posX, posY;
        for (int i = std::max(it->lowY - 8, 0); i < std::max(it->highY - 8, 0); i++)
        {
            for (int j = it->lowX - 8; j < it->highX - 8; j++)
            {
                int sIndex;
                if (abs(i-153) < 3 && abs(j-120) < 3)
                {
                    // only check for POW at this location
                    sIndex = 4;
                }
                else
                {
                    sIndex = 7;
                }
                while (sIndex < (int) m_numSprites)
                {
                    int score = _SpriteMatchScoreAtLocation(
                            m_spriteData[sIndex],
                            j % GAMESCREEN_WIDTH,
                            i);
                    if (score > maxScore)
                    {
                        maxScore = score;
                        bestSprite = sIndex;
                        posX = j % GAMESCREEN_WIDTH;
                        posY = i;
                    }
                    sIndex++;
                }
            }
        }
        //std::cout << "Best matching sprite: " << m_spriteStateConfig[bestSprite].bgraFilename <<
            //" with score " << maxScore << " at (" << posX << ", " << posY << ")\n";

        if (maxScore > 20)
        {
            GameObjectState g(
                    m_spriteStateConfig[bestSprite].spriteState,
                    (posX+8) % GAMESCREEN_WIDTH,
                    posY+8);
            _gameObjects.push_back(g);
        }
    }

}

// debug function
void GameStateExtractor::_WriteRGBDataToFile(const std::string &filename)
{
    std::ofstream file(filename, std::ofstream::trunc);
    for (int i = 0; i < GAMESCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < GAMESCREEN_WIDTH; j++)
        {
            file << (int) m_emptyGameScreen[i][j] << "-" << (int) _gameScreen[i][j] << "\n";
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
