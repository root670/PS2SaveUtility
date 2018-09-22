#include <algorithm>
#include <stdio.h>
#include <iostream>

#include "Utilities.h"

std::vector<unsigned char> readFileContents(const std::string &path)
{
    std::vector<unsigned char> ret;
    FILE *f = fopen(path.c_str(), "rb");
    if (!f)
    {
        printf("Error: couldn't open file %s.\n", path.c_str());
        return ret;
    }

    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    ret.resize(length);
    fseek(f, 0, SEEK_SET);
    fread(ret.data(), 1, length, f);
    fclose(f);

    return ret;
}

int roundUp(int i, int j)
{
    return (i + j - 1) / j * j;
}

std::string readFixedLengthString(const char *str, size_t maxlen)
{
    return std::string(str, strnlen(str, maxlen));
}

std::string getFileExtension(const std::string &path)
{
    std::string::size_type idx= path.rfind('.');
    
    if(idx != std::string::npos)
    {
        std::string ext = path.substr(idx + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }
    else
    {
        return "";
    }
}

PS2File::PS2File()
    : mode(DEFAULT_FILE_MODE)
{
    getCurrentTime(dateCreated);
    getCurrentTime(dateModified);
}

void PS2File::setDateCreated(const sceMcStDateTime &dateCreated)
{
    if(*reinterpret_cast<const uint64_t *>(&dateCreated) != 0)
        this->dateCreated = dateCreated;
}

void PS2File::setDateModified(const sceMcStDateTime &dateModified)
{
    if(*reinterpret_cast<const uint64_t *>(&dateModified) != 0)
        this->dateModified = dateModified;
}

void PS2File::setMode(uint32_t mode)
{
    if((mode & 0x8010) == 0x8010) // PS2 file attribute
        this->mode = mode;
}

std::ostream& operator<<(std::ostream &os, const PS2File &file)
{
    os << "Name: " << file.name;
    os << ", Length: " << file.data.size();
    os << ", Created: " << printTime(file.dateCreated);
    os << ", Modified: " << printTime(file.dateModified);

    return os;
}

PS2Directory::PS2Directory()
    : mode(DEFAULT_DIRECTORY_MODE)
{
    getCurrentTime(dateCreated);
    getCurrentTime(dateModified);
}

void PS2Directory::setDateCreated(const sceMcStDateTime &dateCreated)
{
    if(*reinterpret_cast<const uint64_t *>(&dateCreated) != 0)
        this->dateCreated = dateCreated;
}

void PS2Directory::setDateModified(const sceMcStDateTime &dateModified)
{
    if(*reinterpret_cast<const uint64_t *>(&dateModified) != 0)
        this->dateModified = dateModified;
}

void PS2Directory::setMode(uint32_t mode)
{
    if((mode & 0x8020) == 0x8020) // PS2 directory attribute
        this->mode = mode;
}

std::ostream& operator<<(std::ostream &os, const PS2Directory &dir)
{
    os << "Name: " << dir.name;
    os << ", Created: " << printTime(dir.dateCreated);
    os << ", Modified: " << printTime(dir.dateModified);

    return os;
}

void getCurrentTime(sceMcStDateTime &dateTime)
{
    // BUG: This will set the current time in UTC, but it needs to be
    // converted to +9 UTC (Japan) as the PS2 expects it to be.
    time_t currentTime;
    time(&currentTime);
    struct tm *realTime = gmtime(&currentTime);
    
    dateTime.Resv2  = 0;
    dateTime.Sec    = realTime->tm_sec;
    dateTime.Min    = realTime->tm_min;
    dateTime.Hour   = realTime->tm_hour;
    dateTime.Day    = realTime->tm_mday;
    dateTime.Month  = 1 + realTime->tm_mon;
    dateTime.Year   = 1900 + realTime->tm_year;
}

std::string printTime(const sceMcStDateTime &dateTime)
{
    return std::to_string(dateTime.Day) + "/"
    + std::to_string(dateTime.Month) + "/"
    + std::to_string(dateTime.Year) + " "
    + std::to_string(dateTime.Hour) + ":"
    + std::to_string(dateTime.Min) + ":"
    + std::to_string(dateTime.Sec);
}
