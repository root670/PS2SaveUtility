#pragma once

#include <cstdint>
#include <vector>
#include <ctime>
#include <string>

std::vector<unsigned char> readFileContents(const std::string &path);
int roundUp(int i, int j);
std::string readFixedLengthString(const char *str, size_t maxlen);
std::string getFileExtension(const std::string &path);

#pragma pack(push, 1)
typedef struct _sceMcStDateTime {
	uint8_t  Resv2;
	uint8_t  Sec;
	uint8_t  Min;
	uint8_t  Hour;
	uint8_t  Day;
	uint8_t  Month;
	uint16_t Year;
} sceMcStDateTime;
#pragma pack(pop)

void getCurrentTime(sceMcStDateTime &dateTime);
std::string printTime(const sceMcStDateTime &dateTime);

class PS2File
{
public:
    PS2File();
    ~PS2File() {};
    
    std::string					name;
	std::vector<unsigned char>	data;
    
    sceMcStDateTime getDateCreated() const { return dateCreated; }
    void            setDateCreated(const sceMcStDateTime &dateCreated);
    
    sceMcStDateTime getDateModified() const { return dateModified; }
    void            setDateModified(const sceMcStDateTime &dateModified);
    
    uint32_t getMode() const { return mode; }
    void     setMode(uint32_t mode);
    
    friend std::ostream& operator<<(std::ostream &os, const PS2File &file);
    
    static const uint32_t DEFAULT_FILE_MODE = 0x8497; // File with RWX permissions
    
private:
    sceMcStDateTime dateCreated;
    sceMcStDateTime dateModified;
    uint32_t        mode;
};

class PS2Directory
{
public:
    PS2Directory();
    ~PS2Directory() {};
	
    std::string				name;
	std::vector<PS2File>	files;
    
    sceMcStDateTime getDateCreated() const { return dateCreated; }
    void            setDateCreated(const sceMcStDateTime &dateCreated);
    
    sceMcStDateTime getDateModified() const { return dateModified; }
    void            setDateModified(const sceMcStDateTime &dateModified);
    
    uint32_t getMode() const { return mode; }
    void     setMode(uint32_t mode);
    
    friend std::ostream& operator<<(std::ostream &os, const PS2Directory &file);
    
    static const uint32_t DEFAULT_DIRECTORY_MODE = 0x8427; // Directory with RWX permissions
    
private:
    sceMcStDateTime dateCreated;
    sceMcStDateTime dateModified;
    uint32_t        mode;
};
