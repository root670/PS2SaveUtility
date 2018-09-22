#include <iostream>

#include "SaveFile.h"
#include "CBSFile.h"
#include "MAXFile.h"
#include "PSUFile.h"
#include "XPSFile.h"

std::unique_ptr<ISaveFile> SaveFileFactory::createSaveFile(const std::string &path)
{
    if(CBSFile::isCorrectFileExtension(path))
    {
        return std::unique_ptr<ISaveFile>(new CBSFile);
    }
    else if(MAXFile::isCorrectFileExtension(path))
    {
        return std::unique_ptr<ISaveFile>(new MAXFile);
    }
    else if(PSUFile::isCorrectFileExtension(path))
    {
        return std::unique_ptr<ISaveFile>(new PSUFile);
    }
    else if(XPSFile::isCorrectFileExtension(path))
    {
        return std::unique_ptr<ISaveFile>(new XPSFile);
    }
    
    std::cout << "Error: File format used by " << path << " is not supported." << std::endl;
    return std::unique_ptr<ISaveFile>();
}
