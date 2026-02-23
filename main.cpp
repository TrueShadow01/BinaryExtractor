#include <iostream>
#include <fstream>
#include <string>

#include "BinaryReader.h"

struct ArchiveHeader {
	uint32_t magic = 0;
	uint32_t version = 0;
	uint32_t fileCount = 0;
};

ArchiveHeader ReadHeader(BinaryReader& binReader);

struct FileEntry {
	std::string name = "";
	uint32_t size = 0;
	uint32_t offset = 0;
};

FileEntry ReadFileEntry(BinaryReader& binReader);

constexpr uint32_t MAGIC_FORG = 0x47524F46;

void ExtractFile(BinaryReader& binReader, const FileEntry& entry);

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

	// loop over each entry and store it in a vector incase there is more than 1 file
	std::vector<FileEntry> entries;
	for (uint32_t i = 0; i < header.fileCount; i++) {
		entries.push_back(ReadFileEntry(binReader));
	}

	size_t metadataEnd = binReader.Tell();
	for (const auto& entry : entries) {
		if (entry.offset < metadataEnd) {
			std::cerr << "Entry offset inside metadata region!" << std::endl;
			exit(1);
		}
	}

	std::cout << "File magic: " << std::hex << header.magic << std::endl;;
	std::cout << "File Version: " << header.version << std::endl;
	std::cout << "File Count: " << header.fileCount << std::endl;

	// print values from vector
	for (const auto& entry : entries) {
		std::cout << "Name: " << entry.name << std::endl;
		std::cout << "Size: " << entry.size << std::endl;
		std::cout << "Offset: " << entry.offset << std::endl;
		ExtractFile(binReader, entry);
	}

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
	if (header.fileCount == 0 || header.fileCount > 10000) {
		std::cerr << "Invalid file count!" << std::endl;
		exit(1);
	}

	return header;
}

FileEntry ReadFileEntry(BinaryReader& binReader) {
	FileEntry entry;

	uint32_t  nameLength = binReader.ReadUInt32();
	if (nameLength == 0 || nameLength > 255) {
		std::cerr << "Invalid name length!" << std::endl;
		exit(1);
	}

	entry.name = binReader.ReadString(nameLength);
	entry.size = binReader.ReadUInt32();
	entry.offset = binReader.ReadUInt32();

	return entry;
}

// Extract the File by parsing the Binary Reader and File Entry
void ExtractFile(BinaryReader& binReader, const FileEntry& entry) {
	// check file size
	size_t fileSize = binReader.GetFileSize();
	if (entry.offset > fileSize || entry.size > fileSize || entry.offset > fileSize - entry.size) {
		std::cerr << "Invalid entry range!" << std::endl;
		exit(1);
	}

	// move file pointer to specified offset
	binReader.Seek(entry.offset);
	std::cout << "Seeking to: " << entry.offset << std::endl;
	std::cout << "Current pos: " << binReader.Tell() << std::endl;

	// read data
	auto data = binReader.ReadBytes(entry.size);
	std::cout << "Read bytes: " << data.size() << std::endl;

	// create output file with the name and it's content
	std::ofstream out(entry.name, std::ios::binary);
	if (!out) {
		std::cerr << "Failed to create output file!" << std::endl;
		exit(1);
	}

	out.write(reinterpret_cast<char*>(data.data()), data.size());

	if (!out) {
		std::cerr << "Failed to write bytes" << std::endl;
		exit(1);
	}
}