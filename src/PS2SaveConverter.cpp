#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <zlib.h>
#include <dirent.h>

#include "Utilities.h"
#include "SaveFile.h"

bool convertSaveFile(const std::string &pathIn, const std::string &pathOut)
{
    bool res(true);
    PS2Directory dir;
    
    std::cout << "Converting " << pathIn << " to " << pathOut << std::endl;
    
    auto saveFileIn = SaveFileFactory::createSaveFile(pathIn);
    res = res && saveFileIn != nullptr;
    res = res && saveFileIn->read(pathIn, dir);
    
    auto saveFileOut = SaveFileFactory::createSaveFile(pathOut);
    res = res && saveFileOut != nullptr;
    res = res && saveFileOut->write(pathOut, dir);
    
    if(res)
    {
        std::cout << "Conversion was successful!" << std::endl;
    }
    else
    {
        std::cout << "Conversion failed!" << std::endl;
    }
    
    return res;
}

void printHelp(const std::string &executablePath)
{
    std::cout << "PS2 Save Utility" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << executablePath << " <input file> <output file>" << std::endl;
    //std::cout << "  " << executablePath << " <input file> -f <format>" << std::endl;
    //std::cout << "  " << executablePath << " <input directory> -f <format>" << std::endl;
}

int main(int argc, char *argv[])
{
    if(argc == 3)
    {
        return convertSaveFile(argv[1], argv[2]) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else
    {
        printHelp(argv[0]);
        return EXIT_FAILURE;
    }
}
