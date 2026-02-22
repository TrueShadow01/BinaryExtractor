#include <iostream>
#include <fstream>

#include "BinaryReader.h"

struct ArchiveHeader {
	uint32_t magic;
	uint32_t version;
	uint32_t fileCount;
};

ArchiveHeader ReadHeader(BinaryReader& binReader);

constexpr uint32_t MAGIC_FORG = 0x47524F46;

int main() {
	std::cout << "Custom BIN Extractor\n" << std::endl;

	// open the specified file in binary format and check if we are able to open it
	std::ifstream file("E:\\Extractor Test Files\\test.bin", std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Cannot open File!" << std::endl;
		exit(1);
	}

	BinaryReader binReader(file);
	ArchiveHeader header = ReadHeader(binReader);

	std::cout << "File magic: " << std::hex << header.magic << std::endl;;
	std::cout << "File Version: " << header.version << std::endl;
	std::cout << "File Count: " << header.fileCount << std::endl;

	exit(0);
}

ArchiveHeader ReadHeader(BinaryReader& binReader) {
	ArchiveHeader header;

	header.magic = binReader.ReadUInt32();

	if (header.magic != MAGIC_FORG) {
		std::cerr << "Invalid Archive format!" << std::endl;
		exit(1);
	}

	header.version = binReader.ReadUInt32();
	header.fileCount = binReader.ReadUInt32();

	return header;
}