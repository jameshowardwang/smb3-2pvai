#include "GameStateExtractor.h"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        return 0;
    }

    GameStateExtractor extractor;
    std::string inFile(argv[1]);
    std::string outFile(argv[2]);
    extractor.InitFromBGRAFrameBufferFile(inFile, 128, 128, 128*4);
    extractor.WriteRGBDataToFile(outFile);

    return 0;
}
