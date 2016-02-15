#include "GameStateExtractor.h"
#include "ExtractorConfig.h"

#include <cassert>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/types.h>

static bool m_globalsInitialized = false;
static unsigned char m_emptyGameScreen[GAMESCREEN_HEIGHT][GAMESCREEN_WIDTH];
static unsigned char m_spriteData[m_numSprites][SPRITE_HEIGHT][SPRITE_WIDTH];
int m_fd_gamestate;
void *m_mmap_ptr_gamestate;
int m_bad_counter;


class GameScreenRegion
{
public:
    GameScreenRegion(int i, int j) : lowX(i), highX(i), lowY(j), highY(j), numPixels(0)
    {};

    int lowX, highX;
    int lowY, highY;
    int numPixels;

    // debug purposes
    int scoreMatch;
    std::string spriteMatched;
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

        m_fd_gamestate = open("gamestate", O_RDWR | O_CREAT | O_CLOEXEC, 0666 );
        if (m_fd_gamestate == -1) {
            std::cerr << "Error: Can't open gamestate file!";
            return;
        }

        if (ftruncate(m_fd_gamestate, 10000) == -1) {
            std::cerr << "Error: Can't resize gamestate file!";
            return;
        }

        m_mmap_ptr_gamestate = mmap(NULL, 10000, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd_gamestate, 0);
        if (m_mmap_ptr_gamestate == MAP_FAILED) {
            std::cerr << "Error: Can't memory-map game state file!";
            return;
        }

        m_bad_counter = 0;
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
    if (numPixels > 4000)
    {
        *((int *)m_mmap_ptr_gamestate) = 0;
        *(((int *)m_mmap_ptr_gamestate) + 1) = 0;
        *(((int *)m_mmap_ptr_gamestate) + 2) = 0;
        *(((int *)m_mmap_ptr_gamestate) + 3) = 0;
        return;
    }

    _DetectDiffRegions();
    for (std::vector<GameScreenRegion>::iterator it = _regions.begin(); it != _regions.end(); ++it) {
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

        if (maxScore > 20)
        {
            GameObjectState g(
                    m_spriteStateConfig[bestSprite].spriteState,
                    (posX+8) % GAMESCREEN_WIDTH,
                    posY+8);
            _gameObjects.push_back(g);
        }

        // for debug only
        it->scoreMatch = maxScore;
        it->spriteMatched = bestSprite != -1 ? 
                m_spriteStateConfig[bestSprite].bgraFilename :
                "noMatch";
    }

    bool marioFound = false, luigiFound = false;
    for (std::vector<GameObjectState>::iterator it = _gameObjects.begin(); it != _gameObjects.end(); ++it) {
        GameObjectState g = *it;
        if (g.gameObject == MarioBumpLeft ||
            g.gameObject == MarioBumpRight ||
            g.gameObject == MarioDazed ||
            g.gameObject == MarioDead || 
            g.gameObject == MarioJumpLeft ||
            g.gameObject == MarioJumpRight ||
            g.gameObject == MarioStandLeft ||
            g.gameObject == MarioStandRight ||
            g.gameObject == MarioWalkLeft ||
            g.gameObject == MarioWalkRight)
        {
            *((int *)m_mmap_ptr_gamestate) = g.posX;
            *(((int *)m_mmap_ptr_gamestate) + 1) = g.posY;
            marioFound = true;
        }

        if (g.gameObject == LuigiBumpLeft ||
            g.gameObject == LuigiBumpRight ||
            g.gameObject == LuigiDazed ||
            g.gameObject == LuigiDead || 
            g.gameObject == LuigiJumpLeft ||
            g.gameObject == LuigiJumpRight ||
            g.gameObject == LuigiStandLeft ||
            g.gameObject == LuigiStandRight ||
            g.gameObject == LuigiWalkLeft ||
            g.gameObject == LuigiWalkRight)
        {
            *(((int *)m_mmap_ptr_gamestate) + 2) = g.posX;
            *(((int *)m_mmap_ptr_gamestate) + 3) = g.posY;
            luigiFound = true;
        }

    }

    if (!marioFound)
    {
        std::cerr << "Mario not found\n";
        std::stringstream filename;
        filename << "no-mario-" << m_bad_counter++ << ".bgra";
        _WriteBGRADataToFile(filename.str());
        filename << ".rgn";
        _WriteRegionsToFile(filename.str());
    }

    if (!luigiFound)
    {
        std::cerr << "Luigi not found\n";
        std::stringstream filename("no-luigi-");
        filename << "no-luigi-" << m_bad_counter++ << ".bgra";
        _WriteBGRADataToFile(filename.str());
        filename << ".rgn";
        _WriteRegionsToFile(filename.str());
    }

}

// debug function
void GameStateExtractor::_WriteBGRADataToFile(const std::string &filename)
{
    std::ofstream file(filename, std::ofstream::trunc);
    for (int i = 0; i < GAMESCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < GAMESCREEN_WIDTH; j++)
        {
            ColorRGB c = SMB2PVPalette[_gameScreen[i][j]];
            if (_gameScreen[i][j] == 0)
            {
                file << (char) 0 << (char) 0 << (char) 0 << (char) 0;
            }
            else
            {
                file << c.blue << c.green << c.red << (char) 255;
            }
        }
    }
    file.close();
}

void GameStateExtractor::_WriteRegionsToFile(const std::string &filename)
{
    std::ofstream file(filename, std::ofstream::trunc);
    
    for (std::vector<GameScreenRegion>::iterator it = _regions.begin(); it != _regions.end(); ++it) {
        file << "(" << it->lowX << ", " << it->lowY << ") (" << it->highX << ", " << it->highY << ") " << it->scoreMatch << " " << it->spriteMatched << "\n";
    }

    file.close();
}
