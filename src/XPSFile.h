#pragma once

#include <string>

#include "Utilities.h"
#include "SaveFile.h"

class XPSFile : public ISaveFile
{
public:
    XPSFile() {}
    ~XPSFile() {}
    
    bool read(const std::string &path, PS2Directory &dir)const;
    bool write(const std::string &path, const PS2Directory &dir)const;
    bool isValid(const std::vector<unsigned char> &data)const;
    bool isValid(const std::string &path)const;
    static bool isCorrectFileExtension(const std::string &path);
};
