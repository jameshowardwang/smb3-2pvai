#include "GameStateExtractor.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        return 0;
    }

    GameStateExtractor extractor;
    std::string inFile(argv[1]);
    extractor.InitFromBGRAFrameBufferFile(inFile, 960, 720, -960*4);
    extractor.ProcessGameState();

    return 0;
}
