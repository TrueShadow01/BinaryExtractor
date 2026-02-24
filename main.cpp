#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <filesystem>

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

struct PackerEntry {
	std::string name;
	uint32_t size;
	uint32_t offset;
	std::vector<uint8_t> data;
};

constexpr uint32_t MAGIC_FORG = 0x47524F46;

void ExtractFile(BinaryReader& binReader, const FileEntry& entry);

int main(int argc, char* argv[]) {
	try {
		std::cout << "Custom BIN Extractor\n" << std::endl;

		if (argc < 2) {
			throw std::runtime_error("Usage: BinaryExtractor.exe <archive file>");
		}

		// open the specified file in binary format and check if we are able to open it
		std::ifstream file(argv[1], std::ios::binary);
		if (!file.is_open()) {
			std::cerr << "Cannot open File!" << std::endl;
			throw std::runtime_error("Unable to open File!\nFilename: " + std::string(argv[1]));
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
				throw std::runtime_error("Entry offset inside metadata region!");
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
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

ArchiveHeader ReadHeader(BinaryReader& binReader) {
	ArchiveHeader header;

	header.magic = binReader.ReadUInt32();

	if (header.magic != MAGIC_FORG) {
		std::cerr << "Invalid Archive format!" << std::endl;
		throw std::runtime_error("Invalid Archive format!");
	}

	header.version = binReader.ReadUInt32();
	header.fileCount = binReader.ReadUInt32();
	if (header.fileCount == 0 || header.fileCount > 10000) {
		std::cerr << "Invalid file count!" << std::endl;
		throw std::runtime_error("Invalid file count!");
	}

	return header;
}

FileEntry ReadFileEntry(BinaryReader& binReader) {
	FileEntry entry;

	uint32_t  nameLength = binReader.ReadUInt32();
	if (nameLength == 0 || nameLength > 255) {
		std::cerr << "Invalid name length!" << std::endl;
		throw std::runtime_error("Invalid name length!");
	}

	// boundary check
	size_t fileSize = binReader.GetFileSize();
	size_t currentPos = binReader.Tell();

	if (currentPos + nameLength > fileSize) {
		std::cerr << "Name exceeds file size!" << std::endl;
		throw std::runtime_error("Name exceeds file size!");
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
		throw std::runtime_error("Invalid entry range!");
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
		throw std::runtime_error("Failed to create output file!");
	}

	out.write(reinterpret_cast<char*>(data.data()), data.size());

	if (!out) {
		std::cerr << "Failed to write bytes" << std::endl;
		throw std::runtime_error("Failed to write bytes!");
	}
}

std::vector<PackerEntry> LoadDirectory(const std::filesystem::path& inputDir) {
	if (!std::filesystem::exists(inputDir) || !std::filesystem::is_directory(inputDir)) {
		throw std::runtime_error("Invalid input directory!");
	}

	std::vector<PackerEntry> entries;

	for (const auto& dirEntry : std::filesystem::directory_iterator(inputDir)) {
		if (!dirEntry.is_regular_file()) {
			continue;
		}

		std::ifstream file(dirEntry.path(), std::ios::binary);
		if (!file) {
			throw std::runtime_error("Failed to open file: " + dirEntry.path().string() + "!");
		}

		file.seekg(0, std::ios::end);
		std::streamsize fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		if (fileSize < 0) {
			throw std::runtime_error("Invalid file size!");
		}

		if (fileSize > UINT32_MAX) {
			throw std::runtime_error("File too large for archive format!");
		}

		std::vector<uint8_t> buffer(static_cast<size_t>(fileSize));

		if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
			throw std::runtime_error("Failed to read file data!");
		}

		PackerEntry entry;
		entry.name = dirEntry.path().filename().string();
		entry.size = static_cast<uint32_t>(fileSize);
		entry.offset = 0; // calculating offset later
		entry.data = std::move(buffer);

		entries.push_back(std::move(entry));
	}

	return entries;
}