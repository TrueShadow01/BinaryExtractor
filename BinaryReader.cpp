#include "BinaryReader.h"
#include <iostream>

BinaryReader::BinaryReader(std::ifstream& file) : m_file(file) {

}

// reads 4 bytes (32-bit read)
// returns a uint32_t with the read value from specified file
uint32_t BinaryReader::ReadUInt32() {
	// read 4 bytes of specified file
	uint8_t bytesToRead[4];
	m_file.read((char*)bytesToRead, 4);
	// error checking
	if (!m_file) {
		std::cerr << "Failed to read File!" << std::endl;
		exit(1);
	}

	// bit shifting stuff
	uint32_t value = (uint32_t)bytesToRead[0] | ((uint32_t)bytesToRead[1] << 8) | ((uint32_t)bytesToRead[2] << 16) | ((uint32_t)bytesToRead[3] << 24);

	return value;
}

// Moves the File Pointer to a specific offset
void BinaryReader::Seek(size_t offset) {
	// move file pointer from the beginning of the file to the offest
	m_file.seekg(offset, std::ios::beg);
	// error checking
	if (!m_file) {
		std::cerr << "Failed to move File pointer!" << std::endl;
		exit(1);
	}
}

// Shows you where the file pointer is currently
size_t BinaryReader::Tell() {
	// get file pointer position
	std::streampos pos = m_file.tellg();

	// check if file pointer position is valid
	if (pos == std::streampos(-1)) {
		std::cerr << "Failed to get file position!" << std::endl;
		exit(1);
	}

	return static_cast<size_t>(pos);
}