#pragma once

#include <string>

#include "Utilities.h"

class ISaveFile
{
public:
    virtual ~ISaveFile() {}
    
    virtual bool read(const std::string &path, PS2Directory &dir)const = 0;
    virtual bool write(const std::string &path, const PS2Directory &dir)const = 0;
    virtual bool isValid(const std::vector<unsigned char> &data)const = 0;
    virtual bool isValid(const std::string &path)const = 0;
};

class SaveFileFactory
{
public:
    static std::unique_ptr<ISaveFile> createSaveFile(const std::string &path);
};
