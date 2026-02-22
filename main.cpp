#include <iostream>
#include <fstream>

#include "BinaryReader.h"

int main() {
	std::cout << "Custom BIN Extractor\n" << std::endl;

	// open the specified file in binary format and check if we are able to open it
	std::ifstream file("E:\\Extractor Test Files\\test.bin", std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Cannot open File!" << std::endl;
		exit(1);
	}

	BinaryReader binReader(file);

	// read the file magic to determine it's type
	uint32_t magic = binReader.ReadUInt32();

	std::cout << "File magic: " << std::hex << magic << std::endl;;

	// read it's version
	uint32_t version = 0;
	version = binReader.ReadUInt32();

	std::cout << "File Version: " << version << std::endl;

	exit(0);
}