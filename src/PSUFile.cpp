#include "PSUFile.h"
#include "Utilities.h"

#include <cstring>

#pragma pack(push, 1)
typedef struct psuEntry
{
    uint32_t        mode;
    uint32_t        length;
    sceMcStDateTime created;
    uint32_t        cluster;
    uint32_t        dirIndex;
    sceMcStDateTime modified;
    uint32_t        attr;
    char            padding1[28];
    char            name[32];
    char            padding2[0x1A0];
} psuEntry_t;
#pragma pack(pop)

bool PSUFile::read(const std::string &path, PS2Directory &dir)const
{
    std::vector<unsigned char> psuData = readFileContents(path);
    if(!isValid(psuData))
    {
        printf("Error: not a valid PSU save file.\n");
        return false;
    }
    
    const psuEntry_t *mainHeader = reinterpret_cast<const psuEntry_t *>(&psuData[0]);
    dir.name = readFixedLengthString(mainHeader->name, 32);
    dir.setMode(mainHeader->mode);
    dir.setDateCreated(mainHeader->created);
    dir.setDateModified(mainHeader->modified);
    
    int offset = 3 * sizeof(psuEntry_t); // Skip over main directory, ".", and ".."
    for(int i = 0; i < mainHeader->length - 2; i++)
    {
        const psuEntry_t *entry = reinterpret_cast<const psuEntry_t *>(&psuData[offset]);
        offset += sizeof(psuEntry_t);
        const unsigned char *entryData = &psuData[offset];
        
        if(offset + entry->length > psuData.size())
        {
            printf("Error: data outside of range.\n");
            return false;
        }
        
        PS2File f;
        f.name = readFixedLengthString(entry->name, 32);
        f.setMode(entry->mode);
        f.setDateCreated(entry->created);
        f.setDateModified(entry->modified);
        f.data = std::vector<unsigned char>(entryData, entryData + entry->length);
        dir.files.push_back(f);
        
        offset += roundUp(entry->length, 1024);
    }
    
    return true;
}

static void writeEntryHeader(const psuEntry_t &entry, FILE *f)
{
    if(!f)
        return;
    
    fwrite(&entry, sizeof(entry), 1, f);
    for(int i = 0; i < 512 - sizeof(entry); i++)
        fputc(0, f);
}

bool PSUFile::write(const std::string &path, const PS2Directory &dir)const
{
    // Setup directory header
    psuEntry_t mainHeader;
    memset(&mainHeader, 0, sizeof(mainHeader));
    strncpy(mainHeader.name, dir.name.c_str(), sizeof(mainHeader.name));
    mainHeader.created  = dir.getDateCreated();
    mainHeader.modified = dir.getDateModified();
    mainHeader.length   = dir.files.size() + 2;
    mainHeader.mode     = dir.getMode();

    FILE *f = fopen(path.c_str(), "wb");
    if(!f)
    {
        printf("Error: Can't open %s for writing.\n", path.c_str());
        return false;
    }

    // Write main directory entry
    writeEntryHeader(mainHeader, f);
    
    // Write "."
    strncpy(mainHeader.name, ".", sizeof(mainHeader.name));
    writeEntryHeader(mainHeader, f);
    
    // Write ".."
    strncpy(mainHeader.name, "..", sizeof(mainHeader.name));
    writeEntryHeader(mainHeader, f);
    
    // Write file data
    for(auto &file : dir.files)
    {
        psuEntry_t fileHeader;
        memset(&fileHeader, 0, sizeof(fileHeader));
        strncpy(fileHeader.name, file.name.c_str(), sizeof(fileHeader.name));
        fileHeader.created  = file.getDateCreated();
        fileHeader.modified = file.getDateModified();
        fileHeader.length   = file.data.size();
        fileHeader.mode     = file.getMode();
        writeEntryHeader(fileHeader, f);
        
        fwrite(file.data.data(), file.data.size(), 1, f);
        
        // Pad to 1024 byte boundry
        int padding = roundUp(file.data.size(), 1024) - file.data.size();
        while(padding--)
            fputc(0, f);
    }
    
    fclose(f);
    
    return true;
}

bool PSUFile::isValid(const std::vector<unsigned char> &data)const
{
    bool isValid(true);
    
    isValid = isValid && data.size() > 0x600;
    isValid = isValid && data[0x240] == '.';
    isValid = isValid && data[0x440] == '.' && data[0x441] == '.';
    
    // TODO: More extensive verification.
    
    return isValid;
}

bool PSUFile::isValid(const std::string &path)const
{
    std::vector<unsigned char> data = readFileContents(path);
    return isValid(data);
}

bool PSUFile::isCorrectFileExtension(const std::string &path)
{
    std::string extension = getFileExtension(path);
    return (extension == "psu");
}
