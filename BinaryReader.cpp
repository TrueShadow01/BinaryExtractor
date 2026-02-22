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

void BinaryReader::Seek(size_t offset) {
	m_file.seekg(offset, std::ios::beg);
	if (!m_file) {
		std::cerr << "Failed to move File pointer!" << std::endl;
		exit(1);
	}
}