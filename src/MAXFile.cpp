#include <iostream>

#include "MAXFile.h"
#include "lzari.h"

#pragma pack(push, 1)
typedef struct maxHeader
{
	char		magic[12];
	uint32_t	crc;
	char		dirName[32];
	char		iconSysName[32];
	uint32_t	compressedSize;
	uint32_t	numFiles;
	uint32_t	decompressedSize;
} maxHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct maxEntry
{
	uint32_t	length;
	char		name[32];
} maxEntry_t;
#pragma pack(pop)

bool MAXFile::read(const std::string &path, PS2Directory &dir)const
{
    std::vector<unsigned char> maxData = readFileContents(path);
    if(!isValid(maxData))
    {
        printf("Error: not a valid MAX save file.\n");
        return false;
    }

	const maxHeader_t *header = reinterpret_cast<const maxHeader_t *>(maxData.data());

	dir.name = readFixedLengthString(header->dirName, 32);

	// Decompress
	unsigned char *compressed = maxData.data() + sizeof(maxHeader_t);
	static unsigned char decompressed[16 * 1024 * 1024];

	int ret = unlzari(compressed, header->compressedSize, decompressed, header->decompressedSize);
	if (ret != header->decompressedSize)
	{
		printf("Error: decompression failed.\n");
		return false;
	}

	// Read each file entry
	unsigned int offset(0);
	for (int i = 0; i < (int)header->numFiles; i++)
	{
		maxEntry_t *entry = reinterpret_cast<maxEntry_t *>(&decompressed[offset]);
		offset += sizeof(maxEntry_t);
		unsigned char *entryData = &decompressed[offset];

		PS2File f;
		f.name = readFixedLengthString(entry->name, 32);
		f.data = std::vector<unsigned char>(entryData, entryData + entry->length);
		dir.files.push_back(f);
		
		offset += entry->length;
		offset = roundUp(offset + 8, 16) - 8;
	}

	return true;
}

bool MAXFile::write(const std::string &path, const PS2Directory &dir)const
{
    // TODO: Implement writing MAX save files
    std::cout << "Error: writing to .max is not supported" << std::endl;
    return false;
}

bool MAXFile::isValid(const std::vector<unsigned char> &data)const
{
    bool isValid(true);
    
    isValid = isValid && data.size() > sizeof(maxHeader_t);
    
    const maxHeader_t *header = reinterpret_cast<const maxHeader_t *>(data.data());
    isValid = isValid && std::string(header->magic, 12) == "Ps2PowerSave";
    isValid = isValid && header->compressedSize > 0;
    isValid = isValid && header->decompressedSize > 0;
    isValid = isValid && std::string(header->dirName, 64).length() > 0;
    isValid = isValid && std::string(header->iconSysName, 64).length() > 0;
    isValid = isValid && header->numFiles > 0;
    
    // TODO: Verify file contents using header->crc.
    
    return isValid;
}

bool MAXFile::isValid(const std::string &path)const
{
    std::vector<unsigned char> data = readFileContents(path);
    return isValid(data);
}

bool MAXFile::isCorrectFileExtension(const std::string &path)
{
    std::string extension = getFileExtension(path);
    return (extension == "max") ||
           (extension == "pws");
}
