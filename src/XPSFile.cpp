#include <iostream>

#include "XPSFile.h"

#pragma pack(push, 1)
typedef struct xpsEntry
{
    uint16_t        headerLength;
    char            name[64];
    uint32_t        length;
    uint32_t        startSector;
    uint32_t        endSector;
    uint32_t        mode;
    sceMcStDateTime created;
    sceMcStDateTime modified;
    uint32_t        padding;
    uint32_t        unk;
    sceMcStDateTime unknownDate;
    char            title[64]; // same as title in main header if it's a directory entry
    char            titleSJIS[64]; // same as title if it's a file entry
} xpsEntry_t;
#pragma pack(pop)

bool XPSFile::read(const std::string &path, PS2Directory &dir)const
{
    std::vector<unsigned char> xpsData = readFileContents(path);
    if(!isValid(xpsData))
    {
        printf("Error: not a valid XPS/SPS file.\n");
        return false;
    }
    
    // Main Header:
    //  uint32_t    version // might be length of magic
    //  char        magic[13] // "SharkPortSave"
    //  uint32_t    padding
    //  uint32_t    titleLength
    //  char        title[titleLength]
    //  uint32_t    descriptionLength
    //  char        description[descriptionLength]
    //  uint32_t    commentLength
    //  char        comment[commentLength]
    //  uint32_t    dataLength // size of rest of file - 4
    unsigned int titleLengthOffset = 21;
    unsigned int titleLength = *reinterpret_cast<uint32_t *>(&xpsData[titleLengthOffset]);
    unsigned int descriptionLengthOffset = titleLengthOffset + 4 + titleLength;
    unsigned int descriptionLength = *reinterpret_cast<uint32_t *>(&xpsData[descriptionLengthOffset]);
    unsigned int commentLengthOffset = descriptionLengthOffset + 4 + descriptionLength;
    unsigned int commentLength = *reinterpret_cast<uint32_t *>(&xpsData[commentLengthOffset]);
    unsigned int dataLengthOffset = commentLengthOffset + 4 + commentLength;
    unsigned int dataLength = *reinterpret_cast<uint32_t *>(&xpsData[dataLengthOffset]);
    unsigned int headerOffset = dataLengthOffset + 4;
    
    xpsEntry_t *mainHeader = reinterpret_cast<xpsEntry_t *>(&xpsData[headerOffset]);
    dir.name = readFixedLengthString(mainHeader->name, 64);
    dir.setMode(mainHeader->mode);
    
    unsigned int offset = headerOffset + mainHeader->headerLength;
    for(int i = 0; i < mainHeader->length - 2; i++)
    {
        xpsEntry_t *entry = reinterpret_cast<xpsEntry_t *>(&xpsData[offset]);
        offset += entry->headerLength;
        unsigned char *entryData = &xpsData[offset];
        
        if(offset + entry->length > xpsData.size())
        {
            printf("Error: data outside of range.\n");
            return false;
        }

        PS2File f;
        f.name          = readFixedLengthString(entry->name, 64);
        f.setMode(entry->mode);
        f.setDateCreated(entry->created);
        f.setDateModified(entry->modified);
        f.data          = std::vector<unsigned char>(entryData, entryData + entry->length);
        dir.files.push_back(f);
        
        if(f.name.length() == 0)
        {
            printf("Error: unexpected data in file header.\n");
            return false;
        }

        offset += entry->length;
    }
    
    return true;
}

bool XPSFile::write(const std::string &path, const PS2Directory &dir)const
{
    // TODO: Implement writing XPS save files
    std::cout << "Error: writing to .xps/.sps is not supported" << std::endl;
    return false;
}

bool XPSFile::isValid(const std::vector<unsigned char> &data)const
{
    bool isValid(true);
    int offset(0);
    
    isValid = isValid && data.size() > 0x270; // estimate
    isValid = isValid && *reinterpret_cast<const uint32_t *>(&data[offset]) == 0xD;
    offset += 4;
    isValid = isValid && std::string(reinterpret_cast<const char *>(&data[offset]), 13) == "SharkPortSave";
    offset += 17;
    uint32_t titleLength = *reinterpret_cast<const uint32_t *>(&data[offset]) + 1;
    isValid = isValid && titleLength > 0;
    offset += 4;
    isValid = isValid && std::string(reinterpret_cast<const char *>(&data[offset])).length() == titleLength;
    
    // TODO: Verify the rest of the main header
    
    return isValid;
}

bool XPSFile::isValid(const std::string &path) const
{
    std::vector<unsigned char> data = readFileContents(path);
    return isValid(data);
}

bool XPSFile::isCorrectFileExtension(const std::string &path)
{
    std::string extension = getFileExtension(path);
    return (extension == "xps") ||
           (extension == "sps");
}
